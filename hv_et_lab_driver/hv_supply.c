#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>

#include "mcp4922.h"
#include "mcp3204.h"


/*
#define F_CPU 8000000UL  // 8 MHz
#include <util/delay.h>
*/

#include "scpi.h"
#include "uart.h"
#include "hv_supply.h"



char return_buf[80];

uint8_t dev=0;
str_powersupply powersupply[NUM_SUPPLIES];





//long-waarde (ongeschaald) omrekenen naar dac-waarde.

// 0..10v ->0..500V --> 0..4096
long long2dacval (long lval) 
{
 long dacval;
 dacval=(lval>>7) - (lval>>11)-(lval>>13);
 return dacval;
}

// dacwaarde omrekenen naar ascii-waarde.

void print_dacval (long val) 

{
 long highval, lowval;
 
 highval=(val>>10)+ (val>>16)+(val>>17);  // val/1000
 lowval=val- ((highval<<10) - (highval<<4) -(highval<<3));   // val *1000
 
 uart_printf ("%d.%d V\r\n", highval, lowval);
}


int16_t get_dacval (char *s, int *err)

{
 long lval=0;
 int16_t dacval;
 char *s2;
 
 #ifdef _DEBUG_
 uart_printf ("get_dacval:[%s]\r\n", s);
#endif
 s2=NULL; 
 
 *err=0;
 lval=scpi_get_longfrac(s,err, &s2);
 if (err) return *err;
#ifdef _DEBUG_ 
 uart_printf ("get_dacval got %l  (err:%d) [%s]\r\n", lval, err, s2);
#endif 
 if (scpi_check_buf_empty(s2)==FALSE) return E_UNTERMINATED;

 if ((lval<0) || (lval>512001)) return E_VOLTAGERANGE;
 
 dacval=long2dacval(lval);
 #ifdef _DEBUG_ 
 uart_printf ("get_dacval:dacval %d\r\n", dacval);
#endif 

 return dacval;
}
 



/* selecteer voeding nr. */

int set_instrument_nsel (char *s)

{
 uint8_t val;
 int err;
 char *s2;

 val=scpi_get_digit(s,&err, &s2);
#ifdef _DEBUG_ 
 uart_printf ("set_instr_sel got %s  (err:%d) [%s]\r\n", val, err, s2);
#endif 
 if (err) return err;
 if (scpi_check_buf_empty(s2)==FALSE) return E_UNTERMINATED;
 
 dev=val;
 
 return OK;
}



int get_instrument_nsel (char *s)
{
 uart_printf ("%d\r\n", dev);
 return OK;
}




int set_voltage (char *s)
{
 int err;
 long dacval;
 
 dacval=get_dacval (s, &err);
 if (err!=OK) return err;
 powersupply[dev].ovp_voltage=dacval;
 
 mcp4922_putw( 1, dacval, 0);
 return OK;
}




int set_ovp_voltage (char *s)
{
 int err;
 long dacval;
 
 dacval=get_dacval (s, &err);
 if (err!=OK) return err;
 powersupply[dev].ovp_voltage=dacval;

 mcp4922_putw( 0, dacval, 0);
#ifdef _DEBUG_ 
 uart_printf ("set_ovp:dacval %d\r\n", dacval);
#endif 
 return OK;
}




int set_current (char *s)
{
 long lval=0;
 int16_t dacval;
 int err=0;
 char *s2;
 
 #ifdef _DEBUG_
 uart_printf ("set_current:[%s]\r\n", s);
#endif
 
 lval=scpi_get_longfrac(s,&err, &s2);
 if (err) return err;
#ifdef _DEBUG_ 
 uart_printf ("set_current:got %l  (err:%d) [%s]\r\n", lval, err, s2);
#endif 
 if (scpi_check_buf_empty(s2)==FALSE) return E_UNTERMINATED;

 if ((lval<0) || (lval>2000)) return E_CURRENTRANGE;
 
 powersupply[dev].current=lval;

 dacval=lval<<1;
 mcp4922_putw( 1, dacval, 1);
#ifdef _DEBUG_ 
 uart_printf ("set_current:dacval %d\r\n", dacval);
#endif 
 return OK;
}






