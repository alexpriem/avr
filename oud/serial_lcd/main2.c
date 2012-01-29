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
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "lcd.h"
#include "uart.h"
#include "encoder.h"


#define F_OSC 8000000		           /* oscillator-frequency in Hz */
#define UART_BAUD_RATE 38400
#define UART_BAUD_CALC(UART_BAUD_RATE,F_OSC) ((F_OSC)/((UART_BAUD_RATE)*16l)-1)


/* Initialize UART */

void InitUART( void)
{
 UBRRH = (uint8_t)(UART_BAUD_CALC(UART_BAUD_RATE,F_OSC)>>8);
 UBRRL = (uint8_t)UART_BAUD_CALC(UART_BAUD_RATE,F_OSC);

 UCSRB = ( (1<<RXEN) | (1<<TXEN) );  /* Enable UART receiver and transmitter */
 UCSRC = (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);
 
}

/* Read and write functions */
unsigned char ReceiveByte( void )
{
 while ( !(UCSRA & (1<<RXC)) )  /* Wait for incoming data */
  ;                   /* Return the data */
 return UDR;
}

void TransmitByte( unsigned char data )
{
 while ( !(UCSRA & (1<<UDRE)) )
  ;                    /* Wait for empty transmit buffer */
 UDR = data;            /* Start transmittion */
}




void escape_command (unsigned char command) 

{
 unsigned char x,y;
 unsigned char i;

 switch (command) {
	case 0:  lcd_clrscr ();
			 break;
    case 1:  lcd_home ();
             break;
    case 2:  x=ReceiveByte();
             y=ReceiveByte();
             lcd_gotoxy (x,y);
			 break;
    case 3:  lcd_command(LCD_MOVE_DISP_LEFT);
			 break;
    case 4:  lcd_command(LCD_MOVE_DISP_RIGHT);
			 break;
    case 5:   lcd_command(_BV(LCD_CGRAM));  /* set CG RAM start address 0 */
              for(i=0; i<16; i++)
				 lcd_data(ReceiveByte());
             lcd_gotoxy(0,1);
             break;
    case 6:   lcd_command(_BV(LCD_CGRAM));  /* set CG RAM start address 0 */
              for(i=0; i<63; i++)
				 lcd_data(ReceiveByte());
             lcd_gotoxy(0,1);
             break;
	case 255:  lcd_backlight(ReceiveByte());
			 break;
  }
}


int main(void)
{
    unsigned char c;
    //int val1, val2,prev_val1, prev_val2;
    
    
    /* initialize display, cursor off */
    lcd_init(LCD_DISP_ON);
    lcd_backlight(LCD_BL_ON);


    lcd_clrscr();
    lcd_puts("Serial 38400/8N1\n");


    InitUART ();
    lcd_puts("Ok.");
    lcd_home ();

    
    while (1) {
          
	  c=ReceiveByte();
          switch (c) {
            case 27: escape_command (ReceiveByte());
                     break;
            case 28: lcd_command (ReceiveByte());
                     break;
            case 29: lcd_data (ReceiveByte());
                     break;
			default: lcd_putc(c);
         }
    }


}
