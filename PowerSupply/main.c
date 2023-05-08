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

/* Declare global variables */
uint16_t setVoltage = 0;		// Set voltage in mV
uint16_t setCurrent = 0;		// Set current in mA
uint16_t measuredVoltage = 0;	// Measured voltage in mV
uint16_t measuredCurrent = 0;	// Measured current in mA

/* main */
int main(void)
{	
	/* f clock -> 8 MHz */
	ClockPrescalerSet1();

	/* Initialize IO */
	IOInit();

	/* Initialize SPI */
	SPI_MasterInit();
	DDRB = 3; // tmp

	/* Initialize Display */
	DisplayInit();
	
	// TODO
	BIT_SET(PCMSK1, PCINT10);
	BIT_SET(PCMSK1, PCINT11);
	BIT_SET(PCMSK1, PCINT12);
	BIT_SET(PCMSK1, PCINT13);
	BIT_SET(PCICR, PCIE1);
	
	sei();
	
	setVoltage = 10000;
	setCurrent = 1000;
		
    while (1)
    {
		// TODO setting voltage/current with rotary encoders

		
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
	//BIT_CLEAR(DISPLAY_CTL, DISPLAY_RW);
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

ISR(PCINT1_vect, ISR_BLOCK)
{
	static uint8_t voltageState = 0;
	static uint8_t currentState = 0;
	uint8_t voltageClkState = BIT_CHECK(ENC_PORT, ENC_VOLTAGE_CLK);
	uint8_t voltageDataState = BIT_CHECK(ENC_PORT, ENC_VOLTAGE_DT);
	uint8_t currentClkState = BIT_CHECK(ENC_PORT, ENC_CURRENT_CLK);
	uint8_t currentDataState = BIT_CHECK(ENC_PORT, ENC_CURRENT_DT);
	
	/* Rotary encoder voltage */
	switch (voltageState)
	{
		/* Idle state */
		case 0:
		if (!voltageClkState) // CW
		{
			voltageState = 1;
		}
		else if (!voltageDataState) // CCW
		{
			voltageState = 4;
		}
		break;
		
		/* Clockwise rotation */
		case 1:
		if (!voltageDataState)
		{
			voltageState = 2;
		}
		break;
		
		case 2:
		if (voltageClkState)
		{
			voltageState = 3;
		}
		break;
		
		case 3:
		if (voltageClkState && voltageDataState)
		{
			voltageState = 0;
			setVoltage += 25;
			if (setVoltage > 20000)
			{
				setVoltage = 20000;
			}
		}
		break;
		
		/* Counter clockwise rotation */
		case 4:
		if (!voltageClkState)
		{
			voltageState = 5;
		}
		break;
		
		case 5:
		if (voltageDataState)
		{
			voltageState = 6;
		}
		break;
		
		case 6:
		if (voltageClkState && voltageDataState)
		{
			voltageState = 0;
			setVoltage -= 25;
			if (setVoltage > 20000)
			{
				setVoltage = 0;
			}
		}
		break;
	}	

	/* Rotary encoder current */
	switch (currentState)
	{
		/* Idle state */
		case 0:
		if (!currentClkState) // CW
		{
			currentState = 1;
		}
		else if (!currentDataState) // CCW
		{
			currentState = 4;
		}
		break;
	
		/* Clockwise rotation */
		case 1:
		if (!currentDataState)
		{
			currentState = 2;
		}
		break;
	
		case 2:
		if (currentClkState)
		{
			currentState = 3;
		}
		break;
	
		case 3:
		if (currentClkState && currentDataState)
		{
			currentState = 0;
			setCurrent += 1;
			if (setCurrent > 2000)
			{
				setCurrent = 2000;
			}
		}
		break;
	
		/* Counter clockwise rotation */
		case 4:
		if (!currentClkState)
		{
			currentState = 5;
		}
		break;
	
		case 5:
		if (currentDataState)
		{
			currentState = 6;
		}
		break;
	
		case 6:
		if (currentClkState && currentDataState)
		{
			currentState = 0;
			setCurrent -= 1;
			if (setCurrent > 2000)
			{
				setCurrent = 0;
			}
		}
		break;
	}
}