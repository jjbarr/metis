// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.
#include <Arduino.h>
#include <Wire.h>

void setup() {
    Wire.begin();  // join i2c bus (address optional for master)
    Serial.begin(9600);
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
    //  x++;
    //  delay(500);
}
