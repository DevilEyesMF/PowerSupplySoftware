/*
 * display.h
 *
 * Created: 08/05/2023 13:52:59
 *  Author: Remko Claes
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

/* Defines constants */
#define DISPLAY_DATA PORTC
#define DISPLAY_CTL PORTA
#define DISPLAY_RS PORTA0
#define DISPLAY_RW PORTA1
#define DISPLAY_EN PORTA2

#define ADDR_VOLTAGE_SET 0x00
#define ADDR_VOLTAGE_MEAS 0x09
#define ADDR_CURRENT_SET 0x40
#define ADDR_CURRENT_MEAS 0x49

/* Prototype functions */
void DisplayInit(void);
void DisplayUpdate(uint16_t, uint16_t, uint16_t, uint16_t);
void DisplaySetDDRAM(uint8_t);
void DisplayWriteChar(char);
void DisplayEnablePulse(void);
void IntegerToASCII(uint16_t, char*);
void FormatValue(char*);

#endif /* DISPLAY_H_ */