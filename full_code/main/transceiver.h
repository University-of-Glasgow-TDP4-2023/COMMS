#ifndef TRANSCEIVER_H
#define TRANSCEIVER_H

#include <RF24.h>
#include <SPI.h>

extern RF24 radio;
extern bool role;
extern int payload;
extern int cableLength;
extern int current_motor_data;
extern uint8_t address[][6];
extern bool radioNumber;

void setupRadio();
void RX_TX();
void sendCableLength(int length);
void sendMotorPacket(int speed, int direction, int distance);
int displayMotorData(int data);
void sendControls(int speed, int direction, int stabilisation);
void setSpeed(int data);
void stabilisationError();
void stabilisationOff();
void executePayload(int payload);
void createPayload(int command, int data);

#endif
