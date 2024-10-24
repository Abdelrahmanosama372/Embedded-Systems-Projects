/*
 * fan.c
 *
 *  Created on: Sep 3, 2024
 *      Author: Abdelrahman
 */


#include "fan.h"

uint8_t fanPinState = 2;

void HAL_FanInit()
{
	fanPinState = 0;
	FAN_DDR |= (1 << fanGpioPin);
}

HAL_Status_t isFanInitialized()
{
	if(fanPinState == 2)
	{
		return ERR;
	}else {
		return OK;
	}
}

HAL_Status_t HAL_FanTurnOn()
{
	if(isFanInitialized() == ERR)
	{
		return ERR;
	}

	if(fanPinState == 0)
	{
		FAN_PORT |= (1 << fanGpioPin);	
		fanPinState = 1;
	}

	return OK;
}

HAL_Status_t HAL_FanTurnOff()
{
	if(isFanInitialized() == ERR)
	{
		return ERR;
	}

	FAN_PORT &= ~(1 << fanGpioPin);	
	fanPinState = 0;
	return OK;
}



