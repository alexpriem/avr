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
#include "lcd.h"
#include "uart.h"

#define _DEBUG_ 1


void wait_until_key_pressed(void)
{
    unsigned char temp1, temp2;
    unsigned int i;
    
    do {
        temp1 = PIND;                  // read input
        for(i=0;i<65535;i++);
        temp2 = PIND;                  // read input
        temp1 = (temp1 & temp2);       // debounce input
    } while ( temp1 & _BV(PIND7) );
    
    loop_until_bit_is_set(PIND,PIND7);            /* wait until key is released */
}







void command_loop(void)
{
    uint8_t i;
    char s[32];

    
    lcd_clrscr();

    strcpy (s,"volt 20.1 mV");
    
    strcpy (s,"volt?");

    
    while (1) {
	  if (uart_done!=0) {
              
                 lcd_putc('*');
                 lock(&sem_uart);
                 for (i=1; i<uart_len; i++)
                     lcd_putc(uartbuf[i]);
                 uart_len=0;
                 uart_done=0;
                 unlock(&sem_uart);
                }
                 
    }

}






int main(void)
{
//    unsigned char i;
   

    DDRD &=~ (1 << PD7);        /* Pin PD2 input              */
    PORTD |= (1 << PD7);        /* Pin PD2 pull-up enabled    */
    
    /* initialize display, cursor off */
    lcd_init(LCD_DISP_ON);
    lcd_clrscr();
    lcd_backlight(LCD_BL_ON);
    uart_init ();
    
#ifdef _DEBUG_
    lcd_puts("main()\n");
#endif
    
    command_loop();
    for (;;) {                           /* loop forever */
    }
 return 0;
}
