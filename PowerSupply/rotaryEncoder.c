/*
 * rotaryEncoder.c
 *
 * Created: 08/05/2023 14:01:03
 *  Author: Remko Claes
 */

/* Includes */
#include <avr/io.h>
#include <avr/interrupt.h>

#include "main.h"
#include "rotaryEncoder.h"

/* Global variables */
extern uint16_t setVoltage;
extern uint16_t setCurrent;

void rotaryEncoderInit()
{
	/* Enable interrupt for change on rotary encoder pins */
	BIT_SET(PCMSK1, PCINT10);
	BIT_SET(PCMSK1, PCINT11);
	BIT_SET(PCMSK1, PCINT12);
	BIT_SET(PCMSK1, PCINT13);
	BIT_SET(PCICR, PCIE1);

	/* Enable global interrupts */
	sei();
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