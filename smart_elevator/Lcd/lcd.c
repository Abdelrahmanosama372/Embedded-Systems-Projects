/*
 * led.c
 *
 * Created: 3/17/2024 12:16:01 PM
 *  Author: Abdelrahman
 */ 

#include "lcd.h"

uint8_t number_of_characters = 0;
uint8_t is_clearing = 0;

void LCD_init(){
	_delay_ms(20);
	LCD_CTRL_DDR |= (1<<EN_SWITCH | 1<<RS_SWITCH | 1<<RW_SWITCH);
	LCD_CTRL_PORT &= ~ (1<<EN_SWITCH | 1<<RS_SWITCH | 1<<RW_SWITCH);
	LCD_DATA_DDR = (0xF << DATA_shift);
	_delay_ms(15);
	LCD_writeCommand((char)0x02);
	LCD_writeCommand(LCD_FUNC_4_BIT_MODE);
	LCD_writeCommand(LCD_ENTRY_INC);
	LCD_writeCommand(LCD_BEGIN_AT_FIRST_ROW);
	LCD_writeCommand(LCD_DISP_CURSOR_BLINK);
}

void LCD_lcd_kick(){
	LCD_CTRL_PORT |= (1 << EN_SWITCH);
	_delay_ms(50);
	LCD_CTRL_PORT &= ~(1 << EN_SWITCH);
}

void LCD_writeCommand(unsigned char command){
	//LCD_check_lcd_isbusy();
	LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (command & 0xF0);
	LCD_CTRL_PORT &= ~ ((1<<RW_SWITCH)|(1<<RS_SWITCH));
	LCD_lcd_kick ();
	LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | ((command << DATA_shift) & 0xF0);
	LCD_CTRL_PORT &= ~ ((1<<RW_SWITCH)|(1<<RS_SWITCH));
	LCD_lcd_kick();
}

void LCD_writeChar(char ch){
	if(number_of_characters == 16) {
		LCD_goTo_X_and_Y(1,0);
	}
	if(!is_clearing)
		number_of_characters++;
	LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (ch & 0xF0);
	LCD_CTRL_PORT = (LCD_CTRL_PORT & 0xF8) | (1 << RS_SWITCH);
	LCD_lcd_kick();
	LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | ((ch << DATA_shift) & 0xF0);
	LCD_lcd_kick();
}

void LCD_writeString(char *str){
	while(*str){
		LCD_writeChar(*str);
		str++;
	}
}

void LCD_writeInteger(int number) {
	char buffer[8];	
	snprintf(buffer,sizeof(buffer),"%d",number);
	LCD_writeString(buffer);
}

void LCD_writeFloat(float number){
	int tmp_num;
	tmp_num = (int)number;
	LCD_writeInteger(tmp_num);
	LCD_writeChar('.');
	tmp_num = ((int)(number*100)) % 100;
	tmp_num < 0 ?
		LCD_writeInteger(-tmp_num) : LCD_writeInteger(tmp_num);
}

void LCD_clearDisplay(){
	//LCD_check_lcd_isbusy();
	LCD_writeCommand(LCD_CLEAR_DISPLAY);
	number_of_characters = 0;
}

void LCD_backClearDisplay(){
	LCD_writeCommand(LCD_ENTRY_DEC);
	is_clearing = 1;
	int i;
	for(i=number_of_characters+1; i>0; i--){
		if(i==16)
			LCD_goTo_X_and_Y(0,15);
		LCD_writeChar(' ');
	}
	LCD_writeCommand(LCD_BEGIN_AT_FIRST_ROW);
	LCD_writeCommand(LCD_ENTRY_INC);
	is_clearing = 0;
	number_of_characters = 0;
}

void LCD_goTo_X_and_Y(unsigned char x, unsigned char y){
	if(x == 0){
		LCD_writeCommand(LCD_BEGIN_AT_FIRST_ROW + y);
	}else{
		LCD_writeCommand(LCD_BEGIN_AT_SECOND_ROW + y);
	}
}

void LCD_check_lcd_isbusy(){
	LCD_CTRL_PORT |= (1 << RW_SWITCH);
	LCD_CTRL_PORT &= ~(1 << RS_SWITCH);
	LCD_DATA_DDR &= ~(0x0F);
	LCD_lcd_kick();
	LCD_DATA_DDR |= (0x08);
	LCD_CTRL_PORT &= ~(1 << RW_SWITCH);
}
