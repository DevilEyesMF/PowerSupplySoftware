/*
 * rotaryEncoder.h
 *
 * Created: 08/05/2023 14:01:36
 *  Author: Remko Claes
 */


#ifndef ROTARYENCODER_H_
#define ROTARYENCODER_H_

/* Defines constants */
#define ENC_PORT PINB
#define ENC_VOLTAGE_CLK PINB2
#define ENC_VOLTAGE_DT PINB3
#define ENC_CURRENT_CLK PINB4
#define ENC_CURRENT_DT PINB5

/* Prototype functions */
void rotaryEncoderInit(void);

#endif /* ROTARYENCODER_H_ */