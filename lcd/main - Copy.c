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
    int j=0;
	
	uart_init();
	//void lcd_setup (uint8_t rs, uint8_t rw, uint8_t enable, uint8_t backlight,
	//			uint8_t db0, uint8_t db1, uint8_t db2, uint8_t db3 ) 
	lcd_setup (P_PA4, P_PA5, P_PA6, P_PA7, 
			  P_PA0, P_PA1, P_PA2, P_PA3);	
			  	
	lcd_init (LCD_DISP_ON_CURSOR_BLINK);
	lcd_puts ("txsttekst");
	lcd_puts ("\nextra regel");
	lcd_clrscr();
	lcd_puts ("\nnog een extra regel");
	j=0;
	for (;;) {
		uart_printf("txt:%d\r\n", j);
		j++;
		}
 return 0;
}
