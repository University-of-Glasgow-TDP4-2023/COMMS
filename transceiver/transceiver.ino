#include <SPI.h>
#include "printf.h"
#include "RF24.h"

#define CE_PIN 22
#define CSN_PIN 21
RF24 radio(CE_PIN, CSN_PIN); // instantiate an object for the nRF24L01 transceiver

// Header File
enum  Commands {
  SPEED,
  CALLIBRATION_ERROR,
  poopoo
};

typedef struct Packet {
  Commands command; // 2 Digit Command
  int data; // 4 Digit Data
  int crc; // 4 Digit CRC
} Packet;

// End of Header File

uint8_t address[][6] = { "1Node", "2Node" }; // Let these addresses be used for the pair
bool radioNumber = 1;  // 0 uses address[0] to transmit, 1 uses address[1] to transmit
bool role = false;  // true = TX role, false = RX role

int payload = 1000000000;
Packet packet;

// Packet Format:
// 0 - 1: Command
// 2 - 5: Data
// 6 - 9: CRC

// switch (com) {
//   case SPEED {
//     break
//   }
// }

void setup() {
  packet.command = poopoo;
  packet.data = 2;
  packet.crc = 10;

  Serial.begin(115200);
  while (!Serial) {
    // some boards need to wait to ensure access to serial over USB
  }
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }
  Serial.println(F("Which radio is this? Enter '0' or '1'. Defaults to '0'"));
  while (!Serial.available()) {
    // wait for user input
  }
  char input = Serial.parseInt();
  radioNumber = input == 1;
  Serial.print(F("radioNumber = "));
  Serial.println((int)radioNumber);
  Serial.println(F("*** PRESS 'T' to begin transmitting to the other node"));

  radio.setPALevel(RF24_PA_MAX);  // Power - RF24_PA_MAX is default.
  radio.setPayloadSize(sizeof(Packet));  // float datatype occupies 4 bytes

  radio.openWritingPipe(address[radioNumber]);  // always uses pipe 0
  radio.openReadingPipe(1, address[!radioNumber]);  // using pipe 1

  if (role) {
    radio.stopListening();  // put radio in TX mode
  } else {
    radio.startListening();  // put radio in RX mode
  }

}  // setup

void loop() {

  if (role) {
    // This device is a TX node
    unsigned long start_timer = micros();                // start the timer
    bool report = radio.write((char*)&packet, sizeof(Packet));  // transmit & save the report
    Serial.println(radio.getPayloadSize());
    unsigned long end_timer = micros();                  // end the timer

    if (report) {
      Serial.print(F("Transmission successful! "));  // payload was delivered
      Serial.print(F("Time to transmit = "));
      Serial.print(end_timer - start_timer);  // print the timer result
      Serial.print(F(" us. Sent: "));
      Serial.println(int(((char*)&packet)[0]));  // print payload sent
      Serial.println(int(((char*)&packet)[1]));  // print payload sent
      Serial.println(int(((char*)&packet)[2]));  // print payload sent
      Serial.println(int(((char*)&packet)[3]));  // print payload sent
      Serial.println(int(((char*)&packet)[4]));  // print payload sent
      Serial.println(int(((char*)&packet)[5]));  // print payload sent
      Serial.println(int(((char*)&packet)[6]));  // print payload sent
      Serial.println(int(((char*)&packet)[7]));  // print payload sent
      Serial.println(int(((char*)&packet)[8]));  // print payload sent
      Serial.println(int(((char*)&packet)[9]));  // print payload sent
      Serial.println(int(((char*)&packet)[10]));  // print payload sent
      Serial.println(int(((char*)&packet)[11]));  // print payload sent
    } else {
      Serial.println(F("Transmission failed or timed out"));  // payload was not delivered
    }
    role = false;
    radio.startListening();
    // to make this example readable in the serial monitor
    delay(1000);  // slow transmissions down by 1 second

  } else {
    // This device is a RX node
    uint8_t pipe;
    if (radio.available(&pipe)) {              // is there a payload? get the pipe number that recieved it
      uint8_t bytes = radio.getPayloadSize();  // get the size of the payload
      radio.read(&payload, bytes);             // fetch payload from FIFO
      Serial.print(F("Received "));
      Serial.print(bytes);  // print the size of the payload
      Serial.print(F(" bytes on pipe "));
      Serial.print(pipe);  // print the pipe number
      Serial.print(F(": "));
      Serial.println(payload);  // print the payload's value
    }
  }  // role

  if (Serial.available()) {
    // change the role via the serial monitor
    char c = toupper(Serial.read());
    if (c == 'T' && !role) {
      // Become the TX node

      role = true;
      radio.stopListening();

    } 
  }
}  // loop