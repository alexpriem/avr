
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "bitops.h"
#include "uart.h"
#include "delay.h"
#include "74hc595.h"

#include "uc_info.h"

#define NUM_74HC595 20


// pin 14 is de data-ingang
// pin 11 is de clock ingang van het register 
// na het inklokken een puls geven op pin 12 -> dan nemen de uitgangen de data uit het register over. 

volatile uint8_t *p_cs[NUM_74HC595];         //  
volatile uint8_t *p_data[NUM_74HC595];       //  DS
volatile uint8_t *p_clk[NUM_74HC595];       //  

uint8_t b_cs[NUM_74HC595];
uint8_t b_data[NUM_74HC595];
uint8_t b_clk[NUM_74HC595];
unsigned char  c_data[NUM_74HC595];
unsigned char  copy_data[NUM_74HC595];



void init_74hc595(uint8_t chip, uint8_t clk, uint8_t cs, uint8_t data)

{ uint8_t bclk, bcs, bdata;

 bclk=1<< (clk  & P_BITMASK);
 b_clk [chip] = bclk;
 bdata=1<< (data & P_BITMASK); 
 b_data[chip] = bdata;
 bcs=1<< (cs   & P_BITMASK); 
 b_cs  [chip] =  bcs;
 
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
}


void s595_putc (uint8_t chip, unsigned char c)

{
 volatile uint8_t *pclk, *pcs, *pdata;
 uint8_t bclk, bcs, bdata;
 uint8_t i;

 pclk=p_clk[chip];
 pdata=p_data[chip];
 pcs=p_cs[chip];

 bclk=b_clk[chip];
 bdata=b_data[chip];
 bcs=b_cs[chip];

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




void s595_set_val (uint8_t chip, unsigned char val) {

 c_data[chip]=val;
}

//  klok alle data uit voor numchips chips

void s595_write (uint8_t numchips)

{
 volatile uint8_t *pclk, *pcs, *pdata;
 uint8_t bclk, bcs, bdata;
 uint8_t i, chip, c;

 pclk=p_clk[0];
 bclk=b_clk[0];
 
 pcs=p_cs[0];
 bcs=b_cs[0];

 bit_clr(*pcs, bcs);
 for (chip=0; chip<numchips; chip++) {
	pdata=p_data[chip];
	bdata=b_data[chip];
	bit_clr (*pdata, bdata);
 }
 for (chip=0; chip<numchips; chip++) {	
	copy_data[chip]=c_data[chip];
 } 
 bit_clr (*pclk, bclk);
 
 for (i=0; i<8; i++) {
	for (chip=0; chip<numchips; chip++) {
		 pdata=p_data[chip];
		 bdata=b_data[chip]; 
		 c=copy_data[chip];
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
		pdata=p_data[chip];
		bdata=b_data[chip]; 
		bit_clr (*pdata, bdata);
		
		c=copy_data[chip];		
		copy_data[chip]=c<<1;
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

