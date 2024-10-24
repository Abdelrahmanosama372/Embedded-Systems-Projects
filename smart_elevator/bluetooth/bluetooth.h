/*
 * bluetooth.h
 *
 *  Created on: Sep 3, 2024
 *  Author: Abdelrahman
 */ 


#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#include "../Inc/utilities.h"
#include "stdint.h"

void HAL_BluetoothInit(uint8_t current_level);

uint8_t HAL_BLGetRequestedLevel();

#endif /* BLUETOOTH_H_ */