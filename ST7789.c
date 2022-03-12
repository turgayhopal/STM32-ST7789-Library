/*
 * ST7789.c
 *
 *  Created on: Apr 14, 2020
 *      Author: PC2
 */

#include "ST7789.h"

struct LCD_GPIO LCD_GPIO;

// IO FUNCTIONS

void LCD_setRST(GPIO_TypeDef *PORT, uint16_t PIN) {
	LCD_GPIO.RSTPORT = PORT;
	LCD_GPIO.RSTPIN = PIN;
}

void LCD_setCE(GPIO_TypeDef *PORT, uint16_t PIN) {
	LCD_GPIO.CEPORT = PORT;
	LCD_GPIO.CEPIN = PIN;
}

void LCD_setDC(GPIO_TypeDef *PORT, uint16_t PIN) {
	LCD_GPIO.DCPORT = PORT;
	LCD_GPIO.DCPIN = PIN;
}

void LCD_setSPI(SPI_HandleTypeDef *SPI) {
	LCD_GPIO.SPI = SPI;
}

void RST_SET() {
	HAL_GPIO_WritePin(LCD_GPIO.RSTPORT, LCD_GPIO.RSTPIN, GPIO_PIN_SET);
}
void CE_SET() {
	HAL_GPIO_WritePin(LCD_GPIO.CEPORT, LCD_GPIO.CEPIN, GPIO_PIN_SET);
}
void DC_SET() {
	HAL_GPIO_WritePin(LCD_GPIO.DCPORT, LCD_GPIO.DCPIN, GPIO_PIN_SET);
}

void RST_RESET() {
	HAL_GPIO_WritePin(LCD_GPIO.RSTPORT, LCD_GPIO.RSTPIN, GPIO_PIN_RESET);
}
void CE_RESET() {
	HAL_GPIO_WritePin(LCD_GPIO.CEPORT, LCD_GPIO.CEPIN, GPIO_PIN_RESET);
}
void DC_RESET() {
	HAL_GPIO_WritePin(LCD_GPIO.DCPORT, LCD_GPIO.DCPIN, GPIO_PIN_RESET);
}


// GENERAL FUNCTIONS

void writeCommand(uint8_t cmd) {
	CE_RESET();
	DC_RESET();
	HAL_SPI_Transmit(LCD_GPIO.SPI, &cmd, 1, 100);
	CE_SET();
}
void writeData(uint8_t data) {
	CE_RESET();
	DC_SET();
	HAL_SPI_Transmit(LCD_GPIO.SPI, &data, 1, 100);
	CE_SET();
}

void writeBigData(uint8_t *data, size_t size) {
	CE_RESET();
	DC_SET();
	HAL_SPI_Transmit(LCD_GPIO.SPI, data, size, 100);
	CE_SET();
}

