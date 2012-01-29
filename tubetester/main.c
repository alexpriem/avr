/******************************************************************************
 Title:    Tuberelayswitch
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
#include "74hc595.h"
#include "delay.h"

#include "leds.h"




unsigned int vol, ch;
int bal;
 



int values[8]={1,2,4,8,16,32,64,128};

int main(void)
{
    uint8_t i,j, val, numchips;
	int rev=1;
	
	numchips=3;
	

//  int a,b,c,d;
    uart_init ();
	
	//init_74hc595(uint8_t chip, uint8_t clk, uint8_t cs, uint8_t data)
    DDRC  = 0xff;  // C output            
    DDRA  = 0xff;  // A output              
    DDRB  = 0xff;   // B output

	uart_printf ("rev:%d\n", rev);
	
	init_74hc595(0, P_PC0, P_PC1, P_PA1);
	init_74hc595(1, P_PC0, P_PC1, P_PA0);
	init_74hc595(2, P_PC0, P_PC1, P_PB0);	
	
    //PORTA = 0xff;                   // pullups.
    //PORTB = 0xff;
    //PORTC = 0xff;
	
	i=0;
    for (;;) {
			val=values[i];
			s595_putc (0, val);
			uart_printf ("ok:%d -- %d\r\n",i, val);  
			
			for (j=0; j<numchips; j++) {
				s595_set_val (j, val);
			}
			s595_write(numchips);
			
			i++;
			if (i>7) {
				i=0;
			}
			delay(500);            
        }


}
