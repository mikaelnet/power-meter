/*
 * display.c
 *
 * Created: 2017-08-06 22:16:44
 *  Author: mikael
 */ 

#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <stdbool.h>

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "oled_ssd1306.h"
#include "font5x7.h"
#include "display.h"

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

void printString (uint8_t x, uint8_t row, char *str, uint8_t maxSize)
{
    uint8_t *ptr = oled_data;
    uint8_t length = drawString(ptr, str);
    for (uint8_t i=length ; i < (maxSize-1)*6 && i < sizeof(oled_data); i ++)
        oled_data[i] = 0;
    uint8_t numBytes = (maxSize-1)*6;
    if (x+numBytes > 128)
        numBytes = 128-x;
    ssd1306_setActiveArea(x, x+numBytes-1, row, row);
    ssd1306_writeData(oled_data, numBytes);
}

void printClock (uint16_t clock)
{
    char str[6];
    if (clock >= 3600)
        clock /= 60;    // Display HH:MM instead of default MM:SS
    snprintf_P(str, sizeof(str), PSTR("%02d:%02d"), clock / 60, clock % 60);
    printString(128-5*6, 3, str, sizeof(str));
}

uint8_t printFloat (float value, int16_t *left, uint16_t *right) 
{
    if (value > 100) {
        *left = value / 1000;
        *right = ((uint16_t)(value/10.0)) % 100;
        return 0;
    }
    else if (value > .1) {
        *left = (int16_t)value;
        *right = ((uint16_t)(value * 100.0)) % 100;
        return 1;
    }
    else {
        *left = (int16_t)(value * 1000);
        *right = ((uint16_t)(value * 100000.0)) % 100;
        return 2;
    }
}

void printVoltage (float voltage) 
{
    int16_t left = (int16_t)voltage;
    uint16_t right = ((uint16_t)(voltage * 100)) % 100;

    char str[14];
    snprintf_P(str, sizeof(str), PSTR("U: %d.%02dV"), left, right);
    printString(0, 0, str, sizeof(str));
}

void printCurrent (float value)
{
    char *unit = "";
    int16_t left;
    uint16_t right;
    uint8_t unitIndex = printFloat(value, &left, &right);
    switch (unitIndex) {
        case 1: unit = "m"; break;
        case 2: unit = "u"; break;
    }

    char str[14];
    snprintf_P(str, sizeof(str), PSTR("I: %d.%02d%sA"), left, right, unit);
    printString(0, 1, str, sizeof(str));
}

void printPower (float value)
{
    char *unit = "";
    int16_t left;
    uint16_t right;
    uint8_t unitIndex = printFloat(value, &left, &right);
    switch (unitIndex) {
        case 1: unit = "m"; break;
        case 2: unit = "u"; break;
    }

    char str[14];
    snprintf_P(str, sizeof(str), PSTR("P: %d.%02d%sW"), left, right, unit);
    printString(0, 2, str, sizeof(str));
}

void printCharge (float value)
{
    char *unit = "";
    int16_t left;
    uint16_t right;
    uint8_t unitIndex = printFloat(value, &left, &right);
    switch (unitIndex) {
        case 1: unit = "m"; break;
        case 2: unit = "u"; break;
    }

    char str[14];
    snprintf_P(str, sizeof(str), PSTR("Q: %d.%02d%sWh"), left, right, unit);
    printString(0, 3, str, sizeof(str));
}

void printEdgeValues (float value, uint8_t row, char *unit) 
{
    char *precition = "";
    int16_t left;
    uint16_t right;
    uint8_t unitIndex = printFloat(value, &left, &right);
    switch (unitIndex) {
        case 1: precition = "m"; break;
        case 2: precition = "u"; break;
    }

    char str[10];
    snprintf_P(str, sizeof(str), PSTR("%d.%01d%s%s"), left, right/10, precition, unit);
    printString(86, row, str, sizeof(str));
}

void printMaxMinCurrent (float min, float max, float avg) 
{
    char *unit = "A";

    if (max < 0)
        max = fabs(max);
    printEdgeValues (max, 0, unit);
    
    if (avg < 0)
        avg = fabs(avg);
    printEdgeValues (avg, 1, unit);
    
    if (min < 0)
        min = 0;
    printEdgeValues (min, 2, unit);
}

