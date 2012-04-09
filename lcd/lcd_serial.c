/****************************************************************************
 Title	:   HD44780U LCD library
 Author:    Peter Fleury <pfleury@gmx.ch>  http://jump.to/fleury
 File:	    $Id: lcd.c,v 1.14.2.1 2006/01/29 12:16:41 peter Exp $
 Software:  AVR-GCC 3.3 
 Target:    any AVR device, memory mapped mode only for AT90S4414/8515/Mega

 DESCRIPTION
       Basic routines for interfacing a HD44780U-based text lcd display

       Originally based on Volker Oth's lcd library,
       changed lcd_init(), added additional constants for lcd_command(),
       added 4-bit I/O mode, improved and optimized code.
       
       Memory mapped mode compatible with Kanda STK200, but supports also
       generation of R/W signal through A8 address line.

 USAGE
       See the C include lcd.h file for a description of each function
       
*****************************************************************************/
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "lcd_serial.h"
#include "uart.h" 

#include "bitops.h"

#define MAX_LCD 4

/* pin 1 : gnd
   pin 2 : +5v
   pin 3 : sio
   pin 4 : strobe
   pin 5 : clock 
   */

volatile uint8_t *lcd_p_strobe[MAX_LCD];         //  
volatile uint8_t *lcd_p_clock[MAX_LCD];       //  
volatile uint8_t *lcd_p_io[MAX_LCD];         //  
volatile uint8_t *lcd_pin_io[MAX_LCD];         //  
volatile uint8_t *lcd_ddr_io[MAX_LCD];         //  



uint8_t lcd_b_strobe[MAX_LCD];
uint8_t lcd_b_clock[MAX_LCD];
uint8_t lcd_b_io[MAX_LCD];

struct lcdinfo {
	uint8_t width;   // aantal tekens op 1 regel
	uint8_t lines;  // aantal regels
	uint8_t setup; 	// bit 0: 0: 4bit, 1: 8bit
					// bit 1: 1: HD47780
					// bit 2: 1: KS0073
	uint8_t disp_length; /** number of visible lines of the display */
	uint8_t line_length; /** visibles characters per line of the display */
	uint8_t start_line1; /** DDRAM address of first char of line 1 */
	uint8_t start_line2; /** DDRAM address of first char of line 1 */	
	uint8_t start_line3; /** DDRAM address of first char of line 1 */
	uint8_t start_line4; /** DDRAM address of first char of line 1 */
	uint8_t wrap_lines;  /** 0: no wrap, 1: wrap at end of visibile line */
};

struct lcdinfo lcdinfos[MAX_LCD];

#define lcd_e_delay()   __asm__ __volatile__( "rjmp 1f\n 1:" );



/*************************************************************************
 delay loop for small accurate delays: 16-bit counter, 4 cycles/loop
*************************************************************************/
static inline void _delayFourCycles(unsigned long int __count)
{
    if ( __count == 0 )    
        __asm__ __volatile__( "rjmp 1f\n 1:" );    // 2 cycles
    else
        __asm__ __volatile__ (
    	    "1: sbiw %0,1" "\n\t"                  
    	    "brne 1b"                              // 4 cycles/loop
    	    : "=w" (__count)
    	    : "0" (__count)
    	   );
}
 

/************************************************************************* 
delay for a minimum of <us> microseconds
the number of loops is calculated at compile-time from MCU clock frequency
*************************************************************************/
#define delay(us)  _delayFourCycles( ( ( (unsigned long) XTAL >>2)*us) >> 10 )

 



/*************************************************************************
Low-level function to write byte to LCD controller
Input:    data   byte to write to LCD
          rs     1: write data    
                 0: write instruction
Returns:  none
*************************************************************************/

static void lcd_write(uint8_t chip, uint8_t data, uint8_t rs) 
{
    volatile uint8_t *p_strobe, *p_clock, *p_io;    	
    uint8_t b_strobe, b_clock, b_io; 	   
	uint8_t startbyte,i;
	
    p_strobe=lcd_p_strobe[chip];
	p_clock=lcd_p_clock[chip];
	p_io=lcd_p_io[chip];
	
    b_strobe=lcd_b_strobe[chip];
    b_clock=lcd_b_clock[chip];    
	b_io=lcd_b_io[chip];    
	
	bit_clr	(*p_strobe, b_strobe);
	
	startbyte=0xf8 | rs<<1 ;	
	for (i=0; i<8; i++) {
		bit_clr	(*p_clock, b_clock);

		if ((startbyte & 0x80)==0x80) 
			bit_set (*p_io, b_io);
		else 
			bit_clr (*p_io, b_io);
		bit_set	(*p_clock, b_clock);	
		startbyte=startbyte<<1;			
		}

	
	for (i=0; i<8; i++) {		
		bit_clr	(*p_clock, b_clock);		
		if ((data & 0x80)==0x80) 
			bit_set (*p_io, b_io);
		else 
			bit_clr (*p_io, b_io);
		bit_set	(*p_clock, b_clock);
		data=data<<1;		
		}
	
	bit_set	(*p_strobe, b_strobe);
}


