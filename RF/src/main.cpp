#include "pico/stdlib.h"
#include "NRF24.h"
#include "HM11.H"
#include "stdio.h"

int main()
{

    typedef struct _nrf{
        spi_inst_t* spi;
        uint8_t csn;
        uint8_t ce;

    }NRF;

        gpio_init(15); // led

        gpio_set_dir(15,1);

        sleep_ms(100); // give time for things to settle.

        NRF24 nrf = NRF24(spi1,21,22);
        nrf.config((uint8_t*)"gyroc",2,5); // Name=gyroc, channel=2,messagSize=10

        nrf.modeTX(); // <---- Set as transmitter.

        char message[10];
        uint16_t counter = 0;

        while(1)  
        {
            sprintf(message,"%d",counter++);
            nrf.sendMessage((uint8_t*)message);
            sleep_ms(20);
        } 
        
    return 0;
}

// #include "pico/stdlib.h"
// #include "hardware/spi.h"
// #include "NRF24.h"
// #include <stdio.h>
// #include <string.h>

// int main(){
//     stdio_init_all();
//     NRF24 nrf(spi0, 21, 22); // Integer values are pins for CS and CE respectively
//     nrf.config();
//     nrf.modeRX(); // Recieving Mode as default

//     gpio_init(15); // Initialise the LED
//     gpio_set_dir(15,1); // Sets the direction as output from the Pico
//     gpio_init(14); // Initialise On button
//     gpio_set_dir(14,0);

//     char TXBuffer[32] {0}; // The Transmission Buffer
//     char RXBuffer[32] {0}; // The Recieve Buffer
//     char command;

//     gpio_put(15,0); // Test LED works
//     while(1) {
//         command = getchar();
//         if (command == '1') { // If command = 1
//             printf(" LED turning on!!!!!!!\n");
//             gpio_put(15,1); // If so, turn the light off for 3 seconds
//             sleep_ms(3000);
//             gpio_put(15,0); // Turn the light on
//             sleep_ms(3000);
//             nrf.modeTX(); // Switch to TX mode
//             sprintf(TXBuffer, "1");
//             printf(TXBuffer,"\n");
//             nrf.sendMessage(TXBuffer); // Send the buffer
//             nrf.modeRX(); // Switch back to RX
//         }

//         if (nrf.newMessage() == 1) { // If new message recieved
//             nrf.receiveMessage(RXBuffer);
//             // strcpy(RXBuffer, "Johnny");
//             if(strcmp(RXBuffer, "1") == 0) { // Check if the message is John
//                 printf(RXBuffer,"\n");
//                 printf(" Thomas\n");
//                 gpio_put(15,1); // If so, turn the light off for 1 second
//                 sleep_ms(3000);
//                 gpio_put(15,0); // Turn the light on
//                 sleep_ms(3000);
//             }
//         }
//     }
//     return 0;
// }
