#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

#include "hv_supply.h"

#include "scpi.h"

uint8_t dev;
str_powersupply powersupply[NUM_SUPPLIES];


int main  (void)
{
 double start, stop, step, delay;
 double val;

 start=powersupply[dev].swstart;
 step=powersupply[dev].swstep;
 stop=powersupply[dev].swstop;
 delay=powersupply[dev].swtime/ ((stop-start)/step);


  val=start;
  while (val<stop) {

      powersupply[dev].swtime=powersupply[dev].swtime+4;
        
     }


 return OK;
}

