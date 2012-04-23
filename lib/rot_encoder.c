#include <stdlib.h>

#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "bitops.h"
#include "uart.h"
#include "delay.h"
#include "rot_encoder.h"
#include "constants.h"

#define NUM_ENCODERS 10


// pin 14 is de bit1-ingang
// pin 11 is de clock ingang van het register 
// na het inklokken een puls geven op pin 12 (bit1) -> dan nemen de uitgangen de bit1 uit het register over. 

volatile uint8_t *encoder_p_bit0[NUM_ENCODERS];         //  
volatile uint8_t *encoder_p_bit1[NUM_ENCODERS];         //  

uint8_t encoder_b_bit0[NUM_ENCODERS];
uint8_t encoder_b_bit1[NUM_ENCODERS];
uint8_t encoder_prev_bit0[NUM_ENCODERS];
uint8_t encoder_prev_bit1[NUM_ENCODERS];


int8_t enc_states[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};


void encoder_setup(uint8_t encoder, uint8_t bit0, uint8_t bit1)

{ uint8_t bbit0, bbit1;

 bbit0=1<< (bit0  & P_BITMASK);
 encoder_b_bit0[encoder] = bbit0;
 bbit1=1<< (bit1 & P_BITMASK); 
 encoder_b_bit1[encoder] = bbit1;
  
 bit0=bit0 & P_PORTMASK;
 if (bit0==P_PORTA) {encoder_p_bit0[encoder]=&PINA; DDRA&=~bbit0; PORTA|=bbit0;}
 if (bit0==P_PORTB) {encoder_p_bit0[encoder]=&PINB; DDRB&=~bbit0; PORTB|=bbit0;}
 if (bit0==P_PORTC) {encoder_p_bit0[encoder]=&PINC; DDRC&=~bbit0; PORTC|=bbit0;}
 if (bit0==P_PORTD) {encoder_p_bit0[encoder]=&PIND; DDRD&=~bbit0; PORTD|=bbit0;}

 bit1=bit1 & P_PORTMASK;
 if (bit1==P_PORTA) {encoder_p_bit1[encoder]=&PINA; DDRA&=~bbit1; PORTA|=bbit1;}
 if (bit1==P_PORTB) {encoder_p_bit1[encoder]=&PINB; DDRB&=~bbit1; PORTB|=bbit1;}
 if (bit1==P_PORTC) {encoder_p_bit1[encoder]=&PINC; DDRC&=~bbit1; PORTC|=bbit1;}
 if (bit1==P_PORTD) {encoder_p_bit1[encoder]=&PIND; DDRD&=~bbit1; PORTD|=bbit1;}

 encoder_prev_bit0[encoder]=bbit0;
 encoder_prev_bit1[encoder]=bbit1;
}

int8_t encoder_poll (uint8_t encoder)

{
 uint8_t b0,b1, ab, state;
 static int8_t old_AB = 0;
 
 //old_ab=encoder_prev_bit0[encoder]+encoder_prev_bit1[encoder];
 b0=bit_get(*encoder_p_bit0[encoder], encoder_b_bit0[encoder]); 
 b1=bit_get(*encoder_p_bit1[encoder], encoder_b_bit1[encoder]); 
 if (b0) b0=1;
 if (b1) b1=2;
 ab=b0+b1;
 old_AB<<=2;
 old_AB |= ( ab & 0x03 );
 //if ((b0==prevb0) && (b1==prevb1)) return 0;
 //uart_printf ("%d %d %d %d\r\n", prevb0, prevb1, b0, b1);
 
 state=enc_states[( old_AB & 0x0f )];
 if (state!=0) {
	uart_printf ("s:%d\r\n", state);
	return state;
}
 return FALSE;
}


int8_t encoder_poll_range (uint8_t encoder, int8_t *var, int8_t min, int8_t max, int8_t step)

{
 int8_t status, changed;
 
 changed=FALSE;
 status=encoder_poll (encoder);
 uart_printf ("%d %d %d %d %d\r\n",status, *var, min, max, step);
 if ((status==1) && (*var<max)) {
    uart_printf ("+");	
	*var+=step;	
	if (*var>max) *var=max;	
	return TRUE;
	}
 if ((status==-1) && (*var>min)) {
	uart_printf ("-");
	*var-=step;
	if (*var<min) *var=min;
	changed=TRUE;	
	}
 uart_printf ("%d %d %d %d %d %d\r\n",status, changed, *var, min, max, step);
 return changed;
}
