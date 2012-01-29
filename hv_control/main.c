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

  int a,b,c,d;
  

    uart_init ();



    DDRC  = 0xff;  // C output            
    DDRA  = 0x00;  // A input              
    DDRB  = 0xff;   // B output
    
    PORTA = 0x00;                   
    PORTB = 0xff;  // pullups
    PORTC = 0xff;




  // init_mcp3204(uint8_t chip, uint8_t cs, uint8_t clk,  uint8_t dout, uint8_t din)

    init_mcp3204(0, P_PB3, P_PB7, P_PB5, P_PB6);


    delay(1000);
    
    
    DDRA |=  BIT0 | BIT1  | BIT2;
    PORTA = 0x00;
    uart_printf ("\r\nbooting..\r\n");
//    delay (1000);
    uart_printf ("+220\r\n");
    bit_set (PORTA, BIT2);
//    sleep(45);
    uart_printf ("HVon\r\n");
    bit_set (PORTA, BIT0);
//    delay(1500);
    uart_printf ("HVfull\r\n");
    bit_set (PORTA, BIT1);

   

    for (;;) {
        
        for (val=0; val<4096; val++) {


            a=mcp3204_getw (0,0,ADC_DIFF);
            b=mcp3204_getw (0,0,ADC_DIFF);
            c=mcp3204_getw (0,2,ADC_DIFF);
            d=mcp3204_getw (0,2,ADC_DIFF);

            uart_printf ("di:%x , %x , %x , %x", a,b,c,d);
            a=mcp3204_getw (0,0,ADC_SE);
            b=mcp3204_getw (0,1,ADC_SE);
            c=mcp3204_getw (0,2,ADC_SE);
            d=mcp3204_getw (0,3,ADC_SE);

            uart_printf ("\t se:%x , %x , %x , %x\r\n", a,b,c,d);


            }
        
        }


}
