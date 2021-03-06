
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "bitops.h"
#include "uart.h"
#include "delay.h"
#include "mcp3204.h"

#include "uc_info.h"


#define NUM_mcp3204 4






volatile uint8_t *p_cs[NUM_mcp3204];
volatile uint8_t *p_dout[NUM_mcp3204];
volatile uint8_t *p_din[NUM_mcp3204];
volatile uint8_t *p_clk[NUM_mcp3204];

uint8_t b_cs[NUM_mcp3204];
uint8_t b_dout[NUM_mcp3204];
uint8_t b_din[NUM_mcp3204];
uint8_t b_clk[NUM_mcp3204];



//
// dout is *input!* 
// din is config-pin.
// cs, clk: standaard SPI. 


void init_mcp3204(uint8_t chip, uint8_t cs, uint8_t clk,  uint8_t dout, uint8_t din)

{
 uint8_t bclk, bcs, bdout, bdin;
 
 bclk =  1<< (clk & P_BITMASK);
 b_clk[chip]=bclk;
 bdout= 1<< (dout & P_BITMASK);
 b_dout[chip]=bdout;
 bcs= 1<< (cs   & P_BITMASK);
 b_cs[chip] = bcs;
 bdin= 1<< (din & P_BITMASK);
 b_din[chip] = bdin;

 clk=clk & P_PORTMASK;
 if (clk==P_PORTA) {p_clk[chip]=&PORTA; DDRA|=bclk;  }
 if (clk==P_PORTB) {p_clk[chip]=&PORTB; DDRB|=bclk;}
 if (clk==P_PORTC) {p_clk[chip]=&PORTC; DDRC|=bclk;}
 if (clk==P_PORTD) {p_clk[chip]=&PORTD; DDRD|=bclk;}

 dout=dout & P_PORTMASK;              
 if (dout==P_PORTA) {p_dout[chip]=&PINA; DDRA&=~bdout; bit_set(PORTA, bdout);}
 if (dout==P_PORTB) {p_dout[chip]=&PINB; DDRB&=~bdout; bit_set(PORTB, bdout);}
 if (dout==P_PORTC) {p_dout[chip]=&PINC; DDRC&=~bdout; bit_set(PORTC, bdout);}
 if (dout==P_PORTD) {p_dout[chip]=&PIND; DDRD&=~bdout; bit_set(PORTD, bdout);}

 din=din & P_PORTMASK;
 if (din==P_PORTA) {p_din[chip]=&PORTA; DDRA|=bdin;}
 if (din==P_PORTB) {p_din[chip]=&PORTB; DDRB|=bdin;}
 if (din==P_PORTC) {p_din[chip]=&PORTC; DDRC|=bdin;}
 if (din==P_PORTD) {p_din[chip]=&PORTD; DDRD|=bdin;}

 cs=cs & P_PORTMASK;
 if (cs==P_PORTA) {p_cs[chip]=&PORTA; DDRA|=bcs;}
 if (cs==P_PORTB) {p_cs[chip]=&PORTB; DDRB|=bcs;}
 if (cs==P_PORTC) {p_cs[chip]=&PORTC; DDRC|=bcs;}
 if (cs==P_PORTD) {p_cs[chip]=&PORTD; DDRD|=bcs;}


 bit_set(*p_cs[chip], b_cs[chip]);
 bit_clr(*p_clk[chip], b_clk[chip]);
 bit_set(*p_din[chip], b_din[chip]);  
}



unsigned int mcp3204_getw (uint8_t chip, uint8_t adc, uint8_t mode)


{
 volatile uint8_t *pclk, *pcs, *pdout, *pdin;
 uint8_t bclk, bcs, bdout, bdin;
 uint8_t i;
 unsigned int word;


 pclk=p_clk[chip];
 pdout=p_dout[chip];
 pdin=p_din[chip];
 pcs=p_cs[chip];

 bclk=b_clk[chip];
 bdout=b_dout[chip];
 bdin=b_din[chip];
 bcs=b_cs[chip];

 bit_set(*pdin, bdin);
 bit_clr(*pclk, bclk);
 bit_clr(*pcs, bcs);

// 1e bit: din moet hoog zijn om sequentie te starten.
 udelay (20);
 bit_set (*pclk, bclk);
 udelay (100);
 bit_clr (*pclk, bclk);
 udelay (100);


 if (mode) bit_set(*pdin, bdin);
 else bit_clr(*pdin, bdin);
 bit_set (*pclk, bclk);
 udelay (100);
 bit_clr (*pclk, bclk);
 udelay (100);
 
 if (adc & 4) bit_set(*pdin, bdin);
 else bit_clr(*pdin, bdin);
 bit_set (*pclk, bclk);
 udelay (100);
 bit_clr (*pclk, bclk);
 udelay (100);

 if (adc & 2) bit_set(*pdin, bdin);
 else bit_clr(*pdin, bdin);
 bit_set (*pclk, bclk);
 udelay (100);
 bit_clr (*pclk, bclk);
 udelay (100);

 if (adc & 1) bit_set(*pdin, bdin);
 else bit_clr(*pdin, bdin);
 bit_set (*pclk, bclk);
 udelay (100);
 bit_clr (*pclk, bclk);
 udelay (100);
 
 bit_set (*pclk, bclk);
 udelay (100);
 bit_clr (*pclk, bclk);
 udelay (100);


 word=0;
 for (i=0; i<13; i++) {

     bit_set(*pclk, bclk);
     udelay(100);
     bit_clr(*pclk, bclk);
     udelay(100);          
     word=word<<1;
     if (bit_get (*pdout, bdout)) word++;
     }

 bit_set(*pcs, bcs);
 udelay (100);
 
 
 return ((word>>1) & 0x0fff);
}



/* spiw ongetest */

/*

void mcp4922_spiw (uint8_t chip, uint8_t adc, uint8_t mode)


{
 volatile uint8_t *pclk, *pcs, *pdout, *pdin;
 uint8_t bclk, bcs, bdout, bdin, bldac;

 if ((word & CNTRL)==0) {
     word|=DEFAULT;
     if (dac==0) word|=DAC_A;
     }

 pclk=p_clk[chip];
 pdout=p_dout[chip];
 pcs=p_cs[chip];
 pdin=p_din[chip];

 bclk=b_clk[chip];
 bdout=b_dout[chip];
 bcs=b_cs[chip];
 bdin=b_din[chip];

 bit_clr(*pcs, bcs);
 
 SPDR= 0x04+mode<<1+adc>>2;
 while (!(SPSR & (1<<SPIF)));
 SPDR= adc<<6;
 while (!(SPSR & (1<<SPIF)));
 word=SPDR<<8;
 SPDR=0;
 while (!(SPSR & (1<<SPIF)));
 word+=SPDR;
        
 bit_set(*pcs, bcs);

 return word & 0x0fff;;
}
*/

/* blauw/zw: 46v
paars/zw:6.34 */



