#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#define ADC_MAX 4096

// Define the pin number you want to read from
const int speedPin = A0; //Speed slider pin
const int selectPin = 27; //Select/RST button pin
const int battPin = 13; //Battery low pin
const int powerPin_out = 15; //Power green LED pin
const int errPin_out = 28; //Error Red LED pin

// Initialize the serial communication
void setupPins() {
  
  Serial.begin(9600);
  
  // Set the specified pin as an input
  pinMode(speedPin, INPUT);
  pinMode(selectPin, INPUT);
  pinMode(powerPin_out, OUTPUT);
  pinMode(errPin_out, OUTPUT);
}

//Returns the value of the set speed pin
int getSpeed(void){
  int in_val = analogRead(speedPin);
  return in_val;//abs(in_val - ADC_MAX/2);
}

//Returns the value of the set select pin
int getSelect(void){
  return digitalRead(selectPin);
}

//Checks whether the battery low circuit has triggered
bool getBattery(){
  return (digitalRead(battPin) == 1);
}

//Sets the error LED. SHOULD BE 1 OR 0.
void setErr(int value){
  digitalWrite(errPin_out, value);
}


