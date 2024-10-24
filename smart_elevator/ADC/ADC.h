/*
 * ADC.h
 *
 * Created: 3/27/2024 12:41:23 PM
 *  Author: Abdelrahman
 */ 


#ifndef ADC_H_
#define ADC_H_

#include "avr/io.h"
#define SET_BIT(reg,pos) (reg |= (1 << pos))
#define READ_BIT(reg,pos) (reg &= (1 << pos))

typedef enum {	
	VREF_AREF = 0,
	VREF_VCC ,
	VREF_256 = 3	
}Voltage_Reference;

typedef enum {
	PRESCALAR_2 = 1,
	PRESCALAR_4,
	PRESCALAR_8,
	PRESCALAR_16,
	PRESCALAR_32,
	PRESCALAR_64,
	PRESCALAR_128
}ADC_Prescalar;

typedef enum {
	ADC_CHANNEL_0 = 0,
	ADC_CHANNEL_1,
	ADC_CHANNEL_2,
	ADC_CHANNEL_3,
	ADC_CHANNEL_4,
	ADC_CHANNEL_5,
	ADC_CHANNEL_6,
	ADC_CHANNEL_7
}ADC_Channel;

void ADC_init(Voltage_Reference Vref, ADC_Prescalar prescalar);
uint16_t ADC_read(ADC_Channel channel);

#endif /* ADC_H_ */