/*
 * ST7789.h
 *
 *  Created on: Apr 14, 2020
 *      Author: PC2
 */

#ifndef INC_ST7789_H_
#define INC_ST7789_H_

#include "main.h"
#include "stdio.h"
#include "stdbool.h"
#include "fonts.h"

#define ST7789_NOP			 		0x00
#define ST7789_SWRESET		 		0x01
#define ST7789_RDDID				0x04
#define ST7789_RDDST		 		0x09

#define ST7789_SLPIN		 		0x10
#define ST7789_SLPOUT			    0x11
#define ST7789_PTLON				0x12
#define ST7789_NORON		 		0x13

#define ST7789_INVOFF				0x20
#define ST7789_INVON				0x21
#define ST7789_DISPOFF				0x28
#define ST7789_DISPON		 		0x29
#define ST7789_CASET		 		0x2A
#define ST7789_RASET		 		0x2B
#define ST7789_RAMWR		 		0x2C
#define ST7789_RAMRD		 		0x2E

#define ST7789_PORCTRL				0xB2
#define ST7789_FRCTRL1				0xB3
#define ST7789_FRCTRL2				0xC6
#define ST7789_GCTRL				0xB7
#define ST7789_GATECTRL				0xE4
#define ST7789_PVGAMCTRL			0xE0
#define ST7789_NVGAMCTRL			0xE1
#define ST7789_VCOMS				0xBB
#define ST7789_LCMCTRL				0xC0
#define ST7789_VDVVRHEN				0xC2
#define ST7789_VRHS					0xC3
#define ST7789_VDVS					0xC4
#define ST7789_VCMOFFSET			0xC5
#define ST7789_PWRCTRL				0xD0

#define ST7789_PTLAR		 		0x30
#define ST7789_TEOFF		 		0x34
#define ST7789_TEON			 		0x35
#define ST7789_MADCTL		 		0x36
#define ST7789_IDMOFF				0x38
#define ST7789_IDMON				0x39
#define ST7789_COLMOD		 		0x3A

#define ST7789_MADCTL_MY	 		0x80
#define ST7789_MADCTL_MX	 		0x40
#define ST7789_MADCTL_MV	 		0x20
#define ST7789_MADCTL_ML	 		0x10
#define ST7789_MADCTL_RGB	 		0x00

#define ST7789_COLOR_MODE_16BIT		0x55
#define ST7789_COLOR_MODE_18BIT		0x66

#define ST7789_MATCTL_DEFAULT		0x08

// Some ready-made 16-bit ('565') color settings:

#define ST7789_BLACK		 		0x0000
#define ST7789_WHITE		 		0xFFFF
#define ST7789_RED			 		0xF800
#define ST7789_GREEN		 		0x07E0
#define ST7789_BLUE			 		0x001F
#define ST7789_CYAN			 		0x07FF
#define ST7789_MAGENTA		 		0xF81F
#define ST7789_YELLOW		 		0xFFE0
#define ST7789_ORANGE		 		0xFC00

#define NORMAL_MODE					0
#define PARTIAL_MODE_SPLIT			1
#define PARTIAL_MODE_LIMITED		2

#define ABS(x) ((x) > 0 ? (x) : -(x))

struct LCD_GPIO {
	GPIO_TypeDef *RSTPORT;
	uint16_t RSTPIN;

	GPIO_TypeDef *DCPORT;
	uint16_t DCPIN;

	GPIO_TypeDef *CEPORT;
	uint16_t CEPIN;

	SPI_HandleTypeDef *SPI;
};

// IO FUNCTIONS

void LCD_setRST(GPIO_TypeDef *PORT, uint16_t PIN);
void LCD_setCE(GPIO_TypeDef *PORT, uint16_t PIN);
void LCD_setDC(GPIO_TypeDef *PORT, uint16_t PIN);
void LCD_setSPI(SPI_HandleTypeDef *SPI);

void RST_SET();
void CE_SET();
void DC_SET();

void RST_RESET();
void CE_RESET();
void DC_RESET();

// GENERAL FUNCTIONS

void LCD_Init(uint16_t width, uint16_t height, uint8_t mode);
void LCD_setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void LCD_setRotation(uint8_t rotation);
void LCD_setPartialArea(uint16_t x, uint16_t y);
void LCD_fillColor(uint16_t color);


void LCD_drawPixel(uint16_t x, uint16_t y, uint16_t color);
void LCD_fill(uint16_t xSta, uint16_t ySta, uint16_t xEnd, uint16_t yEnd, uint16_t color);
void LCD_drawPixel_4px(uint16_t x, uint16_t y, uint16_t color);

/* Graphical functions. */
void LCD_drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_drawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_drawCircle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);
void LCD_drawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data);

/* Text functions. */
void LCD_writeChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor);
void LCD_writeString(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor);

/* Extented Graphical functions. */
void LCD_drawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void LCD_drawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color);
void LCD_drawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color);
void LCD_drawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

void writeCommand(uint8_t cmd);
void writeData(uint8_t data);
void writeBigData(uint8_t *data, size_t size);

// PRIVATE

void LCD_SLEEP_ON(void);
void LCD_SLEEP_OFF(void);
void LCD_DISPLAY_PARTIAL_MODE(void);
void LCD_DISPLAY_NORMAL_MODE(void);
void LCD_INVERSION_ON(void);
void LCD_INVERSION_OFF(void);
void LCD_DISPLAY_ON(void);
void LCD_DISPLAY_OFF(void);
void LCD_IDLE_ON(void);
void LCD_IDLE_OFF(void);
void LCD_TEARING_EFFECT_ON_0(void);
void LCD_TEARING_EFFECT_ON_1(void);
void LCD_TEARING_EFFECT_OFF(void);
void LCD_COLOR_MODE_16_BIT();
void LCD_COLOR_MODE_18_BIT();
void LCD_DEFAULT_ADDRESS();
void LCD_DEFAULT_VOLTAGE_CONTROL();
void LCD_DEFAULT_VOLTAGE_GENERATOR_COLTROL();

// GENEREAL STATIC DEFINATOINS

uint16_t WIDTH;
uint16_t HEIGHT;

uint8_t XSTART;
uint8_t YSTART;
uint16_t XEND;
uint16_t YEND;

uint8_t COLSTART;
uint8_t ROWSTART;
uint16_t COLEND;
uint16_t ROWEND;

#endif /* INC_ST7789_H_ */
