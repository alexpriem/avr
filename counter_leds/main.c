/******************************************************************************
 Title:    ruff counter
 Author:   Alex Priem
 Date:     February 2012-Mar 2013
 Software: AVR-GCC 3.3 
 Hardware: ATMega32
           
 Description:
 t
  
*******************************************************************************/
#include <inttypes.h>
#include <avr/io.h>
 
#include "uart.h"
#include "hc595p.h"
#include "led595p.h"
#include "bitops.h"
#include "constants.h"

#include "delay.h"

//#define DEBUG 1
	    

 void handle_txt (char *buf)
 
 {
  uint8_t display;
  unsigned int  num;
  
  display=buf[0]-'a';
  if (display>3) {
  	#ifdef DEBUG
		uart_printf("\r\nunknown display:%d\r\n",display);
	#endif
	return;
  }
  num=buf[1]-'0';
  if (!buf[2]) {
	led595p_put_unumber (display,num); 	
	return;
  }
  num=num*10+(buf[2]-'0');
  if (!buf[3]) {    
	led595p_put_unumber (display,num); 	
	return;
  }
  num=num*10+(buf[3]-'0');
  if (!buf[4]) {    
	led595p_put_unumber (display,num); 	
	return;
  }
  num=num*10+(buf[4]-'0');
  if (!buf[5]) {
	led595p_put_unumber (display,num); 	
	return;
  }
  led595p_put_txt (display,"oflo"); 	
  #ifdef DEBUG
  	uart_printf("\r\ndisplay overflow:%d [%s]\r\n",display,buf);    
  #endif
 }
 
int main(void)
{
 	char c;
	uint8_t i,j, copypos=0;
	char buf[UART_BUFLEN<<2];
		
	
	uart_init();
	#ifdef DEBUG
	uart_puts("\r\ncounter init\r\n");	
	#endif
		
		 		//							sh_cp    st_cp    ds
//void hc595_setup (uint8_t chip, uint8_t clk, uint8_t cs, uint8_t d0, d1, d2, d3)	
	hc595p_setup(0, P_PA0, P_PA1, P_PA3, P_PA5, P_PA7, P_PA4);
	hc595p_setup(1, P_PC0, P_PC1, P_PC3, P_PC5, P_PC7, P_PC4);
	hc595p_setup(2, P_PB0, P_PB1, P_PB3, P_PB5, P_PB7, P_PB2);
	hc595p_setup(3, P_PD2, P_PD6, P_PD3, P_PD5, P_PD7, P_PD4);
	#ifdef DEBUG
    uart_puts("\r\ndone hc595\r\n");	
    #endif
	
	//hc595p_putc(0,0xbb, 0xa0, 0x37, 0xb5);
	hc595p_putc(0,0xff, 0xff, 0xff, 0xff);
	hc595p_putc(1,0xff, 0xff, 0xff, 0xff);
	hc595p_putc(2,0xff, 0xff, 0xff, 0xff);
	hc595p_putc(3,0xff, 0xff, 0xff, 0xff);
	
		  
		
	led595p_put_unumber (0,0);
	led595p_put_unumber (1,0);
	led595p_put_unumber (2,0);
	led595p_put_unumber (3,0);
	#ifdef DEBUG
	uart_printf ("inits done\r\n");
	#endif




/*	
	k=0;
	for(k=0;k<9999;k++) {
		if (k<10) {
			delay (450);}
		if (k<100) {
			delay (50);}
			
		led595p_put_unumber (0,k);
		led595p_put_unumber (1,k);
		led595p_put_unumber (2,k);
		led595p_put_unumber (3,k);
	}
*/
    
    while (1) {	
		if (uart_done!=0) {
			uart_done=0;
	

#if DEBUG
			uart_printf ("\r\n");
			
			for (i=0; i<UART_BUFLEN; i++) {
				uint8_t b;
				
				b=uart_buf[i];
								
				switch (b) { /*
					case 0: uart_putc('~');
							break;
					case '\n':uart_putc('*');					
							break;
					case '\r':uart_putc('+');					
							break;	
							*/
					default:  uart_putc(uart_buf[i]);
					}
			
				}
#endif

			j=0;
			for (i=0; i<UART_BUFLEN; i++) buf[i]=0;
			for (i=0; i<UART_BUFLEN; i++) {					
                    if (copypos>=UART_BUFLEN) {
//					    uart_printf ("OVERFLOW\r\n");	
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
