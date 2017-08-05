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

#include <util/delay.h>

#include "ina219.h"
#include "i2c_soft.h"

static uint8_t i2c_addr;
//static uint8_t ina219_message[4];

static void writeRegister (uint8_t reg, uint16_t value) 
{
    i2c_start();
    i2c_writeByte(I2C_WRITE_ADDR(i2c_addr));
    i2c_writeByte(reg);
    i2c_writeByte((value >> 8) & 0xFF);
    i2c_writeByte(value & 0xFF);
    i2c_stop();
}

static uint16_t readRegister (uint8_t reg) 
{
    uint16_t value;

    i2c_start();
    i2c_writeByte(I2C_WRITE_ADDR(i2c_addr));
    i2c_writeByte(reg);
    i2c_stop();

    _delay_ms(1);

    i2c_start();
    i2c_writeByte(I2C_READ_ADDR(i2c_addr));
    value = i2c_readByte(true) << 8;
    value |= i2c_readByte(true);
    i2c_stop();

    return value;
}

void ina219_begin (uint8_t addr)
{
    i2c_addr = addr;
}

static int16_t getBusVoltage_raw() {
    uint16_t value = readRegister(INA219_REG_BUSVOLTAGE);
    return (int16_t)((value >> 3) * 4);
}

static uint32_t ina219_calValue;
static uint32_t ina219_currentDivider_mA;
static uint32_t ina219_powerDivider_mW;

void ina219_calibrate (INA219_Calibration_t mode) 
{
    uint16_t config;
    switch (mode) {
        case Mode_32V_2A:
            ina219_calValue = 4096;
            ina219_currentDivider_mA = 10;
            ina219_powerDivider_mW = 2;

            writeRegister(INA219_REG_CALIBRATION, ina219_calValue);
            config = INA219_CONFIG_BVOLTAGERANGE_32V |
                INA219_CONFIG_GAIN_8_320MV |
                INA219_CONFIG_BADCRES_12BIT |
                INA219_CONFIG_SADCRES_12BIT_1S_532US |
                INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
            writeRegister(INA219_REG_CONFIG, config);
            break;
        
        case Mode_32V_1A:
            ina219_calValue = 10240;
            ina219_currentDivider_mA = 25;  
            ina219_powerDivider_mW = 1;  
            writeRegister(INA219_REG_CALIBRATION, ina219_calValue);
            config = INA219_CONFIG_BVOLTAGERANGE_32V |
                INA219_CONFIG_GAIN_8_320MV |
                INA219_CONFIG_BADCRES_12BIT |
                INA219_CONFIG_SADCRES_12BIT_1S_532US |
                INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
            writeRegister(INA219_REG_CONFIG, config);
            break;

        case Mode_16V_400mA:
            ina219_calValue = 8192;
            ina219_currentDivider_mA = 20;
            ina219_powerDivider_mW = 1;
            writeRegister(INA219_REG_CALIBRATION, ina219_calValue);
            config = INA219_CONFIG_BVOLTAGERANGE_16V |
                INA219_CONFIG_GAIN_1_40MV |
                INA219_CONFIG_BADCRES_12BIT |
                INA219_CONFIG_SADCRES_12BIT_1S_532US |
                INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
            writeRegister(INA219_REG_CONFIG, config);
            break;
    }
}

int16_t ina219_getBusVoltage() {
    return getBusVoltage_raw();
}

int16_t ina219_getShuntVoltage()
{
    uint16_t value;
    value = readRegister(INA219_REG_SHUNTVOLTAGE);
    return (int16_t)value;
}

float ina219_getCurrent_mA()
{
    uint16_t value;

    // Sometimes a sharp load will reset the INA219, which will
    // reset the cal register, meaning CURRENT and POWER will
    // not be available ... avoid this by always setting a cal
    // value even if it's an unfortunate extra step
    writeRegister(INA219_REG_CALIBRATION, ina219_calValue);

    // Now we can safely read the CURRENT register!
    value = readRegister(INA219_REG_CURRENT);

    float valueDec = (float)value / ina219_currentDivider_mA;
    return valueDec;
}
