/*
 * oled_ssd1306.c
 *
 * Created: 2017-08-03 16:48:55
 *  Author: mikael
 */ 

#include <avr/io.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#include "oled_ssd1306.h"
#include "USI_TWI_Master.h"

static uint8_t i2caddr;
static uint8_t vccstate;
static uint8_t command_message[10];

void ssd1306_command(uint8_t command)
{
    uint8_t *ptr = command_message;
    *ptr++ = i2caddr << 1;
    *ptr++ = 0x00;
    *ptr = command;
    USI_TWI_Start_Transceiver_With_Data(command_message, 2+1);
}

void ssd1306_begin(uint8_t vccState, uint8_t i2cAddr)
{
    i2caddr = i2cAddr;
    vccstate = vccState;

    command_message[0] = i2caddr << 1;
    command_message[1] = 0x00; // Co = 0, D/C = 0

    // Init sequence
    ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE

    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
    ssd1306_command(0x80);                                  // the suggested ratio 0x80

    ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
    ssd1306_command(SSD1306_LCDHEIGHT - 1);                 // 0x7F or 0x5F
    
    ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
    ssd1306_command(0x0);                                   // no offset
    ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
    ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
    ssd1306_command(vccstate == SSD1306_EXTERNALVCC ? 0x10 : 0x14);
    ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
    ssd1306_command(0x00);                                  // 0x0 act like ks0108
    ssd1306_command(SSD1306_SEGREMAP | 0x1);                // 0xA1
    ssd1306_command(SSD1306_COMSCANDEC);                    // 0xC8

#if defined SSD1306_128_32
    ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
    ssd1306_command(0x02);
    ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
    ssd1306_command(0x8F);
#elif defined SSD1306_128_64
    ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
    ssd1306_command(0x12);
    ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
    ssd1306_command(vccstate == SSD1306_EXTERNALVCC ? 0x9F 0xCF);
#elif defined SSD1306_96_16
    ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
    ssd1306_command(0x2);   //ada x12
    ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
    ssd1306_command(vccstate == SSD1306_EXTERNALVCC ? 0x10 : 0xAF);
#else
    #error No OLED display type defined
#endif

    ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xD9
    ssd1306_command(vccstate == SSD1306_EXTERNALVCC ? 0x22 : 0xF1);
    ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
    ssd1306_command(0x40);
    ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
    ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6

    ssd1306_command(SSD1306_DEACTIVATE_SCROLL);             // 0x2E

    ssd1306_command(SSD1306_DISPLAYON);                     // 0xAF
}

void ssd1306_setActiveArea (uint8_t startColumn, uint8_t endColumn, uint8_t startPage, uint8_t endPage)
{
    uint8_t *ptr = command_message;
    *ptr++ = i2caddr << 1;
    *ptr++ = 0x00;
    *ptr++ = SSD1306_COLUMNADDR;
    *ptr++ = startColumn;
    *ptr++ = endColumn;
    *ptr++ = SSD1306_PAGEADDR;
    *ptr++ = startPage;
    *ptr = endPage;
    USI_TWI_Start_Transceiver_With_Data(command_message, 2+6);
}

void ssd1306_writeData(SSD1306_Datagram_t *data, uint8_t lenght)
{
    data->i2caddr = i2caddr << 1;
    data->mode = 0x40;
    USI_TWI_Start_Transceiver_With_Data(&(data->i2caddr), lenght+2);
}

void ssd1306_clear() {
#if SSD1306_LCDHEIGHT == 64
    uint8_t endPage = 7;
#elif SSD1306_LCDHEIGHT == 32
    uint8_t endPage = 3;
#elif SSD1306_LCDHEIGHT == 16
    uint8_t endPage = 1;
#endif
    ssd1306_setActiveArea(0, SSD1306_LCDWIDTH-1, 0, endPage);

    uint8_t buffer[10];
    memset(buffer, 0x00, sizeof(buffer));
    buffer[0] = i2caddr << 1;
    buffer[1] = 0x40;   // D/C# Data
    for (uint8_t i = 0 ; i < (SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8)/(sizeof(buffer)-2) ; i ++) {
        USI_TWI_Start_Transceiver_With_Data(buffer, sizeof(buffer));
    }
}

void ssd1306_dim(bool dim)
{
    uint8_t contrast;
    if (dim) {
        contrast = 0;
    }
    else {
        contrast = vccstate == SSD1306_EXTERNALVCC ? 0x9F : 0xCF;
    }

    ssd1306_command(SSD1306_SETCONTRAST);
    ssd1306_command(contrast);
}

