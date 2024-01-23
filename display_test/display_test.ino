#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>

/* Constructor */
U8G2_ST7586S_S028HN118A_F_4W_SW_SPI u8g2(U8G2_R0, 10, 11, 9, 12, 13);

void setup(void) {
  // SPI.setTX(11);
  // SPI.setSCK(10);
  // SPI.setCS(9);
  u8g2.begin();
}

void loop(void) {
  u8g2.clearBuffer();					// clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
  u8g2.drawStr(0,10,"Hello World!");	// write something to the internal memory
  u8g2.sendBuffer();					// transfer internal memory to the display
  delay(1000);  
}
