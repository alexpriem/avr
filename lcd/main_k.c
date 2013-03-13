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
	
#include "softuart.h"

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
			
uint8_t	mapping[16]={0xbb, 0x5f, 0x5d, 0x57,    //  0 1 2 3     
				 0xd5, 0xd7, 0xdd, 0xbe,    //  4 5 6 7
				 0xae, 0xba, 0xab, 0xaf,     //  8 9 a b
				 0xbf, 0xaa, 0xdf, 0x55};   //  c d e f 
				 
				 
uint8_t keypad_bits[16];
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
 uint8_t c,i;
 
 
  c=0xff;
  while (c==0xff) {
	 c=*keypad_port;
	 }  
  for (i=0; i<16; i++) {
    if (mapping[i]==c) return i;
  }
  uart_printf ("[%x]",c);
  return 0xfe;
}		  
	  
		  
int main(void)
{
    unsigned int  num, count;
	uint8_t a;
	int c, prevc;
	
		  
	
	//softuart_init();
	uart_init();
	uart_puts ("\r\nreset -- init\r\n");
	//softuart_puts("\r\nsoftuart reset -- init\r\n");
	//init_keypad_4x4_s (ports);	
	
	init_keypad_4x4s (P_PORTC);

/*  // relays
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
	 //lcd_setup (0, P_PC2, P_PC4, P_PC6, P_PC0,
//			  P_PC1, P_PC3, P_PC5, P_PC7);	 
	 lcd_setup (0, P_PA2, P_PA4, P_PA6, P_PA0,
			  P_PA1, P_PA3, P_PA5, P_PA7);	 

			  //lcd_setup  (uint8_t chip,  uint8_t strobe, uint8_t clock, uint8_t io)
	lcd_setup_info (0, HD44780, 20, 2);
	
	lcd_init (0, LCD_DISP_ON_CURSOR_BLINK);
	
	
	lcd_puts (0, "axbcde x");
	lcd_puts (0, "\nfghij y");
	
	uart_printf ("start loop \r\n",c);
	c=0;
	prevc=0;
	for (;;) {
		c=keypad_getch();
		a=PINC;
		uart_printf ("%x %x\r\n",a,c);
		num=mapping[c];		
		
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
		  }
	}
	
 return 0;
}
