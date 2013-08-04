/******************************************************************************
 Title:    adc_dac_uc2
 Author:   Alex Priem
 Date:     October 2008
 Software: AVR-GCC 3.3 
 Hardware: ATMega32
           
 Description:
 
  
*******************************************************************************/
#include <inttypes.h>
#include <avr/io.h>
 
#include "uart.h"
#include "uart1.h"
#include "cat4016.h"
#include "bitops.h"

#include "delay.h"
#include "lcd_4_40x4.h"
#include "constants.h"
#include "rot_encoder.h"
#include "keypad.h"

	
#include "softuart.h"


#define WIND_CLOCK 0x80
#define FILL_WIND 0x40
#define MOTOR 0x20
#define WIND_ANTICLOCK 0x10


uint8_t relaystate=0x0;


	/*
	lcd_puts (0, "\nklmno 3");
	lcd_puts (0, "\npqrst 4");
	lcd_puts (0, "\nuvw 5");
	lcd_puts (0, "\nzAB 6");
	lcd_puts (0, "\nEFG 7");
	lcd_puts (0, "\nKLM 8");
	lcd_puts (0, "\n# 9"); 
	*/
	
	
	/*
	lcd_puts (0, "##234567890abcdefghijklmno##rstuvwxyz\n");
	lcd_puts (0, "12##$567890abcdefghijklmnopq##tuvwxyz\n");
	lcd_puts (0, "12345##7890abcdefghijklmnopqs##vwxyz\n");
	lcd_puts (0, "1234567##90abcdefghijklmnopqrstu##yz\n");
	*/
	
//uint8_t mapping[17]={12,13,15,14,  11,9,7,6,   0,8,2,5,   10,7,1,4, 255};
          //keycode:   0 1 2  3      4 5 6  7    8 9 10 11  12 13 14 15  16				
			
uint8_t	mapping[17]={0xbb, 0x5f, 0x5d, 0x57,    //  0 1 2 3     
				 0xd5, 0xd7, 0xdd, 0xbe,    //  4 5 6 7
				 0xae, 0xba, 0xab, 0xaf,     //  8 9 a b
				 0xbf, 0xaa, 0xdf, 0x55,
				 0x1f                        // c+1

				 };   //  c d e f 
				 
				 
//uint8_t keypad_bits[16];
volatile uint8_t *keypad_port;

void init_keypad_4x4s (uint8_t port)
						
{
   
	if (port==P_PORTA) {keypad_port=&PINA; DDRA=0; PORTA=0xff;}
	if (port==P_PORTB) {keypad_port=&PINB; DDRB=0; PORTB=0xff;}
	if (port==P_PORTC) {keypad_port=&PINC; DDRC=0; PORTC=0xff;}
	if (port==P_PORTD) {keypad_port=&PIND; DDRD=0; PORTD=0xff;}
}					

uint8_t keypad_getch (void) {
 uint8_t c,i;
 
  c=*keypad_port;  
  if (c==0xff) return 0xff;
  for (i=0; i<16; i++) {
    if (mapping[i]==c) return i;
  }
  uart1_printf ("[%x]",c);
  return 0xfe;
}		  

uint8_t keypad_w_getch (void) {
 uint8_t c,c2,i;
 
 
  c=0xff;
  while (c==0xff) {
	 c=*keypad_port;
	 }  
  
  delay(50);
  i=0;
  c2=*keypad_port;
  while (c!=c2) {
  	 c2=*keypad_port;
  	 i++;
  	 if (i>10) return 0xfd;
  	}
  
  for (i=0; i<17; i++) {
    if (mapping[i]==c) return i;
  }
  

  uart1_printf ("not in mapping:%x\r\n",c);
  return 0xfe;
}		  
	  

void set_wind_dir_none (void) {
	uart1_printf ("left\r\n");	
	
	relaystate=(relaystate & ~(WIND_CLOCK | WIND_ANTICLOCK));
	bit_clr (PORTD, WIND_CLOCK );
	bit_clr (PORTD, WIND_ANTICLOCK );
}


void set_wind_dir_left (void) {
	uart1_printf ("left\r\n");	
	relaystate=(relaystate & ~(WIND_CLOCK | WIND_ANTICLOCK)) | WIND_CLOCK;
	bit_clr (PORTD, WIND_CLOCK );
	bit_set (PORTD, WIND_ANTICLOCK );	
}

void set_wind_dir_right (void) {
	uart1_printf ("right\r\n");
	relaystate=(relaystate & ~(WIND_CLOCK | WIND_ANTICLOCK)) | WIND_ANTICLOCK;
	bit_clr (PORTD, WIND_CLOCK );
	bit_set (PORTD, WIND_ANTICLOCK );	
}

void set_fill (void) {
	
	relaystate=(relaystate & ~FILL_WIND);
	bit_clr (PORTD, FILL_WIND );
}

void set_wind (void){
	relaystate=(relaystate | FILL_WIND);
	bit_set (PORTD, FILL_WIND );	
}

void stop_motor (void){
	relaystate=(relaystate & ~MOTOR);
	bit_clr (PORTD, MOTOR );
}

void start_motor (void){
	relaystate=(relaystate | MOTOR);
	bit_set (PORTD, MOTOR );
}

void fill (unsigned int num) {	
	uart1_printf ("fill:%d\r\n",num);
	set_fill();
	sleep(100);
	start_motor();
}


