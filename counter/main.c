/******************************************************************************
 Title:    adc_dac_uc2
 Author:   Alex Priem
 Date:     October 2008
 Software: AVR-GCC 3.3 
 Hardware: ATMega32
           
 Description:
 
  
*******************************************************************************/
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
 
#include "uart.h"
#include "cat4016.h"
#include "bitops.h"

#include "delay.h"
#include "lcd_4_40x4.h"
#include "constants.h"
#include "keypad.h"

// define DEBUG 1
	

#define WIND_CLOCK 0x80
#define FILL_WIND 0x40
#define MOTOR 0x20
#define WIND_ANTICLOCK 0x10

#define INACTIVE 0
#define FILL 1
#define WIND_LEFT 2
#define WIND_RIGHT 3
#define WIND 4
#define HELP 5
#define CORR 6
#define LAYER 7


#define NONE 0
#define LEFT 1
#define RIGHT 2

#define ADD 0
#define SUB 1

#define TRUE 1
#define FALSE 0
#define switchpin 0x01

long int timer=0;

long int timestamp0=0, timestamp1=0;


uint8_t action=INACTIVE, corrmode;
unsigned int num, total_turns_done, turns_done, turns_prog;
unsigned int corr, turns_before_corr, total_turns_done_before_corr;
uint8_t prev_switchstate;
uint8_t direction;
uint8_t helppage;

uint8_t c, prevc;

char layer[80];


void update_speed (void);
void update_turns (void);
void update_num (void);
void update_command (void);
void update_layer(void);

void fill (void);
void stop_motor (void);
void start_motor (void);

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
				 


#ifdef DEBUG

void test_relais(void) {

	
	bit_clr(PORTD, P_BIT4);
	bit_clr(PORTD, P_BIT5);
	bit_clr(PORTD, P_BIT6);
	bit_clr(PORTD, P_BIT7);
	bit_set(PORTD, WIND_CLOCK);
	uart_puts ("0x80\r\n");
	lcd_puts (0, "bit7 (wind-clock)\n");
	delay(2000);

	bit_set(PORTD, MOTOR);
	uart_puts ("0x40\r\n");
	lcd_puts (0, "bit5 (motor)\n");
	delay(2000);

	bit_set(PORTD, FILL_WIND);	
	uart_puts ("0x20\r\n");
	lcd_puts (0, "bit6 (fill-wind)\n");
	delay(2000);

	bit_set(PORTD, WIND_ANTICLOCK);
	uart_puts ("0x10\r\n");
	lcd_puts (0, "bit4 (wind anticlock)\n");
	delay(2000);


	lcd_clrscr(0);
	uart_puts ("mot\r\n");
	lcd_puts (0, "motor\n");

    start_motor();
	delay(2000);
    stop_motor();
	delay(500);

	uart_puts ("fill\r\n");
	lcd_puts (0, "fill\n");
	fill();
	delay(2000);
	stop_motor();

	uart_puts ("windleft\r\n");
	lcd_puts (0, "windleft\n");
    delay(500);
	set_wind_dir_left();
	delay(5000);
	stop_motor();
}

#endif


int freeRam (void) {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}


void init_timer (void) {
	TCCR1B|= (1<<CS11);
	TCNT1=0;
	TIMSK1 |= (1<<TOIE1);
	sei();
}

ISR(TIMER1_OVF_vect){
	timer++;
}
				 

volatile uint8_t *keypad_port;

void init_keypad_4x4s (uint8_t port)
						
{
	if (port==P_PORTA) {keypad_port=&PINA; DDRA=0; PORTA=0xff;}
	if (port==P_PORTB) {keypad_port=&PINB; DDRB=0; PORTB=0xff;}
	if (port==P_PORTC) {keypad_port=&PINC; DDRC=0; PORTC=0xff;}
	if (port==P_PORTD) {keypad_port=&PIND; DDRD=0; PORTD=0xff;}
}					


