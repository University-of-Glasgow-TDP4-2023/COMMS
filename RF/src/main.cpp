#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "NRF24.h"
#include <stdio.h>
#include <string.h>

int main(){
    stdio_init_all();
    NRF24 nrf(spi0, 21, 22); // Integer values are pins for CS and CE respectively
    nrf.config();
    nrf.modeRX(); // Recieving Mode as default

    gpio_init(15); // Initialise the LED
    gpio_set_dir(15,1); // Sets the direction as output from the Pico
    gpio_init(14); // Initialise On button
    gpio_set_dir(14,0);

    char TXBuffer[32] {0}; // The Transmission Buffer
    char RXBuffer[32] {0}; // The Recieve Buffer
    char command;

    gpio_put(15,0); // Test LED works
    while(1) {
        command = getchar();
        if (command == '1') { // If command = 1
            printf(" LED turning on!!!!!!!\n");
            gpio_put(15,1); // If so, turn the light off for 3 seconds
            sleep_ms(3000);
            gpio_put(15,0); // Turn the light on
            sleep_ms(3000);
            nrf.modeTX(); // Switch to TX mode
            sprintf(TXBuffer, "1");
            printf(TXBuffer,"\n");
            nrf.sendMessage(TXBuffer); // Send the buffer
            nrf.modeRX(); // Switch back to RX
        }

        if (nrf.newMessage() == 1) { // If new message recieved
            nrf.receiveMessage(RXBuffer);
            // strcpy(RXBuffer, "Johnny");
            if(strcmp(RXBuffer, "1") == 0) { // Check if the message is John
                printf(RXBuffer,"\n");
                printf(" Thomas\n");
                gpio_put(15,1); // If so, turn the light off for 1 second
                sleep_ms(3000);
                gpio_put(15,0); // Turn the light on
                sleep_ms(3000);
            }
        }
    }
    return 0;
}
