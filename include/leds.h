#ifndef _LEDS_
#define _LEDS_


void led_setup(uint8_t led, uint8_t portbit);
void leds_setup(uint8_t led);

void set_led (uint8_t led);
void clr_led (uint8_t led);

void set_leds (uint8_t led);

#endif
