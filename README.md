# TDP4 Comms team

## Display
To use any display code you must download the modified constructor and place it in the u8g2 library. This is `Documents/Arduino/libraries/u8g2/src/clib` if you're using the Arduino IDE. The following lines must also be included in any code to run properly. The constructor just after inclusion of any libraries `U8G2_ST7586S_YMC240160_F_6800 u8g2(U8G2_R0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11);` and `u8g2.setContrast(65);` in setup just after `u8g2.begin();`.
