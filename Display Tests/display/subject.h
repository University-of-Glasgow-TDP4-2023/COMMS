#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

// Define the pin number you want to read from
const int speedPin = A3; 
const int selectPin = 27;
const int powerPin_out = 12;
const int greenPin_out = 26;
//const int errPin_out = 20;

int speedValue;
int selectValue;

void setupPins() {
  // Initialize the serial communication
  Serial.begin(9600);
  
  // Set the specified pin as an input
  pinMode(speedPin, INPUT);
  pinMode(selectPin, INPUT);
  pinMode(powerPin_out, OUTPUT);
  pinMode(greenPin_out, OUTPUT);
  //pinMode(errPin_out, OUTPUT);
}

void read() {
  // Read the value from the digital input pin
  speedValue = analogRead(speedPin);
  selectValue = digitalRead(selectPin);
}

int getSpeed(){
  return speedValue;
}

void loopRead() {
  

  // Print the read value to the Serial Monitor
  Serial.print("IO Pin Value: ");
  Serial.println(speedPin);

  // Add a delay to make the output readable
  delay(1000);
}
