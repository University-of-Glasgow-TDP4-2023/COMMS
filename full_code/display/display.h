#ifndef DISPLAY_H
#define DISPLAY_H

#include <U8g2lib.h>

// External variables
extern bool batteryLow;
extern bool errorDetected;
extern bool startup;
extern int flashCount;
extern int displayPosition;
extern int displayDirection;
extern float displaySpeed;
extern int stabilisation;

// Function declarations and definitions
bool batteryLow = false;
bool errorDetected = false;
bool startup = true;
int flashCount = 0;
int displayPosition = 0;
int displayDirection = 0;
float displaySpeed = 0;
int stabilisation = 1;

void rst_ISR()
{
    u8g2_show_error();
}

void u8g2_prepare()
{
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setFontRefHeightExtendedText();
    u8g2.setDrawColor(1);
    u8g2.setFontPosTop();
    u8g2.setFontDirection(0);
}

void draw()
{
    u8g2_prepare();
    if (errorDetected)
    {
        u8g2_show_error();
        return;
    }
    u8g2_main_screen(displayPosition, displaySpeed, displayDirection, stabilisation, flashCount);
    if (batteryLow)
    {
        u8g2_low_battery();
    }
}

void setupDisplay()
{
    u8g2.begin();
    u8g2.setContrast(65);
    u8g2.clearDisplay();
    u8g2.setDisplayRotation(U8G2_R2);
    u8g2.setFlipMode(0);
}

bool handleStartup()
{
    u8g2_startup_screen();
    STARTUP_TIME -= REFRESH_PERIOD;
    return STARTUP_TIME > 0;
}

void buttonCallback(uint gpio, uint32_t events)
{
    count++;
    if (count == 1)
    {
        timer.start();
    }
    else if (count == 2)
    {
        timer.stop();
        int buttonTime = timer.read();
        if (buttonTime >= 2000)
        {
            stabilisation = !stabilisation;
            Serial.println("STABILISATION OFF");
        }
        else
        {
            direction = !direction;
        }
        count = 0;
    }
}

#endif // DISPLAY_H
