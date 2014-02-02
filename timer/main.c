#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdlib.h>
 
#include "uart.h"
#include "bitops.h"

#include "delay.h"
#include "constants.h"
	

long int t;

void timer_init (void) 	 

{
	TCCR1B |=  (1<<CS11); // ((1 << CS10) | (1 << CS12));    
	TCNT1 = 0;
	TIMSK |= (1 << TOIE1);
	sei();
}

ISR(TIMER1_OVF_vect)
{
   t++;
} 


print_delta (delta) 
{
	cps=32768000/delta;
	cps2=cps-1000000*(cps/1000000);
	cps2=cps2/1000;
	cps=cps/1000000;
	uart_printf ("cps:%l %l\r\n\r\n",cps, cps2);
}

		  
int main(void)
{	
	int count;	
	long int ts[10];
	int dt;
	long int cps,cps2;
	count=1;
	
	uart_init();
	timer_init ();
	uart_puts ("\r\nreset -- init\r\n");
	
	ts[0]=93;
	ts[1]=102;
	dt=ts[1]-ts[0];
	t=65530;
	for (;;) {		
		uart_printf ("%l %d\r\n",t, count);		
		delay(100000);		
		count++; 
			
	}
 return 0;
}
