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
	
uint8_t mapping[17]={12,13,15,14,  11,9,7,6,   0,8,2,5,   10,7,1,4, 255};
          //keycode:   0 1 2  3      4 5 6  7    8 9 10 11  12 13 14 15  16				

		

uint8_t keypad_bits[16];
volatile uint8_t *keypad_ports[16];

void init_keypad_4x4_s (uint8_t *portpins_key)
						
{
 uint8_t i, keypad_bit,k;
 
  for (i=0; i<16; i++) {
	keypad_bit=1<< (portpins_key[i]  & P_BITMASK);
	uart_printf("bit %d:%x\r\n",i,keypad_bit);
	keypad_bits[i]=keypad_bit;	
	k=portpins_key[i] & P_PORTMASK;	 
	if (k==P_PORTA) {keypad_ports[i]=&PINA; DDRA&=~keypad_bit; bit_set(PORTA, keypad_bit);}
	if (k==P_PORTB) {keypad_ports[i]=&PINB; DDRB&=~keypad_bit; bit_set(PORTB, keypad_bit);}
	if (k==P_PORTC) {keypad_ports[i]=&PINC; DDRC&=~keypad_bit; bit_set(PORTC, keypad_bit);}
	if (k==P_PORTD) {keypad_ports[i]=&PIND; DDRD&=~keypad_bit; bit_set(PORTD, keypad_bit);}
	}
	
}					

void get_key (void) {
 uint8_t i,bit;
 volatile uint8_t *port;
 
  for (i=0; i<16; i++) {
    bit=keypad_bits[i];
	port=keypad_ports[i];
	if ((i & 0x03)==0) uart_puts(" ");
	if ((bit_get(*port, bit) & bit)==bit) 
		uart_puts("1");
	else
		uart_puts("0");	
  }
  uart_printf (" %x %x %x",PINA, PINB, PIND);
  uart_puts("\r\n");
   
}		  
	  
		  
int main(void)
{
    unsigned int  num, count;
	uint8_t a;
	int c, prevc;
	uint8_t ports[16]={ P_PA6, P_PA4, P_PA2, P_PA0, 
		P_PA7, P_PA5, P_PA3, P_PA1, 
		P_PB1, P_PB3, P_PB5, P_PD2, 
		P_PB0, P_PB2, P_PB4, P_PD4};		
			/*			 key_c,  key_b, key_0,  key_a,
						 key_d,  key_9, key_8,  key_7,
						 key_e,  key_6, key_5,  key_4,
						 key_f,  key_3, key_2,  key_1
						 */
		  
	
	softuart_init();
	uart_puts ("\r\nreset -- init\r\n");
	softuart_puts("\r\nsoftuart reset -- init\r\n");
	init_keypad_4x4_s (ports);	
/*	for (;;) {
		get_key();
		}
	*/
	
	keypad_4x4_setup (P_PA1, P_PA3, P_PA5, P_PA7,
	                 P_PA0, P_PA2, P_PA4, P_PA6);

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

	//void lcd_setup (uint8_t rs, uint8_t rw, uint8_t enable1, uint8_t enable2,
	//			uint8_t db0, uint8_t db1, uint8_t db2, uint8_t db3 ) 
	 lcd_setup (0, P_PC2, P_PC4, P_PC6, P_PC0,
			  P_PC1, P_PC3, P_PC5, P_PC7);	 
	//lcd_setup  (uint8_t chip,  uint8_t strobe, uint8_t clock, uint8_t io)
	lcd_setup_info (0, HD44780, 20, 2);
	
	lcd_init (0, LCD_DISP_ON_CURSOR_BLINK);
	
	
	lcd_puts (0, "abcde 1");
	lcd_puts (0, "\nfghij 2");
	
	uart_printf ("start loop \r\n",c);
	c=0;
	prevc=0;
	for (;;) {
		c=keypad_4x4_getc();	
		a=PINA;
		uart_printf ("%x\r\n",a);
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