/*************************************************************************
Low-level function to read byte from LCD controller
Input:    rs     1: read data    
                 0: read busy flag / address counter
Returns:  byte read from LCD controller
*************************************************************************/

static uint8_t lcd_read(uint8_t chip, uint8_t rs) 
{
	volatile uint8_t *p_strobe, *p_clock, *p_io, *ddr_io, *pin_io;    	
    uint8_t b_strobe, b_clock, b_io; 	   
	uint8_t data;
	uint8_t startbyte,i;
	
    p_strobe=lcd_p_strobe[chip];
	p_clock=lcd_p_clock[chip];
	p_io=lcd_p_io[chip];
	ddr_io=lcd_ddr_io[chip];
	pin_io=lcd_pin_io[chip];
	
    b_strobe=lcd_b_strobe[chip];
    b_clock=lcd_b_clock[chip];    
	b_io=lcd_b_io[chip];    
	startbyte=0xfc | rs<<1 ;	
	for (i=0; i<8; i++) {
		bit_clr	(*p_clock, b_clock);		
		if ((startbyte & 0x80)==0x80) 
			bit_set (*p_io, b_io);
		else 
			bit_clr (*p_io, b_io);
		bit_set	(*p_clock, b_clock);
		startbyte=startbyte<<1;		
		}

	*ddr_io&=~b_io;	
	delay(2);
	
	for (i=0; i<8; i++) {		
		bit_clr	(*p_clock, b_clock);
		bit_set	(*p_clock, b_clock);
		if ( bit_get(*pin_io, b_io) ) data=data|1;		
		data=data<<1;		
		}
	
	*ddr_io|=b_io;
    return data;
}


/**************************************
loops while lcd is busy, returns address counter
*************************************************************************/
static uint8_t lcd_waitbusy(uint8_t chip)

{
    register uint8_t c;
    
    /* wait until busy flag is cleared */
    while ( (c=lcd_read(chip, 0)) & (1<<LCD_BUSY)) {}
    
    /* the address counter is updated 4us after the busy flag is cleared */
    delay(2);

    /* now read the address counter */
    return (lcd_read(chip, 0));  // return address counter
    
}/* lcd_waitbusy */


/*************************************************************************
Move cursor to the start of next line or to the first line if the cursor 
is already on the last line.
*************************************************************************/
static inline void lcd_newline(uint8_t chip, uint8_t pos)
{
    register uint8_t addressCounter;
	struct lcdinfo *l;
	uint8_t lines, disp_length;
	uint8_t  start_line1, start_line2, start_line3, start_line4;
	
	l=&lcdinfos[chip];

	lines=l->lines;
	disp_length=l->disp_length;
	start_line1=l->start_line1;		
	start_line2=l->start_line2;
	start_line3=l->start_line3;
	start_line4=l->start_line4;
	
	if (lines==1) addressCounter = start_line1;
	
	if (lines==2) {
		addressCounter=start_line1;
		if ((pos>start_line1) && (pos <= start_line1+disp_length)) 
			addressCounter=start_line2;		
	}
	if (lines==4) {
		addressCounter=start_line1;
		if ((pos>start_line1) && (pos <= start_line1+disp_length)) 
			addressCounter=start_line2;
		if ((pos>start_line2) && (pos <= start_line2+disp_length)) 
			addressCounter=start_line3;
		if ((pos>start_line3) && (pos <= start_line3+disp_length)) 
			addressCounter=start_line4;			
		if ((pos>start_line4) && (pos <= start_line4+disp_length)) 
			addressCounter=start_line1;			
	}
    lcd_command(chip, (1<<LCD_DDRAM)+addressCounter);

}/* lcd_newline */


/*
** PUBLIC FUNCTIONS 
*/



