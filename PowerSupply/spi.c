/*
 * spi.c
 *
 * Created: 08/05/2023 13:37:29
 *  Author: Remko Claes
 */

/* Includes */
#include <avr/io.h>

#include "main.h"
#include "spi.h"

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
	ChipSelect(CS_DAC, ACTIVE);
	SPI_MasterTransmit(data >> 8);		// high byte
	SPI_MasterTransmit(data & 0x00ff);	// low byte
	ChipSelect(CS_DAC, INACTIVE);
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
	ChipSelect(CS_ADC, INACTIVE);

	return data;
}
