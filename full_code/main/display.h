#ifndef DISPLAY_H
#define DISPLAY_H

#include <U8g2lib.h>
#include "shapes.h"
#include "subject.h"

extern U8G2_ST7586S_YMC240160_2_6800 u8g2;
extern bool BATTERY_LOW;
extern bool ERR;
extern int flash_count;
extern int display_error;
extern uint display_position;
extern int display_direction;
extern float display_speed;
extern int stabilisation;

void u8g2_prepare(void);
void draw(void);
void rst_ISR();
bool startup(void);

void u8g2_show_error();
void u8g2_callibrating();
void u8g2_not_callibrated(int flash_count);
void u8g2_main_screen(int position, float speed, int direction, int error, int stabilisation, int flash_count);
void u8g2_low_battery();
void u8g2_startup_screen();
bool getBattery();
int getSpeed();
void setErr(bool err);

#endif
