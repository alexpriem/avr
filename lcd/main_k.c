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
#include "uart1.h"
#include "cat4016.h"
#include "bitops.h"

#include "delay.h"
#include "lcd_4_40x4.h"
#include "constants.h"
#include "rot_encoder.h"
#include "keypad.h"
#include "hc595.h"
	
#include "softuart.h"



uint8_t relaystate=0x0;


	/*
	lcd_puts (0, "\nklmno 3");
	lcd_puts (0, "\npqrst 4");
	lcd_puts (0, "\nuvw 5");
	lcd_puts (0, "\nzAB 6");
	lcd_puts (0, "\nEFG 7");
	lcd_puts (0, "\nKLM 8");
	lcd_puts (0, "\n# 9"); 
	*/
	
	
	/*
	lcd_puts (0, "##234567890abcdefghijklmno##rstuvwxyz\n");
	lcd_puts (0, "12##$567890abcdefghijklmnopq##tuvwxyz\n");
	lcd_puts (0, "12345##7890abcdefghijklmnopqs##vwxyz\n");
	lcd_puts (0, "1234567##90abcdefghijklmnopqrstu##yz\n");
	*/
	
//uint8_t mapping[17]={12,13,15,14,  11,9,7,6,   0,8,2,5,   10,7,1,4, 255};
          //keycode:   0 1 2  3      4 5 6  7    8 9 10 11  12 13 14 15  16				
			
uint8_t	mapping[17]={0xbb, 0x5f, 0x5d, 0x57,    //  0 1 2 3     
				 0xd5, 0xd7, 0xdd, 0xbe,    //  4 5 6 7
				 0xae, 0xba, 0xab, 0xaf,     //  8 9 a b
				 0xbf, 0xaa, 0xdf, 0x55,
				 0x1f                        // c+1

				 };   //  c d e f 
				 
				 
//uint8_t keypad_bits[16];
volatile uint8_t *keypad_port;

void init_keypad_4x4s (uint8_t port)
						
{
   
	if (port==P_PORTA) {keypad_port=&PINA; DDRA=0; PORTA=0xff;}
	if (port==P_PORTB) {keypad_port=&PINB; DDRB=0; PORTB=0xff;}
	if (port==P_PORTC) {keypad_port=&PINC; DDRC=0; PORTC=0xff;}
	if (port==P_PORTD) {keypad_port=&PIND; DDRD=0; PORTD=0xff;}
}					

uint8_t keypad_getch (void) {
 uint8_t c,i;
 
  c=*keypad_port;  
  if (c==0xff) return 0xff;
  for (i=0; i<16; i++) {
    if (mapping[i]==c) return i;
  }
  uart_printf ("[%x]",c);
  return 0xfe;
}		  

uint8_t keypad_w_getch (void) {
 uint8_t c,c2,i;
 
 
  c=0xff;
  while (c==0xff) {
	 c=*keypad_port;
	 }  
  
  delay(50);
  i=0;
  c2=*keypad_port;
  while (c!=c2) {
  	 c2=*keypad_port;
  	 i++;
  	 if (i>10) return 0xfd;
  	}
  
  for (i=0; i<17; i++) {
    if (mapping[i]==c) return i;
  }
  

  uart_printf ("not in mapping:%x\r\n",c);
  return 0xfe;
}		  
	  

void set_wind_dir_none (void) {
	uart_printf ("left\r\n");	
	relaystate=(relaystate & 0x5f);  // of | 0xa0
	hc595_putc (0,relaystate);
}


void set_wind_dir_left (void) {
	uart_printf ("left\r\n");	
	relaystate=(relaystate & 0xdf) | 0x80;
	hc595_putc (0,relaystate);
}

void set_wind_dir_right (void) {
	uart_printf ("right\r\n");
	relaystate=(relaystate & 0x7f) | 0x20;
	hc595_putc (0,relaystate);
}

void set_fill (void) {
	relaystate=relaystate & 0xf7;
	hc595_putc (0,relaystate);
}

void set_wind (void){
	relaystate=relaystate | 0x08;
	hc595_putc (0,relaystate);
}

void stop_motor (void){
	relaystate=relaystate | 0x40;
	hc595_putc (0,relaystate);
}

void start_motor (void){
	relaystate=relaystate & 0xbf;
	hc595_putc (0,relaystate);
}

void fill (unsigned int num) {	
	uart_printf ("fill:%d\r\n",num);
	set_fill();
	sleep(100);
	start_motor();

}


void wind (unsigned int num) {	
	uart_printf ("wind:%d\r\n",num);
	set_wind();
	sleep(100);
	start_motor();
}

void wind_left (unsigned int num) {	
	uart_printf ("wind_right:%d\r\n",num);
	set_wind_dir_left ();
	set_wind();
	sleep(100);
	start_motor();
	wind(num);
}

void wind_right (unsigned int num) {
	
	uart_printf ("wind_left:%d\r\n",num);
	set_wind_dir_right ();
	set_wind();
	sleep(100);
	start_motor();
	wind(num);	
}


void update_num (unsigned int num) {

	uart_printf ("num: %d\r\n",num);
}