int set_standby (char *s)
{
 uint8_t val;
 int err;
 char *s2;
 
 val=scpi_get_digit(s,&err, &s2);
#ifdef _DEBUG_ 
 uart_printf ("set_standby got %s  (err:%d) [%s]\r\n", val, err, s2);
#endif 
 if (err) return err;
 if (scpi_check_buf_empty(s2)==FALSE) return E_UNTERMINATED;
 
 if (val==0) {
	mcp4922_putw( 0, 0, 1);
	return OK;
 }
 if (val==1) {
	mcp4922_putw( 0, 1650, 1);
	return OK;
 }
	
 return E_STANDBYRANGE;
}



int get_voltage (char *s)
{
 long adcval;

 adcval=mcp3204_getw (0, 1, 0);
 print_dacval(adcval);
 
 return OK;
}

int get_current (char *s)
{
 long val;
 unsigned int adcval, highval, lowval;

 adcval=mcp3204_getw (0, 1, 1);
 val=adcval*2048;       // mapping van 0..4096 --> 0..10V --> 0..2A
 
 highval=(val>>10)+ (val>>16)+(val>>17);  // val/1000
 lowval=val- ((highval<<10) - (highval<<4) -(highval<<3));   // val *1000
 
 uart_printf ("%d.%d V\r\n", highval, lowval);
 return OK;
}

int get_ovp_voltage (char *s)
{ 
 print_dacval(powersupply[dev].ovp_voltage); 
 return OK;
}

int get_standby (char *s)
{

 return OK;
}




int device_reset (void) 

{
 scpi_parse_line ("ovp 512.0\n" );
 scpi_parse_line ("current 1.0\n" );
 scpi_parse_line ("volt 0.0\n" );
 scpi_parse_line ("st 1\n" );
 return OK;
}



int set_sweep_start (char *s)
{
 int err;
 long dacval;
 
 dacval=get_dacval (s, &err);
 if (err!=OK) return err;
 powersupply[dev].swstart=dacval;
 
 return OK;
}


int get_sweep_start (char *s)
{

 print_dacval(powersupply[dev].swstart); 
 return OK;
}


int set_sweep_stop (char *s)
{
 int err;
 long dacval;
 
 dacval=get_dacval (s, &err);
 if (err!=OK) return err;
 powersupply[dev].swstop=dacval;
 
 return OK;
}


int get_sweep_stop (char *s)
{
 print_dacval(powersupply[dev].swstop); 
 return OK;
}


int set_sweep_step (char *s)
{
 int err;
 long dacval;
 
 dacval=get_dacval (s, &err);
 if (err!=OK) return err;
 powersupply[dev].swstep=dacval;
 return OK;
}


int get_sweep_step (char *s)
{
 print_dacval(powersupply[dev].swstep); 
 return OK;
}



int set_sweep_time (char *s)
{
 long val;
 int err;
 
 val=0; err=0;
 if (err) return err;

 powersupply[dev].swtime=val;



 return OK;
}


int get_sweep_time (char *s)
{

 //dtostrf(powersupply[dev].swtime, 6, 3, return_buf);
 return OK;
}


/*

int do_sweep (char *s)
{
 uint8_t dev;
 long start, stop, step;
 long val;
 char buf[16];
 str_powersupply *p;

 dev=0;
 uart_puts (s);
 p=&powersupply[dev];
 start=p->swstart;
 step=p->swstep;
 stop=p->swstop;
// delay=powersupply[dev].swtime/ ((stop-start)/step);
 
  start=0;
//  stop=100;

 val=start;
 while (val<stop) {
        uart_puts(buf);
        uart_putc('\n');
//        _delay_ms (delay);
     }


 return OK;
}


*/