/*************************************************************************
Title:    output data to a serial port.

**************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "uart.h"
#ifdef _DEBUG_
#include "lcd.h"
#endif 

 
#define F_OSC 16000000		           /* uc-oscillator-frequency in Hz */
#define UART_BAUD_RATE 9600
#define UART_BAUD_CALC(UART_BAUD_RATE,F_OSC) ((F_OSC)/((UART_BAUD_RATE)*16l)-1)



char uart_buf[UART_BUFLEN];
uint8_t uart_pos, uart_done;


/* Initialize UART */




void uart_init( void)
{
 UBRRH = (uint8_t)(UART_BAUD_CALC(UART_BAUD_RATE,F_OSC)>>8);
 UBRRL = (uint8_t)UART_BAUD_CALC(UART_BAUD_RATE,F_OSC);

 UCSRB = ( (1<<RXEN) | (1<<TXEN) );  /* Enable UART receiver and transmitter */
 UCSRC = (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);

 uart_done=0;
 uart_pos=0;
 UCSRB |= (1<<RXCIE);               /* interrupts toestaan*/
 sei();
}





void uart_putc (unsigned char data )
{
 while ( !(UCSRA & (1<<UDRE)) )
  ;                    /* Wait for empty transmit buffer */
 UDR = data;            /* Start transmittion */
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
    char buf[32];


    va_start(ap, fmt);    /* make ap point to the first unnamed arg */
    for (p = fmt; *p; p++) {
        if (*p != '%') {
            uart_putc(*p);
            continue;
        }
        switch (*++p) {
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
  
ISR (SIG_UART_RECV)
{
   unsigned char c;
   c=UDR;
   uart_buf[uart_pos]=c;   
   uart_pos++;
   if (uart_pos>=UART_BUFLEN) uart_pos=0;   
   if ((c==13) || (c==10)) uart_done=1;   
   uart_putc(c);
   
}




unsigned char uart_getc(void)
{
 for (;;) {
	if((UCSRA&(1<<7)) != 0) {		
		return UDR;
	}
  }
}
