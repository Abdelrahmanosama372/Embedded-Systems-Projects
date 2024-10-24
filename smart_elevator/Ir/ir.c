/*
 * ir.c
 *
 * Created on: Sep 3, 2024
 *  Author: Abdelrahman
 */ 

#include "ir.h"
#include <avr/interrupt.h>

extern uint8_t requested_level;
extern uint8_t current_level;
extern uint8_t is_current_level_displayed;

extern void HAL_motorTurnOff();

void HAL_IRInit()
{
	MCUCR |= (1 << ISC00) | (1 << ISC01);  // Rising edge on INT0 (PIND2)
	MCUCR |= (1 << ISC10) | (1 << ISC11);  // Rising edge on INT1 (PIND3)

	// Enable external interrupts INT0 and INT1
	GICR |= (1 << IR1_PIN) | (1 << IR2_PIN);

	// Enable global interrupts
	sei();
}

uint8_t HAL_IRGetCurrentlevel()
{
	uint8_t currentlevel = 0;
	
	if((IR_PORT & (1 << IR1_PIN)) > 0)
	{
		currentlevel = 0;	
	}else if((IR_PORT & (1 << IR2_PIN)) > 0)
	{
		currentlevel = 1;
	}
	return currentlevel;
}

ISR(INT0_vect) {
	if(requested_level == LEVEL0)
	{
		HAL_motorTurnOff();
		current_level = requested_level;
		is_current_level_displayed = 0;
	}
}

ISR(INT1_vect) {
	if(requested_level == LEVEL1)
	{
		HAL_motorTurnOff();
		current_level = requested_level;
		is_current_level_displayed = 0;
	}
}