/*************************************************************************
Send LCD controller instruction command
Input:   instruction to send to LCD controller, see HD44780 data sheet
Returns: none
*************************************************************************/



/*************************************************************************
Send LCD controller instruction command
Input:   instruction to send to LCD controller, see HD44780 data sheet
Returns: none
*************************************************************************/
void lcd_command(uint8_t chip, uint8_t cmd)
{
    //lcd_waitbusy(chip);
    lcd_write(chip,cmd,0);
}


/*************************************************************************
Send data byte to LCD controller 
Input:   data to send to LCD controller, see HD44780 data sheet
Returns: none
*************************************************************************/
void lcd_data(uint8_t chip, uint8_t data)
{
    //lcd_waitbusy(chip);
    lcd_write(chip,data,1);
}



/*************************************************************************
Set cursor to specified position
Input:    x  horizontal position  (0: left most position)
          y  vertical position    (0: first line)
Returns:  none
*************************************************************************/
void lcd_gotoxy(uint8_t chip, uint8_t x, uint8_t y)
{
	struct lcdinfo *l;
	uint8_t lines, start_line1;
	
	
	l=&lcdinfos[chip];	
	lines=l->lines;
	start_line1=l->start_line1;
	if (lines==1) lcd_command(chip, (1<<LCD_DDRAM)+start_line1+x);
	if (lines==2) {
		if ( y==0 ) 
			lcd_command(chip, (1<<LCD_DDRAM)+l->start_line1+x);
		else
			lcd_command(chip, (1<<LCD_DDRAM)+l->start_line2+x);
		}
	
	if (lines==4) {	
		if ( y==0 )
			lcd_command(chip, (1<<LCD_DDRAM)+l->start_line1+x);
		else if ( y==1)
			lcd_command(chip, (1<<LCD_DDRAM)+l->start_line2+x);
		else if ( y==2)
			lcd_command(chip, (1<<LCD_DDRAM)+l->start_line3+x);
		else /* y==3 */
			lcd_command(chip, (1<<LCD_DDRAM)+l->start_line4+x);
	}

}/* lcd_gotoxy */


/*************************************************************************
*************************************************************************/
int lcd_getxy(uint8_t chip)
{
    //return lcd_waitbusy(chip);
	return 0;
}


/*************************************************************************
Clear display and set cursor to home position
*************************************************************************/
void lcd_clrscr(uint8_t chip)
{
    lcd_command(chip, 1<<LCD_CLR);
	delay(1000);
}


/*************************************************************************
Set cursor to home position
*************************************************************************/
void lcd_home(uint8_t chip)
{
    lcd_command(chip, 1<<LCD_HOME);
}


/*************************************************************************
Display character at current cursor position 
Input:    character to be displayed                                       
Returns:  none
*************************************************************************/
void lcd_putc(uint8_t chip, char c)
{
    uint8_t pos, disp_length, start_line1, lines;
	struct lcdinfo *l;

	
	if (lines==1) {
		if ( pos == start_line1+disp_length ) {
			lcd_write(chip, (1<<LCD_DDRAM)+start_line1,0);
		}
	}
	if (lines==2) {
		uint8_t  start_line2;
						
		start_line2=l->start_line2;

		if ( pos == start_line1+disp_length ) 
			lcd_write(chip, (1<<LCD_DDRAM)+start_line2,0);    
		else if ( pos == start_line2+disp_length )
			lcd_write(chip, (1<<LCD_DDRAM)+start_line1,0);		
	}
	if (lines==4) {
		uint8_t  start_line2, start_line3, start_line4;
			
		start_line2=l->start_line2;
		start_line3=l->start_line3;
		start_line4=l->start_line4;

		if ( pos == start_line1+disp_length ) 
			lcd_write(chip, (1<<LCD_DDRAM)+start_line2,0);    
		else if ( pos == start_line2+disp_length ) 
			lcd_write(chip, (1<<LCD_DDRAM)+start_line3,0);
		else if ( pos == start_line3+disp_length ) 
			lcd_write(chip, (1<<LCD_DDRAM)+start_line4,0);
		else if ( pos == start_line4+disp_length ) 
			lcd_write(chip, (1<<LCD_DDRAM)+start_line1,0);		
	
	}
	
    //lcd_waitbusy(chip);	
	lcd_write(chip, c, 1);
    
}/* lcd_putc */


