
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "bitops.h"
#include "uart.h"
#include "delay.h"
#include "hc595.h"

#define NUM_HC595 20


// pin 14 is de data-ingang   (DS)
// pin 11 is de clock ingang van het register  (SH_CP)
// na het inklokken een puls geven op pin 12 (ST_CP) -> dan nemen de uitgangen de data uit het register over. 

volatile uint8_t *hc595_p_cs[NUM_HC595];         //  ST_CP -- 12
volatile uint8_t *hc595_p_data[NUM_HC595];       //  DS    -- 14
volatile uint8_t *hc595_p_clk[NUM_HC595];       //   SH_CP -- 11

uint8_t hc595_b_cs[NUM_HC595];
uint8_t hc595_b_data[NUM_HC595];
uint8_t hc595_b_clk[NUM_HC595];
unsigned char  hc595_c_data[NUM_HC595];
unsigned char  hc595_copy_data[NUM_HC595];



void hc595_setup (uint8_t chip, uint8_t clk, uint8_t cs, uint8_t data)

{ uint8_t bclk, bcs, bdata;

 bclk=1<< (clk  & P_BITMASK);
 hc595_b_clk[chip] = bclk;
 bdata=1<< (data & P_BITMASK); 
 hc595_b_data[chip] = bdata;
 bcs=1<< (cs   & P_BITMASK); 
 hc595_b_cs[chip] =  bcs;
 
 clk=clk & P_PORTMASK;
 if (clk==P_PORTA) {hc595_p_clk[chip]=&PORTA; DDRA|=bclk;}
 if (clk==P_PORTB) {hc595_p_clk[chip]=&PORTB; DDRB|=bclk;}
 if (clk==P_PORTC) {hc595_p_clk[chip]=&PORTC; DDRC|=bclk;}
 if (clk==P_PORTD) {hc595_p_clk[chip]=&PORTD; DDRD|=bclk;}

 data=data & P_PORTMASK;
 if (data==P_PORTA) {hc595_p_data[chip]=&PORTA; DDRA|=bdata; }
 if (data==P_PORTB) {hc595_p_data[chip]=&PORTB; DDRB|=bdata;}
 if (data==P_PORTC) {hc595_p_data[chip]=&PORTC; DDRC|=bdata;}
 if (data==P_PORTD) {hc595_p_data[chip]=&PORTD; DDRD|=bdata;}

 cs=cs & P_PORTMASK;
 if (cs==P_PORTA) {hc595_p_cs[chip]=&PORTA; DDRA|=bcs;}
 if (cs==P_PORTB) {hc595_p_cs[chip]=&PORTB; DDRB|=bcs;}
 if (cs==P_PORTC) {hc595_p_cs[chip]=&PORTC; DDRC|=bcs;}
 if (cs==P_PORTD) {hc595_p_cs[chip]=&PORTD; DDRD|=bcs;}
}


void hc595_putc (uint8_t chip, unsigned char c)

{
 volatile uint8_t *pclk, *pcs, *pdata;
 uint8_t bclk, bcs, bdata;
 uint8_t i;

 pclk=hc595_p_clk[chip];
 pdata=hc595_p_data[chip];
 pcs=hc595_p_cs[chip];

 bclk=hc595_b_clk[chip];
 bdata=hc595_b_data[chip];
 bcs=hc595_b_cs[chip];

 bit_clr(*pcs, bcs);
 bit_clr (*pdata, bdata);
 bit_clr (*pclk, bclk);
 
 for (i=0; i<8; i++) {

     if ((c & 0x80) ==0) {
           bit_clr (*pdata, bdata);
          //uart_putc ('0');
          }
     else {
            bit_set (*pdata, bdata);
           //uart_putc ('1');
           }
     udelay(10);          
     
     bit_set (*pclk, bclk);
     udelay (20);
          
     bit_clr (*pdata, bdata);
     bit_clr (*pclk, bclk);

     
     c=c<<1;
     }

 udelay (20);
 bit_set(*pcs, bcs);
 udelay (20);
 bit_clr(*pcs, bcs);
 udelay (20);
 
}




void hc595_set_val (uint8_t chip, unsigned char val) {

 hc595_c_data[chip]=val;
}

//  klok alle data uit voor numchips chips

void hc595_write (uint8_t numchips)

{
 volatile uint8_t *pclk, *pcs, *pdata;
 uint8_t bclk, bcs, bdata;
 uint8_t i, chip, c;

 pclk=hc595_p_clk[0];
 bclk=hc595_b_clk[0];
 
 pcs=hc595_p_cs[0];
 bcs=hc595_b_cs[0];

 bit_clr(*pcs, bcs);
 for (chip=0; chip<numchips; chip++) {
	pdata=hc595_p_data[chip];
	bdata=hc595_b_data[chip];
	bit_clr (*pdata, bdata);
 }
 for (chip=0; chip<numchips; chip++) {	
	hc595_copy_data[chip]=hc595_c_data[chip];
 } 
 bit_clr (*pclk, bclk);
 
 for (i=0; i<8; i++) {
	for (chip=0; chip<numchips; chip++) {
		 pdata=hc595_p_data[chip];
		 bdata=hc595_b_data[chip]; 
		 c=hc595_copy_data[chip];
		 if ((c & 0x80) ==0) {		
			 bit_clr (*pdata, bdata);
			 //uart_putc ('0');
            }
		 else {
			 bit_set (*pdata, bdata);
             //uart_putc ('1');
            }
		}
	udelay(10);          
		    
    bit_set (*pclk, bclk);
    udelay (20);
    for (chip=0; chip<numchips; chip++) {     
		pdata=hc595_p_data[chip];
		bdata=hc595_b_data[chip]; 
		bit_clr (*pdata, bdata);
		
		c=hc595_copy_data[chip];		
		hc595_copy_data[chip]=c<<1;
	 }
     bit_clr (*pclk, bclk);
	 //uart_printf ("\r\n");
    }

 udelay (20);
 bit_set(*pcs, bcs);
 udelay (20);
 bit_clr(*pcs, bcs);
 udelay (20);
}

