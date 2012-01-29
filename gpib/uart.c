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


#define F_OSC 16000000		           /* oscillator-frequency in Hz */
#define UART_BAUD_RATE 9600
#define UART_BAUD_CALC(UART_BAUD_RATE,F_OSC) ((F_OSC)/((UART_BAUD_RATE)*16l)-1)




uint8_t uart_len, uart_done, sem_uart;
unsigned char uartbuf[32];

/* Initialize UART */




void uart_init( void)
{
 UBRRH = (uint8_t)(UART_BAUD_CALC(UART_BAUD_RATE,F_OSC)>>8);
 UBRRL = (uint8_t)UART_BAUD_CALC(UART_BAUD_RATE,F_OSC);

 UCSRB = ( (1<<RXEN) | (1<<TXEN) );  /* Enable UART receiver and transmitter */
 UCSRC = (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);

 uart_len=0;
 uart_done=0;
 unlock(&sem_uart);
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
    unsigned int uval;
    char buf[32];


    va_start(ap, fmt);    /* make ap point to the first unnamed arg */
    for (p = fmt; *p; p++) {
        if (*p != '%') {
            uart_putc(*p);
            continue;
        }
        switch (*++p) {
        case 'u':
            uval = va_arg(ap, int);
            utoa(uval, buf,10);    
            uart_puts(buf);
            break;        
        case 'd':
        case 'i':
            ival = va_arg(ap, int);
            itoa(ival, buf,10);    
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
            uart_puts(sval);
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
   lock (&sem_uart);
   c=UDR;
#ifdef _DEBUG_
   lcd_putc(c);
#endif

   uartbuf[uart_len]=UDR;
   uart_len++;
   if (c=='\n')
        uart_done=1;
   unlock(&sem_uart);      
}





void lock (uint8_t *sem)

{
    
/* while (*sem!=0)
    {  } */
 *sem=1;
}


void unlock (uint8_t *sem)

{
 *sem=0;
}



