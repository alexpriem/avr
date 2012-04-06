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
#include "lcd.h"

#include "bitops.h"



volatile uint8_t *lcd_p_rs;         //  
volatile uint8_t *lcd_p_rw;         //  
volatile uint8_t *lcd_p_enable;       //  
volatile uint8_t *lcd_p_backlight;       //  

volatile uint8_t *lcd_p_db0;         //  
volatile uint8_t *lcd_p_db1;         //  
volatile uint8_t *lcd_p_db2;         //  
volatile uint8_t *lcd_p_db3;         //  

volatile uint8_t *lcd_pin_db0;         //  
volatile uint8_t *lcd_pin_db1;         //  
volatile uint8_t *lcd_pin_db2;         //  
volatile uint8_t *lcd_pin_db3;         //  

volatile uint8_t *lcd_ddr_db0;         //  
volatile uint8_t *lcd_ddr_db1;         //  
volatile uint8_t *lcd_ddr_db2;         //  
volatile uint8_t *lcd_ddr_db3;         //  


uint8_t lcd_b_rs;
uint8_t lcd_b_rw;
uint8_t lcd_b_enable;
uint8_t lcd_b_backlight;

uint8_t lcd_b_db0;
uint8_t lcd_b_db1;
uint8_t lcd_b_db2;
uint8_t lcd_b_db3;



#define lcd_e_delay()   __asm__ __volatile__( "rjmp 1f\n 1:" );


#if LCD_LINES==1
#define LCD_FUNCTION_DEFAULT    LCD_FUNCTION_4BIT_1LINE 
#else
#define LCD_FUNCTION_DEFAULT    LCD_FUNCTION_4BIT_2LINES 
#endif 

#define LCD_FUNCTION_DEFAULT  LCD_FUNCTION_4BIT_2LINES


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

 

/* toggle Enable Pin to initiate write */
static void lcd_e_toggle(void)
{
    bit_set (*lcd_p_enable, lcd_b_enable);
    //bit_set (*lcd_p_backlight, lcd_b_backlight);
	lcd_e_delay();
    bit_clr (*lcd_p_enable, lcd_b_enable);
	//bit_clr (*lcd_p_backlight, lcd_b_backlight);
}



/*************************************************************************
Low-level function to write byte to LCD controller
Input:    data   byte to write to LCD
          rs     1: write data    
                 0: write instruction
Returns:  none
*************************************************************************/

static void lcd_write(uint8_t data, uint8_t rs) 
{
    

    if (rs) {   /* write data        (RS=1, RW=0) */
	   bit_set (*lcd_p_rs, lcd_b_rs);      
    } else {    /* write instruction (RS=0, RW=0) */
       bit_clr (*lcd_p_rs, lcd_b_rs);
    }
    bit_clr (*lcd_p_rw, lcd_b_rw);
        /* configure data pins as output */
    *lcd_ddr_db0|=lcd_b_db0;
	*lcd_ddr_db1|=lcd_b_db1;
	*lcd_ddr_db2|=lcd_b_db2;
	*lcd_ddr_db3|=lcd_b_db3;
        /* output high nibble first */
	bit_clr (*lcd_p_db0, lcd_b_db0);
	bit_clr (*lcd_p_db1, lcd_b_db1);
	bit_clr (*lcd_p_db2, lcd_b_db2);
	bit_clr (*lcd_p_db3, lcd_b_db3);
	if(data & 0x80) bit_set (*lcd_p_db3, lcd_b_db3);
	if(data & 0x40) bit_set (*lcd_p_db2, lcd_b_db2);
	if(data & 0x20) bit_set (*lcd_p_db1, lcd_b_db1);
	if(data & 0x10) bit_set (*lcd_p_db0, lcd_b_db0);
	lcd_e_toggle();
	
	/* output low nibble */        
	bit_clr (*lcd_p_db0, lcd_b_db0);
	bit_clr (*lcd_p_db1, lcd_b_db1);
	bit_clr (*lcd_p_db2, lcd_b_db2);
	bit_clr (*lcd_p_db3, lcd_b_db3);
	if(data & 0x08) bit_set (*lcd_p_db3, lcd_b_db3);
	if(data & 0x04) bit_set (*lcd_p_db2, lcd_b_db2);
	if(data & 0x02) bit_set (*lcd_p_db1, lcd_b_db1);
	if(data & 0x01) bit_set (*lcd_p_db0, lcd_b_db0);
	lcd_e_toggle();        
	
	/* all data pins high (inactive) */
	bit_set (*lcd_p_db0, lcd_b_db0);
	bit_set (*lcd_p_db1, lcd_b_db1);
	bit_set (*lcd_p_db2, lcd_b_db2);
	bit_set (*lcd_p_db3, lcd_b_db3);
}


