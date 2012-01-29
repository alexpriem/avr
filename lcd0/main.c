/******************************************************************************
 Title:    adc_dac_uc2
 Author:   Alex Priem
 Date:     October 2008
 Software: AVR-GCC 3.3 
 Hardware: ATMega32
           
 Description:
 
  
*******************************************************************************/
#include <inttypes.h>
#include <avr/io.h>
 
#include "uart.h"
#include "cat4016.h"
#include "bitops.h"

#include "delay.h"
#include "lcd.h"

#define TRUE 1
#define FALSE 0
					

int main(void)
{
 
	lcd_init (LCD_DISP_ON_CURSOR_BLINK);	
	lcd_puts ("xtesttekst");

	
 return 0;
}
