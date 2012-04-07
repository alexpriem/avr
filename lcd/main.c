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
#include "lcd_multi4.h"
#include "constants.h"
#include "rot_encoder.h"
#include "pga2311.h"
					

int main(void)
{
    int j=0, vol_l, vol_r,d;
	
	
	uart_init();
	uart_puts ("\r\nreset -- init\r\n");
	//void lcd_setup (uint8_t rs, uint8_t rw, uint8_t enable,
	//			uint8_t db0, uint8_t db1, uint8_t db2, uint8_t db3 ) 
	lcd_setup (0, P_PC2, P_PC4, P_PC6, P_PC0,
			  P_PC1, P_PC3, P_PC5, P_PC7);	
	lcd_setup_info (0, HD44780, 20, 4);
	

	//a2311_setup (uint8_t chip, uint8_t zcen, uint8_t cs, uint8_t sdi, uint8_t clk, uint8_t mute)	
	pga2311_setup (0, P_PA0, P_PA1, P_PA2, P_PA3, P_PA4);
	pga2311_init (0);
	lcd_init (0, LCD_DISP_ON_CURSOR_BLINK);
	lcd_puts (0, "regel 1");
	lcd_puts (0, "\nregel 2");
	lcd_puts (0, "\nnr 3");
	lcd_puts (0, "\n#4, done");
	lcd_puts (0, "\n#5");
	lcd_puts (0, "\n#6");
	lcd_puts (0, "\n#7");
	j=0;
	
	vol_l=128;
	vol_r=128;
	encoder_setup (0,P_PD2, P_PD3);
	encoder_setup (1,P_PD4, P_PD5);
	
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
