#ifndef UART_H
#define UART_H



extern void uart_init (void);
extern void uart_putc (unsigned char c);
extern void uart_puts (char *s);


extern void lock (uint8_t *sem);
extern void unlock (uint8_t *sem);


extern uint8_t uart_len, uart_done, sem_uart;
extern unsigned char uartbuf[32];


#endif