void LCD_setRotation(uint8_t rotation) {

	writeCommand(ST7789_MADCTL);

	if (rotation == 0) {
		writeData(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
		XSTART = COLSTART;
		YSTART = ROWSTART;
		XEND = COLEND;
		YEND = ROWEND + 80;
	} else if (rotation == 1) {
		writeData(ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
		XSTART = ROWSTART;
		YSTART = COLSTART;
		XEND = ROWEND + 80;
		YEND = COLEND;
	} else if (rotation == 2) {
		writeData(ST7789_MADCTL_RGB);
		XSTART = COLSTART;
		YSTART = ROWSTART;
		XEND = COLEND;
		YEND = ROWEND;
	} else if (rotation == 3) {
		writeData(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
		XSTART = ROWSTART;
		YSTART = COLSTART;
		XEND = ROWEND;
		YEND = COLEND;
	}
}

void LCD_setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {

	uint16_t x_start = x + XSTART;
	uint16_t y_start = y + YSTART;

	uint16_t x_end = w + XSTART;
	uint16_t y_end = h + YSTART;

	writeCommand(ST7789_CASET);  // SET COLUMN

	{
		uint8_t data[] = { x_start >> 8, x_start & 0xFF, x_end >> 8, x_end
				& 0xFF };
		writeBigData(data, sizeof(data));
	}

	writeCommand(ST7789_RASET); // SET ROW

	{
		uint8_t data[] = { y_start >> 8, y_start & 0xFF, y_end >> 8, y_end
				& 0xFF };
		writeBigData(data, sizeof(data));
	}

	writeCommand(ST7789_RAMWR); // RAM WRITE

}

void LCD_setPartialArea(uint16_t x, uint16_t y) {

	writeCommand(ST7789_PTLAR);
	uint8_t data[] = { x >> 8, x & 0xFF, y >> 8, y & 0xFF };
	writeBigData(data, sizeof(data));

}
void LCD_Init(uint16_t width, uint16_t height, uint8_t mode) {

	HEIGHT = height;
	WIDTH = width;

	COLSTART = 0;
	ROWSTART = 0;
	COLEND = WIDTH;
	ROWEND = HEIGHT;

	// INIT COMMANDS
	RST_RESET();
	HAL_Delay(50);
	RST_SET();
	HAL_Delay(50);

	writeCommand(ST7789_SWRESET);
	HAL_Delay(120);

	LCD_SLEEP_OFF();

	LCD_setRotation(2);

	LCD_DEFAULT_VOLTAGE_CONTROL();

	LCD_DEFAULT_VOLTAGE_GENERATOR_COLTROL();

	LCD_COLOR_MODE_16_BIT();

	LCD_SLEEP_OFF();

	LCD_IDLE_ON();

	LCD_INVERSION_ON();

	LCD_DEFAULT_ADDRESS();

	if (mode == NORMAL_MODE) {

		LCD_DISPLAY_NORMAL_MODE();
		LCD_DISPLAY_ON();

	} else if (mode == PARTIAL_MODE_SPLIT) {

		LCD_DISPLAY_PARTIAL_MODE();
		LCD_setPartialArea(125, 115);
		LCD_DISPLAY_ON();

	} else if (mode == PARTIAL_MODE_LIMITED) {

		LCD_DISPLAY_PARTIAL_MODE();
		LCD_setPartialArea(40, 200);
		LCD_DISPLAY_ON();
	}

	LCD_setRotation(2);

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LCD_fillColor(uint16_t color) {

	LCD_setAddrWindow(XSTART, YSTART, XEND - 1, YEND - 1);
	for (uint16_t i = XSTART; i < XEND; ++i) {
		for (uint16_t j = YSTART; j < YEND; ++j) {
			uint8_t data[] = { color >> 8, color & 0xFF };
			writeBigData(data, sizeof(data));
		}
	}

}

void LCD_drawPixel(uint16_t x, uint16_t y, uint16_t color) {

	if ((x < 0) || (x >= XEND) || (y < 0) || (y >= YEND)) {
		return;
	}

	LCD_setAddrWindow(x, y, x, y);
	uint8_t data[] = { color >> 8, color & 0xFF };
	writeBigData(data, sizeof(data));
}

void LCD_fill(uint16_t xSta, uint16_t ySta, uint16_t xEnd, uint16_t yEnd,
		uint16_t color) {

	if ((xEnd < 0) || (xEnd >= XEND) || (yEnd < 0) || (yEnd >= YEND)) {
		return;
	}

	uint16_t i, j;
	LCD_setAddrWindow(xSta, ySta, xEnd, yEnd);
	for (i = ySta; i <= yEnd; i++)
		for (j = xSta; j <= xEnd; j++) {
			uint8_t data[] = { color >> 8, color & 0xFF };
			writeBigData(data, sizeof(data));
		}
}

void LCD_drawPixel_4px(uint16_t x, uint16_t y, uint16_t color) {

	if ((x <= 0) || (x > XEND) || (y <= 0) || (y > YEND)) {
		return;
	}

	LCD_fill(x - 1, y - 1, x + 1, y + 1, color);
}

void LCD_drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,

		uint16_t color) {
	uint16_t swap;
	uint16_t steep = ABS(y1 - y0) > ABS(x1 - x0);
	if (steep) {
		swap = x0;
		x0 = y0;
		y0 = swap;

		swap = x1;
		x1 = y1;
		y1 = swap;
	}

	if (x0 > x1) {
		swap = x0;
		x0 = x1;
		x1 = swap;

		swap = y0;
		y0 = y1;
		y1 = swap;
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = ABS(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}

	for (; x0 <= x1; x0++) {
		if (steep) {
			LCD_drawPixel(y0, x0, color);
		} else {
			LCD_drawPixel(x0, y0, color);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

void LCD_drawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
		uint16_t color) {

	LCD_drawLine(x1, y1, x2, y1, color);
	LCD_drawLine(x1, y1, x1, y2, color);
	LCD_drawLine(x1, y2, x2, y2, color);
	LCD_drawLine(x2, y1, x2, y2, color);

}

void LCD_drawCircle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color) {
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	LCD_drawPixel(x0, y0 + r, color);
	LCD_drawPixel(x0, y0 - r, color);
	LCD_drawPixel(x0 + r, y0, color);
	LCD_drawPixel(x0 - r, y0, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		LCD_drawPixel(x0 + x, y0 + y, color);
		LCD_drawPixel(x0 - x, y0 + y, color);
		LCD_drawPixel(x0 + x, y0 - y, color);
		LCD_drawPixel(x0 - x, y0 - y, color);

		LCD_drawPixel(x0 + y, y0 + x, color);
		LCD_drawPixel(x0 - y, y0 + x, color);
		LCD_drawPixel(x0 + y, y0 - x, color);
		LCD_drawPixel(x0 - y, y0 - x, color);
	}
}

void LCD_drawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
		const uint16_t *data) {
	if ((x >= XEND) || (y >= YEND))
		return;
	if ((x + w - 1) >= XEND)
		return;
	if ((y + h - 1) >= YEND)
		return;

	LCD_setAddrWindow(x, y, x + w - 1, y + h - 1);
	writeBigData((uint8_t*) data, sizeof(uint16_t) * w * h);
}


void LCD_writeChar(uint16_t x, uint16_t y, char ch, FontDef font,
		uint16_t color, uint16_t bgcolor) {

	uint32_t i, b, j;
	LCD_setAddrWindow(x, y, x + font.width - 1, y + font.height - 1);

	for (i = 0; i < font.height; i++) {
		b = font.data[(ch - 32) * font.height + i];
		for (j = 0; j < font.width; j++) {
			if ((b << j) & 0x8000) {
				uint8_t data[] = { color >> 8, color & 0xFF };
				writeBigData(data, sizeof(data));
			} else {
				uint8_t data[] = { bgcolor >> 8, bgcolor & 0xFF };
				writeBigData(data, sizeof(data));
			}
		}
	}
}

void LCD_writeString(uint16_t x, uint16_t y, const char *str, FontDef font,
		uint16_t color, uint16_t bgcolor) {

	while (*str) {
		if (x + font.width >= XEND) {
			x = 0;
			y += font.height;
			if (y + font.height >= YEND) {
				break;
			}

			if (*str == ' ') {
				// skip spaces in the beginning of the new line
				str++;
				continue;
			}
		}
		LCD_writeChar(x, y, *str, font, color, bgcolor);
		x += font.width;
		str++;
	}
}

void LCD_drawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
		uint16_t color) {
	uint8_t i;

	/* Check input parameters */
	if (x >= XEND || y >= YEND) {
		/* Return error */
		return;
	}

	/* Check width and height */
	if ((x + w) >= XEND) {
		w = XEND - x;
	}
	if ((y + h) >= YEND) {
		h = YEND - y;
	}

	/* Draw lines */
	for (i = 0; i <= h; i++) {
		/* Draw lines */
		LCD_drawLine(x, y + i, x + w, y + i, color);
	}
}

void LCD_drawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
		uint16_t x3, uint16_t y3, uint16_t color) {

	/* Draw lines */
	LCD_drawLine(x1, y1, x2, y2, color);
	LCD_drawLine(x2, y2, x3, y3, color);
	LCD_drawLine(x3, y3, x1, y1, color);
}

void LCD_drawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2,
		uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color) {

	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, yinc1 =
			0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
			curpixel = 0;

	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	} else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	} else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay) {
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	} else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		LCD_drawLine(x, y, x3, y3, color);

		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
}

