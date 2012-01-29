
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "bitops.h"
#include "uart.h"
#include "delay.h"
#include "mcp3201.h"

#include "uc_info.h"


#define NUM_mcp3201 4


volatile uint8_t *p_cs[NUM_mcp3202];
volatile uint8_t *p_data[NUM_mcp3202];
volatile uint8_t *p_clk[NUM_mcp3202];

uint8_t b_cs[NUM_mcp3202];
uint8_t b_dout[NUM_mcp3202];
uint8_t b_clk[NUM_mcp3202];




void init_mcp3201(uint8_t chip, uint8_t cs, uint8_t clk,  uint8_t data)

{
 uint8_t bclk, bcs, bdout, bdin;
 
 bclk =  1<< (clk & P_BITMASK);
 b_clk[chip]=bclk;
 bdata= 1<< (data & P_BITMASK);
 b_data[chip]=bdout;
 bcs= 1<< (cs   & P_BITMASK);
 b_cs[chip] = bcs;

 clk=clk & P_PORTMASK;
 if (clk==P_PORTA) {p_clk[chip]=&PORTA; DDRA|=bclk;  }
 if (clk==P_PORTB) {p_clk[chip]=&PORTB; DDRB|=bclk;}
 if (clk==P_PORTC) {p_clk[chip]=&PORTC; DDRC|=bclk;}
 if (clk==P_PORTD) {p_clk[chip]=&PORTD; DDRD|=bclk;}

 data=data & P_PORTMASK;              
 if (data==P_PORTA) {p_data[chip]=&PINA; DDRA&=~bdout; bit_set(PORTA, bdata);}
 if (data==P_PORTB) {p_data[chip]=&PINB; DDRB&=~bdout; bit_set(PORTB, bdata);}
 if (data==P_PORTC) {p_data[chip]=&PINC; DDRC&=~bdout; bit_set(PORTC, bdata);}
 if (data==P_PORTD) {p_data[chip]=&PIND; DDRD&=~bdout; bit_set(PORTD, bdata);}

 cs=cs & P_PORTMASK;
 if (cs==P_PORTA) {p_cs[chip]=&PORTA; DDRA|=bcs;}
 if (cs==P_PORTB) {p_cs[chip]=&PORTB; DDRB|=bcs;}
 if (cs==P_PORTC) {p_cs[chip]=&PORTC; DDRC|=bcs;}
 if (cs==P_PORTD) {p_cs[chip]=&PORTD; DDRD|=bcs;}

 bit_set(*p_cs[chip], b_cs[chip]);
 bit_clr(*p_clk[chip], b_clk[chip]);
 bit_set(*p_din[chip], b_din[chip]);  
}


unsigned int mcp3201_getw (uint8_t chip)


{
 volatile uint8_t *pclk, *pcs, *pdata;
 uint8_t bclk, bcs, bdata;
 uint8_t v,i;
 unsigned int word;

 pclk=p_clk[chip];
 pdata=p_data[chip];
 pcs=p_cs[chip];

 bclk=b_clk[chip];
 bdata=b_data[chip];
 bcs=b_cs[chip];

 bit_clr(*pclk, bclk);
 bit_clr(*pcs, bcs);

 udelay (20);
 word=0;
 for (i=0; i<16; i++) {

     bit_set(*pclk, bclk);
     udelay(100);
     bit_clr(*pclk, bclk);
     udelay(100);          
     word=word<<1;
     if (bit_get (*pdout, bdout)) word++;    
     }

 udelay (100);
 bit_set(*pcs, bcs);

 return ((word>>1) & 0x0fff);
}





