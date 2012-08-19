#ifndef _LED595P_H_
#define _LED595P_H_

void  led595p_putc (uint8_t chip, unsigned char c);
void  led595p_put4 (uint8_t chip, unsigned long c);
void  led595p_put_unumber (uint8_t chip, unsigned int c);
void  led595p_put_snumber (uint8_t chip, int c);
void  led595p_put_txt (uint8_t chip, char *s);
void  led595p_write (uint8_t numchips);
void  led595p_set_val (uint8_t chip, unsigned char c);

#endif
