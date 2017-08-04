/*
 * power-meter.c
 *
 * Created: 2017-08-02 21:59:45
 * Author : mikael
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <stdbool.h>
#include <util/delay.h>

#include <stdio.h>

#include "drivers/timer.h"
#include "drivers/USI_TWI_Master.h"
#include "drivers/oled_ssd1306.h"
#include "drivers/ina219.h"
#include "drivers/font5x7.h"

SSD1306_Datagram_t oledData;

uint8_t* drawChar (uint8_t *ptr, uint8_t ch)
{
    memcpy_P(ptr, font5x7+ch*5, 5);
    *(ptr+5) = 0x00;
    return ptr+6;
}

uint8_t drawString (uint8_t *ptr, char *str)
{
    uint8_t length = 0;
    for (char *s = str ; *s && length <= 128; s++) {
        ptr = drawChar(ptr, *s);
        length += 6;
    }
    return length;
}

void printTime() 
{
    uint16_t clock = timer_clock >> 2;
    char str[6];
    snprintf_P(str, sizeof(str), PSTR("%02d:%02d"), clock / 60, clock % 60);
    
    uint8_t *ptr = oledData.buffer;
    uint8_t length = drawString(ptr, str);

    ssd1306_setActiveArea(128-length, 128-1, 0, 0);
    ssd1306_writeData(&oledData, length);
}

static int16_t busVoltage, shuntVoltage;
static float current;
void printBusVoltage() 
{
    busVoltage = ina219_getBusVoltage();
    char str[20];
    snprintf_P(str, sizeof(str), PSTR("Bus: %d.%03dmV"), busVoltage / 1000, busVoltage % 1000);

    uint8_t *ptr = oledData.buffer;
    uint8_t length = drawString(ptr, str);
    
    ssd1306_setActiveArea(0, length, 0, 0);
    ssd1306_writeData(&oledData, length);
}

void printShuntVoltage() {
    char str[20];
    shuntVoltage = ina219_getShuntVoltage();
    snprintf_P(str, sizeof(str), PSTR("Shunt: %d.%02dmV"), shuntVoltage / 100, shuntVoltage % 100);
    
    uint8_t *ptr = oledData.buffer;
    uint8_t length = drawString(ptr, str);

    ssd1306_setActiveArea(0, length, 1, 1);
    ssd1306_writeData(&oledData, length);
}

void printCurrent() {
    char str[20];
    current = ina219_getCurrent_mA();
    int decimal = current;
    uint8_t fragment = ((int)(current*100.0))%100;
    
    snprintf_P(str, sizeof(str), PSTR("Current: %d.%02dmA"), decimal, fragment);
    
    uint8_t *ptr = oledData.buffer;
    uint8_t length = drawString(ptr, str);

    ssd1306_setActiveArea(0, length, 2, 2);
    ssd1306_writeData(&oledData, length);
}

void printPower() {
    uint16_t clock = timer_clock >> 2;
    float power = busVoltage * current / 1000;
    float mAhour = current * ((float)clock/3.6);
    char str[20];

    int d1 = power;
    uint8_t f1 = ((int)(power*100.0))%100;
    int d2 = mAhour;
    uint8_t f2 = ((int)(mAhour*10.0))%10;

    snprintf_P(str, sizeof(str), PSTR("%d.%02dmW %d.%01duAh"), d1, f1, d2, f2);
    
    uint8_t *ptr = oledData.buffer;
    uint8_t length = drawString(ptr, str);

    ssd1306_setActiveArea(0, length, 3, 3);
    ssd1306_writeData(&oledData, length);
}

int main(void)
{
    timer1_begin();
    USI_TWI_Master_Initialise();

    sei();
    ina219_begin(INA219_ADDRESS);
    ina219_calibrate(Mode_16V_400mA);
    ssd1306_begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
    ssd1306_clear();
    
    for (int8_t i=0 ; i < 8 ; i ++) {
        oledData.buffer[i] = 1 << i;
        oledData.buffer[i+8] = 1 << (7-i);
        oledData.buffer[i+16] = oledData.buffer[i+24] = (i & 1) ? 0x55 : 0xAA;
    }
    ssd1306_setActiveArea(128-16, 127, 1, 2);
    ssd1306_writeData(&oledData, 32);

    /* Replace with your application code */
    bool state = false, lastState = false;
    while (1) 
    {
        state = timer_clock & 1;
        if (state != lastState) {
            //ssd1306_dim(state);
            lastState = state;
            printTime();
            printBusVoltage();
            printShuntVoltage();
            printCurrent();
            printPower();
        }
    }
}

