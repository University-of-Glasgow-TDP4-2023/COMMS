#include "display.h"

void u8g2_prepare()
{
    u8g2.setDisplayRotation(U8G2_R0);
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setFontRefHeightExtendedText();
    u8g2.setDrawColor(1);
    u8g2.setFontPosTop();
    u8g2.setFontDirection(0);
}

void draw(void)
{
    u8g2_prepare();
    if (display_error == 8)
    { // Error 8 means currently callibrating
        u8g2_callibrating();
    }
    else if (display_error == 9)
    {                                      // Error 9 means not callibrated
        u8g2_not_callibrated(flash_count); // Flash count flashes the message
    }
    else
    {
        u8g2_main_screen(display_position, display_speed, display_direction, display_error, stabilisation, flash_count);
        if (BATTERY_LOW)
        {
            u8g2_low_battery();
        }
    }
}

void rst_ISR()
{
    u8g2_show_error();
}

bool startup()
{
    u8g2_startup_screen();
    STARTUP_TIME -= REFRESH_PERIOD;
    return (STARTUP_TIME > 0);
}

