#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "bitops.h"

#define NUM_2311 10

volatile uint8_t *pga2311_p_zcen[NUM_2311]; 
volatile uint8_t *pga2311_p_cs[NUM_2311];       
volatile uint8_t *pga2311_p_sdi[NUM_2311];  
volatile uint8_t *pga2311_p_clk[NUM_2311];  
volatile uint8_t *pga2311_p_mute[NUM_2311]; 


uint8_t pga2311_b_zcen[NUM_2311];
uint8_t pga2311_b_cs[NUM_2311];
uint8_t pga2311_b_sdi[NUM_2311];
uint8_t pga2311_b_clk[NUM_2311];
uint8_t pga2311_b_mute[NUM_2311];





void pga2311_setup(uint8_t chip, uint8_t zcen, uint8_t cs, uint8_t sdi, uint8_t clk, uint8_t mute)

{ uint8_t bzcen, bsdi, bcs, bclk, bmute;
 volatile uint8_t *pzcen, *pmute;

 bzcen=1<< (zcen  & P_BITMASK);
 pga2311_b_zcen[chip] = bzcen;
 bcs=1<< (cs & P_BITMASK); 
 pga2311_b_cs[chip] = bcs;
 bsdi=1<< (sdi   & P_BITMASK); 
 pga2311_b_sdi[chip] =  bsdi;
 bclk=1<< (clk   & P_BITMASK); 
 pga2311_b_clk[chip] =  bclk;
 bmute=1<< (mute   & P_BITMASK); 
 pga2311_b_mute[chip] =  bmute;
 
 zcen=zcen & P_PORTMASK;
 if (zcen==P_PORTA) {pga2311_p_zcen[chip]=&PORTA; DDRA|=bzcen;}
 if (zcen==P_PORTB) {pga2311_p_zcen[chip]=&PORTB; DDRB|=bzcen;}
 if (zcen==P_PORTC) {pga2311_p_zcen[chip]=&PORTC; DDRC|=bzcen;}
 if (zcen==P_PORTD) {pga2311_p_zcen[chip]=&PORTD; DDRD|=bzcen;}
 pzcen=pga2311_p_zcen[chip];
 
 cs=cs & P_PORTMASK;
 if (cs==P_PORTA) {pga2311_p_cs[chip]=&PORTA; DDRA|=bcs; }
 if (cs==P_PORTB) {pga2311_p_cs[chip]=&PORTB; DDRB|=bcs;}
 if (cs==P_PORTC) {pga2311_p_cs[chip]=&PORTC; DDRC|=bcs;}
 if (cs==P_PORTD) {pga2311_p_cs[chip]=&PORTD; DDRD|=bcs;}

 sdi=sdi & P_PORTMASK;
 if (sdi==P_PORTA) {pga2311_p_sdi[chip]=&PORTA; DDRA|=bsdi;}
 if (sdi==P_PORTB) {pga2311_p_sdi[chip]=&PORTB; DDRB|=bsdi;}
 if (sdi==P_PORTC) {pga2311_p_sdi[chip]=&PORTC; DDRC|=bsdi;}
 if (sdi==P_PORTD) {pga2311_p_sdi[chip]=&PORTD; DDRD|=bsdi;}

 clk=clk & P_PORTMASK;
 if (clk==P_PORTA) {pga2311_p_clk[chip]=&PORTA; DDRA|=bclk;}
 if (clk==P_PORTB) {pga2311_p_clk[chip]=&PORTB; DDRB|=bclk;}
 if (clk==P_PORTC) {pga2311_p_clk[chip]=&PORTC; DDRC|=bclk;}
 if (clk==P_PORTD) {pga2311_p_clk[chip]=&PORTD; DDRD|=bclk;} 

 mute=mute & P_PORTMASK;
 if (mute==P_PORTA) {pga2311_p_mute[chip]=&PORTA; DDRA|=bmute;}
 if (mute==P_PORTB) {pga2311_p_mute[chip]=&PORTB; DDRB|=bmute;}
 if (mute==P_PORTC) {pga2311_p_mute[chip]=&PORTC; DDRC|=bmute;}
 if (mute==P_PORTD) {pga2311_p_mute[chip]=&PORTD; DDRD|=bmute;} 
 pmute=pga2311_p_mute[chip];
 
}

void pga2311_init (uint8_t chip) 

{ uint8_t bzcen, bmute;
  volatile uint8_t *pzcen, *pmute;
  
  bzcen=pga2311_b_zcen[chip];
  bmute=pga2311_b_mute[chip];
  
  pzcen=pga2311_p_zcen[chip];
  pmute=pga2311_p_mute[chip];
  
  bit_set(*pzcen, bzcen);
  bit_set(*pmute, bmute);
}

void pga2311_mute (uint8_t chip, int8_t mute) 

{
  uint8_t bmute;
  volatile uint8_t *pmute;
    
  bmute=pga2311_b_mute[chip]; 
  pmute=pga2311_p_mute[chip];
  if (mute)  bit_set (*pmute, bmute);
  else  bit_clr (*pmute, bmute);
}



void pga2311_set_gain (uint8_t chip, uint8_t vol_l, uint8_t vol_r) 
{
 uint8_t i;
 uint8_t bsdi, bcs, bclk;
 volatile uint8_t *psdi, *pcs, *pclk;
 
 pclk=pga2311_p_clk[chip];
 psdi=pga2311_p_sdi[chip];
 pcs=pga2311_p_cs[chip];

 bclk=pga2311_b_clk[chip];
 bsdi=pga2311_b_sdi[chip];
 bcs=pga2311_b_cs[chip];

 bit_clr (*psdi, bsdi);
 bit_clr (*pclk, bclk);
 bit_clr (*pcs, bcs);
 
 for (i=7; i>=0; i--) {
	if (vol_r & 0x80) 
		bit_set (*psdi, bsdi);
	else 
		bit_clr (*psdi, bsdi);
		
	bit_set (*pclk, bclk);
	vol_r=vol_r<<1;		// max 6.25 MHz met PGA
	//_delayFourCycles (1);    
	bit_clr (*pclk, bclk); 
	}


 for (i=7; i>=0; i++) {
	if (vol_l & 0x80) 
		bit_set (*psdi, bsdi);
	else 
		bit_clr (*psdi, bsdi);
		
	bit_set (*pclk, bclk);
	vol_l=vol_l<<1;		// max 6.25 MHz met PGA	
	bit_clr (*pclk, bclk); 
	}

 bit_set(*pcs, bcs);
}