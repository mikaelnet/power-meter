/*
 * power-meter.c
 *
 * Created: 2017-08-02 21:59:45
 * Author : mikael
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdbool.h>
#include <util\delay.h>

#include "drivers\USI_TWI_Master.h"
#include "drivers\oled_ssd1306.h"

inline void led_on() {
    PORTB |= _BV(PB4);
}
inline void led_off() {
    PORTB &= ~_BV(PB4);
}

SSD1306_Datagram_t oledData;

int main(void)
{
    //DDRB |= _BV(PB4);
    //led_off();
        
    USI_TWI_Master_Initialise();
    sei();
    ssd1306_begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
    ssd1306_clear();
    
    for (int8_t i=0 ; i < 8 ; i ++) {
        oledData.buffer[i] = 1 << i;
        oledData.buffer[i+8] = 1 << (7-i);
        oledData.buffer[i+16] = oledData.buffer[i+24] = (i & 1) ? 0x55 : 0xAA;
    }
    ssd1306_setActiveArea(64-8, 64+8-1, 1, 2);
    ssd1306_writeData(&oledData, 32);

    //ssd1306_display();
    //led_on();

    /* Replace with your application code */
    while (1) 
    {
        //ssd1306_dim(true);
        //_delay_ms(250);
        //ssd1306_dim(false);
        //_delay_ms(250);
    }
}

