/*
 * main.h
 *
 * Created: 02/05/2023 02:09:17
 *  Author: Remko Claes
 */ 

#ifndef MAIN_H_
#define MAIN_H_

/* Set clock speed */
#define F_CPU 8000000UL

/* Defines constants */
#define CS_ADC PORTB0
#define CS_DAC PORTB1
#define ACTIVE 1
#define INACTIVE 0

#define DAC_CURRENT 0
#define DAC_VOLTAGE 1
#define ADC_CURRENT 0
#define ADC_VOLTAGE 1

/* macro's */
#define BIT_SET(a,b) ((a) |= (1ULL<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) (((a)>>(b)) & 1ULL)

#include <stdint.h>

/* prototype functions */
void ClockPrescalerSet1(void);
void IOInit(void);
void SPI_MasterInit(void);void SPI_MasterTransmit(uint8_t data);
void ChipSelect(uint8_t, uint8_t);
void DAC_Set(uint8_t, uint16_t);
uint16_t ADC_Read(uint8_t);
void DisplayInit(void);
void DisplayUpdate(uint16_t, uint16_t, uint16_t, uint16_t);
void IntegerToASCII_5digits(uint16_t, char*);
void FormatValue(char*);

#endif /* MAIN_H_ */