#include <Arduino.h>
#include <SPI.h>
#include "RF24.h"

#define PACKET_SIZE 4
#define LED 2

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
    pinMode(LED,OUTPUT);
    Serial.begin(115200);
    //Serial.println("Setting Up...");
    
    if (!radio.begin()) {
        Serial.println(F("radio hardware is not responding!!"));
        while (1) {}  // hold in infinite loop
    }
    radio.setPALevel(RF24_PA_MAX);  // RF24_PA_MAX is default.
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

int steerPos = 90;
bool direction = false;

void loop() {
    uint8_t buffer[PACKET_SIZE];

    /*if(direction){
        steerPos++;
    }else{
        steerPos--;
    }

    if(steerPos >= 180 || steerPos <= 0){
        direction = !direction;
    }

    buffer[0] = steerPos;
    buffer[1] = 90;
    buffer[2] = 0;
    buffer[3] = 0;

    Serial.print("Steer: ");
    Serial.println(buffer[0]);*/

    if(Serial.available() >= PACKET_SIZE+2){
        digitalWrite(LED,HIGH);
        if(Serial.read() != 255){ //expect first byte of command to be 255
            return;
        }

        if(Serial.read() != 127){//expect second byte of command to be 127
            return;
        }
        Serial.readBytes(buffer, PACKET_SIZE);
        bool report = radio.write(buffer, PACKET_SIZE);  // transmit & save the report
        if(!report){
            Serial.println("TX Failed");
        }
        digitalWrite(LED,LOW);
    }else{
        Serial.println("No Serial Available");
        buffer[0] = 90;
        buffer[1] = 90;
        buffer[2] = 0;
        buffer[3] = 100;
        bool report = radio.write(buffer, PACKET_SIZE);  // transmit & save the report
        if(!report){
            Serial.println("TX Failed");
        }
    }
    //delay(500);
}