#include <Arduino.h> 
#include <SPI.h> 
#include <U8g2lib.h> 

 

/* Constructor */ 
U8G2_ST7586S_MD240128A6W_1_4W_SW_SPI u8g2(U8G2_R0, 10, 11, 9, 12, 13) 
 

/* u8g2.begin() is required and will sent the setup/init sequence to the display */ 

void setup(void) { 

  u8g2.begin(); 

} 

 

/* draw something on the display with the `firstPage()`/`nextPage()` loop*/ 

void loop(void) { 

  u8g2.firstPage(); 

  do { 

    u8g2.setFont(u8g2_font_ncenB14_tr); 

    u8g2.drawStr(0,20,"Hello World!"); 

  } while ( u8g2.nextPage() ); 

  delay(1000); 

} 