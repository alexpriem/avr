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
#include "delay.h"



uint8_t getc_3x4_keypad (void) 

{
 uint8_t c=0x12;

 c=PINB;
 return c;
}


