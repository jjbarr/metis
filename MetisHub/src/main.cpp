



// This example code is in the public domain.
#include <../../MetisShelfProtocol.h>
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
