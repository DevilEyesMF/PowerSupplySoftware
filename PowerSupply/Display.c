/*
 * Display.c
 *
 * Created: 08/05/2023 13:52:38
 *  Author: Remko Claes
 */

/* Includes */
#include <avr/io.h>

#include "main.h"
#include "display.h"
#include "util/delay.h"

void DisplayInit()
{
	BIT_CLEAR(DISPLAY_CTL, DISPLAY_RW);
	BIT_CLEAR(DISPLAY_CTL, DISPLAY_RS);
	BIT_CLEAR(DISPLAY_CTL, DISPLAY_EN);

	/*
	 * Function set
	 * bit 4: Data bus length = 8
	 * bit 3: Number of lines = 2
	 * bit 2: font size = 5x8
	 */
	_delay_ms(15);
	DISPLAY_DATA = 0b00111000;
	DisplayEnablePulse();

	_delay_ms(5);
	DISPLAY_DATA = 0b00111000;
	DisplayEnablePulse();

	_delay_us(100);
	DISPLAY_DATA = 0b00111000;
	DisplayEnablePulse();

	/*
	 * Display on
	 * bit 2: Display on
	 * bit 1: Cursor
	 * bit 0: Cursor position
	 */
	DISPLAY_DATA = 0b00001101;
	DisplayEnablePulse();

	/*
	 * Entry mode set
	 * bit 1: Direction
	 * bit 0: Display shift
	 */
	DISPLAY_DATA = 0b00000100;
	DisplayEnablePulse();

	/* Display clear */
	DISPLAY_DATA = 0b00000001;
	DisplayEnablePulse();
	_delay_ms(2);
}

void DisplayUpdate(uint16_t setVoltage, uint16_t measuredVoltage, uint16_t setCurrent, uint16_t measuredCurrent)
{
	/* Declare local char arrays for ASCII codes */
	char c_setVoltage[6], c_measuredVoltage[6], c_setCurrent[6], c_measuredCurrent[6];

	/* Transform values to ASCII */
	IntegerToASCII(setVoltage, c_setVoltage);
	IntegerToASCII(measuredVoltage, c_measuredVoltage);
	IntegerToASCII(setCurrent, c_setCurrent);
	IntegerToASCII(measuredCurrent, c_measuredCurrent);

	/* Format the strings for the LCD */
	FormatValue(c_setVoltage);
	FormatValue(c_measuredVoltage);
	FormatValue(c_setCurrent);
	FormatValue(c_measuredCurrent);

	BIT_CLEAR(DISPLAY_CTL, DISPLAY_RS);
	DISPLAY_DATA = 0b10;
	DisplayEnablePulse();

	/* Print set voltage */
	DisplaySetDDRAM(ADDR_VOLTAGE_SET);

	for (int i = 0; i < 6; i++)
	{
		DisplayWriteChar(c_setVoltage[i]);
	}
	DisplayWriteChar('V');

	/* Print measured voltage */
	DisplaySetDDRAM(ADDR_VOLTAGE_MEAS);

	for (int i = 0; i < 6; i++)
	{
		DisplayWriteChar(c_measuredVoltage[i]);
	}
	DisplayWriteChar('V');

	/* Print set current */
	DisplaySetDDRAM(ADDR_CURRENT_SET);

	for (int i = 0; i < 6; i++)
	{
		DisplayWriteChar(c_setCurrent[i]);
	}
	DisplayWriteChar('A');

	/* Print measured current */
	DisplaySetDDRAM(ADDR_CURRENT_MEAS);

	for (int i = 0; i < 6; i++)
	{
		DisplayWriteChar(c_measuredCurrent[i]);
	}
	DisplayWriteChar('A');
}

void DisplaySetDDRAM(uint8_t addressRAM)
{
	BIT_CLEAR(DISPLAY_CTL, DISPLAY_RS);

	DISPLAY_DATA = addressRAM | 0b10000000;
	DisplayEnablePulse();
}

void DisplayWriteChar(char c)
{
	BIT_SET(DISPLAY_CTL, DISPLAY_RS);

	DISPLAY_DATA = c;
	DisplayEnablePulse();
}

void DisplayEnablePulse()
{
	_delay_us(5);
	BIT_SET(DISPLAY_CTL, DISPLAY_EN);
	_delay_us(5);
	BIT_CLEAR(DISPLAY_CTL, DISPLAY_EN);
	_delay_us(40);
}

void IntegerToASCII(uint16_t number, char *c_number)
{
	/* Separate the digits */
    for (int i = 0, j = 10000; i < 5; i++, j/=10)
    {
	    c_number[i] = number / j + 0x30; // + 0x30 to convert to ASCII
	    number %= j;
    }
}

void FormatValue(char c[6])
{
	/* Swap first zero with space if needed */
	if (c[0] == '0')
	{
		c[0] = ' ';
	}

	for (int i = 5; i > 2; i--)
	{
		c[i] = c[i-1];
	}

	c[2] = ',';
}