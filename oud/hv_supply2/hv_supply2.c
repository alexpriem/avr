#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

#include "hv_supply.h"
#include "lcd.h"




int do_sweep (char *s)
{
 uint8_t dev;
 double start, stop, step;
 double val;
 char buf[16];
 str_powersupply *p;


 dev=0;
 lcd_puts (s);
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
        lcd_puts(buf);
        lcd_putc('\n');
//        _delay_ms (delay);
     }


 return OK;
}
