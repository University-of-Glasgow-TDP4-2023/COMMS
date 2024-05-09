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
#include "shapes.h"
#include "subject.h"
#include "transceiver.h"

#define REFRESH_PERIOD 150

bool BATTERY_LOW = false; //whether battery is low
bool ERR = false; //error detected
bool STARTUP = true; //whether to show startup screen
int STARTUP_TIME = 50000; //how long to show startup screen for


volatile int rstState = 0; 

void rst_ISR(){
  u8g2_show_error();
}

void u8g2_prepare(void) {
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

uint8_t draw_state = 0;
uint pos = 0;

void draw(void) {
  u8g2_prepare();
  if (ERR){
    u8g2_show_error();
    return;
  }
  u8g2_main_screen(pos, getSpeed());
  if (BATTERY_LOW){
    u8g2_low_battery();
  }
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
}


bool startup(void){
  u8g2_startup_screen();
  STARTUP_TIME = STARTUP_TIME-REFRESH_PERIOD;
  return (STARTUP_TIME-REFRESH_PERIOD > 0);
}

void loop(void) {
  // picture loop  
  u8g2.firstPage();  
  do {

    RX_TX();

    if (STARTUP){//check if device is starting up
      STARTUP = startup();
    }else{
      draw();
    }
    //TODO: Get position#
    int data = getData()
    int unprocessed_speed = data / 100000;
    float speed = unprocessed_speed;
    speed = speed / 100;
    int direction = (data / 10000) % 10;
    int distance = ((data % 10000)/10)-100;
    pos = distance
    //simulate position changing
    //pos = (pos + 1)%100;

    //TODO: Button response here
    BATTERY_LOW = getBattery();
    ERR = false; //get error here
    setErr(ERR);


  } while( u8g2.nextPage());
  
  // delay between each page
  delay(REFRESH_PERIOD);

}