/*************************************************************************
Low-level function to read byte from LCD controller
Input:    rs     1: read data    
                 0: read busy flag / address counter
Returns:  byte read from LCD controller
*************************************************************************/

static uint8_t lcd_read(uint8_t rs) 
{
    uint8_t data;    
    
    if (rs)
		bit_set (*lcd_p_rs, lcd_b_rs);                            /* RS=1: read data      */
    else
		bit_clr (*lcd_p_rs, lcd_b_rs);      					/* RS=0: read busy flag */              
    bit_set (*lcd_p_rw, lcd_b_rw);                           /* RW=1  read mode      */
    
        /* configure data pins as input */
    *lcd_ddr_db0&=~lcd_b_db0;
	*lcd_ddr_db1&=~lcd_b_db1;
	*lcd_ddr_db2&=~lcd_b_db2;
	*lcd_ddr_db3&=~lcd_b_db3;
                
	/* read high nibble first */
	bit_set (*lcd_p_enable, lcd_b_enable);
	//bit_set (*lcd_p_backlight, lcd_b_backlight);
	lcd_e_delay();        
	data = 0;
	if ( bit_get(*lcd_pin_db0, lcd_b_db0) ) data |= 0x10;
	if ( bit_get(*lcd_pin_db1, lcd_b_db1) ) data |= 0x20;
	if ( bit_get(*lcd_pin_db2, lcd_b_db2) ) data |= 0x40;
	if ( bit_get(*lcd_pin_db3, lcd_b_db3) ) data |= 0x80;		

	bit_clr (*lcd_p_enable, lcd_b_enable);
	bit_clr (*lcd_p_backlight, lcd_b_backlight);
	lcd_e_delay();                       /* Enable 500ns low       */

	/* read low nibble */    
	bit_set (*lcd_p_enable, lcd_b_enable);
	//bit_set (*lcd_p_backlight, lcd_b_backlight);
	lcd_e_delay();
	if ( bit_get(*lcd_pin_db0, lcd_b_db0) ) data |= 0x01;
	if ( bit_get(*lcd_pin_db1, lcd_b_db1) ) data |= 0x02;
	if ( bit_get(*lcd_pin_db2, lcd_b_db2) ) data |= 0x04;
	if ( bit_get(*lcd_pin_db3, lcd_b_db3) ) data |= 0x08;		
	bit_clr (*lcd_p_enable, lcd_b_enable);
    //bit_clr (*lcd_p_backlight, lcd_b_backlight);
    return data;
}


/*************************************************************************
loops while lcd is busy, returns address counter
*************************************************************************/
static uint8_t lcd_waitbusy(void)

{
    register uint8_t c;
    
    /* wait until busy flag is cleared */
    while ( (c=lcd_read(0)) & (1<<LCD_BUSY)) {}
    
    /* the address counter is updated 4us after the busy flag is cleared */
    delay(2);

    /* now read the address counter */
    return (lcd_read(0));  // return address counter
    
}/* lcd_waitbusy */


