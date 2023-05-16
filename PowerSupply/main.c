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

#include "spi.h"
#include "display.h"
#include "rotaryEncoder.h"

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

	/* Initialize Display */
	DisplayInit();

	/* Initialize rotary encoders */
	rotaryEncoderInit();

	setVoltage = 10000;
	setCurrent = 250;

    while (1)
    {
		/* Set voltage */
		DAC_Set(DAC_VOLTAGE, setVoltage / 5);

		/* Set current */
		DAC_Set(DAC_CURRENT, (setCurrent + 1) << 1);

		/*
		 * Measure voltage
		 * 12-bit resolution measures voltage in steps of 5 mV
		 */
		measuredVoltage = ADC_Read(ADC_VOLTAGE) * 5;

		/*
		 * Measure current
		 * 12-bit resolution measures current in steps of 0.5 mA
		 * only 11 bits are used -> steps of 1 mA
		 */
		measuredCurrent = ADC_Read(ADC_CURRENT) >> 1;

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