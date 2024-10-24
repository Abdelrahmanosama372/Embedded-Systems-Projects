/*
 * main.c
 *
 * Created on: Sep 3, 2024
 * Author : Abdelrahman
 */ 

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>

#include "Smoke_sensor/smoke_sensor.h"
#include "Lcd/lcd.h"
#include "Fan/fan.h"
#include "Ir/ir.h"
#include "bluetooth/bluetooth.h"
#include "motor/motor.h"
#include "LM35/lm35.h"

uint8_t current_level = 0;
uint8_t requested_level = 0;
uint8_t is_current_level_displayed = 1;

int main(void)
{
	current_level = HAL_IRGetCurrentlevel();
	requested_level = current_level;
	
	HAL_BluetoothInit(current_level);
	HAL_FanInit();
	HAL_SmokeSensorInit();
	HAL_motorInit();
	HAL_LM35Init();
	HAL_IRInit();
	LCD_init();
	
	uint8_t prev_temp_val = HAL_LM35CalculateTemperature();
	uint8_t curr_temp_val = prev_temp_val;
	
	LCD_writeString("Temperature: ");
	LCD_writeInteger(prev_temp_val);
	LCD_writeString(" Level: ");
	LCD_writeInteger(current_level);
	
	
    while (1) 
    {
		curr_temp_val = HAL_LM35CalculateTemperature();
		if(curr_temp_val != prev_temp_val){
			LCD_goTo_X_and_Y(0,13);
			LCD_writeInteger(curr_temp_val);
			prev_temp_val = curr_temp_val;		
		}
		
		if(curr_temp_val >= 35 || HAL_isSmokeDetected() == 1)
		{
			HAL_FanTurnOn();
		}else {
			HAL_FanTurnOff();	
		}
		
		requested_level = HAL_BLGetRequestedLevel();

		if(requested_level > current_level)
		{
			HAL_motorTurnOnCW();
		}else if(requested_level < current_level)
		{
			HAL_motorTurnOnCCW();
		}
		
		if(is_current_level_displayed == 0)
		{
			LCD_goTo_X_and_Y(1,7);
			LCD_writeInteger(current_level);
			is_current_level_displayed = 1;
		}
	}
}

