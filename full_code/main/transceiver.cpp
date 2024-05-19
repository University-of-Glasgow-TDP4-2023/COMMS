#include "transceiver.h"

// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);
int payload;
int cableLength;
int current_motor_data;
uint8_t address[][6] = {"1Node", "2Node"};
bool radioNumber = 1; // 0 uses address[0] to transmit, 1 uses address[1] to transmit
bool role = false;    // true = TX role, false = RX role

uint display_position = 0;
int display_direction = 0;
float display_speed = 0;
int display_error = 0;

// MOTOR INTEGRATION
void sendCableLength(int length)
{
    int payloadData = (length * 10000);
    createPayload(10, payloadData);
}

void sendMotorPacket(int speed, int direction, int distance){
    int payloadData = (speed * 100000 + direction * 10000 + distance * 10);
    createPayload(11, payloadData);
}

int displayMotorData(int data) 
{
    float input_speed = data / 100000;
    int input_direction = (data / 10000) % 10;
    int input_distance = (data % 10000) / 10;
    int input_error = data % 10;
    display_speed = input_speed / 100;
    display_position = input_distance;
    display_direction = input_direction;
    display_error = input_error;
    return data;
}

void sendControls(int speed, int direction, int stabilisation)
{
    int payloadData = (speed * 10000 + direction * 10000 + stabilisation * 1000);
    createPayload(12, payloadData);
}

void setSpeed(int data)
{
    int unprocessed_speed = data / 100000 float speed = unprocessed_speed
                                speed = speed / 100 int direction = (data / 10000) % 10 int stabilisation = ((data % 10000) / 1000)
}

void stabilisationError()
{
    createPayload(13, 0);
}

void stabilisationOff()
{
    createPayload(14, 0);
}
// END OF MOTOR INTEGRATION

void executePayload(int payload)
{
    // Decode the Payload
    int command = payload / (int)pow(10, 10 - 2); // Extracts the command
    int data = (payload % (int)pow(10, 10 - 2));  // Extracts the data

    switch (command)
    {
    case 10:
    { // Command One - Cable Length
        cableLength = data / 10000;
        break;
    }
    case 11:
    { // Command Two - Motor Data
        current_motor_data = displayMotorData(data);
        break;
    }
    case 12:
    { // Command Three - Set Speed
        setSpeed(data);
        break;
    }
    }
}

void createPayload(int command, int data)
{
    payload = (command * 100000000) + (data);
    role = true;
    radio.stopListening();
}
