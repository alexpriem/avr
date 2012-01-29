#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

/*
#define F_CPU 8000000UL  // 8 MHz
#include <util/delay.h>
*/

#include "hv_supply.h"
#include "lcd.h"


char return_buf[32];

uint8_t dev;
uint8_t err;
str_powersupply powersupply[NUM_SUPPLIES];



int set_voltage (char *s)
{
 long val;
 int err;

 val=0; err=0; 
 if (err) return err;

 powersupply[dev].voltage=val;

/* hier natuurlijk daadwerkelijk spanning zetten... */

 return OK;
}


int get_voltage (char *s)
{

 dtostrf(powersupply[dev].voltage, 6, 3, return_buf);
 return OK;
}



int set_current (char *s)
{
 long val;
 int err;

 val=0; err=0; 
 if (err) return err;

 powersupply[dev].current=val;

/* modecheck, plus hier natuurlijk daadwerkelijk stroom zetten... */

 return OK;
}


int get_current (char *s)
{

 dtostrf(powersupply[dev].current, 6, 3, return_buf);
 return OK;
}






int set_sweep_start (char *s)
{
 long val;
 int err;
 
  val=0; err=0; err=0;
 if (err) return err;

 powersupply[dev].swstart=val;

/* modecheck, plus hier natuurlijk daadwerkelijk stroom zetten... */

 return OK;
}


int get_sweep_start (char *s)
{

 dtostrf(powersupply[dev].swstart, 6, 3, return_buf);
 return OK;
}


int set_sweep_stop (char *s)
{
 long val;
 int err;
 
 val=0; err=0; 
 if (err) return err;

 powersupply[dev].swstop=val;

/* modecheck, plus hier natuurlijk daadwerkelijk stroom zetten... */

 return OK;
}


int get_sweep_stop (char *s)
{

 dtostrf(powersupply[dev].swstop, 6, 3, return_buf);
 return OK;
}


int set_sweep_step (char *s)
{
 long val;
 int err;
 
 val=0; err=0; 
 if (err) return err;

 powersupply[dev].swstep=val;

/* modecheck, plus hier natuurlijk daadwerkelijk stroom zetten... */

 return OK;
}


int get_sweep_step (char *s)
{

 dtostrf(powersupply[dev].swstep, 6, 3, return_buf);
 return OK;
}



int set_sweep_time (char *s)
{
 long val;
 int err;
 
 val=0; err=0;
 if (err) return err;

 powersupply[dev].swtime=val;

/* modecheck, plus hier natuurlijk daadwerkelijk stroom zetten... */

 return OK;
}


int get_sweep_time (char *s)
{

 dtostrf(powersupply[dev].swtime, 6, 3, return_buf);
 return OK;
}




int set_output (char *s)
{
 long val;
 int err;
 
 val=0; err=0;
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

 dtostrf(dev, 6, 3, return_buf);
 return OK;
}





