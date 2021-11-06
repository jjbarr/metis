

// This example code is in the public domain.
#include <../../MetisShelfProtocol.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>

// --- prototypes ---
void onShelfPing(int numBytes);
byte readShelfReg(byte shelfAddr, byte regAddr);
void writeShelfReg(byte shelfAddr, byte regAddr, byte value);
void enrollNewShelf();
UID generateNewUID();

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

    Serial.print(generateNewUID());
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

void enrollNewShelf() {
    Serial.println("enrolling shelf");

    UID uid = readShelfUID(UNASSIGNED_SHELF_ADDR);
    Serial.print("uid = ");
    Serial.println(uid, HEX);

    if (uid == 0) {
        UID newuid = generateNewUID();
        Serial.print("assigning new uid 0x");
        Serial.println(newuid, HEX);
        writeShelfUID(UNASSIGNED_SHELF_ADDR, newuid);
    }

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
