#include "delay.h"

#define XTAL 16000000

/*************************************************************************
 delay loop for small accurate delays: 16-bit counter, 4 cycles/loop
*************************************************************************/
static inline void _delayFourCycles(unsigned int __count)
{
    if ( __count == 0 )    
        __asm__ __volatile__( "rjmp 1f\n 1:" );    // 2 cycles
    else
        __asm__ __volatile__ (
    	    "1: sbiw %0,1" "\n\t"                  
    	    "brne 1b"                              // 4 cycles/loop
    	    : "=w" (__count)
    	    : "0" (__count)
    	   );
}


void udelay (unsigned int us)
{

    _delayFourCycles( ( ( 1*(XTAL/4000) )*us)/1000 );
}


void delay (unsigned int ms)

{
 int i;
 
 for (i=0; i<ms; i++)    
     udelay(1000);
}


void sleep (unsigned int s)

{
 int i;
 
 for (i=0; i<s; i++)    
     delay(1000);
}




