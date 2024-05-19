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
#include "hardware/gpio.h"
#include "shapes.h"
#include "subject.h"
#include "Timer.h"
// #include "transceiver.h"

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

// START OF RF Code
#include <SPI.h>
#include "printf.h"
#include "RF24.h"

#define CE_PIN 22
#define CSN_PIN 21
// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);
uint8_t address[][6] = {"1Node", "2Node"};
bool radioNumber = 1; // 0 uses address[0] to transmit, 1 uses address[1] to transmit
bool role = false;    // true = TX role, false = RX role
int payload = 0;
int cableLength = 0;
int current_motor_data = 0;
int count = 0;
int off_count = 0;
uint8_t draw_state = 0;
uint display_position = 0;
int display_direction = 0;
float display_speed = 0;
int display_error = 0;

void setupRadio()
{
    Serial.begin(115200);
    radioNumber = 1;
    radio.setPALevel(RF24_PA_MAX);                   // Maximum transmission strength
    radio.setPayloadSize(sizeof(payload));           // float datatype occupies 4 bytes
    radio.openWritingPipe(address[radioNumber]);     // set the TX address of the RX node into the TX pipe
    radio.openReadingPipe(1, address[!radioNumber]); // set the RX address of the TX node into a RX pipe

    if (role)
    {
        radio.stopListening(); // put radio in TX mode
    }
    else
    {
        radio.startListening(); // put radio in RX mode
    }
}

void RX_TX()
{
    if (role)
    {
        // This device is a TX node
        unsigned long start_timer = micros();               // start the timer
        bool report = radio.write(&payload, sizeof(float)); // transmit & save the report
        unsigned long end_timer = micros();                 // end the timer

        if (report)
        {
            Serial.print(end_timer - start_timer); // print the transmission time (for testing only)
            Serial.println(payload);               // print payload sent
        }
        else
        {
            for (int i = 0; i > 3; i++)
            {                                                          // Attempt resending 3 times
                delay(100);                                            // Delay for 1ms
                Serial.println(F("Transmission failed or timed out")); // payload was not delivered
                bool report = radio.write(&payload, sizeof(float));
                if (report)
                {
                    break;
                }
            }
        }
        radio.setDataRate(RF24_250KBPS)
            role = false;
        radio.startListening();
        delay(500);
    }
    else
    {
        // This device is a RX node
        uint8_t pipe;
        if (radio.available(&pipe))
        {                                           // is there a payload? get the pipe number that recieved it
            uint8_t bytes = radio.getPayloadSize(); // get the size of the payload
            radio.read(&payload, bytes);            // fetch payload from FIFO
            executePayload(payload);
        }
    }
}

// MOTOR INTEGRATION
void sendCableLength(int length)
{
    int payloadData = (length * 10000);
    createPayload(10, payloadData);
}
void sendMotorPacket(int speed, int direction, int distance)
{
    int payloadData = (speed * 100000 + direction * 10000 + distance * 10);
    createPayload(11, payloadData);
}
int displayMotorData(int data)
{
    float input_speed = data / 100000;
    int input_direction = (data / 10000) % 10;
    int input_distance = (data % 10000) / 10;
    int input_error = data % 10;
    display_speed = input_speed / 100;
    display_position = input_distance;
    display_direction = input_direction;
    display_error = input_error;
}

void sendControls(int speed, int direction, int stabilisation)
{
    int payloadData = (speed * 10000 + direction * 10000 + stabilisation * 1000);
    createPayload(12, payloadData);
}
void setSpeed(int data)
{
    int unprocessed_speed = data / 100000 float speed = unprocessed_speed
                                speed = speed / 100 int direction = (data / 10000) % 10 int stabilisation = ((data % 10000) / 1000)
}
void stabilisationError()
{
    createPayload(13, 0);
}
void stabilisationOff()
{
    createPayload(14, 0);
}
// END OF MOTOR INTEGRATION

void executePayload(int payload)
{
    // Decode the Payload
    int command = payload / (int)pow(10, 10 - 2); // Extracts the command
    int data = (payload % (int)pow(10, 10 - 2));  // Extracts the data

    switch (command)
    {
    case 10:
    { // Command One - Cable Length
        cableLength = data / 10000;
        break;
    }
    case 11:
    { // Command Two - Motor Data
        current_motor_data = displayMotorData(data);
        break;
    }
    case 12:
    { // Command Three - Set Speed
        setSpeed(data);
        break;
    }
    }
}
void createPayload(int command, int data)
{
    payload = (command * 100000000) + (data); // Add the payload and the data to the packet integer
    role = true;                              // set the Radio Module to TX mode
    radio.stopListening();                    // the radio will no longer listen for packets
}
// END OF RF Code

void rst_ISR()
{
    u8g2_show_error();
}

void u8g2_prepare(void)
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

bool startup(void)
{
    u8g2_startup_screen();
    STARTUP_TIME = STARTUP_TIME - REFRESH_PERIOD;
    return (STARTUP_TIME - REFRESH_PERIOD > 0);
}

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

void loop(void)
{
    // picture loop
    u8g2.firstPage();
    sendControls(getSpeed(), direction, stabilisation);
    do
    {
        RX_TX();
        if (STARTUP)
        { // check if device is starting up
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
        ERR = false; // get error here
        setErr(ERR);

    } while (u8g2.nextPage());

    // delay between each page
    delay(REFRESH_PERIOD);
}