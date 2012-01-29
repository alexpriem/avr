/*************************************************************************
Title:    testing output to a HD44780 based LCD display.
Author:   Peter Fleury  <pfleury@gmx.ch>  http://jump.to/fleury
File:     $Id: test_lcd.c,v 1.6 2004/12/10 13:53:59 peter Exp $
Software: AVR-GCC 3.3
Hardware: HD44780 compatible LCD text display
          ATS90S8515/ATmega if memory-mapped LCD interface is used
          any AVR with 7 free I/O pins if 4-bit IO port mode is used
**************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "uart.h"
#ifdef _DEBUG_
#include "lcd.h"
#endif


#define F_OSC 8000000		           /* oscillator-frequency in Hz */
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



