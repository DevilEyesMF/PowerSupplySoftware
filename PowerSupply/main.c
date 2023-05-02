/*
 * PowerSupply.c
 *
 * Created: 01/05/2023 22:01:19
 * Author : Remko Claes
 */ 

/* Set clock speed */
#define F_CPU 8000000UL

/* Includes */
#include "main.h"

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

/* main */
int main(void)
{
	/* f clock -> 8 MHz */
	setClockPrescaler1();
	
	/* Initialize IO */
	initIO();
	
	/* Initialize SPI */
	SPI_MasterInit();
			
    while (1) 
    {
		SPI_MasterTransmit(SPDR);
    }
}

void setClockPrescaler1()
{
	CCP = 0xd8;
	CLKPR = 0b00000000;
}

void initIO()
{
	DDRA = 0x07;
	DDRB = 0x00; // CS lijnen uitzoeken
	DDRC = 0xff;
}

void SPI_MasterInit()
{
	/* MOSI and SCK -> output, RST and MISO -> input */
	DDRD = 0b00001001;
	/* Enable SPI, Master, clock -> 500 KHz */
	SPCR = 0b01010001;
}

void SPI_MasterTransmit(uint8_t data)
{
	/* Start transmission */
	SPDR = data;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
}

void initDisplay()
{
	
}

void updateDisplay(uint16_t setVoltage, uint16_t measuredVoltage, uint16_t setCurrent, uint16_t measuredCurrent)
{
	/* Declare local char arrays for ASCII codes */
	char c_setVoltage[6], c_measuredVoltage[6], c_setCurrent[6], c_measuredCurrent[6];
	
	/* Transform values to ASCII */
	integerToASCII_5digits(setVoltage, c_setVoltage);
	integerToASCII_5digits(measuredVoltage, c_measuredVoltage);
	integerToASCII_5digits(setCurrent, c_setCurrent);
	integerToASCII_5digits(measuredCurrent, c_measuredCurrent);
	
	/* Format the strings for the LCD */
	formatValue(c_setVoltage);
	formatValue(c_measuredVoltage);
	formatValue(c_setCurrent);
	formatValue(c_measuredCurrent);
	
	/* Print set voltage */

	/* Print 2 spaces */
	
	/* Print measured voltage */
	
	/* new line */
	
	/* Print set current */
	
	/* Print 2 spaces */
	
	/* Print measured current */
	
}

void integerToASCII_5digits(uint16_t number, char *c_number)
{	
	/* Separate the digits */
    for (int i = 0, j = 10000; i < 5; i++, j/=10)
    {
	    c_number[i] = number / j + 0x30;
	    number %= j;
    }
}

void formatValue(char c[6])
{
	if (c[0] == '0')
	{
		c[0] = ' ';
	}
	
	for (int i = 5; i >= 2; i--)
	{
		c[i+1] = c[i];
	}
	
	c[2] = ',';
}