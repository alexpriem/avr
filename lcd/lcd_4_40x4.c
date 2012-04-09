/****************************************************************************
 Title	:   HD44780U LCD library
 Author:    Peter Fleury <pfleury@gmx.ch>  http://jump.to/fleury
 File:	    $Id: lcd.c,v 1.14.2.1 2006/01/29 12:16:41 peter Exp $
 Software:  AVR-GCC 3.3 
 Target:    any AVR device

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
#include "lcd_4_40x4.h"

#include "bitops.h"

#define MAX_LCD 4

#define BUFLINES 20

char txtbuf [BUFLINES*40];


volatile uint8_t *lcd_p_rs[MAX_LCD];         //  
volatile uint8_t *lcd_p_rw[MAX_LCD];         //  
volatile uint8_t *lcd_p_enable1[MAX_LCD];       //  
volatile uint8_t *lcd_p_enable2[MAX_LCD];       //  

volatile uint8_t *lcd_p_db0[MAX_LCD];         //  
volatile uint8_t *lcd_p_db1[MAX_LCD];         //  
volatile uint8_t *lcd_p_db2[MAX_LCD];         //  
volatile uint8_t *lcd_p_db3[MAX_LCD];         //  

volatile uint8_t *lcd_pin_db0[MAX_LCD];         //  
volatile uint8_t *lcd_pin_db1[MAX_LCD];         //  
volatile uint8_t *lcd_pin_db2[MAX_LCD];         //  
volatile uint8_t *lcd_pin_db3[MAX_LCD];         //  

volatile uint8_t *lcd_ddr_db0[MAX_LCD];         //  
volatile uint8_t *lcd_ddr_db1[MAX_LCD];         //  
volatile uint8_t *lcd_ddr_db2[MAX_LCD];         //  
volatile uint8_t *lcd_ddr_db3[MAX_LCD];         //  



uint8_t lcd_b_rs[MAX_LCD];
uint8_t lcd_b_rw[MAX_LCD];
uint8_t lcd_b_enable1[MAX_LCD];
uint8_t lcd_b_enable2[MAX_LCD];

uint8_t lcd_b_db0[MAX_LCD];
uint8_t lcd_b_db1[MAX_LCD];
uint8_t lcd_b_db2[MAX_LCD];
uint8_t lcd_b_db3[MAX_LCD];



struct lcdinfo {
	uint8_t x;   // x-pos
	uint8_t y;   // y-pos
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

 

/* toggle Enable Pin to initiate write */
static void lcd_e_toggle(uint8_t chip, uint8_t lcd)
{
    volatile uint8_t *p_enable;
    uint8_t b_enable; 
	
	if (lcd==1) {
		p_enable=lcd_p_enable1[chip]; 
		b_enable=lcd_b_enable1[chip];
	} else {	
		p_enable=lcd_p_enable2[chip]; 
		b_enable=lcd_b_enable2[chip];
	}
	
	bit_set (*p_enable, b_enable);
	lcd_e_delay();
	bit_clr (*p_enable, b_enable);
	
	
}



/*************************************************************************
Low-level function to write byte to LCD controller
Input:    data   byte to write to LCD
          rs     1: write data    
                 0: write instruction
Returns:  none
*************************************************************************/

