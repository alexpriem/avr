#include <stdlib.h>
#include <avr/io.h>
#include "ad_da.h"
#include "uart.h"
#include "bitops.h"

#define NUM_ADCS 4


#define bit_get(p,m) ((p) & (m))
#define bit_set(p,m) ((p) |= (m))
#define bit_clr(p,m) ((p) &= ~(m))
#define bit_flip(p,m) ((p) ^= (m))
#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m))
#define BIT(x) (0x01 << (x))
#define LONGBIT(x) ((unsigned long)0x00000001 << (x)) 



uint8_t cs_bit[NUM_ADCS];
uint8_t clk_bit[NUM_ADCS];
uint8_t dout_bit[NUM_ADCS];
int val[NUM_ADCS];


void delay (void)
{ int j;

  for (j=0; j<20; j++) {
    __asm__ __volatile__( "rjmp 1f\n 1:" );
    }

}

void init_adc (uint8_t adc, uint8_t cs, uint8_t dout, uint8_t clk)
{

 cs = 1<<cs;
 clk = 1<<clk;
 dout=1<<dout;
 
 clk_bit[adc] = clk;
 cs_bit[adc]  = cs;
 dout_bit[adc]= dout;

  bit_set (DDRD, cs|clk); 
  bit_clr (DDRD, dout);
  bit_set (PORTD, dout);  /* pullup */ 


/* port zetten; default PORTD */

  bit_set (PORTD, cs);
}



unsigned int  get_adc (uint8_t adc)

{
 uint8_t cs, clk, dout, j;
 unsigned int val=0;
    
 cs=cs_bit[adc];
 clk=clk_bit[adc];
 dout=dout_bit[adc];



 bit_set (PORTD, cs|clk); delay();
 bit_clr (PORTD, cs);
 bit_clr (PORTD, clk); delay();


 for (j=0; j<14; j++) {
     bit_set (PORTD, clk); delay();
     bit_clr (PORTD, clk); delay();
     if (j>1) {
         val=val<<1;
         if (bit_get (PIND, dout)) val|=1;
         }
     }
 
 bit_set (PORTD, cs); delay();

 return val;
}
