
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "bitops.h"
#include "delay.h"
#include "mcp3201.h"

#define NUM_MCP3201 4

volatile uint8_t *mcp3201_p_cs[NUM_MCP3201];
volatile uint8_t *mcp3201_p_data[NUM_MCP3201];
volatile uint8_t *mcp3201_p_clk[NUM_MCP3201];

uint8_t mcp3201_b_cs[NUM_MCP3201];
uint8_t mcp3201_b_data[NUM_MCP3201];
uint8_t mcp3201_b_clk[NUM_MCP3201];


void init_mcp3201(uint8_t chip, uint8_t cs, uint8_t clk,  uint8_t data)

{
 uint8_t bclk, bcs, bdata;
 
 bclk =  1<< (clk & P_BITMASK);
 mcp3201_b_clk[chip]=bclk;
 bdata= 1<< (data & P_BITMASK);
 mcp3201_b_data[chip]=bdata;
 bcs= 1<< (cs   & P_BITMASK);
 mcp3201_b_cs[chip] = bcs;

 clk=clk & P_PORTMASK;
 if (clk==P_PORTA) {mcp3201_p_clk[chip]=&PORTA; DDRA|=bclk;  }
 if (clk==P_PORTB) {mcp3201_p_clk[chip]=&PORTB; DDRB|=bclk;}
 if (clk==P_PORTC) {mcp3201_p_clk[chip]=&PORTC; DDRC|=bclk;}
 if (clk==P_PORTD) {mcp3201_p_clk[chip]=&PORTD; DDRD|=bclk;}

 data=data & P_PORTMASK;              
 if (data==P_PORTA) {mcp3201_p_data[chip]=&PINA; DDRA&=~bdata; bit_set(PORTA, bdata);}
 if (data==P_PORTB) {mcp3201_p_data[chip]=&PINB; DDRB&=~bdata; bit_set(PORTB, bdata);}
 if (data==P_PORTC) {mcp3201_p_data[chip]=&PINC; DDRC&=~bdata; bit_set(PORTC, bdata);}
 if (data==P_PORTD) {mcp3201_p_data[chip]=&PIND; DDRD&=~bdata; bit_set(PORTD, bdata);}

 cs=cs & P_PORTMASK;
 if (cs==P_PORTA) {mcp3201_p_cs[chip]=&PORTA; DDRA|=bcs;}
 if (cs==P_PORTB) {mcp3201_p_cs[chip]=&PORTB; DDRB|=bcs;}
 if (cs==P_PORTC) {mcp3201_p_cs[chip]=&PORTC; DDRC|=bcs;}
 if (cs==P_PORTD) {mcp3201_p_cs[chip]=&PORTD; DDRD|=bcs;}

 bit_set(*mcp3201_p_cs[chip], mcp3201_b_cs[chip]);
 bit_clr(*mcp3201_p_clk[chip], mcp3201_b_clk[chip]);
 bit_set(*mcp3201_p_data[chip], mcp3201_b_data[chip]);  
}


unsigned int mcp3201_getw (uint8_t chip)


{
 volatile uint8_t *pclk, *pcs, *pdata;
 uint8_t bclk, bcs, bdata;
 uint8_t i;
 unsigned int word;

 pclk=mcp3201_p_clk[chip];
 pdata=mcp3201_p_data[chip];
 pcs=mcp3201_p_cs[chip];

 bclk=mcp3201_b_clk[chip];
 bdata=mcp3201_b_data[chip];
 bcs=mcp3201_b_cs[chip];

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
     if (bit_get (*pdata, bdata)) word++;    
     }

 udelay (100);
 bit_set(*pcs, bcs);

 return ((word>>1) & 0x0fff);
}





