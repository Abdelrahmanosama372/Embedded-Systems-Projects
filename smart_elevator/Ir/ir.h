/*
 * ir.h
 *
 * Created on: Sep 3, 2024
 *  Author: Abdelrahman
 */ 


#ifndef IR_H_
#define IR_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#include "../Inc/utilities.h"
#include "stdint.h"

#define IR_PORT    PORTD
#define IR_DDR     DDRD
#define IR1_PIN INT0
#define IR2_PIN INT1

#define LEVEL0   0
#define LEVEL1   1

void HAL_IRInit();
uint8_t HAL_IRGetCurrentlevel();


#endif /* IR_H_ */