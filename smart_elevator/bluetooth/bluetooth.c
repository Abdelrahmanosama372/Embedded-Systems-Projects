/*
 * bluetooth.c
 *
 *  Created on: Sep 3, 2024
 *  Author: Abdelrahman
 */ 

#include "bluetooth.h"

uint8_t bl_requested_level = 0;

void HAL_BluetoothInit(uint8_t current_level)
{
	UCSRB |= (1 << RXEN) | (1 << RXCIE);
	UBRRL = 51; // baudrate 9600 at 8Mhz
	bl_requested_level = current_level;
}

uint8_t HAL_BLGetRequestedLevel()
{
	return bl_requested_level;
}

ISR(USART_RXC_vect)
{
	bl_requested_level = UDR - '0';
}