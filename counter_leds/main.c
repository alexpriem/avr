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
#include "constants.h"

#include "delay.h"


 void handle_txt (char *buf)
 
 {
  uint8_t display;
  unsigned int  num;
  
  display=buf[0]-'a';
  if (display>3) {
	uart_printf("\r\nunknown display:%d\r\n",display);
	return;
  }
  num=buf[1]-'0';
  if (!buf[2]) {
	cat4016_put_unumber (display,num); 	
	return;
  }
  num=num*10+(buf[2]-'0');
  if (!buf[3]) {    
	cat4016_put_unumber (display,num); 	
	return;
  }
  num=num*10+(buf[3]-'0');
  if (!buf[4]) {    
	cat4016_put_unumber (display,num); 	
	return;
  }
  num=num*10+(buf[4]-'0');
  if (!buf[5]) {
	cat4016_put_unumber (display,num); 	
	return;
  }
  cat4016_put_txt (display,"oflo"); 	
  uart_printf("\r\ndisplay overflow:%d [%s]\r\n",display,buf);    
 }
 
int main(void)
{
 	char c;
	uint8_t i,j,done,b, copypos=0;
	char buf[UART_BUFLEN<<2];
	
	
	uart_init();
	uart_puts("\r\ncounter init\r\n");	
	
	DDRA  = 0xff;  // A output   
	DDRC  = 0xff;  // C output   LEDS 1+2	
	DDRD  = 0xff;  // DE output   LEDS 3+4 
	
  // init_cat4016(uint8_t chip, uint8_t clk, uint8_t latch, uint8_t data, uint8_t blank)
	init_cat4016 (2, P_PC6, P_PC0,  P_PC4, PC2);
	init_cat4016 (1, P_PA7, P_PA1,  P_PA5, PA3);
	init_cat4016 (0, P_PA6, P_PA0,  P_PA4, PA2);	
	
    uart_puts("\r\ndone cat4016\r\n");	
	
	j=0;
	
	cat4016_put_unumber (0,1111);
	cat4016_put_unumber (1,2222);
	cat4016_put_unumber (2,3333);
	uart_printf ("inits done\r\n");
	
	uart_echo=FALSE;
    
    while (1) {	
		if (uart_done!=0) {
			uart_done=0;
/*			
			uart_printf ("\r\n");
			for (i=0; i<UART_BUFLEN; i++) {
				b=uart_buf[i];
				switch (b) {
					case 0: uart_putc('~');
							break;
					case '\n':uart_putc('*');					
							break;
					case '\r':uart_putc('+');					
							break;							
					default: uart_putc(uart_buf[i]);
					}
				}
	*/		

			j=0;
			for (i=0; i<UART_BUFLEN; i++) buf[i]=0;
			for (i=0; i<UART_BUFLEN; i++) {					
                    if (copypos>=UART_BUFLEN) {
//					    uart_printf ("OV\r\n");	
						copypos=0;
					}
					c=uart_buf[copypos];
					if (c=='\n') 
						break;
					if (c!=0) {						
					    uart_buf[copypos]=0;
						copypos++;
						if ((c!='\r') && (c!='\n')) {
							buf[j]=c;													
							j++;
							}
					}
//					uart_printf ("%d %d %d %c %c [%s]\r\n", 
//									i,j, copypos, buf[j], uart_buf[copypos],buf);
					
				}
//			uart_printf ("\r\n\r\nmain:got [%s], len %d, pos %d\r\n", buf, j, copypos);
			if (j>0)
				handle_txt (buf);
		}
    }

		
		
		
	
	

	
 return 0;
}
