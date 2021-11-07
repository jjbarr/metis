

// This example code is in the public domain.
#include <../../MetisShelfProtocol.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>

#define NUMSHELFDEVICES 16
#define DEBUGPRINTS 1

// --- prototypes ---
void onShelfPing(int numBytes);
byte readShelfReg(byte shelfAddr, byte regAddr);
int writeShelfReg(byte shelfAddr, byte regAddr, byte value);
void enrollNewShelf();
UID generateNewUID();
int writeDrawer(UID shelfID, byte col, byte row, bool onOff);
void pollShelves();
void pollSerial();

// --- globals structs ---
typedef struct {
    UID uid;
    byte addr;
} ShelfDev;

ShelfDev shelves[NUMSHELFDEVICES + 1] = {};  //= {{0,0}}

// msg uplink = {};

// --- utility function ---
#if DEBUGPRINTS == 0
    #define debugout(...)
    #define debugoutln(...)
#else
    #define debugout Serial.print
    #define debugoutln Serial.println
#endif

byte handledEndTransmission(void) {
    byte worked = Wire.endTransmission(true);
    if (worked) {
        debugoutln("endTransmission failed on with code ");
        debugoutln(worked, HEX);
        pollShelves();  // clean up globally allocated structs
    } else {
        // debugoutln("handle worked");
    }
    return worked;
}

// --- nitty gritty ---

void setup() {
    Wire.begin(HUB_ADDR);
    Wire.onReceive(onShelfPing);
    // serinter_init();
    Serial.begin(9600);

    debugoutln(" --- starting --- ");
}

byte todo = 0;
#define TODO_NOTHING 0
#define TODO_ENROLL_SHELF 1
void loop() {
    bool did_thing = false;

    if (todo & TODO_ENROLL_SHELF) {
        enrollNewShelf();
        todo &= ~TODO_ENROLL_SHELF;
        did_thing |= true;
    }
    if (todo == 0 && did_thing) {
        debugoutln("waitng...");
        // delay(1500);
    }

    pollSerial();

    // pollShelves();

    // for (int i = 0; shelves[i].uid != 0; i++) {
    //     ShelfDev shelf = shelves[i];
    //     int width      = readShelfReg(shelf.addr, SHELF_DRAWERS_WIDE);
    //     int height     = readShelfReg(shelf.addr, SHELF_DRAWERS_HIGH);
    //     for (int row = 0; row < height; row += 1) {
    //         for (int col = 0; col < width; col += 1) {
    //             if (writeDrawer(shelf.uid, col, row, true))
    //                 break;
    //             delay(500);
    //             if (writeDrawer(shelf.uid, col, row, false))
    //                 break;
    //         }
    //     }
    // }
}

// -- impls and tools ---
#define DEV_ADDR_NOT_FOUND 0xff
byte getDevAddrForUID(UID uid) {
    for (int i = 0; i < NUMSHELFDEVICES; i++) {
        if (shelves[i].uid == uid) {
            return shelves[i].addr;
        }
    }
    return DEV_ADDR_NOT_FOUND;
}

int setDevAddrForUID(UID uid, byte addr) {
    // return -1 means it failed
    for (int i = 0; i < NUMSHELFDEVICES; i++) {
        ShelfDev shelf = shelves[i];
        if (shelf.uid == uid || shelf.uid == 0) {
            shelf.uid  = uid;
            shelf.addr = addr;
            shelves[i] = shelf;
            return 0;
        }
    }
    return -1;
}

UID generateNewUID(void) {
    UID lastuid = (((UID)EEPROM[3] << 24) | ((UID)EEPROM[2] << 16) |
                   ((UID)EEPROM[1] << 8) | ((UID)EEPROM[0] << 0));

    UID newuid = lastuid + 1;

    EEPROM[3] = (newuid >> 24) & 0xff;
    EEPROM[2] = (newuid >> 16) & 0xff;
    EEPROM[1] = (newuid >> 8) & 0xff;
    EEPROM[0] = (newuid >> 0) & 0xff;

    return newuid;
}

