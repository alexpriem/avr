#include <stdlib.h>

#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "bitops.h"
#include "uart.h"
#include "delay.h"
#include "cat4016.h"

#define NUM_4016 20


// pin 14 is de data-ingang
// pin 11 is de clock ingang van het register 
// na het inklokken een puls geven op pin 12 (latch) -> dan nemen de uitgangen de data uit het register over. 

volatile uint8_t *cat4016_p_blank[NUM_4016];         //  
volatile uint8_t *cat4016_p_latch[NUM_4016];         //  
volatile uint8_t *cat4016_p_data[NUM_4016];       //  
volatile uint8_t *cat4016_p_clk[NUM_4016];       //  

uint8_t cat4016_b_blank[NUM_4016];
uint8_t cat4016_b_latch[NUM_4016];
uint8_t cat4016_b_data[NUM_4016];
uint8_t cat4016_b_clk[NUM_4016];



/* data */


unsigned char numbers[11]={
   0x3f,  // 0
   0x06,  // 1
   0x5b,  // 2
   0x4f,  // 3
   0x66,  // 4
   0x6d,  // 5
   0x7d,  // 6
   0x07,  // 7
   0x7f,  // 8
   0x6f,  // 9
   0x00     // blank
   };

   // codes voor 0-9:

unsigned char hex_numbers[16]={
   0x3f,  // 0
   0x06,  // 1
   0x5b,  // 2
   0x4f,  // 3
   0x66,  // 4
   0x6d,  // 5
   0x7d,  // 6
   0x07,  // 7
   0x7f,  // 8
   0x6f,  // 9

   0x5f,  // a
   0x7c,  // b
   0x58,  // c
   0x5e,  // d
   0x7b,  // e
   0x71,  // f
   };
   

   unsigned char letters[27]={
   0x5f,  // a
   0x7c,  // b
   0x58,  // c
   0x5e,  // d
   0x7b,  // e
   0x71,  // f
    
   0x6f,  // g
   0x74,  // h
   0x04,  // i
   0x0c,  // j
   0x76,  // k
   0x38,  // l
   0x54,  // m
   0x54,  // n
   0x5c,  // o
   0x73,  // p
   0x67,  // q
   0x50,  // r
   0x6d,  // s
   0x78,  // t
   0x1c,  // u
   0x3e,  // v
   0x7e,  // w
   0x76,  // x
   0x6e, // y
   0x5b, // z
   0x00 // blank
   };
   
   
   unsigned char special_up[27]={
   0x61,  // 
   0x23,  // 
   0x43,  // 
   0x62,  // 
   };  // 
   
   unsigned char special_down[27]={
   0x4c,  // 
   0x54,  // 
   0x58,  // 
   0x1c,  // 
   };  // 
   
   unsigned char special_updown[6]={
   0x3e,  // 
   0x3d,  // c
   0x3b,  //    
   0x37,  // d
   0x2f, 
   0x1f,
   };  // j






void init_cat4016(uint8_t chip, uint8_t clk, uint8_t latch, uint8_t data, uint8_t blank)

{ uint8_t bclk, blatch, bdata, bblank;
 volatile uint8_t *pblank;

 bclk=1<< (clk  & P_BITMASK);
 cat4016_b_clk[chip] = bclk;
 bdata=1<< (data & P_BITMASK); 
 cat4016_b_data[chip] = bdata;
 blatch=1<< (latch   & P_BITMASK); 
 cat4016_b_latch[chip] =  blatch;
 bblank=1<< (blank   & P_BITMASK); 
 cat4016_b_blank[chip] =  bblank;
 
 
 clk=clk & P_PORTMASK;
 if (clk==P_PORTA) {cat4016_p_clk[chip]=&PORTA; DDRA|=bclk;}
 if (clk==P_PORTB) {cat4016_p_clk[chip]=&PORTB; DDRB|=bclk;}
 if (clk==P_PORTC) {cat4016_p_clk[chip]=&PORTC; DDRC|=bclk;}
 if (clk==P_PORTD) {cat4016_p_clk[chip]=&PORTD; DDRD|=bclk;}

 data=data & P_PORTMASK;
 if (data==P_PORTA) {cat4016_p_data[chip]=&PORTA; DDRA|=bdata; }
 if (data==P_PORTB) {cat4016_p_data[chip]=&PORTB; DDRB|=bdata;}
 if (data==P_PORTC) {cat4016_p_data[chip]=&PORTC; DDRC|=bdata;}
 if (data==P_PORTD) {cat4016_p_data[chip]=&PORTD; DDRD|=bdata;}

 latch=latch & P_PORTMASK;
 if (latch==P_PORTA) {cat4016_p_latch[chip]=&PORTA; DDRA|=blatch;}
 if (latch==P_PORTB) {cat4016_p_latch[chip]=&PORTB; DDRB|=blatch;}
 if (latch==P_PORTC) {cat4016_p_latch[chip]=&PORTC; DDRC|=blatch;}
 if (latch==P_PORTD) {cat4016_p_latch[chip]=&PORTD; DDRD|=blatch;}

 blank=blank & P_PORTMASK;
 if (blank==P_PORTA) {cat4016_p_blank[chip]=&PORTA; DDRA|=bblank;}
 if (blank==P_PORTB) {cat4016_p_blank[chip]=&PORTB; DDRB|=bblank;}
 if (blank==P_PORTC) {cat4016_p_blank[chip]=&PORTC; DDRC|=bblank;}
 if (blank==P_PORTD) {cat4016_p_blank[chip]=&PORTD; DDRD|=bblank;} 
 
 pblank=cat4016_p_blank[chip];
 bit_clr (*pblank, bblank);
}






