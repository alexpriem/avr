#ifndef UART_H
#define UART_H
#include <stdarg.h>

#define UART_BUFLEN 32

extern void uart_init (void);
extern void uart_putc (unsigned char c);
extern void uart_puts (char *s);
extern unsigned char uart_getc (void);
extern void uart_printf(char *fmt, ...);

extern uint8_t uart_pos, uart_done;
extern char uart_buf[UART_BUFLEN];



#endif
