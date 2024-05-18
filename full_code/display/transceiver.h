#ifndef TRANSCEIVER_H
#define TRANSCEIVER_H

#include <Arduino.h>
#include <SPI.h>
#include "RF24.h"

// Define pins for RF24
#define CE_PIN 22
#define CSN_PIN 21

// External variables
extern RF24 radio;
extern bool role;
extern int payload;
extern int cableLength;
extern int currentMotorData;

// Function declarations and definitions
RF24 radio(CE_PIN, CSN_PIN);
uint8_t address[][6] = {"1Node", "2Node"};
bool radioNumber = 1;
bool role = false;
int payload = 0;
int cableLength = 0;
int currentMotorData = 0;

void setupRadio()
{
    Serial.begin(115200);
    if (!radio.begin())
    {
        Serial.println(F("radio hardware is not responding!!"));
        while (1)
        {
        }
    }
    radio.setPALevel(RF24_PA_MAX);
    radio.setPayloadSize(sizeof(payload));
    radio.openWritingPipe(address[radioNumber]);
    radio.openReadingPipe(1, address[!radioNumber]);

    if (role)
    {
        radio.stopListening();
    }
    else
    {
        radio.startListening();
    }
}

void RX_TX()
{
    if (role)
    {
        unsigned long start_timer = micros();
        bool report = radio.write(&payload, sizeof(payload));
        unsigned long end_timer = micros();

        if (report)
        {
            Serial.print(F("Transmission successful! Time to transmit = "));
            Serial.print(end_timer - start_timer);
            Serial.print(F(" us. Sent: "));
            Serial.println(payload);
        }
        else
        {
            Serial.println(F("Transmission failed or timed out"));
        }
        role = false;
        radio.startListening();
        delay(500);
    }
    else
    {
        uint8_t pipe;
        if (radio.available(&pipe))
        {
            uint8_t bytes = radio.getPayloadSize();
            radio.read(&payload, bytes);
            Serial.println(payload);
            executePayload(payload);
        }
    }

    if (Serial.available())
    {
        char c = toupper(Serial.read());
        if (c == 'T' && !role)
        {
            sendMotorPacket(405, 1, 199);
        }
        else if (c == 'Y' && !role)
        {
            sendCableLength(1000);
        }
        else if (c == 'U' && !role)
        {
            sendMotorPacket(405, 1, 124);
        }
    }
}

void sendCableLength(int length)
{
    int payloadData = length * 10000;
    createPayload(10, payloadData);
}

void sendMotorPacket(int speed, int direction, int distance)
{
    int payloadData = speed * 100000 + direction * 10000 + distance * 10;
    createPayload(11, payloadData);
}

void sendControls(int speed, int direction, int stabilisation)
{
    int payloadData = speed * 10000 + direction * 1000 + stabilisation * 100;
    createPayload(12, payloadData);
}

void stabilisationError()
{
    createPayload(13, 0);
}

void stabilisationOff()
{
    createPayload(14, 0);
}

void executePayload(int payload)
{
    int command = payload / 100000000;
    int data = payload % 100000000;

    switch (command)
    {
    case 10:
        cableLength = data / 10000;
        break;
    case 11:
        currentMotorData = displayMotorData(data);
        break;
    case 12:
        setSpeed(data);
        break;
    case 13:
        Serial.println("STABILISATION ERROR");
        break;
    case 14:
        Serial.println("STABILISATION OFF");
        break;
    default:
        Serial.println("Unknown command");
        break;
    }
}

void createPayload(int command, int data)
{
    payload = command * 100000000 + data;
    role = true;
    radio.stopListening();
}

#endif // TRANSCEIVER_H