void LCD_drawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {


	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	LCD_drawPixel(x0, y0 + r, color);
	LCD_drawPixel(x0, y0 - r, color);
	LCD_drawPixel(x0 + r, y0, color);
	LCD_drawPixel(x0 - r, y0, color);
	LCD_drawLine(x0 - r, y0, x0 + r, y0, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		LCD_drawLine(x0 - x, y0 + y, x0 + x, y0 + y, color);
		LCD_drawLine(x0 + x, y0 - y, x0 - x, y0 - y, color);

		LCD_drawLine(x0 + y, y0 + x, x0 - y, y0 + x, color);
		LCD_drawLine(x0 + y, y0 - x, x0 - y, y0 - x, color);
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LCD_SLEEP_ON(void) {
	writeCommand(ST7789_SLPIN);
	HAL_Delay(120);
}
void LCD_SLEEP_OFF(void) {
	writeCommand(ST7789_SLPOUT);
	HAL_Delay(120);
}
void LCD_DISPLAY_PARTIAL_MODE(void) {
	writeCommand(ST7789_PTLON);
}
void LCD_DISPLAY_NORMAL_MODE(void) {
	writeCommand(ST7789_NORON);
}
void LCD_INVERSION_ON(void) {
	writeCommand(ST7789_INVON);
}
void LCD_INVERSION_OFF(void) {
	writeCommand(ST7789_INVOFF);
}
void LCD_DISPLAY_ON(void) {
	writeCommand(ST7789_DISPON);
}
void LCD_DISPLAY_OFF(void) {
	writeCommand(ST7789_DISPOFF);
}
void LCD_IDLE_ON(void) {
	writeCommand(ST7789_IDMOFF);
}
void LCD_IDLE_OFF(void) {
	writeCommand(ST7789_IDMON);
}
void LCD_TEARING_EFFECT_ON_0(void) {
	writeCommand(ST7789_TEON);
	writeData(0x00);
}
void LCD_TEARING_EFFECT_ON_1(void) {
	writeCommand(ST7789_TEON);
	writeData(0x01);
}
void LCD_TEARING_EFFECT_OFF(void) {
	writeCommand(ST7789_TEOFF);
}
void LCD_COLOR_MODE_16_BIT() {
	writeCommand(ST7789_COLMOD);
	writeData(ST7789_COLOR_MODE_16BIT);
}
void LCD_COLOR_MODE_18_BIT() {
	writeCommand(ST7789_COLMOD);
	writeData(ST7789_COLOR_MODE_18BIT);
}
void LCD_DEFAULT_ADDRESS() {
	writeCommand(ST7789_CASET);
	{
		uint8_t data[] = { COLSTART >> 8, COLSTART & 0xFF, WIDTH >> 8, WIDTH
				& 0xFF };
		writeBigData(data, sizeof(data));
	}

	writeCommand(ST7789_RASET);
	{
		uint8_t data[] = { ROWSTART >> 8, ROWSTART & 0xFF, HEIGHT >> 8, HEIGHT
				& 0xFF };
		writeBigData(data, sizeof(data));
	}

	writeCommand(ST7789_RAMWR);
}
void LCD_DEFAULT_VOLTAGE_CONTROL() {
	writeCommand(ST7789_PVGAMCTRL);
	{
		uint8_t data[] = { 0xD0, 0x00, 0x02, 0x07, 0x0B, 0x1A, 0x31, 0x54, 0x40,
				0x29, 0x12, 0x12, 0x12, 0x17 };
		writeBigData(data, sizeof(data));
	}

	writeCommand(ST7789_NVGAMCTRL);
	{
		uint8_t data[] = { 0xD0, 0x00, 0x02, 0x07, 0x05, 0x25, 0x2D, 0x44, 0x44,
				0x1C, 0x18, 0x16, 0x1C, 0x1D };
		writeBigData(data, sizeof(data));
	}
}

void LCD_DEFAULT_VOLTAGE_GENERATOR_COLTROL() {

	writeCommand(ST7789_PORCTRL);
	{
		uint8_t data[] = { 0x0C, 0x0C, 0x00, 0x33, 0x33 };
		writeBigData(data, sizeof(data));
	}

	writeCommand(ST7789_FRCTRL1);
	{
		uint8_t data[] = { 0x00, 0x0F, 0x0F };
		writeBigData(data, sizeof(data));
	}

	writeCommand(ST7789_GCTRL);
	writeData(0x35);

	writeCommand(ST7789_VCOMS);
	writeData(0x20);

	writeCommand(ST7789_LCMCTRL);
	writeData(0x2C);

	writeCommand(ST7789_VDVVRHEN);
	{
		uint8_t data[] = { 0x01, 0xFF };
		writeBigData(data, sizeof(data));
	}

	writeCommand(ST7789_VRHS);
	writeData(0x0B);

	writeCommand(ST7789_VDVS);
	writeData(0x20);

	writeCommand(ST7789_VCMOFFSET);
	writeData(0x20);

	writeCommand(ST7789_FRCTRL2);
	writeData(0x0F);

	writeCommand(ST7789_PWRCTRL);
	{
		uint8_t data[] = { 0xA4, 0x81 };
		writeBigData(data, sizeof(data));
	}

	writeCommand(ST7789_GATECTRL);
	{
		uint8_t data[] = { 0x27, 0x00, 0x10 };
		writeBigData(data, sizeof(data));
	}

}
