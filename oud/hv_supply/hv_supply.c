#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <errno.h>


/*
#define F_CPU 8000000UL  // 8 MHz
#include <util/delay.h>
*/

#include "scpi.h"
#include "scpi_cmds.h"
#include "hv_supply.h"
#include "lcd.h"
#include "uart.h"



uint8_t dev;
str_powersupply powersupply[NUM_SUPPLIES];








int set_voltage (char *s)
{
 long val;
 int err;
 
 err=scpi_get_long_unit(s,&val);
 if (err) return err;

 powersupply[dev].voltage=val;

/* hier natuurlijk daadwerkelijk spanning zetten... */

 return OK;
}


int get_voltage (char *s)
{

 ltostr(powersupply[dev].voltage, return_buf);
 return OK;
}



int set_current (char *s)
{
 long val;
 int err;
 
 err=scpi_get_long_unit(s,&val);
 if (err) return err;

 powersupply[dev].current=val;

/* modecheck, plus hier natuurlijk daadwerkelijk stroom zetten... */

 return OK;
}


int get_current (char *s)
{

 ltostr(powersupply[dev].current, return_buf);
 return OK;
}






int set_sweep_start (char *s)
{
 long val;
 int err;
 
 err=scpi_get_long_unit(s,&val);
 if (err) return err;

 powersupply[dev].swstart=val;

/* modecheck, plus hier natuurlijk daadwerkelijk stroom zetten... */

 return OK;
}


int get_sweep_start (char *s)
{

 ltostr(powersupply[dev].swstart, return_buf);
 return OK;
}


int set_sweep_stop (char *s)
{
 long val;
 int err;
 
 err=scpi_get_long_unit(s,&val);
 if (err) return err;

 powersupply[dev].swstop=val;

/* modecheck, plus hier natuurlijk daadwerkelijk stroom zetten... */

 return OK;
}


int get_sweep_stop (char *s)
{

 ltostr(powersupply[dev].swstop, return_buf);
 return OK;
}


int set_sweep_step (char *s)
{
 long val;
 int err;
 
 err=scpi_get_long_unit(s,&val);
 if (err) return err;

 powersupply[dev].swstep=val;

/* modecheck, plus hier natuurlijk daadwerkelijk stroom zetten... */

 return OK;
}


int get_sweep_step (char *s)
{

 ltostr(powersupply[dev].swstep, return_buf);
 return OK;
}



int set_sweep_time (char *s)
{
 long val;
 int err;
 
 err=scpi_get_long_unit(s,&val);
 if (err) return err;

 powersupply[dev].swtime=val;

/* modecheck, plus hier natuurlijk daadwerkelijk stroom zetten... */

 return OK;
}


int get_sweep_time (char *s)
{

 ltostr(powersupply[dev].swtime, return_buf);
 return OK;
}




int set_output (char *s)
{
 long val;
 int err;
 
 err=scpi_get_long_unit(s,&val);
 if (err) return err;

 powersupply[dev].enable=0;

/* modecheck, plus hier natuurlijk daadwerkelijk stroom zetten... */

 return OK;
}









/* selecteer voeding nr. */

int set_instrument_nsel (char *s)

{
 uint8_t nr;
 char *s2;

 nr=strtol (s, &s2, 0);
 if ((s==s2) || (*s2!=0)) return ERROR;
 
 if ((nr>=1) && (nr<127))
     dev=nr;
     return OK;
 
 return ERROR;
}



int get_instrument_nsel (char *s)
{

 ltostr(dev, return_buf);
 return OK;
}










int do_sweep (char *s)
{
 long start, stop, step,delay;
 long val;
 char buf[16];
 
 uart_puts (s);
 start=powersupply[dev].swstart;
 step=powersupply[dev].swstep;
 stop=powersupply[dev].swstop;
 delay=powersupply[dev].swtime/ ((stop-start)/step);
 

 val=start;
 while (val<stop) {
        ltostr(val, buf);
        uart_puts(buf);
        uart_puts("\n\r");
//        _delay_ms (delay);
        val+=step;
     }


 return OK;
}
