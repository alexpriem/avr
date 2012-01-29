/*------------------------------------------------------------------------ 
 * 
 * ENCODER - Checks a rotary encoder (polling mode) and returns the following: 
 * 
 *               0:  no movement 
 *              -1:  ccw rotation 
 *               1:  cw rotation 
 *
 *               Where ENC_A and ENC_B are the pins to be 
 *               read from the encoder.
 *------------------------------------------------------------------------*/ 
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "encoder.h"



#define ENC_A (PIND & ENC1A_PIN)
#define ENC_B (PIND & ENC1B_PIN)

signed char 
encoder(void) 
{ 

  
   static unsigned char a, b; 
  
   


   if (!a & !b) { 
      if (ENC_A) { 
         a = ENC_A; 
         return (-1); 
      } 
      if (ENC_B) { 
         b = ENC_B; 
         return (1); 
      } 
   } 
   if (a & !b) { 
      if (!ENC_A) { 
         a = ENC_A; 
         return (1); 
      } 
      if (ENC_B) { 
         b = ENC_B; 
         return (-1); 
      } 
   } 
   if (a & b) { 
      if (!ENC_A) { 
         a = ENC_A; 
         return (-1); 
      } 
      if (!ENC_B) { 
         b = ENC_B; 
         return (1); 
      } 
   } 
   if (!a & b) { 
      if (ENC_A) { 
         a = ENC_A; 
         return (1); 
      } 
      if (!ENC_B) { 
         b = ENC_B; 
         return (-1); 
      } 
   } 
   return (0); 
} 