void stop_winding (void)
{
	char buf[8];

	stop_motor();
	update_command(); 
    if (action==INACTIVE) return;  // als we wikkelen zonder power aan op relays
    if (turns_done==0) {
		action=INACTIVE;
		return;
	} 
	
	if (action==WIND_LEFT) { strcat (layer,"L");}
	if (action==WIND_RIGHT) { strcat (layer,"R");}
	if (action==FILL) { strcat (layer,"F");}
	action=INACTIVE;
	itoa(turns_done,buf,10);
	strcat(layer,buf);
	update_layer();
}

void test_switch (void) {
	
	uint8_t switchstate;

		
	switchstate=PINB & switchpin;
//	uart_printf ("s:%x\r\n",switchstate);
	 if ((switchstate!=prev_switchstate) & (prev_switchstate==0)) {
	 	turns_done++;
		timestamp0=timestamp1;
		timestamp1=timer;
		update_speed();

		if ((action==WIND_LEFT) || (action==WIND_RIGHT)) total_turns_done++;
	 	update_turns();
		if (turns_done>=turns_prog) {
			stop_winding();
			}
	 }
	 prev_switchstate=switchstate;
}

uint8_t keypad_w_getch (void) {
 uint8_t c,c2,i;
 
 
  c=0xff;
  while (c==0xff) {   /* mainloop*/
	 c=*keypad_port;
	 test_switch();
	 }  
  
  delay(50);
  i=0;
  c2=*keypad_port;
  while (c!=c2) {  	 
  	 c2=*keypad_port;
  	 i++;
  	 if (i>10) return 0xfd;
  	 test_switch();
  	}
  
  for (i=0; i<17; i++) {
    if (mapping[i]==c) return i;
  }
  

  //uart_printf ("not in mapping:%x\r\n",c);
  return 0xfe;
}		  




	  

void set_wind_dir_none (void) {
#ifdef DEBUG
	uart_puts ("nodir\r\n");	
#endif
	bit_clr (PORTD, WIND_CLOCK );
	bit_clr (PORTD, WIND_ANTICLOCK );
    direction=NONE;
}


void set_wind_dir_left (void) {
#ifdef DEBUG
	uart_puts ("left\r\n");	
#endif
	bit_clr (PORTD, WIND_CLOCK );
	bit_set (PORTD, WIND_ANTICLOCK );	
    direction=LEFT;
}

void set_wind_dir_right (void) {
#ifdef DEBUG
	uart_puts ("right\r\n");
#endif
	bit_set (PORTD, WIND_CLOCK );
	bit_clr (PORTD, WIND_ANTICLOCK );
    direction=RIGHT;
	
}


void stop_motor (void){
#ifdef DEBUG
	uart_puts("stop motor\r\n");
#endif
	bit_set (PORTD, MOTOR );
}

void start_motor (void){
#ifdef DEBUG
	uart_puts ("start motor\r\n");
#endif
	bit_clr (PORTD, MOTOR );
}


void fill (void) {
#ifdef DEBUG
	uart_puts ("fill\r\n");
#endif
	set_wind_dir_none ();
	bit_clr (PORTD, FILL_WIND );
	turns_done=0;
	action=FILL;
	delay(100);
	start_motor();
}

void wind_left (void) {
#ifdef DEBUG
	uart_puts ("wind_left\r\n");
#endif
	set_wind_dir_left ();
	bit_set (PORTD, FILL_WIND );	
	action=WIND_LEFT;
	turns_done=0;
	delay(100);
	start_motor();
}

void wind_right (void) {
#ifdef DEBUG
	uart_puts ("wind_right\r\n");
#endif
	set_wind_dir_right ();
	bit_set (PORTD, FILL_WIND );	
	turns_done=0;
	action=WIND_RIGHT;
	delay(100);
	start_motor();
}

void update_layer (void) {
#ifdef DEBUG
	uart_puts ("layer:%s\r\n", layer);
#endif
	lcd_gotoxy (0,0,3);
	lcd_puts (0,"Layer:");
	lcd_puts (0,layer);
}
void update_command (void) {

    char buf[30];

	buf[0]=0;
	switch (action) {
		case INACTIVE: strcpy (buf,"waiting");
						break;
		case FILL: strcpy (buf,"filling");
						break;
		case WIND_LEFT: strcpy (buf,"wind left");
						break;
		case WIND_RIGHT: strcpy (buf,"wind right");
						break;
		case CORR: strcpy (buf,"Correct layer");
					if (corrmode==ADD) {strcat (buf,":+");}
					if (corrmode==SUB) {strcat (buf,":-");}
					break;
		case LAYER: strcpy (buf,"Layer");
						break;
	}
#ifdef DEBUG
	uart_puts(buf);
	uart_puts("\r\n");
#endif
	lcd_gotoxy (0,0,0);
	lcd_puts (0,"                     ");
	lcd_gotoxy (0,0,0);
	lcd_puts (0,buf);

}


