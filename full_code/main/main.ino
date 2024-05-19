#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "Timer.h"
#include "display.h"
#include "transceiver.h"

#define REFRESH_PERIOD 150

bool BATTERY_LOW = false; // whether battery is low
bool ERR = false;         // error detected
bool STARTUP = true;      // whether to show startup screen
int STARTUP_TIME = 50000; // how long to show startup screen for
int buttonState = 0;      // variable for reading the pushbutton status
int buttonValue = analogRead(buttonPin);
int direction;
int stabilisation = 1; // Stabilisation ON is 1
Timer timer;
volatile int rstState = 0;
int flash_count = 0;

void buttonOn(uint gpio, uint32_t events)
{
    timer.start();
}

void buttonOff(uint gpio, uint32_t events)
{
    timer.stop();
    int buttom_time = timer.read();
    if (buttom_time >= 1500)
    {                                   // If button pressed for 1.5s or longer
        stabilisation = !stabilisation; // Switch off stabilisation
    }
    else
    {
        direction = !direction; // Otherwise, change direction
    }
}

void setup(void)
{

    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);
    pinMode(2, OUTPUT);
    pinMode(9, OUTPUT);
    digitalWrite(2, 0);
    digitalWrite(9, 0);
    digitalWrite(powerPin_out, HIGH);
    attachInterrupt(27, rst_ISR, FALLING);
    setupPins();

    u8g2.begin();
    u8g2.setContrast(65);
    u8g2.clearDisplay();
    u8g2.setDisplayRotation(U8G2_R2);
    u8g2.setFlipMode(0);

    STARTUP = true;      // whether to show startup screen
    STARTUP_TIME = 2000; // how long to show startup screen for

    setupRadio();
    gpio_set_irq_enabled_with_callback(buttonPin, GPIO_IRQ_EDGE_RISE, true, &buttonOn);
    gpio_set_irq_enabled_with_callback(buttonPin, GPIO_IRQ_EDGE_FALL, true, &buttonOff);
}

void loop()
{
    u8g2.firstPage();
    sendControls(getSpeed(), direction, stabilisation);
    do
    {
        RX_TX();
        if (STARTUP)
        {
            STARTUP = startup();
        }
        else
        {
            flash_count = flash_count + 1;
            draw();
            if (flash_count == 16)
            {
                flash_count = 0;
            }
        }
        BATTERY_LOW = getBattery();
        ERR = false;
        setErr(ERR);
    } while (u8g2.nextPage());

    delay(REFRESH_PERIOD);
}
