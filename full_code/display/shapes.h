#ifndef SHAPES_H
#define SHAPES_H

#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define STARTER_TEXT "UofG Electronics Team Project 4:"

#define CROSS_WIDTH 24
#define CROSS_HEIGHT 24
static const unsigned char CROSS_BITS[] U8X8_PROGMEM = {
    0x00, 0x18, 0x00, 0x00, 0x24, 0x00, 0x00, 0x24, 0x00, 0x00, 0x42, 0x00,
    0x00, 0x42, 0x00, 0x00, 0x42, 0x00, 0x00, 0x81, 0x00, 0x00, 0x81, 0x00,
    0xC0, 0x00, 0x03, 0x38, 0x3C, 0x1C, 0x06, 0x42, 0x60, 0x01, 0x42, 0x80,
    0x01, 0x42, 0x80, 0x06, 0x42, 0x60, 0x38, 0x3C, 0x1C, 0xC0, 0x00, 0x03,
    0x00, 0x81, 0x00, 0x00, 0x81, 0x00, 0x00, 0x42, 0x00, 0x00, 0x42, 0x00,
    0x00, 0x42, 0x00, 0x00, 0x24, 0x00, 0x00, 0x24, 0x00, 0x00, 0x18, 0x00};

#define CROSS_FILL_WIDTH 24
#define CROSS_FILL_HEIGHT 24
static const unsigned char CROSS_FILL_BITS[] U8X8_PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x18, 0x64, 0x00, 0x26,
    0x84, 0x00, 0x21, 0x08, 0x81, 0x10, 0x08, 0x42, 0x10, 0x10, 0x3C, 0x08,
    0x20, 0x00, 0x04, 0x40, 0x00, 0x02, 0x80, 0x00, 0x01, 0x80, 0x18, 0x01,
    0x80, 0x18, 0x01, 0x80, 0x00, 0x01, 0x40, 0x00, 0x02, 0x20, 0x00, 0x04,
    0x10, 0x3C, 0x08, 0x08, 0x42, 0x10, 0x08, 0x81, 0x10, 0x84, 0x00, 0x21,
    0x64, 0x00, 0x26, 0x18, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#define CROSS_BLOCK_WIDTH 14
#define CROSS_BLOCK_HEIGHT 14
static const unsigned char CROSS_BLOCK_BITS[] U8X8_PROGMEM = {
    0xFF, 0x3F, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20,
    0xC1, 0x20, 0xC1, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20,
    0x01, 0x20, 0xFF, 0x3F};

enum Direction
{
  FORWARD,
  BACKWARD
};

enum Stabilisation
{
  OFF,
  ON
};

U8G2_ST7586S_YMC240160_2_6800 u8g2(U8G2_MIRROR, 10, 7, 11, 6, 12, 5, 0, 4, 1, 2, 9, 3);

void u8g2_box_title(uint8_t a);
void u8g2_main_screen(uint8_t position, uint8_t speed, Direction direction, Stabilisation stabilisation, int flash_count);
void u8g2_startup_screen(void);
void u8g2_low_battery(void);
void u8g2_show_error(void);
void u8g2_box_frame(uint8_t a);
void u8g2_disc_circle(uint8_t a);
void u8g2_r_frame(uint8_t a);
void u8g2_string(uint8_t a);
void u8g2_line(uint8_t a);
void u8g2_triangle(uint8_t a);
void u8g2_ascii_1(void);
void u8g2_ascii_2(void);
void u8g2_extra_page(uint8_t a);
void u8g2_xor(uint8_t a);
void u8g2_bitmap_overlay(uint8_t a);
void u8g2_bitmap_modes(uint8_t transparent);

// Function Definitions

void u8g2_box_title(uint8_t a)
{
  u8g2.setBitmapMode(1);
  u8g2.setFontMode(1);
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(4, 11, STARTER_TEXT);
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(5, 92, "Position along the cable: %");
  u8g2.drawLine(5, 110, 234, 110);
  u8g2.drawDisc(x, 110, 2);
  u8g2.drawFrame(0, 0, u8g2.getDisplayWidth(), u8g2.getDisplayHeight());
}

void u8g2_main_screen(uint8_t position, uint8_t speed, Direction direction, Stabilisation stabilisation, int flash_count)
{
  u8g2.setBitmapMode(1);
  u8g2.setFontMode(1);
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(4, 3, STARTER_TEXT);
  u8g2.drawLine(0, 13, 240, 13);

  // Position
  char position_string[50];
  snprintf(position_string, sizeof(position_string), "Position along the cable: %d%%", position);
  u8g2.drawStr(5, 21, position_string);

  // Speed
  char speed_string[14];
  snprintf(speed_string, sizeof(speed_string), "Speed: %dm/s", speed);
  u8g2.drawStr(5, 51, speed_string);

  // Percentage
  char percentage_string[50];
  snprintf(percentage_string, sizeof(percentage_string), "Percentage along cable: %d%%", position);
  u8g2.drawStr(5, 60, percentage_string);

  // Direction
  const char *direction_text = direction == FORWARD ? "Forward" : "Backward";
  u8g2.drawStr(5, 30, direction_text);

  // Stabilisation
  const char *stabilisation_text = stabilisation == ON ? "On" : "Off";
  u8g2.drawStr(5, 40, stabilisation_text);

  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(5, 60, "Percentage along cable:");

  u8g2.drawFrame(0, 0, u8g2.getDisplayWidth(), u8g2.getDisplayHeight());
  u8g2.drawLine(5, 100, 234, 100);
  u8g2.drawDisc(position, 100, 2);

  // Draw the cross based on flash count
  const unsigned char *cross_bitmap = (flash_count % 2 == 0) ? CROSS_BITS : CROSS_FILL_BITS;
  u8g2.drawBitmap(120, 55, CROSS_WIDTH / 8, CROSS_HEIGHT, cross_bitmap);
}

void u8g2_startup_screen(void)
{
  u8g2.setBitmapMode(1);
  u8g2.setFontMode(1);
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(40, 60, "Initialization...");
  u8g2.drawStr(40, 80, "Please wait.");
  delay(1000);
  u8g2.clearBuffer();
}

void u8g2_low_battery(void)
{
  u8g2.setBitmapMode(1);
  u8g2.setFontMode(1);
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(40, 60, "Low Battery");
  u8g2.drawStr(40, 80, "Please recharge.");
  delay(1000);
  u8g2.clearBuffer();
}

void u8g2_show_error(void)
{
  u8g2.setBitmapMode(1);
  u8g2.setFontMode(1);
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(40, 60, "Error Occurred");
  u8g2.drawStr(40, 80, "Restarting...");
  delay(1000);
  u8g2.clearBuffer();
}

// Placeholder functions for completeness
void u8g2_box_frame(uint8_t a) {}
void u8g2_disc_circle(uint8_t a) {}
void u8g2_r_frame(uint8_t a) {}
void u8g2_string(uint8_t a) {}
void u8g2_line(uint8_t a) {}
void u8g2_triangle(uint8_t a) {}
void u8g2_ascii_1(void) {}
void u8g2_ascii_2(void) {}
void u8g2_extra_page(uint8_t a) {}
void u8g2_xor(uint8_t a) {}
void u8g2_bitmap_overlay(uint8_t a) {}
void u8g2_bitmap_modes(uint8_t transparent) {}

#endif // SHAPES_H
