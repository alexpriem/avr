#ifndef _S74HC595_H_
#define _S74HC595_H_

void hc595_setup(uint8_t chip, uint8_t clk, uint8_t cs, uint8_t data);
void hc595_putc (uint8_t chip, unsigned char c);
void hc595_write (uint8_t numchips);
void hc595_set_val (uint8_t chip, unsigned char c);

#endif
