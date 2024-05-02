#include <SPI.h>
#include "printf.h"
#include "RF24.h"

#define CE_PIN 22
#define CSN_PIN 21
// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);
uint8_t address[][6] = { "1Node", "2Node" };
bool radioNumber = 1;  // 0 uses address[0] to transmit, 1 uses address[1] to transmit
bool role = false;  // true = TX role, false = RX role
int payload = 2000000000;

void setupRadio() {
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
  radio.setPALevel(RF24_PA_MAX);  // RF24_PA_MAX is default.
  radio.setPayloadSize(sizeof(payload));  // float datatype occupies 4 bytes
  radio.openWritingPipe(address[radioNumber]); // set the TX address of the RX node into the TX pipe
  radio.openReadingPipe(1, address[!radioNumber]); // set the RX address of the TX node into a RX pipe

  if (role) {
    radio.stopListening();  // put radio in TX mode
  } else {
    radio.startListening();  // put radio in RX mode
  }
}

void RX_TX() {
  if (role) {
    // This device is a TX node
    unsigned long start_timer = micros();                // start the timer
    bool report = radio.write(&payload, sizeof(float));  // transmit & save the report
    unsigned long end_timer = micros();                  // end the timer

    if (report) {
      Serial.print(F("Transmission successful! "));  // payload was delivered
      Serial.print(F("Time to transmit = "));
      Serial.print(end_timer - start_timer);  // print the timer result
      Serial.print(F(" us. Sent: "));
      Serial.println(payload);  // print payload sent
    } else {
      Serial.println(F("Transmission failed or timed out"));  // payload was not delivered
    }
    role = false;
    radio.startListening();
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
      executePayload(payload);
    }
  }  // role

  if (Serial.available()) {
    char c = toupper(Serial.read());
    if (c == 'T' && !role) { // Become the TX node
      createPayload(2, 100);
      //role = true;
      //radio.stopListening();
    }
    if (c == 'Y' && !role) { // Become the TX node
      createPayload(1, 100);
    }
  }
}

void executePayload(int payload) {
  // Decode the Payload
  int command = payload / (int)pow(10, floor(log10(payload)));;
  switch (command) {
    case 1: // Command One
      Serial.println("Juan");
    break;
    case 2: // Command Two
      Serial.println("Fresh Fish");
    break;
  }
}

void createPayload(int command, int data) {
  payload = command * 1000000000;
  role = true;
  radio.stopListening();
}

void setup() {
  setupRadio();
}

void loop() {
  RX_TX();
}  // loop