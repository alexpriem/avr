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
#include <stdlib.h>
 
#include "uart.h"
#include "cat4016.h"
#include "bitops.h"

#include "delay.h"
#include "lcd_serial.h"
#include "constants.h"
#include "rot_encoder.h"
#include "keypad.h"
#include "hc595.h"
	

		  
int main(void)
{	
	int8_t vol, changed;
	char buf[10];
	
	uart_init();
	uart_puts ("\r\nreset -- init\r\n");
	//				sh_cp    st_cp    ds
	hc595_setup (0, P_PC5, P_PC3, P_PC1);
	//hc595_setup (0, P_PC5, P_PC1, P_PC3);
	/*
	hc595_putc (0,0x01);
	delay(5000);
	hc595_putc (0,0x02);
	delay(5000);
	hc595_putc (0,0x04);
	delay(5000);
	hc595_putc (0,0x08);
	delay(5000);
	hc595_putc (0,0x10);
	delay(5000);
	hc595_putc (0,0x20);
	delay(5000);
	hc595_putc (0,0x40);
	delay(5000);
	hc595_putc (0,0x80);
	*/

	//void lcd_setup (uint8_t rs, uint8_t rw, uint8_t enable1, uint8_t enable2,
	//			uint8_t db0, uint8_t db1, uint8_t db2, uint8_t db3 ) 
	 lcd_setup (0, P_PA2, P_PA4, P_PA0);			  
	 
			  //lcd_setup  (uint8_t chip,  uint8_t strobe, uint8_t clock, uint8_t io)
	lcd_setup_info (0, HD44780, 20, 2);
	
	//lcd_init (0, LCD_DISP_ON_CURSOR_BLINK);
	lcd_init (0, LCD_DISP_ON);
	
	encoder_setup (0, P_PC0, P_PC2);
	
	

	
	
	uart_printf ("start loop \r\n");
	
	for (;;) {
		changed=encoder_poll_range (0, &vol, 0,100,1);
		//uart_printf ("%d %d\r\n",vol, changed);
		
		if (changed==TRUE) {
			lcd_clrscr (0);
			lcd_puts (0, "volume: ");
			itoa(vol, buf,10);    
			lcd_puts (0, buf);
			hc595_putc (0,vol);
			}		
	}
 return 0;
}
