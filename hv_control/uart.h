#ifndef UART_H
#define UART_H
#include <stdarg.h>



extern void uart_init (void);
extern void uart_putc (unsigned char c);
extern void uart_puts (char *s);
extern void uart_printf(char *fmt, ...);

extern void lock (uint8_t *sem);
extern void unlock (uint8_t *sem);


extern uint8_t uart_len, uart_done, sem_uart;
extern unsigned char uartbuf[32];



#endif