/*************************************************************************
Move cursor to the start of next line or to the first line if the cursor 
is already on the last line.
*************************************************************************/
static inline void lcd_newline(uint8_t pos)
{
    register uint8_t addressCounter;

#if LCD_LINES==1
    addressCounter = 0;
#endif
#if LCD_LINES==2
    if ( pos < (LCD_START_LINE2) )
        addressCounter = LCD_START_LINE2;
    else
        addressCounter = LCD_START_LINE1;
#endif
#if LCD_LINES==4
#if KS0073_4LINES_MODE
    if ( pos < LCD_START_LINE2 )
        addressCounter = LCD_START_LINE2;
    else if ( (pos >= LCD_START_LINE2) && (pos < LCD_START_LINE3) )
        addressCounter = LCD_START_LINE3;
    else if ( (pos >= LCD_START_LINE3) && (pos < LCD_START_LINE4) )
        addressCounter = LCD_START_LINE4;
    else 
        addressCounter = LCD_START_LINE1;
#else
    if ( pos < LCD_START_LINE3 )
        addressCounter = LCD_START_LINE2;
    else if ( (pos >= LCD_START_LINE2) && (pos < LCD_START_LINE4) )
        addressCounter = LCD_START_LINE3;
    else if ( (pos >= LCD_START_LINE3) && (pos < LCD_START_LINE2) )
        addressCounter = LCD_START_LINE4;
    else 
        addressCounter = LCD_START_LINE1;
#endif
#endif
    lcd_command((1<<LCD_DDRAM)+addressCounter);

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
void lcd_command(uint8_t cmd)
{
    lcd_waitbusy();
    lcd_write(cmd,0);
}


/*************************************************************************
Send data byte to LCD controller 
Input:   data to send to LCD controller, see HD44780 data sheet
Returns: none
*************************************************************************/
void lcd_data(uint8_t data)
{
    lcd_waitbusy();
    lcd_write(data,1);
}



/*************************************************************************
Set cursor to specified position
Input:    x  horizontal position  (0: left most position)
          y  vertical position    (0: first line)
Returns:  none
*************************************************************************/
void lcd_gotoxy(uint8_t x, uint8_t y)
{
#if LCD_LINES==1
    lcd_command((1<<LCD_DDRAM)+LCD_START_LINE1+x);
#endif
#if LCD_LINES==2
    if ( y==0 ) 
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE1+x);
    else
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE2+x);
#endif
#if LCD_LINES==4
    if ( y==0 )
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE1+x);
    else if ( y==1)
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE2+x);
    else if ( y==2)
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE3+x);
    else /* y==3 */
        lcd_command((1<<LCD_DDRAM)+LCD_START_LINE4+x);
#endif

}/* lcd_gotoxy */


/*************************************************************************
*************************************************************************/
int lcd_getxy(void)
{
    return lcd_waitbusy();
}


/*************************************************************************
Clear display and set cursor to home position
*************************************************************************/
void lcd_clrscr(void)
{
    lcd_command(1<<LCD_CLR);
}


/*************************************************************************
Set cursor to home position
*************************************************************************/
void lcd_home(void)
{
    lcd_command(1<<LCD_HOME);
}


/*************************************************************************
Display character at current cursor position 
Input:    character to be displayed                                       
Returns:  none
*************************************************************************/
void lcd_putc(char c)
{
    uint8_t pos;


    pos = lcd_waitbusy();   // read busy-flag and address counter
    if (c=='\n')
    {
        lcd_newline(pos);
    }
    else
    {
#if LCD_WRAP_LINES==1
#if LCD_LINES==1
        if ( pos == LCD_START_LINE1+LCD_DISP_LENGTH ) {
            lcd_write((1<<LCD_DDRAM)+LCD_START_LINE1,0);
        }
#elif LCD_LINES==2
        if ( pos == LCD_START_LINE1+LCD_DISP_LENGTH ) {
            lcd_write((1<<LCD_DDRAM)+LCD_START_LINE2,0);    
        }else if ( pos == LCD_START_LINE2+LCD_DISP_LENGTH ){
            lcd_write((1<<LCD_DDRAM)+LCD_START_LINE1,0);
        }
#elif LCD_LINES==4
        if ( pos == LCD_START_LINE1+LCD_DISP_LENGTH ) {
            lcd_write((1<<LCD_DDRAM)+LCD_START_LINE2,0);    
        }else if ( pos == LCD_START_LINE2+LCD_DISP_LENGTH ) {
            lcd_write((1<<LCD_DDRAM)+LCD_START_LINE3,0);
        }else if ( pos == LCD_START_LINE3+LCD_DISP_LENGTH ) {
            lcd_write((1<<LCD_DDRAM)+LCD_START_LINE4,0);
        }else if ( pos == LCD_START_LINE4+LCD_DISP_LENGTH ) {
            lcd_write((1<<LCD_DDRAM)+LCD_START_LINE1,0);
        }
#endif
        lcd_waitbusy();
#endif
        lcd_write(c, 1);
    }

}/* lcd_putc */


