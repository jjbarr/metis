//

#include <Arduino.h>
#include <Wire.h>

// Cunction Prototype for the I2C peripheral event handler
void receiveEvent(int howMany);

#define INDICATOR LED_BUILTIN
#define UNASSIGNED_DEV_ADDR 4

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

    // --- I2C peripheral handlers / etc ---
    // once we have done that we boot up the I2C peripheral.
    // However since this has yet to be assigned an address it boots as
    Wire.begin(UNASSIGNED_DEV_ADDR);  // join i2c bus with address #4
    // register the I2C Handlers
    Wire.onReceive(receiveEvent);  // register event for incoming i2c writes
    // Wire.onRequest(nf_name);

    // --- I2C main ---
    // start it as a main too!
    Wire.begin();

    Wire.beginTransmission(4);
    Wire.write("hello me!\n");
    Wire.endTransmission();

    // ...
    Serial.println(" done!");
}

void loop() {
    delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
    // while (1 < Wire.available())  // loop through all but the last
    // {
    //     char c = Wire.read();  // receive byte as a character
    //     Serial.print(c);       // print the character
    // }
    int x = Wire.read();    // receive byte as an integer
    Serial.print((char)x);  // print the integer
}