void wind (unsigned int num) {	
	uart1_printf ("wind:%d\r\n",num);
	set_wind();
	sleep(100);
	start_motor();
}

void wind_left (unsigned int num) {	
	uart1_printf ("wind_right:%d\r\n",num);
	set_wind_dir_left ();
	set_wind();
	sleep(100);
	start_motor();
	wind(num);
}

void wind_right (unsigned int num) {
	
	uart1_printf ("wind_left:%d\r\n",num);
	set_wind_dir_right ();
	set_wind();
	sleep(100);
	start_motor();
	wind(num);	
}


void update_num (unsigned int num) {

	uart1_printf ("num: %d\r\n",num);
}

void special (void) {
	uart1_printf ("special\r\n");
}
		  

uint16_t dumps=0;

void dump_uart1 (void) {
	char buf[40];
	uint8_t i;


	uart_printf("%d",dumps);

	for (i=0; i<32; i++) {
		buf[i]=uart1_buf[i];		
	}
	buf[32]=13;
	buf[33]=10;
	buf[34]=0;
	uart_puts(buf);
	dumps++;
	uart_puts("ok\r\n");
}


void dump_uart0 (void) {
	char buf[40];
	uint8_t i;


	uart1_printf("%d",dumps);

	for (i=0; i<32; i++) {
		buf[i]=uart1_buf[i];		
	}
	buf[32]=13;
	buf[33]=10;
	buf[34]=0;
	uart1_puts(buf);
	dumps++;
	uart1_puts("ok\r\n");
}



int main(void)
{
    unsigned int  num, prevnum;
	uint8_t a,p;
	int c, prevc;
	
		  
	
	//softuart_init();
	uart_init();
	uart1_init();
	//delay(1000);
	uart1_puts ("\r\nreset -- init port 1\r\n");	
	
	//dump_uart1();
	uart_puts ("a123\n");
	uart_puts ("b456\n");
	uart_puts ("c789\n");
	uart_puts ("d20\n");
	//dump_uart1();
	//uart_puts ("b222\n");
	//dump_uart1();
	//uart_puts ("c333\n");
	//dump_uart1();
//	uart_puts ("d444\n");
	//dump_uart1();
	//softuart_puts("\r\nsoftuart reset -- init\r\n");
	//init_keypad_4x4_s (ports);	
	
	init_keypad_4x4s (P_PORTC);

	lcd_setup (0, P_PA2, P_PA4, P_PA6, P_PA0,
			  P_PA1, P_PA3, P_PA5, P_PA7);	 

			  //lcd_setup  (uint8_t chip,  uint8_t strobe, uint8_t clock, uint8_t io)
	lcd_setup_info (0, HD44780, 20, 2);
	
	lcd_init (0, LCD_DISP_ON_CURSOR_BLINK);

  // relays


			//							sh_cp:11 (geel)    st_cp:12(paars)    ds:14(wit)
	//void hc595_setup (uint8_t chip, uint8_t clk, uint8_t cs, uint8_t data)

	DDRD|=P_BIT7 | P_BIT6 | P_BIT5 | P_BIT4;

	
	p=P_BIT4;

	bit_set(PORTD, P_BIT4);
	uart1_puts ("0xff\r\n");
	lcd_puts (0, "0xff");
	delay(1000);

	bit_set(PORTD, P_BIT5);	
	uart1_puts ("0x00\r\n");
	lcd_puts (0, "0x00");
	delay(1000);

	bit_set(PORTD, P_BIT6);
	uart1_puts ("0xff\r\n");
	lcd_puts (0, "0xff");
	delay(1000);

	bit_set(PORTD, P_BIT7);
	uart1_puts ("0x00\r\n");
	lcd_puts (0, "0x00");
	delay(1000);



	//void lcd_setup (uint8_t rs, uint8_t rw, uint8_t enable1, uint8_t enable2,
	//			uint8_t db0, uint8_t db1, uint8_t db2, uint8_t db3 ) 
	 //lcd_setup (0, P_PC2, P_PC4, P_PC6, P_PC0,
//			  P_PC1, P_PC3, P_PC5, P_PC7);	 
	
	
	lcd_puts (0, "axbcde x");
	lcd_puts (0, "\nfghij y");
	
	uart1_printf ("start loop \r\n",c);
	c=0;
	prevc=0;
	num=0;
	prevnum=0;
	for (;;) {

		c=keypad_w_getch();
		

		if (c!=prevc)  {
			a=PINC;
			uart1_printf ("%x %x %d\r\n",a,c,num);
			prevc=c;
			switch (c) {
				case 0x0f: wind(num);
							update_num(num);
							break;
				case 0x0e: wind_right (num);					   
						update_num(num);
						   break;
				case 0x0d: wind_left (num);	
							update_num(num);					
						   break;
				case 0x0c: num=0;					   
							prevnum=0;
							update_num(num);
						   break;
				case 0x0a: fill (num);		
							update_num(num);			   
							break;
				case 0x0b: num=prevnum;		   				
							update_num(num);
			   				break;
			   	case 0x10: special();
			   			   break;
			   	case 0xff:
			   	case 0xfd:
			   	case 0xfe: break;


				default: if (num<1000)  {
							prevnum=num;
							num=num*10 + c;						
							update_num(num);
						}
				}			
			
			}
		

	}
	
 return 0;
}
