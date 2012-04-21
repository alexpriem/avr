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
#include "lcd_4_40x4.h"
#include "constants.h"
#include "rot_encoder.h"
#include "keypad.h"
#include "hc595.h"
	

		  
int main(void)
{
	int c, prevc;
	
		  
	
	uart_init();
	uart_puts ("\r\nreset -- init\r\n");
	
	/*
	hc595_setup(0, P_PD7, P_PD3, P_PD5);
	hc595_putc (0,0x01);
	delay(500);
	hc595_putc (0,0x02);
	delay(500);
	hc595_putc (0,0x04);
	delay(500);
	hc595_putc (0,0x08);
	delay(500);
	hc595_putc (0,0x10);
	delay(500);
	hc595_putc (0,0x20);
	delay(500);
	hc595_putc (0,0x40);
	delay(500);
	hc595_putc (0,0x80);
	*/

	//void lcd_setup (uint8_t rs, uint8_t rw, uint8_t enable1, uint8_t enable2,
	//			uint8_t db0, uint8_t db1, uint8_t db2, uint8_t db3 ) 
	 lcd_setup (0, P_PC2, P_PC4, P_PC6, P_PC0,
			  P_PC1, P_PC3, P_PC5, P_PC7);	 
	 lcd_setup (0, P_PA2, P_PA4, P_PA6, P_PA0,
			  P_PA1, P_PA3, P_PA5, P_PA7);	 

			  //lcd_setup  (uint8_t chip,  uint8_t strobe, uint8_t clock, uint8_t io)
	lcd_setup_info (0, HD44780, 20, 2);
	
	lcd_init (0, LCD_DISP_ON_CURSOR_BLINK);
	
	
	lcd_puts (0, "abcde 1");
	lcd_puts (0, "\nfghij 2");
	
	uart_printf ("start loop \r\n",c);
	c=0;
	prevc=0;
	
 return 0;
}