void special (void) {
	uart_printf ("special\r\n");
}
		  

uint16_t dumps=0;

void dump_uart1 (void) {
	char buf[40];
	uint8_t i;


	uart_printf("%d",dumps);

	for (i=0; i<32; i++) {
		buf[i]=uart1_buf[i];		
	}
	buf[32]=13;
	buf[33]=10;
	buf[34]=0;
	uart_puts(buf);
	dumps++;
	uart_puts("ok\r\n");
}


int main(void)
{
    unsigned int  num, prevnum;
	uint8_t a;
	int c, prevc;
	
		  
	
	//softuart_init();
	uart_init();
	uart1_init();
	delay(3000);
	uart1_puts ("\r\nreset -- init port 1\r\n");	
	//dump_uart1();
	uart_puts ("b555\n");
	//dump_uart1();
	uart_puts ("b222\n");
	//dump_uart1();
	uart_puts ("c333\n");
	//dump_uart1();
	uart_puts ("d444\n");
	//dump_uart1();
	//softuart_puts("\r\nsoftuart reset -- init\r\n");
	//init_keypad_4x4_s (ports);	
	
	init_keypad_4x4s (P_PORTC);

	lcd_setup (0, P_PA2, P_PA4, P_PA6, P_PA0,
			  P_PA1, P_PA3, P_PA5, P_PA7);	 

			  //lcd_setup  (uint8_t chip,  uint8_t strobe, uint8_t clock, uint8_t io)
	lcd_setup_info (0, HD44780, 20, 2);
	
	lcd_init (0, LCD_DISP_ON_CURSOR_BLINK);

  // relays
	hc595_setup(0, P_PD7, P_PD4, P_PD5);
	hc595_putc (0,0x01);

	/*
	uart_puts ("0x01\r\n");
	delay(500);
	hc595_putc (0,0x02);
	uart_puts ("0x02\r\n");
	delay(500);
	hc595_putc (0,0x04);
	uart_puts ("0x04\r\n");
	delay(500); */

	hc595_putc (0,0x08);
	uart1_puts ("0x08\n");
	lcd_puts (0, "0xf");
	delay(1000);

	hc595_putc (0,0x00);
	uart1_puts ("0x00\r\n");
	lcd_puts (0, "0x00");
	delay(1000);

	hc595_putc (0,0x08);
	uart1_puts ("0xff\r\n");
	lcd_puts (0, "0xff");
	delay(1000);

	hc595_putc (0,0x00);
	uart1_puts ("0x00\r\n");
	lcd_puts (0, "0x00");
	delay(1000);

	hc595_putc (0,0x08);
	uart1_puts ("0x08\r\n");
	lcd_puts (0, "0x08");
	delay(1000);
	hc595_putc (0,0x20);
	uart1_puts ("0x20\r\n");
	lcd_puts (0, "0x20");
	delay(1000);
	hc595_putc (0,0x40);
	uart1_puts ("0x40\r\n");
	lcd_puts (0, "0x40");
	delay(1000);
	hc595_putc (0,0x80);
	uart1_puts ("0x80\r\n");
	lcd_puts (0, "0x80");


	//void lcd_setup (uint8_t rs, uint8_t rw, uint8_t enable1, uint8_t enable2,
	//			uint8_t db0, uint8_t db1, uint8_t db2, uint8_t db3 ) 
	 //lcd_setup (0, P_PC2, P_PC4, P_PC6, P_PC0,
//			  P_PC1, P_PC3, P_PC5, P_PC7);	 
	
	
	lcd_puts (0, "axbcde x");
	lcd_puts (0, "\nfghij y");
	
	uart_printf ("start loop \r\n",c);
	c=0;
	prevc=0;
	num=0;
	prevnum=0;
	for (;;) {

		c=keypad_w_getch();
		

		if (c!=prevc)  {
			a=PINC;
			uart_printf ("%x %x %d\r\n",a,c,num);
			prevc=c;
			switch (c) {
				case 0x0f: wind(num);
							update_num(num);
							break;
				case 0x0e: wind_right (num);					   
						update_num(num);
						   break;
				case 0x0d: wind_left (num);	
							update_num(num);					
						   break;
				case 0x0c: num=0;					   
							prevnum=0;
							update_num(num);
						   break;
				case 0x0a: fill (num);		
							update_num(num);			   
							break;
				case 0x0b: num=prevnum;		   				
							update_num(num);
			   				break;
			   	case 0x10: special();
			   			   break;
			   	case 0xff:
			   	case 0xfd:
			   	case 0xfe: break;


				default: if (num<1000)  {
							prevnum=num;
							num=num*10 + c;						
							update_num(num);
						}
				}			
			
			}
		




/*		
		if (c!=prevc) {
			uart_printf (":%d reps omitted\r\n", count);
			uart_printf ("%d %d\r\n", c, num);
			prevc=c;
			count=0;
			} 
		else {
		  count++;
		  if (count>65534) {
			uart_printf ("*%d %d\r\n", c, num);
			count=0;
			}
		  } */
	}
	
 return 0;
}
