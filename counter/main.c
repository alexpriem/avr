/******************************************************************************
 Title:    ruff counter
 Author:   Alex Priem
 Date:     February 2012
 Software: AVR-GCC 3.3 
 Hardware: ATMega32
           
 Description:
 t
  
*******************************************************************************/
#include <inttypes.h>
#include <avr/io.h>
 
#include "uart.h"
#include "cat4016.h"
#include "bitops.h"

#include "delay.h"
#include "leds.h"
#include "keypad.h"
#include "lcd.h"
#include "hc595.h"

#define TRUE 1
#define FALSE 0
					
#define LEFT 1
#define RIGHT 2					
//uint8_t mapping[17]={12,13,14,15,
//                       11, 9, 6, 3,
//					    0, 8, 5, 2,
//					   10, 7, 4, 1, 255};
//uint8_t mapping[17]={1,2,15,3,4,5,14,6,7,8,13,9,10,0,12,11, 255};
  uint8_t mapping[17]={12,13,15,14,  11,9,7,6,   0,8,2,5,   10,7,1,4, 255};
          //keycode:   0 1 2  3      4 5 6  7    8 9 10 11  12 13 14 15  16
   
int get_number(uint8_t disp) 

{
 int num, c;
 uint8_t d0, d1, d2, d3, digit;
 
 c=0x0f;
 d0=0;
 d1=0;
 d2=0;
 d3=0;

 num=0;
 cat4016_put_unumber (disp, num);
 while (c!=0x10) {
	c=keypad_4x4_getc ();  // wachten tot alle toetsen vrij zijn.
	cat4016_put_snumber (0, -c);
	}
	
	
 for (;;) {
	while (c==0x10)				// wacht tot toets ingedrukt is
		{
		c=keypad_4x4_getc();  
		digit=mapping[c];
		cat4016_put_unumber (0, digit);
		}
	digit=mapping[c];
	if (digit==12) return -1;			// C: error, skip input.
	if (digit==10) return num;		// A: accepteer input.
	if (digit==13) {                 // D:  del
		d0=0;		
	}
	if (digit==11) {                  // B: backspace
		d3=0;	
		d2=d3;
		d1=d2;
		d0=d1;		
	}
	if ((digit<10) && (num<1000)) {
		d3=d2;
		d2=d1; 
		d1=d0;
		d0=digit;		
		}
	num=d3*1000+d2*100+d1*10+d0;
	cat4016_put_unumber (disp, num);
	while (c!=0x10) {
		c=keypad_4x4_getc ();  // wachten tot alle toetsen vrij zijn.
		digit=mapping[c];
		cat4016_put_snumber (0, -digit);
	}

	}	
}



void set_dir_left (void) 
{

}


void set_dir_right (void) 
{

}
 
void start_winding (void)
 
{
}
 
void stop_winding (void)
 
{
}

void start_filling (void)
 
{
}
 
void stop_filling (void)
 
{
}
 
int main(void)
{
 //   int rev=0;
	int j,c, num;	
	uint8_t dir, filling, winding;
	int turns=0, fills=0;
	int count,prevc;
	//int layers[8];
	
	
	uart_init();
	uart_puts("\r\ncounter init\r\n");	
	uart_puts("\r\ncounter init\r\n");	
	uart_puts("\r\ncounter init\r\n");	
	
	DDRA = 0x0f;  // E inputs + outputs voor keypad
	PORTA= 0xf0;  // pullups aan
	//PORTC= 0x00;  // pullups uit
	
	DDRA  = 0xff;  // A output   
	DDRC  = 0xff;  // C output   LEDS 1+2	
	DDRD  = 0xff;  // DE output   LEDS 3+4 
	
	//void lcd_setup (uint8_t chip, uint8_t rs, uint8_t rw, uint8_t enable, 
	//			uint8_t db0, uint8_t db1, uint8_t db2, uint8_t db3 ) 
	//lcd_setup (0, P_PB5, P_PB6, P_PB7, 
	//		  P_PB3, P_PB2, P_PB1, P_PB0);	
	
	//lcd_setup (1, P_PB5, P_PB6, P_PB7, 
	//		  P_PB0, P_PB1, P_PB2, P_PB3);	
	
	
	lcd_setup (P_PC2, P_PC4, P_PC6, 0,
			  P_PC7, P_PC5, P_PC3, P_PC1);			
	
//	uart_puts("\r\nsetup done\r\n");	
	lcd_init ( LCD_DISP_ON_CURSOR_BLINK);
//	uart_puts("\r\nlcd init done\r\n");	
	lcd_gotoxy ( 0,0);	
	lcd_puts ( "testtekst");
//	uart_puts("\r\ndone\r\n");	
	
	
  // init_cat4016(uint8_t chip, uint8_t clk, uint8_t latch, uint8_t data, uint8_t blank)
	init_cat4016 (1, P_PC6, P_PC0,  P_PC4, PC2);
	init_cat4016 (2, P_PD7, P_PD1,  P_PD5, PD3);
	init_cat4016 (3, P_PD6, P_PD0,  P_PD4, PD2);	
	
    uart_puts("\r\ndone cat4016\r\n");	
	//										sh_cp    st_cp       ds
	//hc595_setup(uint8_t chip, uint8_t clk, uint8_t cs, uint8_t data)
	
	hc595_setup(0, P_PC0, P_PC2, P_PC4);
	
	
	keypad_4x4_setup (P_PA0, P_PA1, P_PA2, P_PA3,
	                 P_PA4, P_PA5, P_PA6, P_PA7);					 
	
	uart_puts("\r\nall done\r\n");	
	
	j=0;
	//PORTA=0x00;
	
	cat4016_put_unumber (1,1111);
	cat4016_put_unumber (2,2222);
	cat4016_put_unumber (0,3333);
	uart_printf ("inits done\r\n");
	
	for (;;) {
		c=keypad_4x4_getc();
		num=mapping[c];		
		if (c!=prevc) {
			uart_printf (":%d reps omitted\r\n", count);
			uart_printf ("%d %d", c, num);
			prevc=c;
			count=0;
			} 
		else {
		  count++;
		  }
		}
	
	
	for (;;)  {
		c=keypad_4x4_getc();
		num=mapping[c];		
		if (num==15) {
			cat4016_put_txt (3,"fill" );
			fills=get_number(2);
			if (fills>0) {
				cat4016_put_unumber (1,fills);
				start_filling();
				filling=TRUE;
				}
			}
		if (num==14) {
			cat4016_put_txt (3,"tu l" );
			turns=get_number(2);
			if (turns>0) {
				cat4016_put_unumber (1,turns);
				dir=LEFT;
				winding=TRUE;				
				set_dir_left();
				start_winding();
				
				}
			}
		if (num==14) {
			cat4016_put_txt (3,"tu r" );
			turns=get_number(2);
			if (turns>0) {
				cat4016_put_unumber (1,turns);
				dir=RIGHT;
				winding=TRUE;								
				set_dir_right();				
				start_winding();				
				}
			}

			
		if ((num==10) && ((winding) || (filling))) {
			dir=LEFT;
			set_dir_left();
		}
		if ((num==11) && ((winding) || (filling))) {
			dir=RIGHT;
			set_dir_right();
		}

		if ((num==12) || (num==0)) {
			if (filling) stop_filling();
			if (winding) stop_winding();
		}
		j=j+1;
		if (j>9999) j=0;		
		cat4016_put_unumber (0, num);		
		cat4016_put_unumber (1, j);	
	}
	

	
 return 0;
}
