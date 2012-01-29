/******************************************************************************
 Title:    Volume control
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
#include "mcp4922.h"
#include "mcp3202.h"
#include "mcp3204.h"
#include "delay.h"

#include "leds.h"


#define MAXVOL 64
#define MAXBAL 32
#define MAXCHAN 3



unsigned int vol, ch;
int bal;
 





int main(void)
{
    int val;

//  int a,b,c,d;
  

    uart_init ();



    DDRC  = 0xff;  // C output            
    DDRA  = 0xff;  // A input              
    DDRB  = 0xff;   // B output
    
    PORTA = 0xff;                   // pullups.
    PORTB = 0xff;
    PORTC = 0xff;




//                (chip, cs,  clk,  data, ldac )
//    init_mcp4922(0, P_PA3, P_PA0, P_PA6, P_PA1);

//                 chip, uint8_t cs,  uint8_t ldac 
    init_mcp4922_spi (0, P_PA3, P_PA1);
    init_mcp4922(0, P_PA3, P_PA0, P_PA6, P_PA1);
//    init_mcp3204(0, P_PA4, P_PA0, P_PA2, P_PA6);
//    init_leds (P_PORTA);
    
    for (;;) {
        for (val=0; val<4096; val++) {


//            a=mcp3204_getw (0,0,ADC_SE);
//           b=mcp3204_getw (0,1,ADC_SE);
//            c=mcp3204_getw (0,2,ADC_SE);
//            d=mcp3204_getw (0,3,ADC_SE);

//            uart_printf ("d:%d , %d , %d , %d\r\n", a,b,c,d);
          uart_printf ("%d\r\n", val);
//          mcp4922_putw (0,val, 0);
//          mcp4922_putw (0,val, 1);
          mcp4922_spiw (0,val, 0);
          mcp4922_spiw (0,val, 1);


            }
        
        }


}
