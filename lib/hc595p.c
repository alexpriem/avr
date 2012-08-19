
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

volatile uint8_t *hc595p_p_cs[NUM_HC595];         //  ST_CP -- 12
volatile uint8_t *hc595p_p_data0[NUM_HC595];       //  DS    -- 14
volatile uint8_t *hc595p_p_data1[NUM_HC595];       //  DS    -- 14
volatile uint8_t *hc595p_p_data2[NUM_HC595];       //  DS    -- 14
volatile uint8_t *hc595p_p_data3[NUM_HC595];       //  DS    -- 14
volatile uint8_t *hc595p_p_clk[NUM_HC595];       //   SH_CP -- 11

uint8_t hc595p_b_cs[NUM_HC595];
uint8_t hc595p_b_data0[NUM_HC595];
uint8_t hc595p_b_data1[NUM_HC595];
uint8_t hc595p_b_data2[NUM_HC595];
uint8_t hc595p_b_data3[NUM_HC595];
uint8_t hc595p_b_clk[NUM_HC595];



			//							sh_cp    st_cp    ds
void hc595p_setup (uint8_t chip, uint8_t clk, uint8_t cs, uint8_t data0, uint8_t data1, uint8_t data2, uint8_t data3)

{ uint8_t bclk, bcs, bdata0, bdata1, bdata2, bdata3;

 bclk=1<< (clk  & P_BITMASK);
 hc595p_b_clk[chip] = bclk;
 bcs=1<< (cs   & P_BITMASK); 
 hc595p_b_cs[chip] =  bcs;
 bdata0=1<< (data0 & P_BITMASK); 
 hc595p_b_data0[chip] = bdata0; 
 bdata1=1<< (data1 & P_BITMASK); 
 hc595p_b_data1[chip] = bdata1;
 bdata2=1<< (data2 & P_BITMASK); 
 hc595p_b_data2[chip] = bdata2;
 bdata3=1<< (data3 & P_BITMASK); 
 hc595p_b_data3[chip] = bdata3;

 
 clk=clk & P_PORTMASK;
 if (clk==P_PORTA) {hc595p_p_clk[chip]=&PORTA; DDRA|=bclk;}
 if (clk==P_PORTB) {hc595p_p_clk[chip]=&PORTB; DDRB|=bclk;}
 if (clk==P_PORTC) {hc595p_p_clk[chip]=&PORTC; DDRC|=bclk;}
 if (clk==P_PORTD) {hc595p_p_clk[chip]=&PORTD; DDRD|=bclk;}

 cs=cs & P_PORTMASK;
 if (cs==P_PORTA) {hc595p_p_cs[chip]=&PORTA; DDRA|=bcs;}
 if (cs==P_PORTB) {hc595p_p_cs[chip]=&PORTB; DDRB|=bcs;}
 if (cs==P_PORTC) {hc595p_p_cs[chip]=&PORTC; DDRC|=bcs;}
 if (cs==P_PORTD) {hc595p_p_cs[chip]=&PORTD; DDRD|=bcs;}
 
 data0=data0 & P_PORTMASK;
 if (data0==P_PORTA) {hc595p_p_data0[chip]=&PORTA; DDRA|=bdata0;}
 if (data0==P_PORTB) {hc595p_p_data0[chip]=&PORTB; DDRB|=bdata0;}
 if (data0==P_PORTC) {hc595p_p_data0[chip]=&PORTC; DDRC|=bdata0;}
 if (data0==P_PORTD) {hc595p_p_data0[chip]=&PORTD; DDRD|=bdata0;}

 data1=data1 & P_PORTMASK;
 if (data1==P_PORTA) {hc595p_p_data1[chip]=&PORTA; DDRA|=bdata1;}
 if (data1==P_PORTB) {hc595p_p_data1[chip]=&PORTB; DDRB|=bdata1;}
 if (data1==P_PORTC) {hc595p_p_data1[chip]=&PORTC; DDRC|=bdata1;}
 if (data1==P_PORTD) {hc595p_p_data1[chip]=&PORTD; DDRD|=bdata1;}

 data2=data2 & P_PORTMASK;
 if (data2==P_PORTA) {hc595p_p_data2[chip]=&PORTA; DDRA|=bdata2;}
 if (data2==P_PORTB) {hc595p_p_data2[chip]=&PORTB; DDRB|=bdata2;}
 if (data2==P_PORTC) {hc595p_p_data2[chip]=&PORTC; DDRC|=bdata2;}
 if (data2==P_PORTD) {hc595p_p_data2[chip]=&PORTD; DDRD|=bdata2;}

 data3=data3 & P_PORTMASK;
 if (data3==P_PORTA) {hc595p_p_data3[chip]=&PORTA; DDRA|=bdata3;}
 if (data3==P_PORTB) {hc595p_p_data3[chip]=&PORTB; DDRB|=bdata3;}
 if (data3==P_PORTC) {hc595p_p_data3[chip]=&PORTC; DDRC|=bdata3;}
 if (data3==P_PORTD) {hc595p_p_data3[chip]=&PORTD; DDRD|=bdata3;}

}


void hc595p_putc (uint8_t chip, unsigned char c0, unsigned char c1, unsigned char c2, unsigned char c3)

{
 volatile uint8_t *pclk, *pcs, *pdata0, *pdata1, *pdata2, *pdata3;
 uint8_t bclk, bcs, bdata0, bdata1, bdata2, bdata3;
 uint8_t i;

 pclk=hc595p_p_clk[chip];
 pcs=hc595p_p_cs[chip];
 pdata0=hc595p_p_data0[chip];
 pdata1=hc595p_p_data1[chip];
 pdata2=hc595p_p_data2[chip];
 pdata3=hc595p_p_data3[chip];

 bclk=hc595p_b_clk[chip];
 bcs=hc595p_b_cs[chip];
 bdata0=hc595p_b_data0[chip];
 bdata1=hc595p_b_data1[chip];
 bdata2=hc595p_b_data2[chip];
 bdata3=hc595p_b_data3[chip];

 bit_clr(*pcs, bcs);
 bit_clr (*pdata0, bdata0);
 bit_clr (*pdata1, bdata1);
 bit_clr (*pdata2, bdata2);
 bit_clr (*pdata3, bdata3);
 bit_clr (*pclk, bclk);
 
 for (i=0; i<8; i++) { 

    if ((c0 & 0x80) ==0) {
           bit_clr (*pdata0, bdata0);		         
         } else {
            bit_set (*pdata0, bdata0);			          
     }
	if ((c1 & 0x80) ==0) {
           bit_clr (*pdata1, bdata1);		         
         } else {
            bit_set (*pdata1, bdata1);			          
     }
	if ((c2 & 0x80) ==0) {
           bit_clr (*pdata2, bdata2);		
         } else {
            bit_set (*pdata2, bdata2);			          
     }
	if ((c3 & 0x80) ==0) {
           bit_clr (*pdata3, bdata3);		         
         } else {
            bit_set (*pdata3, bdata3);			          
     }	 
     udelay(10);          
     
     bit_set (*pclk, bclk);
     udelay (20);
          
     bit_clr (*pdata0, bdata0);
	 bit_clr (*pdata1, bdata1);
	 bit_clr (*pdata2, bdata2);
	 bit_clr (*pdata3, bdata3);
     bit_clr (*pclk, bclk);
     
     c0=c0<<1;
	 c1=c1<<1;
	 c2=c2<<1;
	 c3=c3<<1;
     }

 udelay (20);
 bit_set(*pcs, bcs);
 udelay (20);
 bit_clr(*pcs, bcs);
 udelay (20);
 
}



