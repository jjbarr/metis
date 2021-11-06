

// This example code is in the public domain.
#include <../../MetisShelfProtocol.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>

#define NUMSHELFDEVICES 16

// --- prototypes ---
void onShelfPing(int numBytes);
byte readShelfReg(byte shelfAddr, byte regAddr);
void writeShelfReg(byte shelfAddr, byte regAddr, byte value);
void enrollNewShelf();
UID generateNewUID();

// globals
typedef struct {
    UID uid;
    byte addr;
} ShelfDev;

ShelfDev shelves[NUMSHELFDEVICES];  //= {{0,0}}

// --- utility function ---
byte handledEndTransmission(void) {
    byte worked = Wire.endTransmission(true);
    if (worked) {
        Serial.println("endTransmission failed on with code ");
        Serial.println(worked, HEX);
    } else {
        // Serial.println("handle worked");
    }
    return worked;
}

// --- nitty gritty ---

void setup() {
    Wire.begin(HUB_ADDR);
    Wire.onReceive(onShelfPing);
    Serial.begin(9600);

    Serial.println(" --- starting --- ");
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
        Serial.println("waitng...");
        // delay(1500);
    }
}

// -- impls and tools ---

byte getDevAddrForUID(UID uid) {
    // return of 0xff means it failed

    for (int i = 0; i < NUMSHELFDEVICES; i++) {
        if (shelves[i].uid == uid) {
            return shelves[i].addr;
        }
    }
    // not found
    return 0xff;
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
    // maps to Serial.print(...) / Serial.println(...)
    Wire.beginTransmission(shelfAddr);
    Wire.write(regAddr);
    handledEndTransmission();

    Wire.requestFrom((uint8_t)shelfAddr, (byte)1);
    return Wire.read();
}

void writeShelfReg(byte shelfAddr, byte regAddr, byte value) {
    Wire.beginTransmission(shelfAddr);
    byte msg[] = {regAddr, value};
    Wire.write((char*)&msg, 2);
    handledEndTransmission();
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
        Serial.print("uid write failed, wrote 0x");
        Serial.println(id, HEX);
        Serial.print(" but read back 0x");
        Serial.println(readbackID, HEX);
    }
}

byte next_shelf_addr = ASSIGNED_SHELF_BASE_ADDR;
void enrollNewShelf() {
    Serial.println("enrolling shelf");

    // check uid
    UID uid = readShelfUID(UNASSIGNED_SHELF_ADDR);
    Serial.print("uid = ");
    Serial.println(uid, HEX);

    if (uid == 0) {
        UID newuid = generateNewUID();
        Serial.print("assigning new uid 0x");
        Serial.println(newuid, HEX);
        writeShelfUID(UNASSIGNED_SHELF_ADDR, newuid);
        uid = newuid;
    }

    // check i2c address
    byte addr = next_shelf_addr;
    next_shelf_addr += 1;
    setDevAddrForUID(uid, addr);
    writeShelfReg(UNASSIGNED_SHELF_ADDR, SHELF_DEV_ADDR, addr);

    delay(100);

        Serial.println("Done enrolling shelf");
}

void onShelfPing(int numBytes) {

    // check I2C input
    // TODO: put this into a function
    if (Wire.available() != 1) {
        Serial.print("enroll requested ");
        Serial.print(Wire.available());
        Serial.print(", invalid request size");
        return;
    }

    byte cmd = Wire.read();

    if (cmd != HUB_ENROL_REQ) {
        Serial.print("unknown command 0x");
        Serial.println(cmd, HEX);
        return;
    }

    Serial.println("queueing enroll in todo");

    todo |= TODO_ENROLL_SHELF;
}
