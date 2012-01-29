/****************************************************************************
 *  Title  :   rotary encoder library (polling and interrupt-driven version)
 *
 *  
 *
 ****************************************************************************/
 


#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "bitops.h"


#define NUM_ENCODERS 4



uint8_t bit1[NUM_ENCODERS];
uint8_t bit2[NUM_ENCODERS];
uint8_t port[NUM_ENCODERS];

uint8_t encoder_states[16]={
       0,   -1,   1,   0,
       1,    0,   0,  -1,
      -1,    0,   0,   1,
       0,    1,  -1,   0};

uint8_t encoder[NUM_ENCODERS];

void init_encoder(uint8_t enc, uint8_t bit_1, uint8_t bit_2)

{
 encoder[enc]=0;
 bit_1=1<<bit_1;
 bit_2=1<<bit_2;
 bit1[enc]= bit_1;
 bit2[enc]= bit_2;
 DDRD &= ~ (bit_1 | bit_2);
 PORTD |= (bit_1 |bit_2);
 if (PIND & bit_1) encoder[enc]|=1;
 if (PIND & bit_2) encoder[enc]|=2;
}


int8_t get_encoder (uint8_t enc)
{
 uint8_t lookup, state;
    
 lookup=encoder[enc]<<2;
 
 if (PIND & bit1[enc]) lookup|=1;
 if (PIND & bit2[enc]) lookup|=2;
 state=encoder_states[lookup];
 encoder[enc]=lookup & 3;
 return state;
}
 
