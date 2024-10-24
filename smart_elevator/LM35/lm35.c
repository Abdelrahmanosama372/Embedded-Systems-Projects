/*
 * lm35.c
 *
 * Created on: Sep 3, 2024
 *  Author: Abdelrahman
 */ 

#include "lm35.h"

void HAL_LM35Init()
{
	ADC_init(VREF_VCC,PRESCALAR_4);
}
	
uint8_t HAL_LM35CalculateTemperature(){
	return (ADC_read(ADC_CHANNEL_0)*500)/1024;
}