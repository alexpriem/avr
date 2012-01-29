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
#include "bitops.h"

#include "delay.h"
#include "leds.h"
#include "keypad.h"





// codes voor 0-9:

unsigned char numbers[11]={
   0x3f,  // 0
   0x21,  // 1
   0x5b,  // 2
   0x73,  // 3
   0x65,  // 4
   0x76,  // 5
   0x7e,  // 6
   0x23,  // 7
   0x7f,  // 8
   0x77,  // 9
   00     // blank
   };

unsigned char hex_numbers[16]={
   0x3f,  // 0
   0x21,  // 1
   0x5b,  // 2
   0x73,  // 3
   0x65,  // 4
   0x76,  // 5
   0x7e,  // 6
   0x23,  // 7
   0x7f,  // 8
   0x77,  // 9
   0x7b,  // a
   0x7c,  // b
   0x58,  // c
   0x79,  // d
   0x5f,  // e
   0x4e,  // f
   };
   

   unsigned char letters[27]={
   0x7b,  // a
   0x7c,  // b
   0x58,  // c
   0x79,  // d
   0x5f,  // e
   0x4e,  // f
   0x67,  // g
   0x6c,  // h
   0x21,  // i
   0x31,  // j
   0x6d,  // k
   0x21,  // l
   0x6a,  // m
   0x68,  // n
   0x78,  // o
   0x4f,  // p
   0x67,  // q
   0x48,  // r
   0x76,  // s
   0x5c,  // t
   0x3d,  // u
   0x38,  // v
   0x38,  // w
   0x6d,  // x
   0x75, // y
   0x5b, // z
   0x00 // blank
   };
   
   
   unsigned char special_up[27]={
   0x43,  // a
   0x45,  // b
   0x46,  // c
   0x7,  // d
   };  // j
   
   unsigned char special_down[27]={
   0x70,  // a
   0x38,  // b
   0x58,  // c
   0x68,  // d
   };  // j
   
   unsigned char special_updown[6]={
   0x3e,  // 
   0x1f,  // c
   0x2f,  //    
   0x37,  // d
   0x3b, 
   0x3d
   };  // j

   
int main(void)
{
    int rev=0;
	int i,c;
	
	DDRB = 0x00;  // C inputs
	PORTB= 0xff;  // pullups aan
	//PORTC= 0x00;  // pullups uit
	DDRA  = 0xff;  // A output  
	
	init_74hc595 (0, P_PA5, P_PA3,  P_PA1);
	init_74hc595 (1, P_PA5, P_PA3,  P_PA2);
	init_74hc595 (2, P_PA5, P_PA3,  P_PA4);
//	init_74hc595 (3, P_PA5, P_PA3,  P_PA0);
	init_74hc595 (3, P_PA5, P_PA3,  P_PA7);

	
//    uart_init ();	
//	uart_printf ("rev:%d\r\n", rev);
	for (;;) {
		c=getc_3x4_keypad ();	
		//uart_printf ("keypad:%d\r\n",c);
		s595_putc(1, hex_numbers[c >>4]);
		s595_putc(0, hex_numbers[c & 0x0f]);
		
		}	
	
	

	 
  
	 
	

	for (i=0; i<=5; i++) {
		delay (200);	
		s595_putc (0, special_updown[i]);		
	 }

/*
	 for (i=0; i<=10; i++) {
		delay (200);	
		s595_putc (0, numbers[i]);		
	 }
	*/
	
	 for (i=0; i<=26; i++) {
		delay (200);	
		s595_putc (0, letters[i]);		
	 }

	 

	 
	/*
	s595_putc (0, 0x01);		
	delay (200);	
	s595_putc (1, 0x80);	
	delay (200);
	s595_putc (2, 0x01);	
	delay (200);
	s595_putc (3, 0x80);	
	*/
	
	//s595_putc (1, 0xf8);
//	s595_putc (2, 0xf8);
//	s595_putc (3, 0xf8);
//	s595_putc (4, 0xf8);	
	uart_printf ("done:\r\n");
	//uart_printf ("done:%d -- %d\r\n",i, val);
			//                     12          11             14
			//					st_cp       sh_cp             ds
// init_74hc595(uint8_t chip, uint8_t clk, uint8_t cs, uint8_t data)
	
	
 return 0;
}
