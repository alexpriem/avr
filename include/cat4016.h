#ifndef _CAT4016_H_
#define _CAT4016_H_

void init_cat4016(uint8_t chip, uint8_t clk, uint8_t cs, uint8_t data, uint8_t blank);
void  cat4016_putc (uint8_t chip, unsigned char c);
void  cat4016_put4 (uint8_t chip, unsigned long c);
void  cat4016_put_unumber (uint8_t chip, unsigned int c);
void  cat4016_put_snumber (uint8_t chip, int c);
void  cat4016_put_txt (uint8_t chip, char *s);
void  cat4016_write (uint8_t numchips);
void  cat4016_set_val (uint8_t chip, unsigned char c);

#endif
