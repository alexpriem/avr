#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

#include "scpi.h"
#include "hv_supply.h"
#include "uart.h"




int do_sweep (char *s)
{
 uint8_t dev;
 long start, stop, step;
 long val;
 char buf[16];
 str_powersupply *p;


 dev=0;
 uart_puts (s);
 p=&s[dev];
 start=p->swstart;
 step=p->swstep;
 stop=p->swstop;
// delay=powersupply[dev].swtime/ ((stop-start)/step);
 
  start=0;
//  stop=100;

 val=start;
 while (val<stop) {
        dtostrf(val, 6, 3, buf);
        uart_puts(buf);
        uart_putc('\n');
//        _delay_ms (delay);
     }


 return OK;
}