void update_speed (void) {

long int cps, cps2;
int delta_t;

	delta_t=timestamp1-timestamp0;
	cps=32768000/delta_t;
	cps2=cps-1000000*(cps/1000000);
	cps2=cps/1000;
	cps=cps/1000000;
	lcd_gotoxy (0,24,2);
	lcd_printf (0,"speed:%l.%l", cps, cps2);
}

void update_num (void) {

#ifdef DEBUG
	uart_printf ("num: %d\r\n",num);
#endif
	lcd_gotoxy (0,0,1);
	lcd_puts (0,"num:");
	lcd_gotoxy (0,5,1);
	lcd_puts (0,"     ");
	lcd_gotoxy (0,5,1);
	lcd_printf (0,"%d",num);
}


void update_turns (void) {

#ifdef DEBUG
	uart_printf ("prog/done: %d / %d\r\n",turns_prog, turns_done);
#endif
	lcd_gotoxy (0,0,2);
	lcd_puts   (0,"prog:");
	lcd_gotoxy (0,5,2);
	lcd_puts   (0,"    :    :     ");
	lcd_gotoxy (0,5,2);
	lcd_printval (0,turns_prog);
	lcd_gotoxy (0,10,2);
	lcd_printval (0,turns_done);
	lcd_gotoxy (0,15,2);
	lcd_printval (0,total_turns_done);

}



void init_count_switch(void) {

	DDRB&=~0x01; bit_set(PORTB, switchpin);
	prev_switchstate=PINB & switchpin;
}


void update_info (void) {

lcd_gotoxy (0,32,0);
lcd_puts (0,"F:help");
}


void show_help (void) {

#define MAX_HELPPAGES 1   // aantal paginas -1 (want tellen vanaf 0)
#ifndef DEBUG
lcd_clrscr (0);
lcd_gotoxy (0,0,0);
if (helppage==0) {
	lcd_puts(0, "digits + A:wind left               F:>>\n");
	lcd_puts(0, "digits + B:wind right              E:<<\n");
	lcd_puts(0, "digits + D:fill\n");
	lcd_puts(0, "digits + E:correct turns        C:cancel");
	} 

if (helppage==1) {
	lcd_puts(0, "while winding: C to stop.          F:>>\n");
	lcd_puts(0, "                                   E:<<\n");
	lcd_puts(0, "                                       \n");
	lcd_puts(0, "                                C:cancel");
	} 
#endif
}