byte readShelfReg(byte shelfAddr, byte regAddr) {
    // TODO add printing here w/ debug macro is DEBUG(...) / DEBUGLN(...) that
    // maps to debugout(...) / debugoutln(...)
    Wire.beginTransmission(shelfAddr);
    Wire.write(regAddr);
    handledEndTransmission();

    Wire.requestFrom((uint8_t)shelfAddr, (byte)1);
    return Wire.read();
}

int writeShelfReg(byte shelfAddr, byte regAddr, byte value) {
    Wire.beginTransmission(shelfAddr);
    byte msg[] = {regAddr, value};
    Wire.write((char*)&msg, 2);
    if (handledEndTransmission())
        return -1;
    return 0;
}

UID readShelfUID(byte shelfAddr) {
    return (((UID)readShelfReg(shelfAddr, SHELF_UID_B3) << 24) |
            ((UID)readShelfReg(shelfAddr, SHELF_UID_B2) << 16) |
            ((UID)readShelfReg(shelfAddr, SHELF_UID_B1) << 8) |
            ((UID)readShelfReg(shelfAddr, SHELF_UID_B0) << 0));
}

void writeShelfUID(byte shelfAddr, UID id) {
    writeShelfReg(shelfAddr, SHELF_UID_B3, (id >> 24) & 0xff);
    writeShelfReg(shelfAddr, SHELF_UID_B2, (id >> 16) & 0xff);
    writeShelfReg(shelfAddr, SHELF_UID_B1, (id >> 8) & 0xff);
    writeShelfReg(shelfAddr, SHELF_UID_B0, (id >> 0) & 0xff);

    UID readbackID = readShelfUID(shelfAddr);
    if (readbackID != id) {
        debugout("uid write failed, wrote 0x");
        debugoutln(id, HEX);
        debugout(" but read back 0x");
        debugoutln(readbackID, HEX);
    }
}

bool _shelf_even_there(byte addr) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
        return true;
    } else {
        return false;
    }
}

void pollShelves() {
    int shiftup = 0;
    for (int i = 0; shelves[i].uid != 0; i += 1) {
        ShelfDev shelf = shelves[i];

        if (!_shelf_even_there(shelf.addr)) {
            debugout("removing shelf 0x");
            debugoutln(shelf.uid);
            shelves[i] = {};
            shiftup += 1;
            continue;
        } else {
            shelves[i]           = {};
            shelves[i - shiftup] = shelf;
        }
    }
}

byte next_shelf_addr = ASSIGNED_SHELF_BASE_ADDR;
void enrollNewShelf() {
    debugoutln("enrolling shelf");

    // check uid
    UID uid = readShelfUID(UNASSIGNED_SHELF_ADDR);
    debugout("uid = ");
    debugoutln(uid, HEX);

    if (uid == 0) {
        UID newuid = generateNewUID();
        debugout("assigning new uid 0x");
        debugoutln(newuid, HEX);
        writeShelfUID(UNASSIGNED_SHELF_ADDR, newuid);
        uid = newuid;
    }

    // check i2c address
    byte addr = next_shelf_addr;
    next_shelf_addr += 1;
    setDevAddrForUID(uid, addr);
    writeShelfReg(UNASSIGNED_SHELF_ADDR, SHELF_DEV_ADDR, addr);

    delay(100);

    debugoutln("Done enrolling shelf");
}

void onShelfPing(int numBytes) {

    // check I2C input
    // TODO: put this into a function
    if (Wire.available() != 1) {
        debugout("enroll requested ");
        debugout(Wire.available());
        debugout(", invalid request size");
        return;
    }

    byte cmd = Wire.read();

    if (cmd != HUB_ENROL_REQ) {
        debugout("unknown command 0x");
        debugoutln(cmd, HEX);
        return;
    }

    debugoutln("queueing enroll in todo");

    todo |= TODO_ENROLL_SHELF;
}