void cat4016_put4 (uint8_t chip, unsigned long c)

{
 volatile uint8_t *pclk, *platch, *pdata;
 unsigned long cc;
 uint8_t bclk, blatch, bdata;
 uint8_t i;
 
 

 pclk=cat4016_p_clk[chip];
 pdata=cat4016_p_data[chip];
 platch=cat4016_p_latch[chip];

 bclk=cat4016_b_clk[chip];
 bdata=cat4016_b_data[chip];
 blatch=cat4016_b_latch[chip];
 cc= c;

 bit_clr(*platch, blatch);
 bit_clr (*pdata, bdata);
 bit_clr (*pclk, bclk);
 
 for (i=0; i<32; i++) {

     if ((cc & 0x80000000)==0) {
           bit_clr (*pdata, bdata);
          //uart_putc ('0');
          }
     else {
            bit_set (*pdata, bdata);
           //uart_putc ('1');
           }
     
     bit_set (*pclk, bclk);  
     bit_clr (*pclk, bclk);     
     cc=cc<<1;
     }

 bit_set(*platch, blatch);
 bit_clr(*platch, blatch);
}




void cat4016_put_unumber (uint8_t chip, unsigned int num)
{
 uint8_t digit;
 int orignum;
 unsigned long w;
 
 w=0;
 if (num>10000)  {
	w=0x5c38715c;  /* 'oflo' */
	cat4016_put4 (chip, w);
	return;
	}
 orignum=num;
 
 if (orignum>=1000) {
	digit=0;
	while (num>=1000) { digit++; num-=1000;}
	w=numbers[digit];
	}
 if (orignum>=100) {
	digit=0;
	while (num>=100) { digit++; num-=100;}
	w|=(unsigned long)numbers[digit]<<8;
 }
 if (orignum>=10) {
	digit=0;
	while (num>=10) { digit++; num-=10;}
	w|=(unsigned long)numbers[digit]<<16;
 }
 w|=(unsigned long)numbers[num]<<24;
 
 cat4016_put4 (chip, w);

}

void cat4016_put_snumber (uint8_t chip, int num)
{
 unsigned long w;
 int orignum;
 uint8_t digit;
 
 w=0x00;
 orignum=num;
 if (num>=10000)  {
	w=0x5c38715c;  /* 'oflo' */
	cat4016_put4 (chip, w);
	return;
	}
if (num<=-1000)  {
	w=0x5c38711c;  /* 'uflo' */
	cat4016_put4 (chip, w);
	return;
	}
	
 if (num<=0) { 
	num=-num;
	w=0x40;
	}
 if (num>=1000) {
	digit=0;
	while (num>=1000) { digit++; num-=1000;}
	w=numbers[digit];
 }	

 
 if (orignum>=100) {
	digit=0;
	while (num>=100) { digit++; num-=100;}
	w|=(unsigned long)numbers[digit]<<8;
 }
 
 if (orignum>=10) {
	digit=0;
	while (num>=10) { digit++; num-=10;}
	w|=(unsigned long)numbers[digit]<<16;
 }
 w|=(unsigned long)numbers[num]<<24;

 cat4016_put4 (chip, w);

}


void cat4016_put_txt (uint8_t chip, char *s)
{
 unsigned long w;
 uint8_t c;

 c=s[0];
 if ((c & 0xf0)==0x30) {
	c-='0';
	w=(unsigned long) numbers[c];	
  } else {
	c=(c | 0x20) -'a'; 
	w=(unsigned long)letters[c];
 }
 
 c=s[1];
 if ((c & 0xf0)==0x30) {
	c-='0';
	w=(unsigned long) numbers[c]<<8;	
  } else {
	c=(c | 0x20) -'a'; 
	w|=(unsigned long)letters[c]<<8;
  }
  
  c=s[2];
  if ((c & 0xf0)==0x30) {
	c-='0';
	w=(unsigned long) numbers[c]<<16;	
  } else {
	c=(c | 0x20) -'a'; 
	w|=(unsigned long)letters[c]<<16;
  }
  
  c=s[3];
  if ((c & 0xf0)==0x30) {
	c-='0';
	w=(unsigned long) numbers[c]<<24;	
  } else {
	c=(c | 0x20) -'a'; 
	w|=(unsigned long)letters[c]<<24;
 }
 
 cat4016_put4 (chip, w);

}

