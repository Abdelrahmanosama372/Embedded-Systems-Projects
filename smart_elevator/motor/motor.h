/*
 * fan.h
 *
 *  Created on: Sep 3, 2024
 *      Author: Abdelrahman
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include <avr/io.h>

#include "../Inc/utilities.h"
#include "stdint.h"

#define motorPinIN1 PIND6
#define motorPinIN2 PIND7

#define MOTOR_DDR    DDRD
#define MOTOR_PORT   PORTD

void HAL_motorInit();
HAL_Status_t HAL_motorTurnOnCW();
HAL_Status_t HAL_motorTurnOnCCW();
HAL_Status_t HAL_motorTurnOff();

#endif /* INC_MOTOR_H_ */