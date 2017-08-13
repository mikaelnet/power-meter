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
#include <math.h>

#include "drivers/timer.h"
#include "drivers/i2c_soft.h"
#include "drivers/oled_ssd1306.h"
#include "drivers/ina219.h"
#include "drivers/display.h"

static float lastVoltage, maxVoltage, minVoltage;
static float lastCurrent, maxCurrent, minCurrent;
static float sumVoltage, sumCurrent;
static uint16_t numSamples;

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
            printPower (fabs(lastVoltage * lastCurrent));
            float avgPower = sumVoltage / numSamples * sumCurrent / numSamples;
            printCharge (fabs(avgPower * clock / (3600*4)));
            float avgCurrent = sumCurrent / numSamples;
            printMaxMinCurrent(minCurrent, maxCurrent, avgCurrent);
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
