/*
 * PowerSupply.c
 *
 * Created: 01/05/2023 22:01:19
 * Author : Remko Claes
 */ 

/* Includes */
#include "main.h"

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

/* main */
int main(void)
{
	/* f clock -> 8 MHz */
	ClockPrescalerSet1();
	
	/* Initialize IO */
	IOInit();
	
	/* Initialize SPI */
	SPI_MasterInit();
			
    while (1) 
    {
		ChipSelect(CS_ADC, ACTIVE);
		SPI_MasterTransmit('B');
		SPI_MasterTransmit('i');
		SPI_MasterTransmit('b');
		SPI_MasterTransmit('b');
		SPI_MasterTransmit('l');
		SPI_MasterTransmit('e');
		ChipSelect(CS_ADC, INACTIVE);
    }
}

void ClockPrescalerSet1()
{
	CCP = 0xd8;
	CLKPR = 0b00000000;
}

void IOInit()
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

void ChipSelect(uint8_t pin, uint8_t state)
{
	switch (state)
	{
		/* Sink */
		case ACTIVE:
			BIT_SET(DDRB, pin);
			BIT_CLEAR(PORTB, pin);
			break;
		/* High-Z */
		case INACTIVE:
		default:
			BIT_CLEAR(DDRB, pin);
			break;
	}
}

void DAC_Set(uint8_t channel, uint16_t data)
{
	/* Mask data */
	data &= 0x0fff;
	
	/* 
	 * Set control bits 
	 * bit 15: Select channel
	 * bit 14: Bypass input buffer
	 * bit 13: Output gain = 1
	 * bit 12: /SHDN bit
	 */
	data |= (0x3000 + (channel << 15));
	
	/* Transmit data */
	ChipSelect(CS_ADC, ACTIVE);
	SPI_MasterTransmit(data >> 8);		// high byte
	SPI_MasterTransmit(data & 0x00ff);	// low byte
	ChipSelect(CS_ADC, INACTIVE);
}

uint16_t ADC_Read(uint8_t channel)
{
	uint16_t data = 0;
	uint8_t dataOut = 0xd0 + (channel << 5);
	
	ChipSelect(CS_ADC, ACTIVE);
	SPI_MasterTransmit(dataOut);
	data = SPDR << 8;
	SPI_MasterTransmit(0x00);
	data |= SPDR;
	
	return data;
}

void DisplayInit()
{
	
}

void DisplayUpdate(uint16_t setVoltage, uint16_t measuredVoltage, uint16_t setCurrent, uint16_t measuredCurrent)
{
	/* Declare local char arrays for ASCII codes */
	char c_setVoltage[6], c_measuredVoltage[6], c_setCurrent[6], c_measuredCurrent[6];
	
	/* Transform values to ASCII */
	IntegerToASCII_5digits(setVoltage, c_setVoltage);
	IntegerToASCII_5digits(measuredVoltage, c_measuredVoltage);
	IntegerToASCII_5digits(setCurrent, c_setCurrent);
	IntegerToASCII_5digits(measuredCurrent, c_measuredCurrent);
	
	/* Format the strings for the LCD */
	FormatValue(c_setVoltage);
	FormatValue(c_measuredVoltage);
	FormatValue(c_setCurrent);
	FormatValue(c_measuredCurrent);
	
	/* Print set voltage */

	/* Print 2 spaces */
	
	/* Print measured voltage */
	
	/* new line */
	
	/* Print set current */
	
	/* Print 2 spaces */
	
	/* Print measured current */
	
}

void IntegerToASCII_5digits(uint16_t number, char *c_number)
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
	
	for (int i = 5; i >= 2; i--)
	{
		c[i+1] = c[i];
	}
	
	c[2] = ',';
}