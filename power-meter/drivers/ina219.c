/*
 * ina219.c
 *
 * Created: 2017-08-04 11:48:55
 *  Author: mikael
 */ 

#include <avr/io.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#include "ina219.h"
#include "USI_TWI_Master.h"

static uint8_t ina219_addr;
static uint8_t ina219_message[4];

static void writeRegister (uint8_t reg, uint16_t value) 
{
	ina219_message[0] = (ina219_addr << 1);
	ina219_message[1] = reg;
	ina219_message[2] = (value >> 8) & 0xFF;
	ina219_message[3] = value & 0xFF;
	
	USI_TWI_Start_Transceiver_With_Data(ina219_message, 4);
}

static void readRegister (uint8_t reg, uint16_t *value) 
{
	ina219_message[0] = (ina219_addr << 1);
	ina219_message[1] = reg;
	
	USI_TWI_Start_Transceiver_With_Data(ina219_message, 2);
	_delay_ms(1);
	
	ina219_message[0] = (ina219_addr << 1) | 0x01;	// I2C read
	USI_TWI_Start_Transceiver_With_Data(ina219_message, 3);
	*value = (ina219_message[1] << 8) | ina219_message[2];
}

void ina219_begin (uint8_t addr)
{
	ina219_addr = addr;
}

static int16_t getBusVoltage_raw() {
	uint16_t value;
	readRegister(INA219_REG_BUSVOLTAGE, &value);
	return (int16_t)((value >> 3) * 4);
}

extern void ina219_calibrate (INA219_Calibration_t mode);
extern uint16_t ina219_getBusVoltage();
extern uint16_t ina219_getShuntVoltage();
extern uint16_t ina219_getCurrent();
