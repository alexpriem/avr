#include <inttypes.h>
#include <avr/io.h>
 
#include "uart.h"
#include "hc595.h"
#include "led595.h"


void  led595_putc (uint8_t chip, unsigned char c)
{}

void  led595_put4 (uint8_t chip, unsigned long c)
{}

void  led595_put_unumber (uint8_t chip, unsigned int c)
{}

void  led595_put_snumber (uint8_t chip, int c)
{}

void  led595_put_txt (uint8_t chip, char *s)
{}

void  led595_write (uint8_t numchips)
{}

void  led595_set_val (uint8_t chip, unsigned char c)
{}