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




int main(void)
{
    unsigned char c;
    int val1, val2,prev_val1, prev_val2;
    uint8_t my_uartlen, x,y, buflen,i, command;
    
    
    init_encoder (0,6,7);
    init_encoder (1,2,3);
    
    /* initialize display, cursor off */
    lcd_init(LCD_DISP_ON);
    lcd_backlight(LCD_BL_ON);


    lcd_clrscr();
    lcd_puts("Serial 38400/8N1\n");


    uart_init ();
    lcd_puts("Ok.\n");



    val1=0;
    val2=0;
    prev_val1=0;
    prev_val2=0;
    uart_empty=1;
    my_uartlen=0;

    while (1) {
        val1-=get_encoder (0);
        val2-=get_encoder (1);
        if ((val1!=prev_val1) || (val2!=prev_val2)) {
            uart_printf ("%d %d\r\n",val1, val2);
            if (uart_empty) {
                lcd_clrscr ();
                lcd_printf ("[%d][%d]\n", val1, val2);
                }
            prev_val1=val1;
            prev_val2=val2;
            }
        if (uart_len!=my_uartlen) {
            while (uart_len!=my_uartlen) {
                c=uartbuf[my_uartlen];
                if (c!=27) {
                    lcd_putc (uartbuf[my_uartlen]);
                    my_uartlen++;
                    continue;
                }
                if (my_uartlen<=uart_len) 
                    buflen=(my_uartlen-uart_len);
                else 
                    buflen=((uint16_t)(uart_len))+256  - my_uartlen;
                if (c==27) {
                    if (buflen<2) continue;
                    command=uartbuf[my_uartlen+1];
                    
                    switch (command) {
                        case 0: lcd_clrscr ();
                                my_uartlen+=2;  
                                break;
                        case 1: lcd_home ();
                                my_uartlen+=2;
                                break;
                        case 2: if (buflen<4) continue;
                                x=uartbuf[my_uartlen+2];
                                y=uartbuf[my_uartlen+3];
                                lcd_gotoxy (x,y);
                                my_uartlen+=4;
                                break;
                        case 3: lcd_command(LCD_MOVE_DISP_LEFT);
                                my_uartlen+=2;  
                                break;
                        case 4: lcd_command(LCD_MOVE_DISP_RIGHT);
                                my_uartlen+=2;  
            			break;
                        case 5: if (buflen<16) continue;
                                lcd_command(_BV(LCD_CGRAM)); 
                                for(i=0; i<16; i++)
        				lcd_data(uartbuf[my_uartlen+2+i]);
                                lcd_gotoxy(0,1);
                                break;
                        case 6: if (buflen<66) continue;
                                lcd_command(_BV(LCD_CGRAM));  
                                for(i=0; i<63; i++)
                                    lcd_data(uartbuf[my_uartlen+2+i]);
                                    lcd_gotoxy(0,1);
                                break;
                        case 255: if (buflen<3) continue;
                                lcd_backlight(uartbuf[my_uartlen+2]);
                                my_uartlen+=2;
                                break;
                    }  /* switch */

                } /* if c=27 */
                
                if (c==28) {                    
                    if (buflen<2) continue;
                    command=uartbuf[my_uartlen+1];
                    lcd_command (command);
                    break;
                } /* if c=28 */
                
               if (c==29) {                    
                    if (buflen<2) continue;
                    command=uartbuf[my_uartlen+1];
                    lcd_data (command);
                    break;
                } /* if c=29 */

                    
            } /* while */
            } /* if */
     }
    

}
