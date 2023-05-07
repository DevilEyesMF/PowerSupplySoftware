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
#include <avr/interrupt.h>

/* main */
int main(void)
{
	/* declare local variables */
	uint16_t setVoltage = 0;		// Set voltage in mV
	uint16_t setCurrent = 0;		// Set current in mA
	uint16_t measuredVoltage = 0;	// Measured voltage in mV
	uint16_t measuredCurrent = 0;	// Measured current in mA
	
	/* f clock -> 8 MHz */
	ClockPrescalerSet1();

	/* Initialize IO */
	IOInit();

	/* Initialize SPI */
	SPI_MasterInit();
	DDRB = 7;

	/* Initialize Display */
	DisplayInit();
	
	// TODO
	BIT_SET(PCMSK1, PCINT10);
	BIT_SET(PCICR, PCIE1);
	
	// sei();
		
    while (1)
    {
		// TODO setting voltage/current with rotary encoders
		setVoltage = 20000;
		setCurrent = 2000;
		
		/* Set voltage */
		//DAC_Set(DAC_VOLTAGE, setVoltage / 5);
		
		/* Set current */
		//DAC_Set(DAC_CURRENT, setCurrent << 1);
		
		/*
		 * Measure voltage
		 * 12-bit resolution measures voltage in steps of 5 mV
		 */
		//measuredVoltage = ADC_Read(ADC_VOLTAGE) * 5;
		
		/*
		 * Measure current
		 * 12-bit resolution measures current in steps of 0.5 mA
		 * only 11 bits are used -> steps of 1 mA
		 */
		//measuredCurrent = ADC_Read(ADC_CURRENT) >> 1;
		
		/* Update the LCD display */
		DisplayUpdate(setVoltage, measuredVoltage, setCurrent, measuredCurrent);
		
		//DisplaySetDDRAM(ADDR_VOLTAGE_SET);
		//DisplayWriteChar('5');
		//_delay_ms(200);
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
	DDRB = 0x00;
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
	DisplaySetDDRAM(ADDR_VOLTAGE_SET);

	for (int i = 0; i < 6; i++)
	{
		DisplayWriteChar(c_setVoltage[i]);
	}
	DisplayWriteChar('V');

	/* Print 2 spaces */

	/* Print measured voltage */
	DisplaySetDDRAM(ADDR_VOLTAGE_MEAS);

	for (int i = 0; i < 6; i++)
	{
		DisplayWriteChar(c_measuredVoltage[i]);
	}
	DisplayWriteChar('V');

	/* new line */

	/* Print set current */
	DisplaySetDDRAM(ADDR_CURRENT_SET);

	for (int i = 0; i < 6; i++)
	{
		DisplayWriteChar(c_setCurrent[i]);
	}
	DisplayWriteChar('A');

	/* Print 2 spaces */

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

ISR(PCINT1_vect, ISR_BLOCK)
{
	if (BIT_CHECK(PINB, PINB2) && !BIT_CHECK(PINB, PINB3))
	{
		
	}
	if (BIT_CHECK(PINB, PINB2) && BIT_CHECK(PINB, PINB3))
	{
		
	}
}