/*************************************************************************
Display string without auto linefeed 
Input:    string to be displayed
Returns:  none
*************************************************************************/
void lcd_puts(const char *s)
/* print string on lcd (no auto linefeed) */
{
    register char c;

    while ( (c = *s++) ) {
        lcd_putc(c);
    }

}/* lcd_puts */


/*************************************************************************
Display string from program memory without auto linefeed 
Input:     string from program memory be be displayed                                        
Returns:   none
*************************************************************************/
void lcd_puts_p(const char *progmem_s)
/* print string from program memory on lcd (no auto linefeed) */
{
    register char c;

    while ( (c = pgm_read_byte(progmem_s++)) ) {
        lcd_putc(c);
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



void lcd_setup (uint8_t rs, uint8_t rw, uint8_t enable, uint8_t backlight,
				uint8_t db0, uint8_t db1, uint8_t db2, uint8_t db3 ) 
{
 
 lcd_b_rs=1<< (rs  & P_BITMASK);
 lcd_b_rw=1<< (rw & P_BITMASK); 
 lcd_b_enable=1<< (enable & P_BITMASK);  
 lcd_b_backlight=1<< (backlight & P_BITMASK);  
 lcd_b_db0=1<< (db0  & P_BITMASK);
 lcd_b_db1=1<< (db1  & P_BITMASK);
 lcd_b_db2=1<< (db2  & P_BITMASK);
 lcd_b_db3=1<< (db3  & P_BITMASK);
 
 rs=rs & P_PORTMASK;
 if (rs==P_PORTA) {lcd_p_rs=&PORTA; DDRA|=lcd_b_rs;}
 if (rs==P_PORTB) {lcd_p_rs=&PORTB; DDRB|=lcd_b_rs;}
 if (rs==P_PORTC) {lcd_p_rs=&PORTC; DDRC|=lcd_b_rs;}
 if (rs==P_PORTD) {lcd_p_rs=&PORTD; DDRD|=lcd_b_rs;}

 rw=rw & P_PORTMASK;
 if (rw==P_PORTA) {lcd_p_rw=&PORTA; DDRA|=lcd_b_rw;}
 if (rw==P_PORTB) {lcd_p_rw=&PORTB; DDRB|=lcd_b_rw;}
 if (rw==P_PORTC) {lcd_p_rw=&PORTC; DDRC|=lcd_b_rw;}
 if (rw==P_PORTD) {lcd_p_rw=&PORTD; DDRD|=lcd_b_rw;}
 
 enable=enable & P_PORTMASK;
 if (enable==P_PORTA) {lcd_p_enable=&PORTA; DDRA|=lcd_b_enable;}
 if (enable==P_PORTB) {lcd_p_enable=&PORTB; DDRB|=lcd_b_enable;}
 if (enable==P_PORTC) {lcd_p_enable=&PORTC; DDRC|=lcd_b_enable;}
 if (enable==P_PORTD) {lcd_p_enable=&PORTD; DDRD|=lcd_b_enable;}

 backlight=backlight & P_PORTMASK;
 if (backlight==P_PORTA) {lcd_p_backlight=&PORTA; DDRA|=lcd_b_backlight;}
 if (backlight==P_PORTB) {lcd_p_backlight=&PORTB; DDRB|=lcd_b_backlight;}
 if (backlight==P_PORTC) {lcd_p_backlight=&PORTC; DDRC|=lcd_b_backlight;}
 if (backlight==P_PORTD) {lcd_p_backlight=&PORTD; DDRD|=lcd_b_backlight;}

 db0=db0 & P_PORTMASK;
 if (db0==P_PORTA) {lcd_p_db0=&PORTA; lcd_pin_db0=&PINA; lcd_ddr_db0=&DDRA; DDRA|=lcd_b_db0; }
 if (db0==P_PORTB) {lcd_p_db0=&PORTB; lcd_pin_db0=&PINB; lcd_ddr_db0=&DDRB; DDRB|=lcd_b_db0;}
 if (db0==P_PORTC) {lcd_p_db0=&PORTC; lcd_pin_db0=&PINC; lcd_ddr_db0=&DDRC; DDRC|=lcd_b_db0;}
 if (db0==P_PORTD) {lcd_p_db0=&PORTD; lcd_pin_db0=&PIND; lcd_ddr_db0=&DDRD; DDRD|=lcd_b_db0;}

 db1=db1 & P_PORTMASK;
 if (db1==P_PORTA) {lcd_p_db1=&PORTA; lcd_pin_db1=&PINA; lcd_ddr_db1=&DDRA; DDRA|=lcd_b_db1;}
 if (db1==P_PORTB) {lcd_p_db1=&PORTB; lcd_pin_db1=&PINB; lcd_ddr_db1=&DDRB; DDRB|=lcd_b_db1;}
 if (db1==P_PORTC) {lcd_p_db1=&PORTC; lcd_pin_db1=&PINC; lcd_ddr_db1=&DDRC; DDRC|=lcd_b_db1;}
 if (db1==P_PORTD) {lcd_p_db1=&PORTD; lcd_pin_db1=&PIND; lcd_ddr_db1=&DDRD; DDRD|=lcd_b_db1;}

 db2=db2 & P_PORTMASK;
 if (db2==P_PORTA) {lcd_p_db2=&PORTA; lcd_pin_db2=&PINA; lcd_ddr_db2=&DDRA; DDRA|=lcd_b_db2;}
 if (db2==P_PORTB) {lcd_p_db2=&PORTB; lcd_pin_db2=&PINB; lcd_ddr_db2=&DDRB; DDRB|=lcd_b_db2;}
 if (db2==P_PORTC) {lcd_p_db2=&PORTC; lcd_pin_db2=&PINC; lcd_ddr_db2=&DDRC; DDRC|=lcd_b_db2;}
 if (db2==P_PORTD) {lcd_p_db2=&PORTD; lcd_pin_db2=&PIND; lcd_ddr_db2=&DDRD; DDRD|=lcd_b_db2;}

 db3=db3 & P_PORTMASK;
 if (db3==P_PORTA) {lcd_p_db3=&PORTA; lcd_pin_db3=&PINA; lcd_ddr_db3=&DDRA; DDRA|=lcd_b_db3;}
 if (db3==P_PORTB) {lcd_p_db3=&PORTB; lcd_pin_db3=&PINB; lcd_ddr_db3=&DDRB; DDRB|=lcd_b_db3;}
 if (db3==P_PORTC) {lcd_p_db3=&PORTC; lcd_pin_db3=&PINC; lcd_ddr_db3=&DDRC; DDRC|=lcd_b_db3;}
 if (db3==P_PORTD) {lcd_p_db3=&PORTD; lcd_pin_db3=&PIND; lcd_ddr_db3=&DDRD; DDRD|=lcd_b_db3;}
}

    /* initial write to lcd is 8bit */


void lcd_init (uint8_t dispAttr) 
{
	bit_set (*lcd_p_db0, lcd_b_db0);
	bit_set (*lcd_p_db1, lcd_b_db1);

    lcd_e_toggle();
    delay(4992);         /* delay, busy flag can't be checked here */
   
    /* repeat last command */ 
    lcd_e_toggle();      
    delay(64);           /* delay, busy flag can't be checked here */
    
    /* repeat last command a third time */
    lcd_e_toggle();      
    delay(64);           /* delay, busy flag can't be checked here */

    /* now configure for 4bit mode */    
	bit_clr (*lcd_p_db0, lcd_b_db0);
    
	lcd_e_toggle();
    delay(64);           /* some displays need this additional delay */

	lcd_command (LCD_DISP_ON_CURSOR_BLINK);	
	
    lcd_command (LCD_FUNCTION_DEFAULT);      /* function set: display lines  */    
    lcd_clrscr ();                           /* display clear                */ 
    lcd_command (LCD_MODE_DEFAULT);          /* set entry mode               */
	//lcd_backlight_on ();
	lcd_command (dispAttr);
}
