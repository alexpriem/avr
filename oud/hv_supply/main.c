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
#include "scpi.h"
#include "encoder.h"
#include "ad_da.h"

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

    strcpy (s,"volt 20.1 V");
    scpi_parse_line (s);
    strcpy (s,"volt?");
    scpi_parse_line (s);

    strcpy (s,"sweep:start 0.5");
    scpi_parse_line (s);
    strcpy (s,"sweep:start?");
    scpi_parse_line (s);

    strcpy (s,"sweep:stop 20");
    scpi_parse_line (s);
    strcpy (s,"sweep:stop?");
    scpi_parse_line (s);

    strcpy (s,"sweep:step 0.5");
    scpi_parse_line (s);
    strcpy (s,"sweep:step?");
    scpi_parse_line (s);
    
    strcpy (s,"sweep:go");
    scpi_parse_line (s);

    
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
    unsigned int val1,val2;
    char buf[16];

   

    DDRD &=~ (1 << PD7);        /* Pin PD2 input              */
    PORTD |= (1 << PD7);        /* Pin PD2 pull-up enabled    */

    DDRA =0xff;
    PORTA=0xff;

    
    /* initialize display, cursor off */
    lcd_init(LCD_DISP_ON);
    lcd_clrscr();
    lcd_backlight(LCD_BL_ON);
    uart_init ();

    init_encoder (0,0,1);
    init_encoder (1,2,3);

    val1=0;
    val2=0;


 init_tda1543 (0, 1, 0, 1, 2 );
 init_tda1543 (2, 3, 3, 1, 2 );
    for (;;) {
        val1-=256*get_encoder (0);
        val2-=256*get_encoder (1);
        uart_printf ("%u %u\r\n",val1, val2);
        lcd_printf ("%u %u\r\n",val1, val2);
        set_tda1543_2 (0, val2, 1, val2);
     }




    init_adc (0, 5,6,7);
//    init_adc (0, 3,4,5);
    for (;;) {
        val1=get_adc (0);
        if (val1!=val2) {
            ltoa (val1,buf,10);
            
            uart_puts (buf);
            lcd_puts (buf);
            uart_puts ("\r\n");
            lcd_puts ("\r\n");
            val2=val1;
            }
        }


    
    
#ifdef _DEBUG_
    uart_puts ("\r\n\nmain()\n");
    lcd_puts("main()\n");
#endif
    
    command_loop();
    for (;;) {                           /* loop forever */
    }
 return 0;
}
