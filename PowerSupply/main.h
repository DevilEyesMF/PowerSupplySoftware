/*
 * main.h
 *
 * Created: 02/05/2023 02:09:17
 *  Author: Remko Claes
 */ 

#ifndef MAIN_H_
#define MAIN_H_

#include <stdint.h>

/* prototype functions */
void setClockPrescaler1(void);
void IO_Init(void);
void SPI_MasterInit(void);void SPI_MasterTransmit(uint8_t data);

#endif /* MAIN_H_ */