/*
 * fan.c
 *
 *  Created on: Sep 3, 2024
 *      Author: Abdelrahman
 */


#include "motor.h"

uint8_t motorPinState = 2;

void HAL_motorInit()
{
	motorPinState = 0;
	MOTOR_DDR |= (1 << motorPinIN1);
	MOTOR_DDR |= (1 << motorPinIN2);
}

HAL_Status_t isMotorInitialized()
{
	if(motorPinState == 2)
	{
		return ERR;
	}else {
		return OK;
	}
}

HAL_Status_t HAL_motorTurnOnCW()
{
	if(isMotorInitialized() == ERR)
	{
		return ERR;
	}

	if(motorPinState == 0)
	{
		MOTOR_PORT |= (1 << motorPinIN1);	
		MOTOR_PORT &= ~(1 << motorPinIN2);	
		motorPinState = 1;
	}

	return OK;
}

HAL_Status_t HAL_motorTurnOnCCW()
{
	if(isMotorInitialized() == ERR)
	{
		return ERR;
	}

	if(motorPinState == 0)
	{
		MOTOR_PORT &= ~(1 << motorPinIN1);
		MOTOR_PORT |= (1 << motorPinIN2);
		motorPinState = 1;
	}

	return OK;
}

HAL_Status_t HAL_motorTurnOff()
{
	if(isMotorInitialized() == ERR)
	{
		return ERR;
	}

	MOTOR_PORT &= ~(1 << motorPinIN1);
	MOTOR_PORT &= ~(1 << motorPinIN2);	
	motorPinState = 0;
	return OK;
}



