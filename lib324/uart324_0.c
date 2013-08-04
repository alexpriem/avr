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
#include "uart324_0.h"
#include "constants.h"

 
#define F_OSC 16000000		           /* uc-oscillator-frequency in Hz */
#define UART_BAUD_RATE 9600
#define UART_BAUD_CALC(UART_BAUD_RATE,F_OSC) ((F_OSC)/((UART_BAUD_RATE)*16l)-1)



char uart_buf[UART_BUFLEN+2];
uint8_t uart_pos, uart_done;
uint8_t uart_echo=TRUE;


/* Initialize UART */




void uart_init( void)
{
  /* PRR &= (uint8_t)~(1 << PRUSART0); 
 PRR1 &= (uint8_t)~(1 << PRUSART1); */

 UBRR0H = (uint8_t)(UART_BAUD_CALC(UART_BAUD_RATE,F_OSC)>>8);
 UBRR0L = (uint8_t)UART_BAUD_CALC(UART_BAUD_RATE,F_OSC);

 UCSR0B = ( (1<<RXEN0) | (1<<TXEN0) );  /* Enable UART receiver and transmitter */
 UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

 uart_done=0;
 uart_pos=0;
 UCSR0B |= (1<<RXCIE0);               /* interrupts toestaan*/
 sei();
}





void uart_putc (unsigned char data )
{
 while ( !(UCSR0A & (1<<UDRE0)) )
  ;                    /* Wait for empty transmit buffer */
 UDR0 = data;            /* Start transmittion */
}


void uart_puts (char *s)

{
 register unsigned char c;

 while ( (c = *s++) ) {
       uart_putc(c);
    }
}



void uart_printf(char *fmt, ...)
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
            uart_putc(*p);
            continue;
        }
        switch (*++p) {
		case 'b':
		    ival = va_arg(ap, int);
		    itoa(ival & 255, buf,2);    
            uart_puts(buf);
            break;
        case 'd':
        case 'i':
            ival = va_arg(ap, int);
            itoa(ival, buf,10);    
            uart_puts(buf);
            break;
        case 'u':
            uval = va_arg(ap, unsigned int);
            utoa(uval, buf,10);    
            uart_puts(buf);
            break;
		case 'U':
            uval = va_arg(ap, unsigned long int);
            ultoa(uval, buf,10);    
            uart_puts(buf);
            break;   		
		case 'l':
			lval = va_arg(ap, long int);
            ltoa(lval, buf,10);    
            uart_puts(buf);
			break;			
		case 'L':
			llval = va_arg(ap, long long int);
            ltoa(llval, buf,10);    
            uart_puts(buf);
			break;
        case 'x':
            ival = va_arg(ap, int);
            itoa(ival, buf,16);    
            uart_puts(buf);
            break;
        case 'c':
            ival = va_arg(ap, int);
            uart_putc(ival);
            break;
        case 's':
            sval= va_arg(ap, char *);
			if (sval==NULL) 
				uart_puts("NULL");
			else 
				uart_puts(sval);
            break;
		case 'p':
		    sval= va_arg(ap, char *);
			if (sval==NULL) 
				uart_puts("NULL");
			else {				
				itoa((int) sval, buf, 16);
				uart_puts(buf);
			}
            break;
        default:
            uart_putc(*p);
            break;
        }
    }
    va_end(ap);
}






 /* USART RX interrupt     */
  
ISR (USART0_RX_vect)
{
   unsigned char c;
   c=UDR0;
   uart_buf[uart_pos]=c;   
   uart_pos++;
   if (uart_pos>=UART_BUFLEN) uart_pos=0;   
   if ((c==13) || (c==10)) uart_done=1;   
   if (uart_echo) uart_putc(c);
}




unsigned char uart_getc(void)
{
 for (;;) {
	if((UCSR0A&(1 << RXC1 )) != 0) {		
		return UDR0;
	}
  }
}
