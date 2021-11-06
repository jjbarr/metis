//

#include <../../MetisShelfProtocol.h>
#include <Arduino.h>
#include <Wire.h>

// Cunction Prototype for the I2C peripheral event handler
void onI2CReceive(int);

// --- hardware / pin names ---
#define INDICATOR LED_BUILTIN

int8_t assigned_addr = -1;  // -1 is not a valid addr as the top bit is high

void setup() {
    // this is for dev purposes
    Serial.begin(9600);  // start serial for output
    Serial.print("initing...");

    // --- boot up ---
    // on boot we need to wait a bit as it is possible we were just hot plugged
    // into  a network while we are waitng we turn on the D13 LED
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(INDICATOR, HIGH);
    delay(2000);
    digitalWrite(INDICATOR, LOW);

    // --- I2C main / peripheral handlers / etc ---
    // once we have done that we boot up the I2C peripheral, this also makes it
    // a I2C controller However since this has yet to be assigned an address as
    // it boots as in unknown device
    Wire.begin(UNASSIGNED_DEV_ADDR);  // join i2c bus with address #4
    // register the I2C Handlers
    Wire.onReceive(onI2CReceive);  // register event for incoming i2c writes
    // Wire.onRequest(fnName);

    // test if we can tranmist to ourselves

    // ...
    Serial.println(" done!");
}

void loop() {
    auto msg = Serial.read();
    if (msg != -1) {
        Serial.print((char)msg);

        Wire.beginTransmission(4);  // transmit to device #4
        Wire.write((char)msg);      // sends five bytes
        //  Wire.write(x);              // sends one byte
        Wire.endTransmission();  // stop transmitting
    }
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void onI2CReceive(int nBytesRxed) {
    // while (1 < Wire.available())  // loop through all but the last
    // {
    //     char c = Wire.read();  // receive byte as a character
    //     Serial.print(c);       // print the character
    // }
    int x = Wire.read();    // receive byte as an integer
    Serial.print((char)x);  // print the integer
}