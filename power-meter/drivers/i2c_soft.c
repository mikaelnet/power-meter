/**********************************************************

Software I2C Library for AVR Devices.

Copyright 2008-2012
eXtreme Electronics, India
www.eXtremeElectronics.co.in
**********************************************************/
 
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <stdbool.h>

#include "i2c_soft.h"

//#define Q_DEL _delay_loop_1(3)
//#define H_DEL _delay_loop_1(5)

void i2c_begin()
{
    PORT_USI &= ~(_BV(PORT_USI_SDA) | _BV(PORT_USI_SCL));

    SOFT_I2C_SDA_HIGH;
    SOFT_I2C_SCL_HIGH;
}

void i2c_start()
{
    SOFT_I2C_SCL_HIGH;
    _delay_us (T4_TWI/4);

    SOFT_I2C_SDA_LOW;
    _delay_us (T4_TWI/4);
}

void i2c_stop()
{
    SOFT_I2C_SDA_LOW;
    _delay_us (T4_TWI/4);

    SOFT_I2C_SCL_HIGH;
    _delay_us (T2_TWI/4);;

    SOFT_I2C_SDA_HIGH;
    _delay_us (T4_TWI/4);
}

bool i2c_writeByte (uint8_t data)
{
    for (uint8_t i=0 ; i < 8 ; i++) {
        SOFT_I2C_SCL_LOW;
        _delay_us (T2_TWI/4);;

        if(data & 0x80)
            SOFT_I2C_SDA_HIGH;
        else
            SOFT_I2C_SDA_LOW;
        _delay_us (T4_TWI/4);

        SOFT_I2C_SCL_HIGH;
        _delay_us (T4_TWI/4);

        do {
            // Wait for SCL pin to go high
        } while( !(PIN_USI & _BV(PIN_USI_SCL)) );
        
        data <<= 1;
    }

    // The 9th clock (ACK Phase)
    SOFT_I2C_SCL_LOW;
    _delay_us (T2_TWI/4);;

    SOFT_I2C_SDA_HIGH;
    _delay_us (T4_TWI/4);

    SOFT_I2C_SCL_HIGH;
    _delay_us (T4_TWI/4);

    bool ack = !(PIN_USI & _BV(PIN_USI_SDA));

    SOFT_I2C_SCL_LOW;
    _delay_us (T4_TWI/4);

    return ack;
}


uint8_t i2c_readByte (bool ack)
{
    uint8_t data = 0x00;

    for (uint8_t i=0 ; i < 8 ; i++) {
        SOFT_I2C_SCL_LOW;
        _delay_us (T4_TWI/4);
        SOFT_I2C_SCL_HIGH;
        _delay_us (T4_TWI/4);

        do {
            // Wait for SCL pin to go high
        } while( !(PIN_USI & _BV(PIN_USI_SCL)) );

        if (PIN_USI & _BV(PIN_USI_SDA))
            data |= (0x80 >> i);
    }

    // Ack phase
    SOFT_I2C_SCL_LOW;
    _delay_us (T2_TWI/4);;

    if (ack) {
        SOFT_I2C_SDA_LOW;
    }
    else {
        SOFT_I2C_SDA_HIGH;
    }
    _delay_us (T4_TWI/4);

    SOFT_I2C_SCL_HIGH;
    _delay_us (T4_TWI/4);

    SOFT_I2C_SCL_LOW;
    _delay_us (T4_TWI/4);

    if (ack) {
        SOFT_I2C_SDA_HIGH;
    }

    return data;
}


bool i2c_writeData (uint8_t *data, uint8_t length)
{
    bool ack = true;
    while (length > 0) {
        ack &= i2c_writeByte(*data);
        data ++;
        length --;
    }
    return ack;
}

/*
// For backward compatibility:
bool i2c_startTransceiverWithData (uint8_t *data, uint8_t length)
{
    if ( *data & _BV(0) ) {
        // read
        if (length < 3) // Nothing to read
            return false;

        i2c_start();
        i2c_writeByteRaw(data[0]);
        for (uint8_t i=1 ; i < length ; i ++) {
            data[i] = i2c_readByteRaw(true);
        }
        i2c_stop();
    }
    else {
        // write
        if (length < 2) // Nothing to write
            return false;

        i2c_start();
        for (uint8_t i=0 ; i < length ; i ++) {
            i2c_writeByteRaw(data[i]);
        }
        i2c_stop();
    }
    return true;
}
*/