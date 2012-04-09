/******************************************************************************
 Title:    Tuberelayswitch
 Author:   Alex Priem
 Date:     January 2012
 Software: AVR-GCC 3.3 
 Hardware: ATMega32
           
 Description:
 
  
*******************************************************************************/
#include <inttypes.h>
#include <avr/io.h>

#include "uart.h"
#include "bitops.h"
#include "delay.h"
#include "cat4016.h" 


volatile uint8_t *pkeypad_4x4_out[5];         //  
volatile uint8_t *pkeypad_4x4_in[5];       //  

uint8_t bkeypad_4x4_out[5];
uint8_t bkeypad_4x4_in[5];



uint8_t getc_3x4_keypad (void) 

{
 uint8_t c=0x12;

 c=PINB;
 return c;
}

void keypad_4x4_setup (  uint8_t in1, uint8_t in2, uint8_t in3, uint8_t in4, 
						uint8_t out1, uint8_t out2, uint8_t out3, uint8_t out4 ) 
{

uint8_t bin1, bin2, bin3, bin4, bout1, bout2, bout3, bout4;
volatile uint8_t bout,i;
volatile uint8_t *pout;

 bin1=1<< (in1  & P_BITMASK);
 bkeypad_4x4_in[0] = bin1;
 bin2=1<< (in2  & P_BITMASK);
 bkeypad_4x4_in[1] = bin2;
 bin3=1<< (in3  & P_BITMASK);
 bkeypad_4x4_in[2] = bin3;
 bin4=1<< (in4  & P_BITMASK);
 bkeypad_4x4_in[3] = bin4;
 
 in1=in1 & P_PORTMASK;
 if (in1==P_PORTA) {pkeypad_4x4_in[0]=&PINA; DDRA&=~bin1; bit_set(PORTA, bin1);}
 if (in1==P_PORTB) {pkeypad_4x4_in[0]=&PINB; DDRB&=~bin1; bit_set(PORTB, bin1);}
 if (in1==P_PORTC) {pkeypad_4x4_in[0]=&PINC; DDRC&=~bin1; bit_set(PORTC, bin1);}
 if (in1==P_PORTD) {pkeypad_4x4_in[0]=&PIND; DDRD&=~bin1; bit_set(PORTD, bin1);}

 in2=in2 & P_PORTMASK;
 if (in2==P_PORTA) {pkeypad_4x4_in[1]=&PINA; DDRA&=~bin2; bit_set(PORTA, bin2);}
 if (in2==P_PORTB) {pkeypad_4x4_in[1]=&PINB; DDRB&=~bin2; bit_set(PORTB, bin2);}
 if (in2==P_PORTC) {pkeypad_4x4_in[1]=&PINC; DDRC&=~bin2; bit_set(PORTC, bin2);}
 if (in2==P_PORTD) {pkeypad_4x4_in[1]=&PIND; DDRD&=~bin2; bit_set(PORTD, bin2);}

 in3=in3 & P_PORTMASK;
 if (in3==P_PORTA) {pkeypad_4x4_in[2]=&PINA; DDRA&=~bin3; bit_set(PORTA, bin3);}
 if (in3==P_PORTB) {pkeypad_4x4_in[2]=&PINB; DDRB&=~bin3; bit_set(PORTB, bin3);}
 if (in3==P_PORTC) {pkeypad_4x4_in[2]=&PINC; DDRC&=~bin3; bit_set(PORTC, bin3);}
 if (in3==P_PORTD) {pkeypad_4x4_in[2]=&PIND; DDRD&=~bin3; bit_set(PORTD, bin3);}

 in4=in4 & P_PORTMASK;
 if (in4==P_PORTA) {pkeypad_4x4_in[3]=&PINA; DDRA&=~bin4; bit_set(PORTA, bin4);}
 if (in4==P_PORTB) {pkeypad_4x4_in[3]=&PINB; DDRB&=~bin4; bit_set(PORTB, bin4);}
 if (in4==P_PORTC) {pkeypad_4x4_in[3]=&PINC; DDRC&=~bin4; bit_set(PORTC, bin4);}
 if (in4==P_PORTD) {pkeypad_4x4_in[3]=&PIND; DDRD&=~bin4; bit_set(PORTD, bin4);}

 
 bout1=1<< (out1  & P_BITMASK);
 bkeypad_4x4_out[0] = bout1;
 bout2=1<< (out2  & P_BITMASK);
 bkeypad_4x4_out[1] = bout2;
 bout3=1<< (out3  & P_BITMASK);
 bkeypad_4x4_out[2] = bout3;
 bout4=1<< (out4  & P_BITMASK);
 bkeypad_4x4_out[3] = bout4;
 
 out1=out1 & P_PORTMASK;
 if (out1==P_PORTA) {pkeypad_4x4_out[0]=&PORTA; DDRA|=bout1;}
 if (out1==P_PORTB) {pkeypad_4x4_out[0]=&PORTB; DDRB|=bout1;}
 if (out1==P_PORTC) {pkeypad_4x4_out[0]=&PORTC; DDRC|=bout1;}
 if (out1==P_PORTD) {pkeypad_4x4_out[0]=&PORTD; DDRD|=bout1;}

 out2=out2 & P_PORTMASK;
 if (out2==P_PORTA) {pkeypad_4x4_out[1]=&PORTA; DDRA|=bout2;}
 if (out2==P_PORTB) {pkeypad_4x4_out[1]=&PORTB; DDRB|=bout2;}
 if (out2==P_PORTC) {pkeypad_4x4_out[1]=&PORTC; DDRC|=bout2;}
 if (out2==P_PORTD) {pkeypad_4x4_out[1]=&PORTD; DDRD|=bout2;}

 out3=out3 & P_PORTMASK;
 if (out3==P_PORTA) {pkeypad_4x4_out[2]=&PORTA; DDRA|=bout3;}
 if (out3==P_PORTB) {pkeypad_4x4_out[2]=&PORTB; DDRB|=bout3;}
 if (out3==P_PORTC) {pkeypad_4x4_out[2]=&PORTC; DDRC|=bout3;}
 if (out3==P_PORTD) {pkeypad_4x4_out[2]=&PORTD; DDRD|=bout3;}

 out4=out4 & P_PORTMASK;
 if (out4==P_PORTA) {pkeypad_4x4_out[3]=&PORTA; DDRA|=bout4;}
 if (out4==P_PORTB) {pkeypad_4x4_out[3]=&PORTB; DDRB|=bout4;}
 if (out4==P_PORTC) {pkeypad_4x4_out[3]=&PORTC; DDRC|=bout4;}
 if (out4==P_PORTD) {pkeypad_4x4_out[3]=&PORTD; DDRD|=bout4;}
 
	for (i=0; i<4; i++) {
		pout=pkeypad_4x4_out[i]; 
		bout=bkeypad_4x4_out[i]; 
		bit_set (*pout, bout);
	}

 DDRA=0xf0;
 PORTA=0x0f;	
}





uint8_t keypad_4x4_getc (void) 

{ uint8_t bout, bin, key,i,j;
  volatile uint8_t *pout, *pin;
 
 key=0x10;
 for (i=0; i<4; i++) {	
	pout=pkeypad_4x4_out[i]; 
	bout=bkeypad_4x4_out[i]; 
	bit_clr (*pout, bout);
 	
	for (j=0; j<4; j++) {
		pin=pkeypad_4x4_in[j]; 
		bin=bkeypad_4x4_in[j]; 
		if (bit_get (*pin, bin)==0) { key=(i+i+i+i) +j; };		
	}
	
	bit_set (*pout, bout);
 }
 
 return key;
}
