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
 
#include "uart.h"
#include "bitops.h"

#include "delay.h"

#define MAX_BOARDS 4

volatile uint8_t *server_p_atn[MAX_BOARDS];           
volatile uint8_t *server_p_data[MAX_BOARDS];           
volatile uint8_t *server_p_ack[MAX_BOARDS];           

uint8_t server_b_atn[MAX_BOARDS];
uint8_t server_b_data[MAX_BOARDS];
uint8_t server_b_ack[MAX_BOARDS];


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
 if (ack==P_PORTA) {server_p_ack[chip]=&PINA; DDRA&=~b_ack; bit_set(PORTA, b_ack);}
 if (ack==P_PORTB) {server_p_ack[chip]=&PINB; DDRB&=~b_ack; bit_set(PORTB, b_ack);}
 if (ack==P_PORTC) {server_p_ack[chip]=&PINC; DDRC&=~b_ack; bit_set(PORTC, b_ack);}
 if (ack==P_PORTD) {server_p_ack[chip]=&PIND; DDRD&=~b_ack; bit_set(PORTD, b_ack);}


}


uint8_t server_putc (uint8_t chip, unsigned char c)

{
 volatile uint8_t *patn, *pack, *pdata;
 uint8_t batn, back, bdata;
 uint8_t i,j;

 patn=server_p_atn[chip];
 pdata=server_p_data[chip];
 pack=server_p_ack[chip];

 batn=server_b_atn[chip];
 bdata=server_b_data[chip];
 back=server_b_ack[chip];

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

     while ((bit_get(*pack, back) ==0) && (j<250)) {
        udelay(10);  
         j+=1;
      }
     if (j>248) return 2;

     bit_set (*patn, batn);
     udelay (20);

     j=0;
     while ((bit_get(*pack, back)  !=0) && (j<250)) {
	     	udelay(10);	 
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
 register unsigned char c, status;

 while ( (c = *s++) ) {
        uart_printf("send:");
        uart_putc(c);
        uart_printf("\r\n");
        status=server_putc(chip, c);
       if (status!=0) return status;
    }
 return 0;
}


  
int main(void)
{
	uint8_t d;

   uart_init ();	
   uart_printf ("\r\ninit\r\n");
   uart_printf ("done\r\n");

   server_setup (0, P_PA2, P_PA6, P_PC2);

   d=server_puts(0,"test");
   uart_printf ("done, status:%d",d);

 return 0;
}
