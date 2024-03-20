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

#define REFRESH_PERIOD 150

bool BATTERY_LOW = false; //whether battery is low
bool ERR = false; //error detected
bool STARTUP = true; //whether to show startup screen
int STARTUP_TIME = 2000; //how long to show startup screen for


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
  adc_gpio_init(15);
  adc_select_input(0);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(10, 0);
  digitalWrite(9, 0);		
  setupPins();

  u8g2.begin();
  u8g2.setContrast(65); 
  u8g2.clearDisplay();
  u8g2.setDisplayRotation(U8G2_R2);
  u8g2.setFlipMode(0);

  STARTUP = true; //whether to show startup screen
  STARTUP_TIME = 2000; //how long to show startup screen for
}


void startup(void){
  u8g2_startup_screen();
  STARTUP_TIME = STARTUP_TIME-REFRESH_PERIOD;
  return (STARTUP_TIME-REFRESH_PERIOD > 0);
}

void loop(void) {
  // picture loop  
  u8g2.firstPage();  
  do {

    if (STARTUP){//check if device is starting up
      STARTUP = startup();
    }

    draw();
    //TODO: Get position
    //simulate position changing
    pos = (pos + 1)%100;

    //TODO: Button response here
    BATTERY_LOW = getBattery();
    ERR = false; //get error here
    setErr(ERR);
  } while( u8g2.nextPage());
  
  // delay between each page
  delay(REFRESH_PERIOD);

}