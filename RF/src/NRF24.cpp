#include "NRF24.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include <string.h>
#include <pico/time.h>


NRF24::NRF24(spi_inst_t *port, uint16_t csn, uint16_t ce)
{
    this->port = port;
    this->csn = csn;
    this->ce = ce;

    spi_init(this->port, 8000000 );
    gpio_set_function(20, GPIO_FUNC_SPI); // Serial Clock
    gpio_set_function(18, GPIO_FUNC_SPI); // Serial Data Into NRF
    gpio_set_function(19, GPIO_FUNC_SPI); // Serial Data Out of NRF

    gpio_init(csn);
    gpio_init(ce);

    gpio_set_dir(csn, 1);
    gpio_set_dir(ce, 1);

    ceLow();
    csnHigh();

}

NRF24::~NRF24()
{
}

uint8_t NRF24::readReg(uint8_t reg){
    uint8_t result = 0;
    reg = ( 0b00011111 & reg);
    csnLow();
    spi_write_blocking(port, &reg,1);
    spi_read_blocking(port, 0xff,&result, 1);
    csnHigh();

    return result;
}

void NRF24::writeReg(uint8_t reg, uint8_t data){
    writeReg(reg, &data, 1);
}
void NRF24::writeReg(uint8_t reg, uint8_t *data, uint8_t size){
    reg = 0b00100000 | ( 0b00011111 & reg);
    csnLow();
    spi_write_blocking(port, &reg, 1);
    spi_write_blocking(port, (uint8_t*)data, size);
    csnHigh();
}

void NRF24::config(){
    csnHigh();
    ceLow();
    sleep_ms(11);

    writeReg(0, 0b00001010); // config.
    sleep_us(1500);

    writeReg(1,0); // no ack.

    writeReg(5, 60); // channel.

    writeReg(0x0a, (uint8_t*)"gyroc",5);
    writeReg(0x10, (uint8_t*)"gyroc",5);

    writeReg(0x11, 32);

    

}

void NRF24::modeTX(){
    uint8_t reg = readReg(0);
    reg &= ~(1<<0);
    writeReg(0, reg);
    
    ceLow(); // Correction from video, thansk schuhmann.
    
    sleep_us(130);
}
void NRF24::modeRX(){
    uint8_t reg = readReg(0);
    reg |= (1<<0);
    writeReg(0, reg);
    
    ceHigh(); // Correction from video, thanks schuhmann.
    
    sleep_us(130);

}

void NRF24::sendMessage(char *msg){
    uint8_t reg = 0b10100000;
    csnLow();
    spi_write_blocking(port, &reg,1);
    spi_write_blocking(port, (uint8_t*)msg, 32);
    csnHigh();

    ceHigh();
    sleep_us(300); // 300us is needed to reliably send messages
    ceLow();
}
void NRF24::receiveMessage(char *msg){
    uint8_t cmd = 0b01100001;   
    csnLow();
    spi_write_blocking(port, &cmd, 1);

    spi_read_blocking(port,0xff, (uint8_t*)msg,32);
    csnHigh();

}

uint8_t NRF24::newMessage(){
    uint8_t fifo_status = readReg(0x17);

    return !(0x00000001 & fifo_status);
}

void NRF24::setChannel(uint8_t ch){
    writeReg(5, ch); // channel.
}

void NRF24::setRXName(char *name){
    if( strlen(name) != 5) return;
    writeReg(0x0a, (uint8_t*)name,5);
}

void NRF24::setTXName(char *name){
    if( strlen(name) != 5) return;
    writeReg(0x10, (uint8_t*)name,5);
}
