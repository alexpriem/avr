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


volatile uint8_t *server_p_atn[MAX_BOARDS];           
volatile uint8_t *server_p_atn[MAX_BOARDS];           
volatile uint8_t *server_p_atn[MAX_BOARDS];           

uint8_t server_b_atn[MAX_BOARDS];
uint8_t server_b_atn[MAX_BOARDS];
uint8_t server_b_atn[MAX_BOARDS];


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



// codes voor 0-9:




void client_loop (uint8_t chip) {

  
  j=0;
  while (j<7) {
  	while (bit_get(*patn, batn)==0) {
  		udelay(1);     
  	}
  	data=bit_get(*pdata, bdata);
  	c=c|data;
  	c=c<<1;     
  	j++;
  	bit_set (*pack, back);

  	while (bit_get(*patn, batn)==1) {
  		udelay(0);
  	}
  	bit_clr (*pack, back);

  	
  } 


}



   
int main(void)
{
	
   uart_init ();	
   uart_printf ("init\r\n");
   uart_printf ("done\r\n");

   client_setup (0, P_PA2, P_PA6, P_PC2);

	client_loop();   

 return 0;
}
