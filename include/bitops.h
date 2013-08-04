#ifndef _BITOPS_H_
#define _BITOPS_H_


#include <inttypes.h>

#define bit_get(p,m) ((p) & (m))
#define bit_set(p,m) ((p) |= (m))
#define bit_clr(p,m) ((p) &= ~(m))
#define bit_flip(p,m) ((p) ^= (m))
#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m))
#define BIT(x) (0x01 << (x))
#define LONGBIT(x) ((unsigned long)0x00000001 << (x))


uint8_t pbit_get(uint8_t portbit);
uint8_t pbit_set(uint8_t portbit);
uint8_t pbit_clr(uint8_t portbit);
uint8_t pbit_flip(uint8_t portbit);
uint8_t pbit_write(uint8_t portbit);


#define P_PORTMASK 0xF0
#define P_BITMASK  0x0F

#define P_PORTA 0x00
#define P_PORTB 0x10
#define P_PORTC 0x20
#define P_PORTD 0x30
#define P_PORTE 0x40
#define P_PORTF 0x50
#define P_PORTG 0x60
#define P_PORTH 0x70


#define P_BIT0 0x01
#define P_BIT1 0x02
#define P_BIT2 0x04
#define P_BIT3 0x08
#define P_BIT4 0x10
#define P_BIT5 0x20
#define P_BIT6 0x40
#define P_BIT7 0x80


#define P_PA0 0x00
#define P_PA1 0x01
#define P_PA2 0x02
#define P_PA3 0x03
#define P_PA4 0x04
#define P_PA5 0x05
#define P_PA6 0x06
#define P_PA7 0x07

#define P_PB0 0x10
#define P_PB1 0x11
#define P_PB2 0x12
#define P_PB3 0x13
#define P_PB4 0x14
#define P_PB5 0x15
#define P_PB6 0x16
#define P_PB7 0x17

#define P_PC0 0x20
#define P_PC1 0x21
#define P_PC2 0x22
#define P_PC3 0x23
#define P_PC4 0x24
#define P_PC5 0x25
#define P_PC6 0x26
#define P_PC7 0x27

#define P_PD0 0x30
#define P_PD1 0x31
#define P_PD2 0x32
#define P_PD3 0x33
#define P_PD4 0x34
#define P_PD5 0x35
#define P_PD6 0x36
#define P_PD7 0x37


#define P_PE0 0x40
#define P_PE1 0x41
#define P_PE2 0x42
#define P_PE3 0x43
#define P_PE4 0x44
#define P_PE5 0x45
#define P_PE6 0x46
#define P_PE7 0x47

#define P_PF0 0x50
#define P_PF1 0x51
#define P_PF2 0x52
#define P_PF3 0x53
#define P_PF4 0x54
#define P_PF5 0x55
#define P_PF6 0x56
#define P_PF7 0x57


#define P_PG0 0x60
#define P_PG1 0x61
#define P_PG2 0x62
#define P_PG3 0x63
#define P_PG4 0x64
#define P_PG5 0x65
#define P_PG6 0x66
#define P_PG7 0x67


#define P_PH0 0x70
#define P_PH1 0x71
#define P_PH2 0x72
#define P_PH3 0x73
#define P_PH4 0x74
#define P_PH5 0x75
#define P_PH6 0x76
#define P_PH7 0x77


#endif /* _BITOPS_H_ */