/*************************************************************************
Display string without auto linefeed 
Input:    string to be displayed
Returns:  none
*************************************************************************/
void lcd_puts(uint8_t chip, const char *s)
/* print string on lcd (no auto linefeed) */
{
    register char c;

    while ( (c = *s++) ) {
        lcd_putc(chip, c);
    }

}/* lcd_puts */


/*************************************************************************
Display string from program memory without auto linefeed 
Input:     string from program memory be be displayed                                        
Returns:   none
*************************************************************************/
void lcd_puts_p(uint8_t chip, const char *progmem_s)
/* print string from program memory on lcd (no auto linefeed) */
{
    register char c;

    while ( (c = pgm_read_byte(progmem_s++)) ) {
        lcd_putc(chip, c);
    }

}/* lcd_puts_p */


/*************************************************************************
Initialize display and select type of cursor 
Input:    dispAttr LCD_DISP_OFF            display off
                   LCD_DISP_ON             display on, cursor off
                   LCD_DISP_ON_CURSOR      display on, cursor on
                   LCD_DISP_CURSOR_BLINK   display on, cursor on flashing
Returns:  none
*************************************************************************/



void lcd_setup (uint8_t chip,  uint8_t strobe, uint8_t clock, uint8_t io)

{
  uint8_t b_strobe, b_clock, b_io; 
 
 b_strobe=1<< (strobe  & P_BITMASK);
 lcd_b_strobe[chip]=b_strobe;
 b_clock=1<< (clock & P_BITMASK); 
 lcd_b_clock[chip]=b_clock;
 b_io=1<< (io & P_BITMASK);  
 lcd_b_io[chip]=b_io;
  
 strobe=strobe & P_PORTMASK;
 if (strobe==P_PORTA) {lcd_p_strobe[chip]=&PORTA; DDRA|=b_strobe;}
 if (strobe==P_PORTB) {lcd_p_strobe[chip]=&PORTB; DDRB|=b_strobe;}
 if (strobe==P_PORTC) {lcd_p_strobe[chip]=&PORTC; DDRC|=b_strobe;}
 if (strobe==P_PORTD) {lcd_p_strobe[chip]=&PORTD; DDRD|=b_strobe;}
 
 clock=clock & P_PORTMASK;
 if (clock==P_PORTA) {lcd_p_clock[chip]=&PORTA; DDRA|=b_clock;}
 if (clock==P_PORTB) {lcd_p_clock[chip]=&PORTB; DDRB|=b_clock;}
 if (clock==P_PORTC) {lcd_p_clock[chip]=&PORTC; DDRC|=b_clock;}
 if (clock==P_PORTD) {lcd_p_clock[chip]=&PORTD; DDRD|=b_clock;}
 
 io=io & P_PORTMASK;
 if (io==P_PORTA) {lcd_p_io[chip]=&PORTA; DDRA|=b_io; 
					lcd_pin_io[chip]=&PINA; lcd_ddr_io[chip]=&DDRA;}
 if (io==P_PORTB) {lcd_p_io[chip]=&PORTB; DDRB|=b_io; 
					lcd_pin_io[chip]=&PINB; lcd_ddr_io[chip]=&DDRB;}
 if (io==P_PORTC) {lcd_p_io[chip]=&PORTC; DDRC|=b_io; 
					lcd_pin_io[chip]=&PINB; lcd_ddr_io[chip]=&DDRC;}
 if (io==P_PORTD) {lcd_p_io[chip]=&PORTD; DDRD|=b_io; 
					lcd_pin_io[chip]=&PIND; lcd_ddr_io[chip]=&DDRD;}

}

    /* initial write to lcd is 8bit */

void lcd_setup_info (uint8_t chip, uint8_t display_type, uint8_t width, uint8_t height) 

{
 struct lcdinfo *l;
 
 l=&lcdinfos[chip]; 
 
 l->setup=display_type| 0;     // 4bit
 l->width=width;
 l->lines=height;

 l->disp_length=20;
 l->line_length=0x40;
 l->start_line1=0x00;
 l->start_line2=0x40;
 l->start_line3=0x14;
 l->start_line4=0x54; 
}
	

void lcd_init (uint8_t chip, uint8_t dispAttr) 
{
	//lcd_command (chip, LCD_DISP_ON_CURSOR_BLINK);	
	    
    //lcd_clrscr (chip);                           /* display clear                */ 
   // lcd_command (chip,LCD_MODE_DEFAULT);          /* set entry mode               */
   
    uart_puts("lcd_init\r\n");
	lcd_command (chip, dispAttr);
}