int main(void)
{
	uint8_t i;
	
		  
#ifdef DEBUG
	uart_init();
	uart_puts ("\r\nreset\r\ninit port 1\r\n");	
#endif
	
	//
/*
	uart_puts ("a123\n");
	uart_puts ("b456\n");
	uart_puts ("c789\n");
	uart_puts ("d20\n");
*/	
	init_keypad_4x4s (P_PORTC);
	init_count_switch();
    init_timer ();

	lcd_setup (0, P_PA2, P_PA4, P_PA6, P_PA0,
			  P_PA1, P_PA3, P_PA5, P_PA7);	 

 //lcd_setup  (uint8_t chip,  uint8_t strobe, uint8_t clock, uint8_t io)
	lcd_setup_info (0, HD44780, 20, 2);
	
	lcd_init (0, LCD_DISP_ON);

  // relays
		//	sh_cp:11 (geel)    st_cp:12(paars)    ds:14(wit)
	//void hc595_setup (uint8_t chip, uint8_t clk, uint8_t cs, uint8_t data)

	DDRD|=P_BIT7 | P_BIT6 | P_BIT5 | P_BIT4;
#ifdef DEBUG
    test_relais();
#endif


	c=0;
	prevc=0;
	num=0;
	turns_done=0;
	turns_prog=0;	
	total_turns_done=0;
	for (i=0; i<80; i++) layer[i]=0;
	stop_motor();
	set_wind_dir_none();

	lcd_clrscr(0);
	lcd_puts (0,"*    Winding controller 20140114");
	lcd_gotoxy(0,39,0);
	lcd_puts (0,"*");
	lcd_gotoxy(0,0,3);
	lcd_puts (0,"*");
	lcd_gotoxy(0,39,3);
	lcd_puts (0,"*");
	lcd_gotoxy(0,2,2);
	lcd_printf(0,"Free RAM:%d b",freeRam());
	sleep(1);


	lcd_clrscr(0);
	update_info ();
    update_command();
	update_num ();
	update_speed ();
	update_turns();

	for (;;) {

		c=keypad_w_getch();

		if (c!=prevc)  {
			/*a=PINC;
			uart_printf ("%x %x %d\r\n",a,c,num); */
			prevc=c;

			if ((action==FILL) || (action==WIND_LEFT) || (action==WIND_RIGHT)) {
				switch (c) {
					case 0x0c: 	stop_winding();
								action=INACTIVE; //dubbel maar extra voor readability
							   	break;
				}
			}  // if !INACTIVE

			if (action==HELP) {

			switch (c) {
					case 0x0f: 	if (helppage<MAX_HELPPAGES) helppage++;
								show_help();
							   	break;
					case 0x0e: 	if (helppage>0) helppage--;
								show_help();
							   	break;
					case 0x0c: 	action=INACTIVE;
								lcd_clrscr(0);
							   	break;
					}
			}  // if HELP

			if (action==CORR) {
			switch (c) {  //corrmode is wat we de *vorige* keer gedaan hebben
					char buf[10];

					case 0x0c: 	turns_done=turns_before_corr;
								total_turns_done=total_turns_done_before_corr;
								action=INACTIVE;
								break;
					case 0x0e: 	turns_done=turns_before_corr;
								total_turns_done=total_turns_done_before_corr;
								if (corrmode==SUB) {
									turns_done+=corr;
									total_turns_done+=corr;
									corrmode=ADD;
									break;
								}
								if (corrmode==ADD) {
									if (turns_done>=corr) {
											turns_done-=corr;
											total_turns_done-=corr;
									}
									corrmode=SUB;
								}
								break;
					case 0x0f: 	turns_done=0;
							 	total_turns_done=0;
								turns_before_corr=0;
								total_turns_done_before_corr=0;
								action=INACTIVE;
								break;
			   		case 0xff:
			   		case 0xfd:
			   		case 0xfe: break;
					default:
								num=0;
								action=INACTIVE;
								itoa(corr,buf,10);
								if (corrmode==SUB) strcat(layer,"+");
								if (corrmode==ADD) strcat(layer,"+");
 								strcat (layer,buf);
								update_layer();
								break;
					}
    				update_command();
					update_turns();
							 
			}


			if (action==INACTIVE) {
				switch (c) {
					case 0x0a: 	wind_left();
								turns_prog=num;
								num=0;
								break;
					case 0x0b:  wind_right ();	
								turns_prog=num;
								num=0;
								break;
					case 0x0d:  fill ();	
								turns_prog=num;
								num=0;
								break;
					case 0x0c: 	num=0;					   
								stop_motor();
							   	break;
					case 0x0e: 	corr=num;
								turns_before_corr=turns_done;
								if (turns_done>=corr) turns_done-=corr;
								action=CORR;
								corrmode=SUB;
							   	break;
					case 0x0f:  helppage=0;
								show_help();
								action=HELP;
								break;
			   		case 0xff:
			   		case 0xfd:
			   		case 0xfe: break;

					default: if (num<1000)  {
							num=num*10 + c;						
							update_num();
							update_speed();
						}
				}
				if (action!=HELP)  {
    				update_command();
					update_turns();
					update_num();
					update_speed();
				}
			  } // if INACTIVE

		
			} // if c==prevc
	}  // mainloop
	
 return 0;
}