static void lcd_write(uint8_t chip, uint8_t lcd, uint8_t data, uint8_t rs) 
{
    volatile uint8_t *p_db0, *p_db1, *p_db2, *p_db3, *p_rs, *p_rw;
    volatile uint8_t *ddr_db0, *ddr_db1, *ddr_db2, *ddr_db3;
	uint8_t b_db0, b_db1, b_db2, b_db3;
    uint8_t b_rs, b_rw; 
    	
	p_db0=lcd_p_db0[chip];
    p_db1=lcd_p_db1[chip];
    p_db2=lcd_p_db2[chip];
    p_db3=lcd_p_db3[chip];

    b_db0=lcd_b_db0[chip];
    b_db1=lcd_b_db1[chip];
    b_db2=lcd_b_db2[chip];
    b_db3=lcd_b_db3[chip];	

    ddr_db0=lcd_ddr_db0[chip];
    ddr_db1=lcd_ddr_db1[chip];
    ddr_db2=lcd_ddr_db2[chip];
    ddr_db3=lcd_ddr_db3[chip];
	
	p_rs=lcd_p_rs[chip];
    p_rw=lcd_p_rw[chip];
	
    b_rs=lcd_b_rs[chip];
    b_rw=lcd_b_rw[chip];    

    if (rs) {   /* write data        (RS=1, RW=0) */
	   bit_set (*p_rs, b_rs);      
    } else {    /* write instruction (RS=0, RW=0) */
       bit_clr (*p_rs, b_rs);
    }
    bit_clr (*p_rw, b_rw);
        /* configure data pins as output */
    *ddr_db0|=b_db0;
	*ddr_db1|=b_db1;
	*ddr_db2|=b_db2;
	*ddr_db3|=b_db3;
        /* output high nibble first */
	bit_clr (*p_db0, b_db0);
	bit_clr (*p_db1, b_db1);
	bit_clr (*p_db2, b_db2);
	bit_clr (*p_db3, b_db3);
	if(data & 0x80) bit_set (*p_db3, b_db3);
	if(data & 0x40) bit_set (*p_db2, b_db2);
	if(data & 0x20) bit_set (*p_db1, b_db1);
	if(data & 0x10) bit_set (*p_db0, b_db0);
	lcd_e_toggle(chip, lcd);
	
	/* output low nibble */        
	bit_clr (*p_db0, b_db0);
	bit_clr (*p_db1, b_db1);
	bit_clr (*p_db2, b_db2);
	bit_clr (*p_db3, b_db3);
	if(data & 0x08) bit_set (*p_db3, b_db3);
	if(data & 0x04) bit_set (*p_db2, b_db2);
	if(data & 0x02) bit_set (*p_db1, b_db1);
	if(data & 0x01) bit_set (*p_db0, b_db0);
	lcd_e_toggle(chip, lcd);        
	
	/* all data pins high (inactive) */
	bit_set (*p_db0, b_db0);
	bit_set (*p_db1, b_db1);
	bit_set (*p_db2, b_db2);
	bit_set (*p_db3, b_db3);
}


/*************************************************************************
Low-level function to read byte from LCD controller
Input:    rs     1: read data    
                 0: read busy flag / address counter
Returns:  byte read from LCD controller
*************************************************************************/

