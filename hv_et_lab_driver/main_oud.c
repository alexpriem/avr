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
#include "74hc595.h"
#include "mcp4922.h"
#include "mcp3204.h"
#include "bitops.h"

#include "delay.h"
#include "scpi.h"
#include "leds.h"


#define MAXVOL 64
#define MAXBAL 32
#define MAXCHAN 3



unsigned int vol, ch;
int bal;
 



int values[8]={1,2,4,8,16,32,64,128};

 
void command_loop(void)
{ 
    uint8_t i, copypos=0;
	char buf[128], *s, *t;
    
	 
	
    while (1) {	
		if (uart_done!=0) {
			//uart_printf ("in loop:%d %d %d [%s]\r\n", uart_len, uart_pos, uart_done, uart_buf);			
			s=buf;
			t=uart_buf+copypos;
			uart_printf ("%p %p %p %p\r\n", buf, uart_buf, s, t);
			uart_printf ("len: %d\r\n", uart_len);
			for (i=0; i<uart_len; i++, copypos++) {					
                     *s=*t; 	
					 s++; t++;
					 uart_printf ("%p %p %c %c\r\n", s, t, *s, *t);
					 if (copypos>=UART_BUFLEN) {
					    uart_printf ("ov %p %p\r\n", s, t);						
						t=uart_buf;
						copypos=0;
					}
				}
			buf[uart_len]=0;
			buf[uart_len-1]=0;
			uart_len=0;
			uart_done=0;
			uart_printf ("\r\n\r\ngot:[%s]\r\n", buf);
			scpi_parse_line (buf);
		}
    }

}




int main(void)
{
	int i, j, adc0, adc1, adc2, adc3;
	int rev=0;	
	
    uart_init ();
	
	
	//init_mcp3202(uint8_t chip, uint8_t cs, uint8_t clk,  uint8_t dout, uint8_t din)
	init_mcp3204(0, P_PD6, P_PD3, P_PD4, P_PD5);
	init_mcp4922(0, P_PA0, P_PA1, P_PA2, P_PA3);
	init_mcp4922(1, P_PC4, P_PC3, P_PC2, P_PC0);
	//init_mcp4922(uint8_t chip, uint8_t cs, uint8_t clk,  uint8_t data, uint8_t ldac )

	uart_printf ("rev:%d\n", rev);
	

	command_loop();	

	
	
	/*
	i=0;
	j=0;
    for (;;) {
			i++;
			j++;
			
			//void mcp4922_putw (uint8_t chip, int16_t word, uint8_t dac)
			mcp4922_putw( 0, i, 0);
			mcp4922_putw( 0, 4095-i, 1);
			mcp4922_putw( 1, j, 0);
			mcp4922_putw( 1, 4095-j, 1);
			
			delay (10);
		       // mcp3202_getw (uint8_t chip, uint8_t adc, uint8_t mode)
			adc0=mcp3204_getw (0, 0, 1);
			adc1=mcp3204_getw (0, 1, 1);
			adc2=mcp3204_getw (0, 2, 1);
			adc3=mcp3204_getw (0, 3, 1);
			 			
			uart_printf ("%d: %d %d , %d %d\r\n",i, adc0, adc1, adc2, adc3 );  			
			i++;
			if (i>4095) {
				i=0;
			}
			            
        }

	*/
 return 0;
}