int readDrawer(UID shelfID, byte col, byte row) {
    byte addr = getDevAddrForUID(shelfID);
    if (addr == DEV_ADDR_NOT_FOUND) {
        debugout("Device id 0x");
        debugout(shelfID, HEX);
        debugoutln(" not found, skipping read");
        return -1;
    }
    if (writeShelfReg(addr, SHELF_SEL_COL, col))
        return -1;
    if (writeShelfReg(addr, SHELF_SEL_ROW, row))
        return -1;
    return readShelfReg(addr, SHELF_DRAWER_STAT);
}

int writeDrawer(UID shelfID, byte col, byte row, bool onOff) {
    byte addr = getDevAddrForUID(shelfID);
    if (addr == DEV_ADDR_NOT_FOUND) {
        debugout("Device id 0x");
        debugout(shelfID, HEX);
        debugoutln(" not found, skipping write");
        return -1;
    }
    if (writeShelfReg(addr, SHELF_SEL_COL, col))
        return -1;
    if (writeShelfReg(addr, SHELF_SEL_ROW, row))
        return -1;
    if (writeShelfReg(addr, SHELF_DRAWER_STAT, onOff))
        return -1;
    return 0;
}

typedef struct {
    bool valid;
    UID uid;
    byte row;
    byte col;
} BodyThreePack;

#define INVALIDTHREEPACK                                                       \
    (BodyThreePack) {                                                          \
        false, 0, 0, 0                                                         \
    }

#define CLIP_OFF_SECTION(body) body = body.substring(body.indexOf('|') + 1)
#define READ_NEXT_VALUE(body) body.substring(0, body.indexOf('|')).toInt()

BodyThreePack parseThreeinput(String body) {

    if (body.indexOf('|') != 0)
        return INVALIDTHREEPACK;

    CLIP_OFF_SECTION(body);
    UID id = READ_NEXT_VALUE(body);

    CLIP_OFF_SECTION(body);
    byte row = READ_NEXT_VALUE(body);

    CLIP_OFF_SECTION(body);
    byte col = READ_NEXT_VALUE(body);

    debugout("id=");
    debugout(id);
    debugout(", ");

    debugout("row=");
    debugout(row);
    debugout(", ");

    debugout("col=");
    debugout(col);
    debugout(", ");

    debugoutln();

    debugout("remaining=");
    debugoutln(body);

    debugoutln();
    return {true, id, row, col};
}

// void serialSetDrawer(String body) {
//     auto pack = parseThreeinput(body);
// }
void toggleClearDrawer(String body) {
    auto pack = parseThreeinput(body);

    int onOff = readDrawer(pack.uid, pack.col, pack.row);
    if (onOff == -1) {
        debugoutln("unable to read pack");
        return;
    }

    writeDrawer(pack.uid, pack.col, pack.row, !(bool)onOff);
}

void pollSerial() {
    if (Serial.peek() == -1)
        return;
    else
        Serial.print("...");

    char msgkind = Serial.read();
    Serial.setTimeout((int)((uint16_t)0 - 1));
    String body = Serial.readStringUntil('\n');
    Serial.read();
    Serial.println(msgkind);

    debugout("body=");
    debugoutln(body);

    switch (msgkind) {
    // case 'e': enumerateDevices(); break;
    // case 'x': clearShelf(); break;
    // case 'd': listDimensions(); break;
    case 'c':
        // toggleClearDrawer(body);  // serialClearDrawer(body);
        // break;
    case 's':
        toggleClearDrawer(body);  // serialSetDrawer(body);
        break;
    default:
        Serial.println("e");
        // Serial.readStringUntil('\n');
        // Serial.read();
        break;
    }

    // while (Serial.read() != -1)
    //     ;

    debugoutln("exiting pollSerial");
}
