/*
 * display.h
 *
 * Created: 2017-08-06 22:16:22
 *  Author: mikael
 */ 


#ifndef DISPLAY_H_
#define DISPLAY_H_

extern uint8_t *drawChar (uint8_t *ptr, uint8_t ch);
extern uint8_t drawString(uint8_t *ptr, char *str);

extern void printClock (uint16_t clock);
extern void printVoltage (float lastVoltage);
extern void printCurrent (float current);
extern void printPower (float power);
extern void printCharge (float value);
extern void printMaxMinCurrent (float min, float max, float avg);

#endif /* DISPLAY_H_ */