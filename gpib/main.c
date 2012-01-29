/******************************************************************************
 Title:    Volume control
 Author:   Alex Priem
 Date:     October 2009
 Software: AVR-GCC 3.3 
 Hardware: ATMega32
           
 Description:
 
  
*******************************************************************************/
#include <inttypes.h>
#include <avr/io.h>

#include "uart.h"
#include "bitops.h"
#include "gpib.h"
#include "delay.h"

#include "leds.h"


#define MAXVOL 64
#define MAXBAL 32
#define MAXCHAN 3



unsigned int vol, ch;
int bal;
 



uint8_t wait_accepted (void) 

{ uint16_t i;
  uint8_t j, retval;
  j=0; 
  i=0;
 
  retval=0xff;

  do {
      if (bit_get(PINA, NDAC)==NDAC) {
            retval=0;
            break;
      }
      i++;
      if (i==0) {
            j++;
            if (j==16)  {            /* is afhankelijk van klokfrequentie/code */
                retval=bit_get(PINA, NRFD)+0x80;
                break;
            }    
      } 
    }
  while (retval);
  
 
  return retval;
}



uint8_t send_data (char *s)

{
 char *t;
 uint8_t err,j;
 
 t=s;
 j=0;
 bit_clr (PORTC, ATN);
 
 while (*t) {
     uart_putc(*t);
     PORTA=j;
     *t++;
     j++;
     PORTA=*t;
     bit_set (PORTC, DAV);
     err=wait_accepted();
     bit_clr (PORTC, DAV);

     //if (err) return err;     
    }

 return 0;
}



int main(void)
{
    uint8_t address, err,i;

    uart_init ();
    
    DDRA  = 0x00;  // A input              
    DDRB  = 0xff;   // B output
    DDRC  = 0xff;  // C output            

    
    DDRA  = 0xff;  // A output            
    DDRB  = 0xff;  // B output
    DDRC  = 0xff;  // C output       
    DDRD  = 0xff;  // C output            
     
                     
    PORTB = 0xff;  // pullups
    PORTC = 0xff;
    PORTD = 0xff;


    uart_printf ("\r\nbooting..\r\n");
    bit_set (PORTD, GPIB_VCC);
    address=16;

//  opzetten als talker: bits als output, progbits als talker.
   
    DDRC  =  TALK_DDR; 
    PORTC =  0xff;
    DDRA  =  TALK_BITS;
    PORTA = 0x00;


    bit_clr (PORTD, GPIB_TEBITS);
    bit_clr (PORTD, GPIB_PEBITS);
    bit_set (PORTD, GPIB_TEPROG);
    bit_clr (PORTD, GPIB_DCPROG);
    
// controle nemen over de bus.
    
    bit_clr (PORTC, REN);
    bit_set (PORTC, IFC);

    bit_set (PORTC, SRQ);
    bit_clr (PORTC, EOI);

    bit_set (PORTC, ATN);    

/*
    for (i=0; i<255; i++) {
        PORTA=i;
        delay(1);
   }


    DDRC=0xff;
    for (i=0; i<255; i++) {
        PORTC=i;
        delay(1);
   }

*/
    PORTA= (TALK_MASK | address)+1;





    bit_clr (PORTC, DAV);
    err=wait_accepted ();
    bit_set (PORTC, DAV);

    uart_printf ("\r\ntalk #16 %x\r\n", err);

    send_data ("F3X\r\n");

    uart_printf ("\r\nsend command %x\r\n", err);

    return 0;
}
