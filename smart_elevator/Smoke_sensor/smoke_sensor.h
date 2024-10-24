/*
 * smoke_sensor.h
 *
 *  Created on: Sep 4, 2024
 *      Author: Abdelrahman
 */

#ifndef INC_SMOKE_SENSOR_H_
#define INC_SMOKE_SENSOR_H_

#include <avr/io.h>

#include "../Inc/utilities.h"
#include "stdint.h"

#define smokePin  PIND5

#define SMOKE_DDR    DDRD
#define SMOKE_PORT   PORTD
#define SMOKE_Pin    PIND

void HAL_SmokeSensorInit();
uint8_t HAL_isSmokeDetected();

#endif /* INC_SMOKE_SENSOR_H_ */
