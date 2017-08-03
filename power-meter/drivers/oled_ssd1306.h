/*
 * oled_ssd1306.h
 *
 * Created: 2017-08-03 16:46:56
 *  Author: mikael
 */ 


#ifndef OLED_SSD1306_H_
#define OLED_SSD1306_H_

#include <inttypes.h>
#include <stdbool.h>

#define SSD1306_I2C_ADDRESS         0x3C    // 011110+SA0+RW, 128x32
#if defined SSD1306_128_64
    #define SSD1306_LCDWIDTH                  128
    #define SSD1306_LCDHEIGHT                 64
#elif defined SSD1306_128_32
    #define SSD1306_LCDWIDTH                  128
    #define SSD1306_LCDHEIGHT                 32
#elif defined SSD1306_96_16
    #define SSD1306_LCDWIDTH                  96
    #define SSD1306_LCDHEIGHT                 16
#else
    #error No OLED display type defined
#endif

#define SSD1306_SETCONTRAST         0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON        0xA5
#define SSD1306_NORMALDISPLAY       0xA6
#define SSD1306_INVERTDISPLAY       0xA7
#define SSD1306_DISPLAYOFF          0xAE
#define SSD1306_DISPLAYON           0xAF

#define SSD1306_SETDISPLAYOFFSET    0xD3
#define SSD1306_SETCOMPINS          0xDA

#define SSD1306_SETVCOMDETECT       0xDB

#define SSD1306_SETDISPLAYCLOCKDIV  0xD5
#define SSD1306_SETPRECHARGE        0xD9

#define SSD1306_SETMULTIPLEX        0xA8

#define SSD1306_SETLOWCOLUMN        0x00
#define SSD1306_SETHIGHCOLUMN       0x10

#define SSD1306_SETSTARTLINE        0x40

#define SSD1306_MEMORYMODE          0x20
#define SSD1306_COLUMNADDR          0x21
#define SSD1306_PAGEADDR            0x22

#define SSD1306_COMSCANINC          0xC0
#define SSD1306_COMSCANDEC          0xC8

#define SSD1306_SEGREMAP            0xA0

#define SSD1306_CHARGEPUMP          0x8D

#define SSD1306_EXTERNALVCC         0x1
#define SSD1306_SWITCHCAPVCC        0x2

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A

typedef struct SSD1306_Datagram {
    uint8_t i2caddr;
    uint8_t mode;
    uint8_t buffer[SSD1306_LCDWIDTH];
} SSD1306_Datagram_t;
//extern uint8_t* ssd1306_data;

extern void ssd1306_begin(uint8_t vccstate, uint8_t i2caddr);
extern void ssd1306_command(uint8_t command);
extern void ssd1306_display();
extern void ssd1306_clear();
extern void ssd1306_dim(bool dim);

extern void ssd1306_setActiveArea (uint8_t startColumn, uint8_t endColumn, uint8_t startPage, uint8_t endPage);
extern void ssd1306_writeData(SSD1306_Datagram_t *data, uint8_t lenght);

#endif /* OLED_SSD1306_H_ */