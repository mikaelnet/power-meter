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
#include "drivers/i2c_soft.h"
#include "drivers/oled_ssd1306.h"
#include "drivers/ina219.h"
#include "drivers/display.h"

static float lastVoltage, maxVoltage, minVoltage;
static float lastCurrent, maxCurrent, minCurrent;
static float sumVoltage, sumCurrent;
static uint16_t numSamples;


//static int16_t busVoltage, shuntVoltage;
//static float current;
/*void printBusVoltage() 
{
    busVoltage = ina219_getBusVoltage();
    char str[20];
    snprintf_P(str, sizeof(str), PSTR("Bus: %d.%03dmV "), busVoltage / 1000, busVoltage % 1000);

    uint8_t *ptr = oled_data;
    uint8_t length = drawString(ptr, str);
    
    ssd1306_setActiveArea(0, length, 0, 0);
    ssd1306_writeData(oled_data, length);
}

void printShuntVoltage() {
    char str[20];
    shuntVoltage = ina219_getShuntVoltage();
    snprintf_P(str, sizeof(str), PSTR("Shunt: %d.%02dmV "), shuntVoltage / 100, shuntVoltage % 100);
    
    uint8_t *ptr = oled_data;
    uint8_t length = drawString(ptr, str);

    ssd1306_setActiveArea(0, length, 1, 1);
    ssd1306_writeData(oled_data, length);
}

void printCurrent() {
    char str[20];
    current = ina219_getCurrent_mA();
    int decimal = current;
    uint8_t fragment = ((int)(current*100.0))%100;
    
    snprintf_P(str, sizeof(str), PSTR("Current: %d.%02dmA "), decimal, fragment);
    
    uint8_t *ptr = oled_data;
    uint8_t length = drawString(ptr, str);

    ssd1306_setActiveArea(0, length, 2, 2);
    ssd1306_writeData(oled_data, length);
}*/

/*void printPower() {
    uint16_t clock = timer_clock >> 2;
    float power = busVoltage * current / 1000;
    float mAhour = current * ((float)clock/3.6);
    char str[20];

    int d1 = power;
    uint8_t f1 = ((int)(power*100.0))%100;
    int d2 = mAhour;
    uint8_t f2 = ((int)(mAhour*10.0))%10;

    snprintf_P(str, sizeof(str), PSTR("%d.%02dmW %d.%01duAh "), d1, f1, d2, f2);
    
    uint8_t *ptr = oled_data;
    uint8_t length = drawString(ptr, str);

    ssd1306_setActiveArea(0, length, 3, 3);
    ssd1306_writeData(oled_data, length);
}*/

int main(void)
{
    timer1_begin();
    i2c_begin();

    sei();
    ina219_begin(INA219_ADDRESS);
    ina219_calibrate(Mode_16V_400mA);
    ssd1306_begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
    ssd1306_clear();
    
    lastVoltage = ina219_getBusVoltage();
    lastCurrent = ina219_getCurrent_mA();
    minVoltage = maxVoltage = lastVoltage;
    minCurrent = maxCurrent = lastCurrent;
    sumVoltage = sumCurrent = 0;
    numSamples = 0;

    /* Replace with your application code */
    //bool state = false, lastState = false;
    uint16_t clock = timer_clock;
    do 
    {
        lastVoltage = ina219_getBusVoltage();
        lastCurrent = ina219_getCurrent_mA();

        if (lastVoltage < minVoltage)
            minVoltage = lastVoltage;
        else if (lastVoltage > maxVoltage)
            maxVoltage = lastVoltage;
        if (lastCurrent < minCurrent)
            minCurrent = lastCurrent;
        else if (lastCurrent > maxCurrent)
            maxCurrent = lastCurrent;

        sumVoltage += lastVoltage;
        sumCurrent += lastCurrent;
        numSamples ++;

        uint8_t tick = clock & 0x03;
        if (tick == 0) {
            printClock (clock >> 2);
            printVoltage (lastVoltage);
            printCurrent (lastCurrent);
            printPower (lastVoltage * lastCurrent);
            float avgPower = sumVoltage / numSamples * sumCurrent / numSamples;
            printCharge (avgPower * clock / (3600*4));
        }

        /*state = timer_clock & 1;
        if (state != lastState) {
            //ssd1306_dim(state);
            lastState = state;
            printTime();
            printBusVoltage();
            printShuntVoltage();
            printCurrent();
            printPower();
        }*/

        // Wait for next timer
        while (clock == timer_clock)
            ;
        clock = timer_clock;
    } while (true);
}

        /*if ( (PINB & _BV(PORTB3)) == 0 ) {
            ina219_calibrate(Mode_16V_400mA);
            printBusVoltage();
            _delay_ms(500);
        }*/
