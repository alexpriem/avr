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
#include "uart.h"
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
}





void uart_putc (unsigned char data )
{
}


void uart_puts (char *s)

{
}



void uart_printf(char *fmt, ...)
{
}









unsigned char uart_getc(void)
{
 return 0;
}
