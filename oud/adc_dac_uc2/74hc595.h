#ifndef _S74HC595_H_
#define _S74HC595_H_

void init_74hc595(uint8_t chip, uint8_t clk, uint8_t cs, uint8_t data);
void  s595_putc (uint8_t chip, unsigned char c);
void  s595_write (uint8_t numchips);
void  s595_set_val (uint8_t chip, unsigned char c);

#endif
