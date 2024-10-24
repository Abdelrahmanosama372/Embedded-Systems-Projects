/*
 * smoke_sensor.c
 *
 *  Created on: Sep 4, 2024
 *      Author: Abdelrahman
 */

#include "smoke_sensor.h"

void HAL_SmokeSensorInit()
{
	SMOKE_DDR &= ~(1 << smokePin);
}

uint8_t HAL_isSmokeDetected()
{
	return (SMOKE_Pin & (1 << smokePin)) > 0;
}
