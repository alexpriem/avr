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
volatile uint8_t *server_p_data[MAX_BOARDS];           
volatile uint8_t *server_p_ack[MAX_BOARDS];           

uint8_t server_b_atn[MAX_BOARDS];
uint8_t server_b_data[MAX_BOARDS];
uint8_t server_b_ack[MAX_BOARDS];


void client_setup (uint8_t chip, uint8_t atn, uint8_t data, uint8_t ack) 

{
 uint8_t b_atn, b_data, b_ack;
  
 
 b_atn=1<< (atn  & P_BITMASK);
 server_b_atn[chip]=b_atn;
 b_data=1<< (data  & P_BITMASK);
 server_b_data[chip]=b_data;
 b_ack=1<< (ack  & P_BITMASK);
 server_b_ack[chip]=b_ack;
 
 
 
 atn=atn & P_PORTMASK;
 if (atn==P_PORTA) {server_p_atn[chip]=&PINA; DDRA&=~b_atn; bit_set(PORTA, b_atn);}
 if (atn==P_PORTB) {server_p_atn[chip]=&PINB; DDRB&=~b_atn; bit_set(PORTB, b_atn);}
 if (atn==P_PORTC) {server_p_atn[chip]=&PINC; DDRC&=~b_atn; bit_set(PORTC, b_atn);}
 if (atn==P_PORTD) {server_p_atn[chip]=&PIND; DDRD&=~b_atn; bit_set(PORTD, b_atn);}


 data=data & P_PORTMASK;
 if (data==P_PORTA) {server_p_data[chip]=&PINA; DDRA&=~b_data; bit_set(PORTA, b_data);}
 if (data==P_PORTB) {server_p_data[chip]=&PINB; DDRB&=~b_data; bit_set(PORTB, b_data);}
 if (data==P_PORTC) {server_p_data[chip]=&PINC; DDRC&=~b_data; bit_set(PORTC, b_data);}
 if (data==P_PORTD) {server_p_data[chip]=&PIND; DDRD&=~b_data; bit_set(PORTD, b_data);}

 ack=ack & P_PORTMASK;
 if (ack==P_PORTA) {server_p_ack[chip]=&PORTA; DDRA|=b_ack;}
 if (ack==P_PORTB) {server_p_ack[chip]=&PORTB; DDRB|=b_ack;}
 if (ack==P_PORTC) {server_p_ack[chip]=&PORTC; DDRC|=b_ack;}
 if (ack==P_PORTD) {server_p_ack[chip]=&PORTD; DDRD|=b_ack;}
}



// codes voor 0-9:




 uint8_t client_loop (uint8_t chip) {

  uint8_t j, data,c,k;
  uint8_t batn, bdata, back;
  volatile uint8_t *patn, *pack, *pdata;



  while (1==1) {
      j=0;
      while (j<7) {
     //     k=0;
          
        	while ((bit_get(*patn, batn)==0) && (k<250)) {
        		udelay(1);     
       //     k++;
        	}
         // if (k>248) return 1;


    	    data=bit_get(*pdata, bdata);
    	    c=c|data;
    	    c=c<<1;     
          uart_printf("c:%x\r\n",c);
    	    j++;
    	    bit_set (*pack, back);

          k=0;
    	    while ((bit_get(*patn, batn)==1) && (k<126)) {
    		       udelay(10);
               k++;
          }
          if (k>120) return 2;
    	    bit_clr (*pack, back);          
      } 
     uart_printf("got:%x [%c]\r\n",c,c);
  }

}



   
int main(void)
{
	uint8_t d;

   uart_init ();	
   uart_printf ("init\r\n");
   uart_printf ("done\r\n");

   client_setup (0, P_PA2, P_PA6, P_PC2);

	d=client_loop(0);   
  uart_printf ("status:%d",d);

 return 0;
}
