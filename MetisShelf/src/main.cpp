

#include <../../MetisShelfProtocol.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include <drawer.h>

// prototypes for code organization
void setup_peripherals(void);

byte readReg(byte regAddr);
void writeReg(byte regAddr, byte value);

bool uidIsValid(void);
UID getUID(void);
byte getUIDByte(int);
void setUIDByte(int, byte);

void writeDrawer(Drawer* drawer, byte col, byte row, bool onOff);

// --- hardware / pin names ---
#define WAKE_DELAY 500
#define INDICATOR LED_BUILTIN
#define CLEAR_UID_ON_BOOT 0

// --- register globals ---
byte selectedCol = 0x11;
byte selectedRow = 0x12;
byte setRegAddr  = 0x13;

Drawer drawer;

// --- the nitty gritty ---
void setup() {
    setup_peripherals();
    drawer_clearstate(&drawer);

    // dubug purposes
    Serial.print("UID: ");
    Serial.println(getUID(), HEX);

    // send an addr request to master
    Serial.println("requesting enroll...");
    Wire.beginTransmission(HUB_ADDR);
    Wire.write(HUB_ENROL_REQ);
    Wire.endTransmission(true);
    Serial.println("... done");

    writeDrawer(&drawer, 0, 0, true);
}

void loop() {
    delay(100);
}

// --- impls and utils ---

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void onI2CWrite(int nBytesRxed) {
    if (Wire.available() == 1) {
        setRegAddr = Wire.read();

        // Serial.print("changing target/dest addr to ");
        // Serial.println(setRegAddr, HEX);
    } else {
        byte regAddr = Wire.read();
        byte value   = Wire.read();

        writeReg(regAddr, value);

        // Serial.print("told to write value ");
        // Serial.print(value);
        // Serial.print(" into reg ");
        // Serial.println(regAddr, HEX);
    }
    // int x = Wire.read();         // receive byte as an integer
    // Serial.print((char)x, HEX);  // print the integer
}

void onI2CRead() {
    // if (Wire.available() != 1) {
    //     Serial.print("all the read requests should be 1 byte long, got ");
    //     Serial.print(Wire.available());
    //     Serial.println(" bytes.");
    // }
    // the addr they are requesting
    Wire.write(readReg(setRegAddr));
}

void setDeviceAddress(byte addr) {

    Wire.begin(addr);
    // register the I2C Handlers
    Wire.onReceive(onI2CWrite);
    Wire.onRequest(onI2CRead);
    Serial.print("device address set to ");
    Serial.println(addr, HEX);
}

byte readDrawerStatus(int x) {
    return drawer_isset(&drawer, selectedRow, selectedCol);
}

byte readReg(byte regAddr) {
    switch (regAddr) {

    case SHELF_UID_B0: return getUIDByte(0);
    case SHELF_UID_B1: return getUIDByte(1);
    case SHELF_UID_B2: return getUIDByte(2);
    case SHELF_UID_B3: return getUIDByte(3);
    case SHELF_DRAWERS_WIDE: return D_WIDTH;
    case SHELF_DRAWERS_HIGH: return D_HEIGHT;
    case SHELF_SEL_COL: return selectedCol;
    case SHELF_SEL_ROW: return selectedRow;
    case SHELF_DRAWER_STAT:
        return readDrawerStatus((selectedCol << 8) | selectedRow);
    default: {
        Serial.print("unknown reg read to ");
        Serial.println(regAddr, HEX);
    };
    }
}

void writeReg(byte regAddr, byte value) {
    switch (regAddr) {
    case SHELF_UID_B0: setUIDByte(0, value); break;
    case SHELF_UID_B1: setUIDByte(1, value); break;
    case SHELF_UID_B2: setUIDByte(2, value); break;
    case SHELF_UID_B3: setUIDByte(3, value); break;

    case SHELF_DEV_ADDR: setDeviceAddress(value); break;
    case SHELF_SEL_COL: selectedCol = value; break;
    case SHELF_SEL_ROW: selectedRow = value; break;
    case SHELF_DRAWER_STAT:
        writeDrawer(&drawer, selectedCol, selectedRow, (bool)value);
        break;
    case SHELF_CMD: {
        // TODO: make this do things.
        Serial.print("turn all leds to ");
        Serial.println(value);
        break;
    }
    default: {
        Serial.print("unknown reg write to ");
        Serial.print(regAddr, HEX);
        Serial.print(" of value ");
        Serial.println(value, HEX);
    };
    }
}

void setup_peripherals(void) {
    // this is for dev purposes
    Serial.begin(9600);  // start serial for output
    Serial.println("initing peripherals...");

    drawer_init();

    // --- boot up ---
    // on boot we need to wait a bit as it is possible we were just hot plugged
    // into  a network while we are waitng we turn on the D13 LED
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(INDICATOR, HIGH);
    delay(WAKE_DELAY);
    digitalWrite(INDICATOR, LOW);

    // --- I2C main / peripheral handlers / etc ---
    // once we have done that we boot up the I2C peripheral, this also makes it
    // a I2C controller However since this has yet to be assigned an address as
    // it boots as in unknown device
    setDeviceAddress(UNASSIGNED_SHELF_ADDR);

    EEPROM.begin();

    // ...
    Serial.println("... done!");

    UID uid = getUID();
    if (CLEAR_UID_ON_BOOT || uid == 0xFFFFFFFF) {
        Serial.println("clearing UID");
        setUIDByte(0, 0);
        setUIDByte(1, 0);
        setUIDByte(2, 0);
        setUIDByte(3, 0);
    }
}

bool uidIsValid(void) {
    return (bool)EEPROM[0];
}

UID getUID(void) {
    return ((EEPROM[3] << 24) | (EEPROM[2] << 16)) |
           ((EEPROM[1] << 8) | (EEPROM[0] << 0));
}

byte getUIDByte(int addr) {
    /// -1 return is means invalid addr requested, but just don't do that please
    if (addr < 0 || addr > 3) {
        return -1;
    } else {
        return EEPROM[addr];
    }
}

void setUIDByte(int byteIndex, byte value) {
    if (byteIndex < 0 || byteIndex > 3) {
        Serial.print("invalid UID byte addr, tried to write to");
        Serial.println(byteIndex);
        return;
    } else {
        EEPROM.write(byteIndex, value);
    }
}

void writeDrawer(Drawer* drawer_ref, byte col, byte row, bool onOff) {
    Serial.print("set led ");
    Serial.print(col);
    Serial.print(" over and ");
    Serial.print(row);
    Serial.print(" down ");
    if (onOff) {
        drawer_set(drawer_ref, row, col);
        Serial.println("on");

    } else {
        drawer_clear(drawer_ref, row, col);
        Serial.println("off");
    }

    drawer_show(drawer_ref);
}