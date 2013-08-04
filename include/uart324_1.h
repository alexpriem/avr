#ifndef UART324_1_H
#define UART324_1_H
#include <stdarg.h>

#define UART1_BUFLEN 32

extern void uart1_init (void);
extern void uart1_putc (unsigned char c);
extern void uart1_puts (char *s);
extern unsigned char uart1_getc (void);
extern void uart1_printf(char *fmt, ...);

extern uint8_t uart1_pos, uart1_done, uart1_echo;
extern char uart1_buf[UART1_BUFLEN+2];



#endif
