#ifndef _LEDS_
#define _LEDS_


void init_led(uint8_t led, uint8_t portbit);
void init_leds(uint8_t led);

void set_led (uint8_t led);
void clr_led (uint8_t led);

void set_leds (uint8_t led);

#endif
