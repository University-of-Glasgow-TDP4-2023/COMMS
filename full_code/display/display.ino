#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif


#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "shapes.h"
#include "subject.h"
#include "Timer.h"
// #include "transceiver.h"

#define REFRESH_PERIOD 150

bool BATTERY_LOW = false; //whether battery is low
bool ERR = false; //error detected
bool STARTUP = true; //whether to show startup screen
int STARTUP_TIME = 50000; //how long to show startup screen for
int buttonState = 0;  // variable for reading the pushbutton status
int buttonValue = analogRead(buttonPin);
int direction;
int stabilisation = 1; // Stabilisation ON is 1
Timer timer;


volatile int rstState = 0; 

// START OF RF Code
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
int payload = 0;
int cableLength = 0;
int current_motor_data = 0;
int count = 0;
int off_count = 0;

void setupRadio() {
  Serial.begin(115200);
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }

  // Serial.println(F("Which radio is this? Enter '0' or '1'. Defaults to '0'"));
  // while (!Serial.available()) {
  //   // wait for user input
  // }

  // char input = Serial.parseInt();
  // radioNumber = input == 1;
  radioNumber = 1;
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
      Serial.println(payload);  // print the payload's value
      executePayload(payload);
    }
  }  // role

  if (Serial.available()) {
    char c = toupper(Serial.read());
    if (c == 'T' && !role) { // Become the TX node
      sendMotorPacket(405,1,199);
    }
    if (c == 'Y' && !role) { // Become the TX node
      sendCableLength(1000);
    }
    if (c == 'U' && !role) { // Become the TX node
      sendMotorPacket(405,1,124);
    }
    // if (c == 'I' && !role) { // Become the TX node
    //   sta;
    // }
  }
}

// MOTOR INTEGRATION
void sendCableLength(int length) {
  int payloadData = (length * 10000);
  createPayload(10,payloadData);
}
void sendMotorPacket(int speed, int direction, int distance) {
  int payloadData = (speed * 100000 + direction * 10000 + distance * 10);
  createPayload(11,payloadData);
}
int displayMotorData(int data) {
  int unprocessed_speed = data / 100000;
  float speed = unprocessed_speed;
  speed = speed / 100;
  int direction = (data / 10000) % 10;
  int distance = ((data % 10000)/10)-100;
  int error = (data % 10);
  Serial.println(speed);
  Serial.println(direction);
  Serial.println(distance);
  Serial.println(error);
  return data;
}
int getData(){
  return current_motor_data;
}
void sendControls(int speed, int direction, int stabilisation) {
  int payloadData = (speed * 10000 + direction * 1000 + stabilisation * 100);
  createPayload(12,payloadData);
}
void setSpeed(int data) {
  int unprocessed_speed = data / 10000;
  float speed = unprocessed_speed;
  speed = speed / 100;
  int direction = (data / 1000) % 10;
  Serial.println(unprocessed_speed);
  Serial.println(direction);
  Serial.println(direction);
}
void stabilisationError() {
  createPayload(13, 0);
}
void stabilisationOff() {
  createPayload(14, 0);
}
// END OF MOTOR INTEGRATION

void executePayload(int payload) {
  // Decode the Payload
  int command = payload / (int)pow(10, 10 - 2);
  int data = (payload % (int)pow(10, 10 - 2));
  
  switch (command) {
    case 10: { // Command One - Cable Length
      cableLength = data/10000;
    break; }
    case 11: { // Command Two - Motor Data
      current_motor_data = displayMotorData(data);
    break; }
    case 12: { // Command Three - Set Speed
      setSpeed(data);
    break; }
    case 13: {
      Serial.println("STABILSATION ERROR");
    break; }
    case 14: {
      // Print Stabilisation Error
    break; }
  }
}
void createPayload(int command, int data) {
  payload = (command * 100000000)+(data);
  role = true;
  radio.stopListening();
}
// END OF RF Code

// START OF DISPLAY CODE

// Set Display Options
void u8g2_prepare(void)
{
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

uint8_t draw_state = 0;
uint pos = 0;
int dir = 0;

void draw(void) {
  u8g2_prepare();
  if (ERR){
    u8g2_show_error();
    return;
  }
  u8g2_main_screen(pos, getSpeed()-25, dir);
  if (BATTERY_LOW){
    u8g2_low_battery();
  }
}

// ISR Response
void rst_ISR()
{
  u8g2_show_error();
}


// END OF DISPLAY CODE

const char *getErrorMessage(int errorNumber)
{
  // Array of error messages
  static const char *errorMessages[] = {
      "No error",
      "WARNING! Controller power error",
      "WARNING! Main power error",
      "WARNING! Stabilisation error",
      "WARNING! Out of range error"};

  // Number of errors in the array
  int numErrors = sizeof(errorMessages) / sizeof(errorMessages[0]);

  // Check if the error number is within the valid range
  if (errorNumber < 0 || errorNumber >= numErrors)
  {
    return "Invalid error number";
  }

  // Return the corresponding error message
  return errorMessages[errorNumber];
}

void setup(void) {

  adc_init();
  adc_gpio_init(26);
  adc_select_input(0);
  pinMode(2, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(2, 0);
  digitalWrite(9, 0);		
  attachInterrupt(27, rst_ISR, FALLING);
  setupPins();

  u8g2.begin();
  u8g2.setContrast(65); 
  u8g2.clearDisplay();
  u8g2.setDisplayRotation(U8G2_R2);
  u8g2.setFlipMode(0);

  STARTUP = true; //whether to show startup screen
  STARTUP_TIME = 2000; //how long to show startup screen for

  setupRadio();
  gpio_set_irq_enabled_with_callback(buttonPin, GPIO_IRQ_EDGE_RISE, true, &buttonCallback);
  gpio_set_irq_enabled_with_callback(buttonPin, GPIO_IRQ_EDGE_FALL, true, &buttonCallback);
}


bool startup(void){
  u8g2_startup_screen();
  STARTUP_TIME = STARTUP_TIME-REFRESH_PERIOD;
  return (STARTUP_TIME-REFRESH_PERIOD > 0);
}

void buttonCallback(uint gpio, uint32_t events) {
  count += 1;
  if (count == 1) {
    timer.start();
  }
  if (count == 2) {
    timer.stop();
    int buttom_time = timer.read();
    if (buttom_time >= 2000) {
      stabilisation = !stabilisation;
      Serial.println("STABILSATION OFF");
    }
    else {
      direction = !direction;
    }
    count = 0;
  }
}

void loop(void) {
  // picture loop  
  u8g2.firstPage();
  sendControls(getSpeed(), direction, stabilisation);
  do {
    RX_TX();
    if (STARTUP){//check if device is starting up
      STARTUP = startup();
    }else{
      draw();
    }
    //Get position

    int data = getData();
    buttonState = digitalRead(buttonPin);
    // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
    int unprocessed_speed = data / 100000;
    float speed = unprocessed_speed;
    speed = speed / 100;
    int direction = (data / 10000) % 10;
    int distance = ((data % 10000)/10)-100;
    pos = distance;
    dir = direction;


    // End button response
    BATTERY_LOW = getBattery();

    //Error response

    ERR = false; //get error here
    int err_int = 0;
    if (ERR){
      setErr(ERR);
      u8g2_show_error(getErrorMessage(err_int));
    }
    


  } while( u8g2.nextPage());
  
  // delay between each page
  delay(REFRESH_PERIOD);
}
