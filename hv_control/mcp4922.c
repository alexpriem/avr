
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "uc_info.h"
#include "uart.h"
#include "delay.h"
#include "mcp4922.h"


#include "bitops.h"

#define NUM_mcp4922 4


volatile uint8_t *p_cs[NUM_mcp4922];
volatile uint8_t *p_data[NUM_mcp4922];
volatile uint8_t *p_clk[NUM_mcp4922];
volatile uint8_t *p_ldac[NUM_mcp4922];

uint8_t b_cs[NUM_mcp4922];
uint8_t b_data[NUM_mcp4922];
uint8_t b_clk[NUM_mcp4922];
uint8_t b_ldac[NUM_mcp4922];







void init_mcp4922(uint8_t chip, uint8_t cs, uint8_t clk,  uint8_t data, uint8_t ldac )

{
 uint8_t bclk, bcs, bdata, bldac;

 bclk=1<< (clk  & P_BITMASK);
 b_clk [chip] = bclk;
 bdata=1<< (data & P_BITMASK); 
 b_data[chip] = bdata;
 bcs=1<< (cs   & P_BITMASK); 
 b_cs  [chip] =  bcs;
 bldac=1<< (ldac & P_BITMASK); 
 b_ldac[chip] = bldac;
 

 clk=clk & P_PORTMASK;
 if (clk==P_PORTA) {p_clk[chip]=&PORTA; DDRA|=bclk;}
 if (clk==P_PORTB) {p_clk[chip]=&PORTB; DDRB|=bclk;}
 if (clk==P_PORTC) {p_clk[chip]=&PORTC; DDRC|=bclk;}
 if (clk==P_PORTD) {p_clk[chip]=&PORTD; DDRD|=bclk;}

 data=data & P_PORTMASK;
 if (data==P_PORTA) {p_data[chip]=&PORTA; DDRA|=bdata; }
 if (data==P_PORTB) {p_data[chip]=&PORTB; DDRB|=bdata;}
 if (data==P_PORTC) {p_data[chip]=&PORTC; DDRC|=bdata;}
 if (data==P_PORTD) {p_data[chip]=&PORTD; DDRD|=bdata;}

 cs=cs & P_PORTMASK;
 if (cs==P_PORTA) {p_cs[chip]=&PORTA; DDRA|=bcs;}
 if (cs==P_PORTB) {p_cs[chip]=&PORTB; DDRB|=bcs;}
 if (cs==P_PORTC) {p_cs[chip]=&PORTC; DDRC|=bcs;}
 if (cs==P_PORTD) {p_cs[chip]=&PORTD; DDRD|=bcs;}

 ldac=ldac & P_PORTMASK;
 if (ldac==P_PORTA) {p_ldac[chip]=&PORTA; DDRA|=bldac;}
 if (ldac==P_PORTB) {p_ldac[chip]=&PORTB; DDRB|=bldac;}
 if (ldac==P_PORTC) {p_ldac[chip]=&PORTC; DDRC|=bldac;}
 if (ldac==P_PORTD) {p_ldac[chip]=&PORTD; DDRD|=bldac;}
               
 bit_set(*p_cs[chip], b_cs[chip]);
 bit_clr(*p_clk[chip], b_clk[chip]);
 bit_set(*p_ldac[chip], b_ldac[chip]);
}

void init_mcp4922_spi(uint8_t chip, uint8_t cs,  uint8_t ldac )

{
 uint8_t bcs, bldac;

 bcs= 1<< (cs & P_BITMASK); 
 bldac=1<< (ldac & P_BITMASK); 
 b_cs[chip]  = bcs;
 b_ldac[chip]= bldac;
 
 cs=cs & P_PORTMASK;

 if (cs==P_PORTA) {p_cs[chip]=&PORTA; DDRA|=bcs;}
 if (cs==P_PORTB) {p_cs[chip]=&PORTB; DDRB|=bcs;}
 if (cs==P_PORTC) {p_cs[chip]=&PORTC; DDRC|=bcs;}
 if (cs==P_PORTD) {p_cs[chip]=&PORTD; DDRD|=bcs;}
//p_cs[chip]=&PORTA;

 ldac=ldac& P_PORTMASK;
 if (ldac==P_PORTA) {p_ldac[chip]=&PORTA; DDRA|=bldac;}
 if (ldac==P_PORTB) {p_ldac[chip]=&PORTB; DDRB|=bldac;}
 if (ldac==P_PORTC) {p_ldac[chip]=&PORTC; DDRC|=bldac;}
 if (ldac==P_PORTD) {p_ldac[chip]=&PORTD; DDRD|=bldac;}
 
 SPCR = (1<<SPE)|(1<<MSTR); //|(1<<SPI2X);
 SPSR = (1<<SPI2X);

 bit_set(*p_cs[chip], b_cs[chip]);
 bit_set(*p_ldac[chip], b_ldac[chip]);
}


void mcp4922_putw (uint8_t chip, int16_t word, uint8_t dac)


{
 volatile uint8_t *pclk, *pcs, *pdata, *pldac;
 uint8_t bclk, bcs, bdata, bldac;
 uint8_t i;


 if ((word & CNTRL)==0) {
     word|=DEFAULT;
     if (dac==0) word|=DAC_A;
     }

 pclk=p_clk[chip];
 pdata=p_data[chip];
 pcs=p_cs[chip];
 pldac=p_ldac[chip];

 bclk=b_clk[chip];
 bdata=b_data[chip];
 bcs=b_cs[chip];
 bldac=b_ldac[chip];

 bit_clr(*pcs, bcs);
 bit_clr (*pdata, bdata);

  
 for (i=0; i<16; i++) {

     bit_clr(*pclk, bclk);
     if ((word & 0x8000)) {
          bit_set (*pdata, bdata);
          }
     else {
           bit_clr (*pdata, bdata);
           }     
     bit_set(*pclk, bclk);                    
     word=word<<1;     
     }
 bit_clr(*pclk, bclk);
 bit_set(*pcs, bcs);
 bit_clr(*pldac, bldac); 
 udelay(2);
 bit_set(*pldac, bldac);
}




void mcp4922_spiw (uint8_t chip, int16_t word, uint8_t dac)


{
 volatile uint8_t *pcs, *pldac;
 uint8_t bcs, bldac;

 if ((word & CNTRL)==0) {
     word|=DEFAULT;
     if (dac==0) word|=DAC_A;
     }

 pcs=p_cs[chip];
 pldac=p_ldac[chip];

 bcs=b_cs[chip];
 bldac=b_ldac[chip];

 uart_printf ("pcs1:%x\n",pcs);
// pcs=&PORTA;
 bit_clr(*pcs, bcs);
// uart_printf ("pcs2:%d\n",pcs);
 
 
 
 SPDR= word>>8;
 while (!(SPSR & (1<<SPIF)));
 SPDR= word & 0xff;
 while (!(SPSR & (1<<SPIF)));



        
// PORTA|=bcs;
 bit_set(*pcs, bcs);
 bit_clr(*pldac, bldac);
 udelay(2);
 bit_set(*pldac, bldac);


}




