/*
 * ADC.c
 *
 * Created: 3/27/2024 12:41:10 PM
 *  Author: Abdelrahman
 */ 

#include "ADC.h"
#include <util/delay.h>

void ADC_init(Voltage_Reference Vref, ADC_Prescalar prescalar){
	ADMUX |= (Vref << 6);
	ADCSRA |= prescalar;
	SET_BIT(ADCSRA,ADEN);
}

uint16_t ADC_read(ADC_Channel channel){
	ADMUX &= 0xE0;
	ADMUX |= channel;
	SET_BIT(ADCSRA,ADSC);
	while(ADCSRA & 0x40);
	return ADC;
}