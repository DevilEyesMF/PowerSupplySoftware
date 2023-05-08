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

/* macro's */
#define BIT_SET(a,b) ((a) |= (1ULL<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) (((a)>>(b)) & 1ULL)

#include <stdint.h>

/* prototype functions */
void ClockPrescalerSet1(void);
void IOInit(void);

#endif /* MAIN_H_ */