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
#include "constants.h"
#include "rot_encoder.h"
#include "pga2311.h"
					

int main(void)
{
    int j=0, vol_l, vol_r,d;
	
	
	uart_init();
	uart_puts ("\r\nreset -- init\r\n");
	//void lcd_setup (uint8_t rs, uint8_t rw, uint8_t enable, uint8_t backlight,
	//			uint8_t db0, uint8_t db1, uint8_t db2, uint8_t db3 ) 
	lcd_setup (P_PA4, P_PA5, P_PA6, P_PA7, 
			  P_PA0, P_PA1, P_PA2, P_PA3);	
	

	//a2311_setup (uint8_t chip, uint8_t zcen, uint8_t cs, uint8_t sdi, uint8_t clk, uint8_t mute)	
	pga2311_setup (0, P_PA0, P_PA1, P_PA2, P_PA3, P_PA4);
	pga2311_init (0);
	lcd_init (LCD_DISP_ON_CURSOR_BLINK);
	lcd_puts ("txsttekst");
	lcd_puts ("\nextra regel");
	lcd_puts ("\nnog een extra regel");
	j=0;
	
	vol_l=128;
	vol_r=128;
	encoder_setup (0,P_PC2, P_PC3);
	encoder_setup (1,P_PC4, P_PC5);
	
	for (;;) {
		d=encoder_poll (0);
		if (d>0) {
			vol_l++; vol_r--;
			}
		if (d<0) {
			vol_l--; vol_r--;
		}
		
		pga2311_set_gain (0, vol_l, vol_r);
		}
 return 0;
}
