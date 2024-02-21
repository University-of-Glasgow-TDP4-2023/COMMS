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

void u8g2_prepare(void) {
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

uint8_t draw_state = 0;

void draw(void) {
  u8g2_prepare();
  switch(draw_state >> 3) {
    case 0: u8g2_box_title(draw_state&7); break;
    case 1: u8g2_box_frame(draw_state&7); break;
    case 2: u8g2_disc_circle(draw_state&7); break;
    case 3: u8g2_r_frame(draw_state&7); break;
    case 4: u8g2_string(draw_state&7); break;
    case 5: u8g2_line(draw_state&7); break;
    case 6: u8g2_triangle(draw_state&7); break;
    case 7: u8g2_ascii_1(); break;
    case 8: u8g2_ascii_2(); break;
    case 9: u8g2_extra_page(draw_state&7); break;
    case 10: u8g2_xor(draw_state&7); break;
    case 11: u8g2_bitmap_modes(0); break;
    case 12: u8g2_bitmap_modes(1); break;
    case 13: u8g2_bitmap_overlay(draw_state&7); break;
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

  u8g2.begin();
  u8g2.setContrast(65); 
  u8g2.clearDisplay();
  u8g2.setDisplayRotation(U8G2_R2);
  u8g2.setFlipMode(0);
}

void loop(void) {
  // picture loop  
  u8g2.firstPage();  
  do {
    draw();
  } while( u8g2.nextPage() );

  uint16_t reading = adc_read();
  if (reading == 3.3) {
    x = 50;
  }

  x = x + 10;
  if (x > 232) {
    x = 7;
  }
  if ( draw_state >= 14*8 )
    draw_state = 0;

  // delay between each page
  delay(150);

}