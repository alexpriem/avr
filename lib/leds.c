
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "uart.h"
#include "delay.h"
#include "leds.h"


#include "bitops.h"

#define NUM_LEDS 16


volatile uint8_t *p_leds[NUM_LEDS];
uint8_t b_leds[NUM_LEDS];



// 1 led op willekeurige poort

void led_setup(uint8_t led, uint8_t portbit)

{
 uint8_t l;
 b_leds[led] = 1<< (portbit & 0x07);
 

 l=portbit & P_PORTMASK;
 if (l==P_PORTA) {p_leds[led]=&PORTA;}
 if (l==P_PORTB) {p_leds[led]=&PORTB;}
 if (l==P_PORTC) {p_leds[led]=&PORTC;}
 if (l==P_PORTD) {p_leds[led]=&PORTD;}
}


// 8 leds op een poort

void leds_setup (uint8_t leds)

{
 uint8_t l,led;

 l=leds & P_PORTMASK;
 for (led=0; led<8; led++) {
     b_leds[led]=1<<led;
     if (l==P_PORTA) {p_leds[led]=&PORTA;}
     if (l==P_PORTB) {p_leds[led]=&PORTB;}
     if (l==P_PORTC) {p_leds[led]=&PORTC;}
     if (l==P_PORTD) {p_leds[led]=&PORTD;}
    }

}


void set_led (uint8_t led)


{
 volatile uint8_t *pled;
 uint8_t bled;

 pled=p_leds[led];
 bled=b_leds[led];

 bit_set(*pled, bled);
}


void clr_led (uint8_t led)


{
 volatile uint8_t *pled;
 uint8_t bled;

 pled=p_leds[led];
 bled=b_leds[led];

 bit_clr(*pled, bled);
}


