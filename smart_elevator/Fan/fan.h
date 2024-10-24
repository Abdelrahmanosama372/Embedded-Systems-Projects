/*
 * fan.h
 *
 *  Created on: Sep 3, 2024
 *      Author: Abdelrahman
 */

#ifndef INC_FAN_H_
#define INC_FAN_H_

#include <avr/io.h>

#include "../Inc/utilities.h"
#include "stdint.h"

#define fanGpioPin PIND4

#define FAN_DDR    DDRD
#define FAN_PORT   PORTD

void HAL_FanInit();
HAL_Status_t HAL_FanTurnOn();
HAL_Status_t HAL_FanTurnOff();

#endif /* INC_FAN_H_ */