static uint8_t lcd_read(uint8_t chip, uint8_t lcd, uint8_t rs) 
{
	volatile uint8_t *p_db0, *p_db1, *p_db2, *p_db3, *p_rs, *p_rw, *p_enable;	
	volatile uint8_t *pin_db0, *pin_db1, *pin_db2, *pin_db3;
    volatile uint8_t *ddr_db0, *ddr_db1, *ddr_db2, *ddr_db3;
    uint8_t b_db0, b_db1, b_db2, b_db3;
	uint8_t b_rs, b_rw, b_enable, data;	

	p_db0=lcd_p_db0[chip];
    p_db1=lcd_p_db1[chip];
    p_db2=lcd_p_db2[chip];
    p_db3=lcd_p_db3[chip];

    b_db0=lcd_b_db0[chip];
    b_db1=lcd_b_db1[chip];
    b_db2=lcd_b_db2[chip];
    b_db3=lcd_b_db3[chip];		
	
	ddr_db0=lcd_ddr_db0[chip];
    ddr_db1=lcd_ddr_db1[chip];
    ddr_db2=lcd_ddr_db2[chip];
    ddr_db3=lcd_ddr_db3[chip];
	
	pin_db0=lcd_pin_db0[chip];
    pin_db1=lcd_pin_db1[chip];
    pin_db2=lcd_pin_db2[chip];
    pin_db3=lcd_pin_db3[chip];

    if (lcd==1){
		p_enable=lcd_p_enable1[chip];	
		b_enable=lcd_b_enable1[chip];
		} else {
		p_enable=lcd_p_enable2[chip];	
		b_enable=lcd_b_enable2[chip];		
	}
    p_rs=lcd_p_rs[chip];	
    b_rs=lcd_b_rs[chip];
    p_rw=lcd_p_rw[chip];	
    b_rw=lcd_b_rw[chip];
	
    if (rs)
		bit_set (*p_rs, b_rs);                            /* RS=1: read data      */
    else
		bit_clr (*p_rs, b_rs);      					/* RS=0: read busy flag */              
    bit_set (*p_rw, b_rw);                           /* RW=1  read mode      */
    
        /* configure data pins as input */
    *ddr_db0&=~b_db0;
	*ddr_db1&=~b_db1;
	*ddr_db2&=~b_db2;
	*ddr_db3&=~b_db3;
                
	/* read high nibble first */
	bit_set (*p_enable, b_enable);
	lcd_e_delay();        
	data = 0;
	if ( bit_get(*pin_db0, b_db0) ) data |= 0x10;
	if ( bit_get(*pin_db1, b_db1) ) data |= 0x20;
	if ( bit_get(*pin_db2, b_db2) ) data |= 0x40;
	if ( bit_get(*pin_db3, b_db3) ) data |= 0x80;		

	bit_clr (*p_enable, b_enable);
	lcd_e_delay();                       /* Enable 500ns low       */

	/* read low nibble */    
	bit_set (*p_enable, b_enable);
	lcd_e_delay();
	if ( bit_get(*pin_db0, b_db0) ) data |= 0x01;
	if ( bit_get(*pin_db1, b_db1) ) data |= 0x02;
	if ( bit_get(*pin_db2, b_db2) ) data |= 0x04;
	if ( bit_get(*pin_db3, b_db3) ) data |= 0x08;		
	bit_clr (*p_enable, b_enable);
    
    return data;
}


/*************************************************************************
loops while lcd is busy, returns address counter
*************************************************************************/
static uint8_t lcd_waitbusy(uint8_t chip, uint8_t lcd)

{
    register uint8_t c;
    
    /* wait until busy flag is cleared */
    while ( (c=lcd_read(chip,lcd, 0)) & (1<<LCD_BUSY)) {}
    
    /* the address counter is updated 4us after the busy flag is cleared */
    delay(2);

    /* now read the address counter */
    return (lcd_read(chip,lcd, 0));  // return address counter
    
}/* lcd_waitbusy */



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
void lcd_command(uint8_t chip,uint8_t lcd, uint8_t cmd)
{
    lcd_waitbusy(chip, lcd);
    lcd_write(chip, lcd, cmd,0);
}


/*************************************************************************
Send data byte to LCD controller 
Input:   data to send to LCD controller, see HD44780 data sheet
Returns: none
*************************************************************************/
void lcd_data(uint8_t chip, uint8_t lcd, uint8_t data)
{
    lcd_waitbusy(chip, lcd);
    lcd_write(chip,lcd, data,1);
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
	if ( y==0 )
		lcd_command(chip,1, (1<<LCD_DDRAM)+l->start_line1+x);
	else if ( y==1)
		lcd_command(chip,1, (1<<LCD_DDRAM)+l->start_line2+x);
	else if ( y==2)
		lcd_command(chip,2, (1<<LCD_DDRAM)+l->start_line1+x);
	else /* y==3 */
		lcd_command(chip,2, (1<<LCD_DDRAM)+l->start_line2+x);

			
}/* lcd_gotoxy */


/*************************************************************************
*************************************************************************/
int lcd_getxy(uint8_t chip)
{
    return lcd_waitbusy(chip, 1);
}


/*************************************************************************
Clear display and set cursor to home position
*************************************************************************/
void lcd_clrscr(uint8_t chip)
{
    lcd_command(chip,1, 1<<LCD_CLR);
	lcd_command(chip,2, 1<<LCD_CLR);
}


