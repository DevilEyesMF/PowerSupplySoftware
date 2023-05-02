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
	IO_Init();
	
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

void IO_Init()
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