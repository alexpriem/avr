#ifndef _S74HC595_H_
#define _S74HC595_H_

void init_74hc595(uint8_t enc, uint8_t bit_1, uint8_t bit_2);
int8_t s595_putc (unsigned char c);

#endif
