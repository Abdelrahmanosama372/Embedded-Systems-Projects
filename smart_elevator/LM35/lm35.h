/*
 * lm35.h
 *
 * Created on: Sep 3, 2024
 *  Author: Abdelrahman
 */ 


#ifndef LM35_H_
#define LM35_H_

#include <avr/io.h>

#include "../ADC/ADC.h"
#include "../Inc/utilities.h"
#include "stdint.h"

void HAL_LM35Init();
uint8_t HAL_LM35CalculateTemperature();


#endif /* LM35_H_ */