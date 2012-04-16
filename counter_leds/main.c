/******************************************************************************
 Title:    ruff counter
 Author:   Alex Priem
 Date:     February 2012
 Software: AVR-GCC 3.3 
 Hardware: ATMega32
           
 Description:
 t
  
*******************************************************************************/
#include <inttypes.h>
#include <avr/io.h>
 
#include "uart.h"
#include "cat4016.h"
#include "bitops.h"

#include "delay.h"

#define TRUE 1
#define FALSE 0
					
   
 
int main(void)
{
 //   int rev=0;
	int j;	
	
	
	uart_init();
	uart_puts("\r\ncounter init\r\n");	
	
	DDRA = 0x0f;  // E inputs + outputs voor keypad
	PORTA= 0xf0;  // pullups aan
	//PORTC= 0x00;  // pullups uit
	
	DDRA  = 0xff;  // A output   
	DDRC  = 0xff;  // C output   LEDS 1+2	
	DDRD  = 0xff;  // DE output   LEDS 3+4 
	
//	uart_puts("\r\nsetup done\r\n");	
//	uart_puts("\r\nlcd init done\r\n");	
//	uart_puts("\r\ndone\r\n");	
	
	
  // init_cat4016(uint8_t chip, uint8_t clk, uint8_t latch, uint8_t data, uint8_t blank)
	init_cat4016 (2, P_PC6, P_PC0,  P_PC4, PC2);
	init_cat4016 (1, P_PA7, P_PA1,  P_PA5, PA3);
	init_cat4016 (0, P_PA6, P_PA0,  P_PA4, PA2);	
	
    uart_puts("\r\ndone cat4016\r\n");	
	
	j=0;
	//PORTA=0x00;
	
	cat4016_put_unumber (0,1111);
	cat4016_put_unumber (1,2222);
	cat4016_put_unumber (2,3333);
	uart_printf ("inits done\r\n");
	
	

	
 return 0;
}
