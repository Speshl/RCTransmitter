#include <Arduino.h>
#include <SPI.h>
#include "RF24.h"

#define PACKET_SIZE 7

RF24 radio(16, 5);  // using pin 16 for the CE pin, and pin 5 for the CSN pin
// Let these addresses be used for the pair
uint8_t address[][6] = { "1Node", "2Node" };
// It is very helpful to think of an address as a path instead of as
// an identifying device destination

// to use different addresses on a pair of radios, we need a variable to
// uniquely identify which address this radio will use to transmit
bool radioNumber = 0;  // 0 uses address[0] to transmit, 1 uses address[1] to transmit

// Used to control whether this node is sending or receiving
bool role = true;  // true = TX role, false = RX role

void setup() {
    Serial.begin(115200);
    //Serial.println("Setting Up...");
    
    if (!radio.begin()) {
        Serial.println(F("radio hardware is not responding!!"));
        while (1) {}  // hold in infinite loop
    }
    radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.
    radio.setPayloadSize(PACKET_SIZE);
    // set the TX address of the RX node into the TX pipe
    radio.openWritingPipe(address[radioNumber]);  // always uses pipe 0

    // set the RX address of the TX node into a RX pipe
    radio.openReadingPipe(1, address[!radioNumber]);  // using pipe 1

    if (role) {
        radio.stopListening();  // put radio in TX mode
    } else {
        radio.startListening();  // put radio in RX mode
    }
}

void loop() {
    uint8_t buffer[PACKET_SIZE];
    if(Serial.available() >= PACKET_SIZE){
        Serial.readBytes(buffer, PACKET_SIZE);
        /*buffer[0] = 127;
        buffer[1] = 126;
        buffer[2] = 125;
        buffer[3] = 124;
        buffer[4] = 123;
        buffer[5] = 122;
        buffer[6] = 121;*/

        //Serial.print("Got Bytes: ");
        for(int i=0; i<PACKET_SIZE; i++){
            Serial.print(buffer[i], DEC);
        }
        //Serial.println("");
        bool report = radio.write(buffer, 7);  // transmit & save the report
        if(!report){
            //Serial.println("TX Failed");
        }
    }
}