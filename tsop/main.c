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
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "delay.h"

uint8_t got_int;
uint8_t cnt;


#define MAX_LEN 256
#define SAFE_LEN 128

/*  
ISR (SIG_INTERRUPT0)
{ 
 got_int=1;
 cnt++;
 if (cnt>20) {
	uart_printf ("5times\r\n0");
	cnt=0;
 }
}

*/
 

/*12345678 12345678                          12345678 
2200000000 11011101 11001000 001101112222  // 1
2200000000 11011101 01110000 100011112222  // 2
2200000000 11011101 01110000 100011112
2200000000 11011101 10001000 011101112222  // 3
2200000000 11011101 11110000 000011112222 // 4
2200000000 11011101 01010000 101011112222 // 5
2200000000 11011101 10110000 010011112222 // 6
2200000000 11011101 11010000 001011112222 // 7
2200000000 11011101 01100000 100111112222 // 8
2200000000 11011101 10010000 011011112222 // 9
1200000000 11011101 01000000 101111112222 // 09
  12345678

22000000001101110101101000100101112222  // up 
22000000001101110101001000101101112222  // down
22000000001101110111101000000101112222  // left
22000000001101110100101000110101112222  // right
22000000001101110110101000010101112222  // enter


22000000001101110111000000001111112222 // vol up
22000000001101110100001000111101112222 // vol down
22000000001101110110000000011111112222 // ch up
22000000001101110100110000110011112222 // ch down
22000000001101110100111000110001112222 // power
  12345678901234567890123456789012
*/
 

 
 
 unsigned long convert_buffer (short int *buf, short int len)  
 {unsigned long cmd=0, d=1;
  short int i;
  d=65536<<15;
 
  for (i=0; i<len; i++) {
	if ((buf[i]==1) || (buf[i]==0)) {	  
	  d=d>>1;
	}
	if (buf[i]==1) cmd|=d;
//	uart_printf ("%d %l %l\r\n", i, d, cmd);
	if (i>32) break;
  }
  cmd=cmd<<1;
  
  return cmd;
 }

 
 int get_longbyte(long l, short int num) 
 
 { int ret=0;
   //uart_printf ("%l:%d\r\n",l, num);
	switch (num) {
		case 0: ret=l & 0xff;
				break;
		case 1: ret=(l>>8) & 0xff;
				break;
		case 2: ret=(l>>16) & 0xff;
				break;
		case 3: ret=(l>>24) & 0xff;		
				break;
	}
	return ret;
 }
 
 

int main(void)
{
	int rev=0;	
	long int loop, cmd;
	short int cmdbuf[MAX_LEN];
	short int i, len_cmd, d, prevd;
	
	int m,h,l,p;
	
	
	
    uart_init ();
	uart_printf ("rev:%d\r\n", rev);	

	/*
	MCUCR = (1<<ISC01) | (1<<ISC00);	
	// enable the interrupt pins
	GICR |= _BV(INT0);
*/
	
	// Set d2 and d3 to be inputs
	DDRD &= ~ _BV(PD2);
// Pullups off
	PORTD &= ~ _BV(PD2);
	
	
	  sei(); 
	  
	 got_int=0;
	 loop=0;
	 len_cmd=0;
	 cnt=0;
	 prevd=0;
	 d=0;
	 
	 
	 loop=65540;
	 m=get_longbyte(loop, 0);
	 l=get_longbyte(loop, 1);
	 h=get_longbyte(loop, 2);
	 p=get_longbyte(loop, 3);
	 /*
	 m=(65530 & 0xff);
	 l=(65530>>8) & 0xff;
	 h=1;
	 p=2; */
	 uart_printf ("got: %d  %d %d %d \r\n", m,l, h,p);
	 
	 
	 for (;;) {						
			while  ((d==prevd) &&  (loop<65535) && (uart_done==0))  { 
				loop++;
				d=PIND & 4;
				}		
			
			prevd=d;
		
			
			if (loop>=65535) {
				//uart_printf ("timeout\r\n");
				loop=0;
				if (len_cmd>0) {
					for (i=0; i< len_cmd; i++)	uart_printf ("%d", cmdbuf[i]);						
					//uart_printf ("\r\n");				
					
					cmd=convert_buffer (cmdbuf, len_cmd);
					m=get_longbyte(cmd, 0);
					l=get_longbyte(cmd, 1);
					h=get_longbyte(cmd, 2);
					p=get_longbyte(cmd, 3);
					uart_printf ("#: %d  %d %d %d \r\n", p,h, l,m);
					len_cmd=0;
				}
				continue;
			} 
			
			if (prevd==4) {
				continue;
			}
			
			got_int=0;
			if (uart_done) {
				uart_printf ("\r\n\r\n");
				uart_done=0;
			}			
						
			if (loop<=300) 	cmdbuf[len_cmd]=0;				
				else 
					if (loop<=600) cmdbuf[len_cmd]=1;
					else
						{							
						if (len_cmd>1) 
								if (cmdbuf[len_cmd-1]==2) 
									len_cmd--;
						}
			
			//cmdbuf[len_cmd]=loop;
			len_cmd++;
			if (len_cmd> SAFE_LEN) {
				len_cmd=0;
				uart_putc('*');
			}
	    	//uart_printf ("got 1, loop:%u %u\r\n",  loop & 65535,  (unsigned int)loop>>2);	
			//uart_printf ("loop:%l\r\n",  loop);
			//uart_putc('+');
			loop=0;
	}
	return 0;
}
