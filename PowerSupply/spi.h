/*
 * spi.h
 *
 * Created: 08/05/2023 13:38:06
 *  Author: Remko Claes
 */

#ifndef SPI_H_
#define SPI_H_

/* Defines constants */
#define CS_ADC PORTB0
#define CS_DAC PORTB1
#define ACTIVE 1
#define INACTIVE 0

#define DAC_CURRENT 0
#define DAC_VOLTAGE 1
#define ADC_CURRENT 0
#define ADC_VOLTAGE 1

/* Prototype functions */
void SPI_MasterInit(void);void SPI_MasterTransmit(uint8_t);
void ChipSelect(uint8_t, uint8_t);
void DAC_Set(uint8_t, uint16_t);
uint16_t ADC_Read(uint8_t);

#endif /* SPI_H_ */