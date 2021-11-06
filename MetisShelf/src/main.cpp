

#include <../../MetisShelfProtocol.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>

// prototypes for code organization
void setup_peripherals(void);
bool uidIsValid(void);
UID getUID(void);
int8_t getUIDByte(int);
void setUIDByte(int, uint8_t);
uint8_t readReg(uint8_t);

typedef int Drawer;
void writeDrawer(Drawer* drawer, uint8_t col, uint8_t row, bool onOff);

// --- hardware / pin names ---
#define WAKE_DELAY 100
#define INDICATOR LED_BUILTIN
#define NUM_DRAWERS_WIDE 5
#define NUM_DRAWERS_HIGH 4
#define CLEAR_UID_ON_BOOT 0

// --- register globals ---
uint8_t selectedCol = 0;
uint8_t selectedRow = 0;

// --- the nitty gritty ---
void setup() {
    setup_peripherals();

    // dubug purposes
    Serial.print("UID: ");
    Serial.println(getUID());

    // send an addr request to master
    Wire.beginTransmission(HUB_ADDR);
    Wire.write(HUB_ENROL_REQ);
    Wire.endTransmission();
}

void loop() {
    delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void onI2CWrite(int nBytesRxed) {
    int x = Wire.read();    // receive byte as an integer
    Serial.print((char)x);  // print the integer
}

void onI2CRead() {
    if (Wire.available() != 1) {
        Serial.print("all the read requests should be 1 byte long, got ");
        Serial.print(Wire.available());
        Serial.println(" bytes.");
    }
    // the addr they are requesting
    uint8_t regAddr = Wire.read();
    Wire.write(readReg(regAddr));
}

void setDeviceAddress(uint8_t addr) {

    Wire.begin(addr);
    // register the I2C Handlers
    Wire.onReceive(onI2CWrite);
    Wire.onRequest(onI2CRead);
    Serial.print("device address set to ");
    Serial.println(addr);
}

uint8_t readDrawerStatus(int x) {
    return 69;
}

uint8_t readReg(uint8_t regAddr) {
    switch (regAddr) {

    case SHELF_UID_B0: return getUIDByte(0);
    case SHELF_UID_B1: return getUIDByte(1);
    case SHELF_UID_B2: return getUIDByte(2);
    case SHELF_UID_B3: return getUIDByte(4);
    case SHELF_DRAWERS_WIDE: return NUM_DRAWERS_WIDE;
    case SHELF_DRAWERS_HIGH: return NUM_DRAWERS_HIGH;
    case SHELF_SEL_COL: return selectedCol;
    case SHELF_SEL_ROW: return selectedRow;
    case SHELF_DRAWER_STAT:
        return readDrawerStatus((selectedCol << 8) | selectedRow);
    default: break;
    }
}

void writeReg(uint8_t regAddr, uint8_t value) {
    int drawer = 0;
    switch (regAddr) {
    case SHELF_UID_B0: setUIDByte(0, value);
    case SHELF_UID_B1: setUIDByte(1, value);
    case SHELF_UID_B2: setUIDByte(2, value);
    case SHELF_UID_B3: setUIDByte(3, value);

    case SHELF_I2C_ADDR: setDeviceAddress(value);
    case SHELF_SEL_COL: selectedCol = value;
    case SHELF_SEL_ROW: selectedRow = value;
    case SHELF_DRAWER_STAT:
        writeDrawer(&drawer, selectedCol, selectedRow, (bool)value);
    case SHELF_CMD: {
        Serial.print("turn all leds to ");
        Serial.println(value);
    }
    default: break;
    }
}

void setup_peripherals(void) {
    // this is for dev purposes
    Serial.begin(9600);  // start serial for output
    Serial.print("initing peripherals...");

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
    Serial.println(" done!");

    if (CLEAR_UID_ON_BOOT) {
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

int8_t getUIDByte(int addr) {
    /// -1 return is means invalid addr requested, but just don't do that please
    if (addr < 0 || addr > 3) {
        return -1;
    } else {
        return EEPROM[addr];
    }
}

void setUIDByte(int byteIndex, uint8_t value) {
    if (byteIndex < 0 || byteIndex > 3) {
        Serial.print("invalid UID byte addr, tried to write to");
        Serial.println(byteIndex);
        return;
    } else {
        EEPROM.write(byteIndex, value);
    }
}

void writeDrawer(Drawer* drawer, uint8_t col, uint8_t row, bool onOff) {
    Serial.print("set led ");
    Serial.print(col);
    Serial.print(" over and ");
    Serial.print(row);
    Serial.print(" down ");
    if (onOff)
        Serial.println("on");
    else
        Serial.println("off");
}