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
#include "74hc595.h"
#include "mcp4922.h"
#include "mcp3202.h"

#include "delay.h"

#include "leds.h"


#define MAXVOL 64
#define MAXBAL 32
#define MAXCHAN 3



unsigned int vol, ch;
int bal;
 



int values[8]={1,2,4,8,16,32,64,128};

int main(void)
{
	int rev=0;
	
	int i,adc0, adc1;
    uart_init ();
	
	

	//init_mcp3202(uint8_t chip, uint8_t cs, uint8_t clk,  uint8_t dout, uint8_t din)
	init_mcp3202(0, P_PB1, P_PB7, P_PA0, P_PB3);
	init_mcp4922(1, P_PB2, P_PB7, P_PB3, P_PB0);
	//init_mcp4922(uint8_t chip, uint8_t cs, uint8_t clk,  uint8_t data, uint8_t ldac )

	uart_printf ("rev:%d\n", rev);
	
	i=0;
    for (;;) {
			i++;
			
			//void mcp4922_putw (uint8_t chip, int16_t word, uint8_t dac)
			mcp4922_putw( 1, i, 0);
			mcp4922_putw( 1, 4095-i, 1);
			delay (1500);
		       // mcp3202_getw (uint8_t chip, uint8_t adc, uint8_t mode)
			adc0=mcp3202_getw (0, 1, 1);
			adc1=mcp3202_getw (0, 0, 1);
			
			 			
			uart_printf ("%d %d %d\r\n",i, adc0, adc1 );  			
			i++;
			if (i>4095) {
				i=0;
			}
			            
        }


}
