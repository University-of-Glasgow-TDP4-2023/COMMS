#include <Arduino.h>
#include <U8g2lib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "transceiver.h"
#include "display.h"

#define REFRESH_PERIOD 150

Timer timer;
volatile int rstState = 0;
int buttonState = 0;
int direction = 0;

void setup()
{
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    pinMode(2, OUTPUT);
    pinMode(9, OUTPUT);
    digitalWrite(2, LOW);
    digitalWrite(9, LOW);

    attachInterrupt(27, rst_ISR, FALLING);
    setupPins();

    setupDisplay();
    setupRadio();

    gpio_set_irq_enabled_with_callback(buttonPin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &buttonCallback);
}

void loop()
{
    u8g2.firstPage();

    do
    {
        RX_TX();
        if (startup)
        {
            startup = handleStartup();
        }
        else
        {
            flashCount = (flashCount + 1) % 16;
            draw();
        }
        batteryLow = getBattery();
        errorDetected = false;
        setErr(errorDetected);

    } while (u8g2.nextPage());

    delay(REFRESH_PERIOD);
}