/*************************************************************************
Set cursor to home position
*************************************************************************/
void lcd_home(uint8_t chip)
{
    lcd_command(chip,1, 1<<LCD_HOME);
}


/*************************************************************************
Display character at current cursor position 
Input:    character to be displayed                                       
Returns:  none
*************************************************************************/
void lcd_putc(uint8_t chip, char c)
{
    uint8_t pos, disp_length, lcd, i,y, skip;
	uint8_t  start_line1, start_line2, start_line3, start_line4;
	register uint8_t addressCounter;
	struct lcdinfo *l;

	
	l=&lcdinfos[chip];	
	y=l->y;
	lcd=1;
	if (y>1) lcd=2;
		
	disp_length=l->disp_length;
	start_line1=l->start_line1;		
	start_line2=l->start_line2;
	start_line3=l->start_line3;
	start_line4=l->start_line4;
	
    pos = lcd_waitbusy(chip, lcd);   // read busy-flag and address counter
    if (c=='\n') {		
		addressCounter=start_line1;	
		for (i=l->x; i<40; i++) {txtbuf[i+y*40]=' ';}
		
		if ((pos>start_line1) && (pos <= start_line1+disp_length)) 
			{addressCounter=start_line2; y++;}
		if ((pos>start_line2) && (pos <= start_line2+disp_length)) 
			{addressCounter=start_line1; y++; lcd++; }
		if ((pos>start_line3) && (pos <= start_line3+disp_length)) 
			{addressCounter=start_line2; y++;}	
		if ((pos>start_line4) && (pos <= start_line4+disp_length)) 
			{addressCounter=start_line1; y++; lcd++; }	
		
		//if (lcd==3) {lcd=1; y=0; }		
		l->y=y;
		lcd_command(chip, lcd, (1<<LCD_DDRAM)+addressCounter);						
		return;
    }	
			
	skip=0;
	if ( pos == start_line1+disp_length ) 
		{addressCounter=start_line3; skip=1;}
	else if ( pos == start_line2+disp_length ) 
		{addressCounter=start_line4; skip=1; }		
	else if ( pos == start_line3+disp_length ) 
		{addressCounter=start_line2; skip=1; y++; }		
	else if ( pos == start_line4+disp_length ) 
		{addressCounter=start_line1; skip=1; y++; lcd++;}		
	
	
	if (skip) {		
		if (lcd==3) { lcd=1; y=0; }
		lcd_command(chip, lcd, (1<<LCD_DDRAM)+addressCounter);
		l->y=y;
		l->x=0;
	}
	txtbuf[l->x+y*40]=c;
	(l->x)++;	
    lcd_waitbusy(chip, lcd);	
	lcd_write(chip, lcd, c, 1);
    
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



void lcd_setup (uint8_t chip, uint8_t rs, uint8_t rw, uint8_t enable1, uint8_t enable2,
				uint8_t db0, uint8_t db1, uint8_t db2, uint8_t db3 ) 
{
  uint8_t b_rs, b_rw, b_enable1, b_enable2;
  uint8_t b_db0, b_db1, b_db2, b_db3;
 
 b_rs=1<< (rs  & P_BITMASK);
 lcd_b_rs[chip]=b_rs;
 b_rw=1<< (rw & P_BITMASK); 
 lcd_b_rw[chip]=b_rw;
 b_enable1=1<< (enable1 & P_BITMASK);  
 lcd_b_enable1[chip]=b_enable1;
 b_enable2=1<< (enable2 & P_BITMASK);  
 lcd_b_enable2[chip]=b_enable2; 
 
 
 b_db0=1<< (db0  & P_BITMASK);
 lcd_b_db0[chip]=b_db0;
 b_db1=1<< (db1  & P_BITMASK);
 lcd_b_db1[chip]=b_db1;
 b_db2=1<< (db2  & P_BITMASK);
 lcd_b_db2[chip]=b_db2;
 b_db3=1<< (db3  & P_BITMASK);
 lcd_b_db3[chip]=b_db3;
 
 rs=rs & P_PORTMASK;
 if (rs==P_PORTA) {lcd_p_rs[chip]=&PORTA; DDRA|=b_rs;}
 if (rs==P_PORTB) {lcd_p_rs[chip]=&PORTB; DDRB|=b_rs;}
 if (rs==P_PORTC) {lcd_p_rs[chip]=&PORTC; DDRC|=b_rs;}
 if (rs==P_PORTD) {lcd_p_rs[chip]=&PORTD; DDRD|=b_rs;}

 rw=rw & P_PORTMASK;
 if (rw==P_PORTA) {lcd_p_rw[chip]=&PORTA; DDRA|=b_rw;}
 if (rw==P_PORTB) {lcd_p_rw[chip]=&PORTB; DDRB|=b_rw;}
 if (rw==P_PORTC) {lcd_p_rw[chip]=&PORTC; DDRC|=b_rw;}
 if (rw==P_PORTD) {lcd_p_rw[chip]=&PORTD; DDRD|=b_rw;}
 
  enable1=enable1 & P_PORTMASK;
 if (enable1==P_PORTA) {lcd_p_enable1[chip]=&PORTA; DDRA|=b_enable1;}
 if (enable1==P_PORTB) {lcd_p_enable1[chip]=&PORTB; DDRB|=b_enable1;}
 if (enable1==P_PORTC) {lcd_p_enable1[chip]=&PORTC; DDRC|=b_enable1;}
 if (enable1==P_PORTD) {lcd_p_enable1[chip]=&PORTD; DDRD|=b_enable1;}
 
 enable2=enable2 & P_PORTMASK;
 if (enable2==P_PORTA) {lcd_p_enable2[chip]=&PORTA; DDRA|=b_enable2;}
 if (enable2==P_PORTB) {lcd_p_enable2[chip]=&PORTB; DDRB|=b_enable2;}
 if (enable2==P_PORTC) {lcd_p_enable2[chip]=&PORTC; DDRC|=b_enable2;}
 if (enable2==P_PORTD) {lcd_p_enable2[chip]=&PORTD; DDRD|=b_enable2;}

 
 db0=db0 & P_PORTMASK;
 if (db0==P_PORTA) {lcd_p_db0[chip]=&PORTA; lcd_pin_db0[chip]=&PINA; 
					lcd_ddr_db0[chip]=&DDRA; DDRA|=b_db0; }
 if (db0==P_PORTB) {lcd_p_db0[chip]=&PORTB; lcd_pin_db0[chip]=&PINB; 
					lcd_ddr_db0[chip]=&DDRB; DDRB|=b_db0;}
 if (db0==P_PORTC) {lcd_p_db0[chip]=&PORTC; lcd_pin_db0[chip]=&PINC;
					lcd_ddr_db0[chip]=&DDRC; DDRC|=b_db0;}
 if (db0==P_PORTD) {lcd_p_db0[chip]=&PORTD; lcd_pin_db0[chip]=&PIND; 
					lcd_ddr_db0[chip]=&DDRD; DDRD|=b_db0;}

 db1=db1 & P_PORTMASK;
 if (db1==P_PORTA) {lcd_p_db1[chip]=&PORTA; lcd_pin_db1[chip]=&PINA; 
					lcd_ddr_db1[chip]=&DDRA; DDRA|=b_db1;}
 if (db1==P_PORTB) {lcd_p_db1[chip]=&PORTB; lcd_pin_db1[chip]=&PINB; 
					lcd_ddr_db1[chip]=&DDRB; DDRB|=b_db1;}
 if (db1==P_PORTC) {lcd_p_db1[chip]=&PORTC; lcd_pin_db1[chip]=&PINC; 
					lcd_ddr_db1[chip]=&DDRC; DDRC|=b_db1;}
 if (db1==P_PORTD) {lcd_p_db1[chip]=&PORTD; lcd_pin_db1[chip]=&PIND; 
					lcd_ddr_db1[chip]=&DDRD; DDRD|=b_db1;}

 db2=db2 & P_PORTMASK;
 if (db2==P_PORTA) {lcd_p_db2[chip]=&PORTA; lcd_pin_db2[chip]=&PINA; 
					lcd_ddr_db2[chip]=&DDRA; DDRA|=b_db2;}
 if (db2==P_PORTB) {lcd_p_db2[chip]=&PORTB; lcd_pin_db2[chip]=&PINB; 
					lcd_ddr_db2[chip]=&DDRB; DDRB|=b_db2;}
 if (db2==P_PORTC) {lcd_p_db2[chip]=&PORTC; lcd_pin_db2[chip]=&PINC; 
					lcd_ddr_db2[chip]=&DDRC; DDRC|=b_db2;}
 if (db2==P_PORTD) {lcd_p_db2[chip]=&PORTD; lcd_pin_db2[chip]=&PIND; 
					lcd_ddr_db2[chip]=&DDRD; DDRD|=b_db2;}

 db3=db3 & P_PORTMASK;
 if (db3==P_PORTA) {lcd_p_db3[chip]=&PORTA; lcd_pin_db3[chip]=&PINA; 
					lcd_ddr_db3[chip]=&DDRA; DDRA|=b_db3;}
 if (db3==P_PORTB) {lcd_p_db3[chip]=&PORTB; lcd_pin_db3[chip]=&PINB; 
					lcd_ddr_db3[chip]=&DDRB; DDRB|=b_db3;}
 if (db3==P_PORTC) {lcd_p_db3[chip]=&PORTC; lcd_pin_db3[chip]=&PINC; 
					lcd_ddr_db3[chip]=&DDRC; DDRC|=b_db3;}
 if (db3==P_PORTD) {lcd_p_db3[chip]=&PORTD; lcd_pin_db3[chip]=&PIND; 
					lcd_ddr_db3[chip]=&DDRD; DDRD|=b_db3;}
}

    /* initial write to lcd is 8bit */

void lcd_setup_info (uint8_t chip, uint8_t display_type, uint8_t width, uint8_t height) 

{
 struct lcdinfo *l;
 
 l=&lcdinfos[chip]; 
 l->x=0;
 l->y=0;
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
	volatile uint8_t *p_db0, *p_db1;
	uint8_t b_db0, b_db1;

	p_db0=lcd_p_db0[chip];
    p_db1=lcd_p_db1[chip];

    b_db0=lcd_b_db0[chip];
    b_db1=lcd_b_db1[chip];
	
	bit_set (*p_db0, b_db0);
	bit_set (*p_db1, b_db1);

    lcd_e_toggle(chip, 1);
	lcd_e_toggle(chip, 2);
    delay(4992);         /* delay, busy flag can't be checked here */
   
    /* repeat last command */ 
    lcd_e_toggle(chip, 1);      
	lcd_e_toggle(chip, 2);
    delay(64);           /* delay, busy flag can't be checked here */
    
    /* repeat last command a third time */    
	lcd_e_toggle(chip, 1);
	lcd_e_toggle(chip, 2);	
    delay(64);           /* delay, busy flag can't be checked here */

    /* now configure for 4bit mode */    
	bit_clr (*p_db0, b_db0);
    
	lcd_e_toggle(chip, 1);
	lcd_e_toggle(chip, 2);
    delay(64);           /* some displays need this additional delay */

	lcd_command (chip, 1, LCD_DISP_ON_CURSOR_BLINK);	
	lcd_command (chip, 2, LCD_DISP_ON_CURSOR_BLINK);	
	
    lcd_command (chip, 1, LCD_FUNCTION_4BIT_2LINES);         
	lcd_command (chip, 2, LCD_FUNCTION_4BIT_2LINES);      /* function set: display lines  */    	
    lcd_clrscr (chip);                           /* display clear                */ 
    lcd_command (chip, 1, LCD_MODE_DEFAULT);          /* set entry mode               */	
	lcd_command (chip, 2, LCD_MODE_DEFAULT);          /* set entry mode               */
	
	lcd_command (chip, 1, dispAttr);
	lcd_command (chip, 2, dispAttr);
}
