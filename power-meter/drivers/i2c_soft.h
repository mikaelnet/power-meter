#ifndef _I2C_SOFT_H
#define _I2C_SOFT_H

#include <avr/io.h>
#include <inttypes.h>
#include <stdbool.h>

//#define TWI_FAST_MODE

#ifdef F_CPU
    #define SYS_CLK   (F_CPU/1000.0)
#else
    #define SYS_CLK   4000.0  // [kHz]
#endif

#ifdef TWI_FAST_MODE               // TWI FAST mode timing limits. SCL = 100-400kHz
    #define T2_TWI    ((SYS_CLK *1300) /1000000) +1 // >1,3us
    #define T4_TWI    ((SYS_CLK * 600) /1000000) +1 // >0,6us
#else                              // TWI STANDARD mode timing limits. SCL <= 100kHz
    #define T2_TWI    ((SYS_CLK *4700) /1000000) +1 // >4,7us
    #define T4_TWI    ((SYS_CLK *4000) /1000000) +1 // >4,0us
#endif


#if defined(__AVR_AT90Mega169__) | defined(__AVR_ATmega169PA__) | \
    defined(__AVR_AT90Mega165__) | defined(__AVR_ATmega165__) | \
    defined(__AVR_ATmega325__) | defined(__AVR_ATmega3250__) | \
    defined(__AVR_ATmega645__) | defined(__AVR_ATmega6450__) | \
    defined(__AVR_ATmega329__) | defined(__AVR_ATmega3290__) | \
    defined(__AVR_ATmega649__) | defined(__AVR_ATmega6490__)
    #define DDR_USI             DDRE
    #define PORT_USI            PORTE
    #define PIN_USI             PINE
    #define PORT_USI_SDA        PORTE5
    #define PORT_USI_SCL        PORTE4
    #define PIN_USI_SDA         PINE5
    #define PIN_USI_SCL         PINE4
#elif defined(__AVR_ATtiny25__) | defined(__AVR_ATtiny45__) | defined(__AVR_ATtiny85__) | \
    defined(__AVR_AT90Tiny26__) | defined(__AVR_ATtiny26__)
    #define DDR_USI             DDRB
    #define PORT_USI            PORTB
    #define PIN_USI             PINB
    #define PORT_USI_SDA        PORTB0
    #define PORT_USI_SCL        PORTB2
    #define PIN_USI_SDA         PINB0
    #define PIN_USI_SCL         PINB2
#elif defined(__AVR_AT90Tiny2313__) | defined(__AVR_ATtiny2313__)
    #define DDR_USI             DDRB
    #define PORT_USI            PORTB
    #define PIN_USI             PINB
    #define PORT_USI_SDA        PORTB5
    #define PORT_USI_SCL        PORTB7
    #define PIN_USI_SDA         PINB5
    #define PIN_USI_SCL         PINB7
#else
    #error I2C ports needs to be defined
#endif


#define SOFT_I2C_SDA_LOW        DDR_USI |= (_BV(PORT_USI_SDA))
#define SOFT_I2C_SDA_HIGH       DDR_USI &= (~_BV(PORT_USI_SDA))

#define SOFT_I2C_SCL_LOW        DDR_USI |= (_BV(PORT_USI_SCL))
#define SOFT_I2C_SCL_HIGH       DDR_USI &= (~_BV(PORT_USI_SCL))

#define I2C_READ_ADDR(addr)     ((addr<<1)|1)
#define I2C_WRITE_ADDR(addr)    ((addr<<1))

extern void i2c_begin();	
extern void i2c_start();
extern void i2c_stop();

extern bool i2c_writeByte (uint8_t data);
extern uint8_t i2c_readByte (bool ack);
extern bool i2c_writeData (uint8_t *data, uint8_t length);

#endif /* _I2C_SOFT_H */