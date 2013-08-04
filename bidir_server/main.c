/******************************************************************************
 Title:    adc_dac_uc2
 Author:   Alex Priem
 Date:     October 2008
 Software: AVR-GCC 3.3 
 Hardware: ATMega32
           
 Description:
 
  
*******************************************************************************/
#include <inttypes.h>
#include <avr/io.h>
 
#include "uart324_0.h"
#include "uart324_1.h"
#include "bitops.h"

#include "delay.h"

#define MAX_BOARDS 4

volatile uint8_t *server_p_atn[MAX_BOARDS];           
volatile uint8_t *server_p_atn[MAX_BOARDS];           
volatile uint8_t *server_p_atn[MAX_BOARDS];           

uint8_t server_b_atn[MAX_BOARDS];
uint8_t server_b_atn[MAX_BOARDS];
uint8_t server_b_atn[MAX_BOARDS];


void server_setup (uint8_t chip, uint8_t atn, uint8_t data, uint8_t ack) 

{
 uint8_t b_atn, b_data, b_ack;
  
 
 b_atn=1<< (atn  & P_BITMASK);
 server_b_atn[chip]=b_atn;
 b_data=1<< (data  & P_BITMASK);
 server_b_data[chip]=b_data;
 b_ack=1<< (ack  & P_BITMASK);
 server_b_ack[chip]=b_ack;
 
 
 
 atn=atn & P_PORTMASK;
 if (atn==P_PORTA) {server_p_atn[chip]=&PORTA; DDRA|=b_atn;}
 if (atn==P_PORTB) {server_p_atn[chip]=&PORTB; DDRB|=b_atn;}
 if (atn==P_PORTC) {server_p_atn[chip]=&PORTC; DDRC|=b_atn;}
 if (atn==P_PORTD) {server_p_atn[chip]=&PORTD; DDRD|=b_atn;}

 data=data & P_PORTMASK;
 if (data==P_PORTA) {server_p_data[chip]=&PORTA; DDRA|=b_data;}
 if (data==P_PORTB) {server_p_data[chip]=&PORTB; DDRB|=b_data;}
 if (data==P_PORTC) {server_p_data[chip]=&PORTC; DDRC|=b_data;}
 if (data==P_PORTD) {server_p_data[chip]=&PORTD; DDRD|=b_data;}

 ack=ack & P_PORTMASK;
 if (ack==P_PORTA) {server_p_ack[chip]=&PORTA; DDRA|=b_ack;}
 if (ack==P_PORTB) {server_p_ack[chip]=&PORTB; DDRB|=b_ack;}
 if (ack==P_PORTC) {server_p_ack[chip]=&PORTC; DDRC|=b_ack;}
 if (ack==P_PORTD) {server_p_ack[chip]=&PORTD; DDRD|=b_ack;}


}


void server_putc (uint8_t chip, char c)


uint8_t server_putc (uint8_t chip, unsigned char c)

{
 volatile uint8_t *patn, *pack, *pdata;
 uint8_t batn, back, bdata;
 uint8_t i,j;

 patn=server_p_clk[chip];
 pdata=server_p_data[chip];
 pack=server_p_cs[chip];

 batn=server_b_clk[chip];
 bdata=server_b_data[chip];
 back=server_b_cs[chip];

 bit_clr (*pdata, bdata);
 bit_clr (*patn, batn);
 
 for (i=0; i<8; i++) {
     if ((c & 0x80) ==0) {
           bit_clr (*pdata, bdata);
          //uart_putc ('0');
          }
     else {
            bit_set (*pdata, bdata);
           //uart_putc ('1');
           }
     udelay(1);          
     
     bit_set (*patn, batn);
     udelay (20);

     while (bit_get(*pack, back)  !=0) && (j<250)) {
	 	udelay(1);	 
	 	j+=1;
		}

	 if (j>248) return 1;

     bit_clr (*pdata, bdata);
     bit_clr (*patn, batn);     
     
     c=c<<1;
     }
  return 0;
}





uint8_t server_puts (uint8_t chip, char *s)

{
 register unsigned char c;

 while ( (c = *s++) ) {
       if (server_putc(chip, c)) <0) return 1;
    }
 return 0;
}


  
int main(void)
{
	
   uart_init ();	
   uart_printf ("init\r\n");
   uart_printf ("done\r\n");

   server_setup (0, P_PA2, P_PA6, P_PC2);

   server_puts("test");

 return 0;
}
