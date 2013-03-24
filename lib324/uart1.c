/*************************************************************************
Title:    output data to a serial port.

**************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include "uart1.h"
#include "constants.h"

 
#define F_OSC 16000000		           /* uc-oscillator-frequency in Hz */
#define UART_BAUD_RATE 9600
#define UART_BAUD_CALC(UART_BAUD_RATE,F_OSC) ((F_OSC)/((UART_BAUD_RATE)*16l)-1)



char uart1_buf[UART1_BUFLEN+2];
uint8_t uart1_pos, uart1_done;
uint8_t uart1_echo=FALSE;


/* Initialize UART */




void uart1_init( void)
{
 /* PRR0 &= (uint8_t)~(1 << PRUSART0);
 PRR1 &= (uint8_t)~(1 << PRUSART1); */

 UBRR1H = (uint8_t)(UART_BAUD_CALC(UART_BAUD_RATE,F_OSC)>>8);
 UBRR1L = (uint8_t)UART_BAUD_CALC(UART_BAUD_RATE,F_OSC);

 UCSR1B = ( (1<<RXEN1) | (1<<TXEN1) );  /* Enable UART receiver and transmitter */
 UCSR1C =  (1 << UCSZ11) | (1 << UCSZ10);

 uart1_done=0;
 uart1_pos=0;
 UCSR1B |= (1<<RXCIE1);               /* interrupts toestaan*/
 sei();
}





void uart1_putc (unsigned char data )
{
 while ( !(UCSR1A & (1<<UDRE1)) )
  ;                    /* Wait for empty transmit buffer */
 UDR1 = data;            /* Start transmittion */
}


void uart1_puts (char *s)

{
 register unsigned char c;

 while ( (c = *s++) ) {
       uart1_putc(c);
    }
}



void uart1_printf(char *fmt, ...)
{
    va_list ap;
    char *p, *sval;

    int ival;
	long int lval;
	long long int llval;
    unsigned int uval;
    char buf[40];


    va_start(ap, fmt);    /* make ap point to the first unnamed arg */
    for (p = fmt; *p; p++) {
        if (*p != '%') {
            uart1_putc(*p);
            continue;
        }
        switch (*++p) {
		case 'b':
		    ival = va_arg(ap, int);
		    itoa(ival & 255, buf,2);    
            uart1_puts(buf);
            break;
        case 'd':
        case 'i':
            ival = va_arg(ap, int);
            itoa(ival, buf,10);    
            uart1_puts(buf);
            break;
        case 'u':
            uval = va_arg(ap, unsigned int);
            utoa(uval, buf,10);    
            uart1_puts(buf);
            break;
		case 'U':
            uval = va_arg(ap, unsigned long int);
            ultoa(uval, buf,10);    
            uart1_puts(buf);
            break;   		
		case 'l':
			lval = va_arg(ap, long int);
            ltoa(lval, buf,10);    
            uart1_puts(buf);
			break;			
		case 'L':
			llval = va_arg(ap, long long int);
            ltoa(llval, buf,10);    
            uart1_puts(buf);
			break;
        case 'x':
            ival = va_arg(ap, int);
            itoa(ival, buf,16);    
            uart1_puts(buf);
            break;
        case 'c':
            ival = va_arg(ap, int);
            uart1_putc(ival);
            break;
        case 's':
            sval= va_arg(ap, char *);
			if (sval==NULL) 
				uart1_puts("NULL");
			else 
				uart1_puts(sval);
            break;
		case 'p':
		    sval= va_arg(ap, char *);
			if (sval==NULL) 
				uart1_puts("NULL");
			else {				
				itoa((int) sval, buf, 16);
				uart1_puts(buf);
			}
            break;
        default:
            uart1_putc(*p);
            break;
        }
    }
    va_end(ap);
}






 /* USART RX interrupt     */
  
ISR (USART1_RX_vect)
{
   unsigned char c;
   c=UDR1;
   uart1_buf[uart1_pos]=c;   
   uart1_pos++;
   if (uart1_pos>=UART1_BUFLEN) uart1_pos=0;   
   if ((c==13) || (c==10)) uart1_done=1;   
   if (uart1_echo) uart1_putc(c);
}




unsigned char uart1_getc(void)
{
 for (;;) {
	if((UCSR1A&(1<< RXC1)) != 0) {		
		return UDR1;
	}
  }
}
