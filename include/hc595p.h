#ifndef _HC595P_H_
#define _HC595P_H_

void hc595p_setup (uint8_t chip, uint8_t clk, uint8_t cs, uint8_t data0, uint8_t data1, uint8_t data2, uint8_t data3);
void hc595p_putc (uint8_t chip, unsigned char c0, unsigned char c1, unsigned char c2, unsigned char c3);

#endif
