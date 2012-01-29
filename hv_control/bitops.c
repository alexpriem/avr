#include <inttypes.h>
#include <avr/io.h>
#include "bitops.h"
#include "uart.h"


uint8_t pbit_get(uint8_t portbit)

{
 uint8_t bit;

 bit=1<<(portbit & 0x07);
 switch (portbit>>4) {
     case 0: return (PINA & bit);
     case 1: return (PINB & bit);
     case 2: return (PINC & bit); 
     case 3: return (PIND & bit); 
#ifdef PORTE
     case 4: return (PINE & bit); 
#endif
#ifdef PORTF
     case 5: return (PINF & bit); 
#endif     
     }
 return 0;
}


uint8_t pbit_set(uint8_t portbit)

{
 uint8_t bit;
              
 bit=1<< (portbit & 0x07);
 //uart_printf ("set port %c %d/%d\r\n",'a'+(portbit>>4), portbit & 0x07, bit);
 switch (portbit>>4) {
     case 0: bit_set(PORTA, bit); break;
     case 1: bit_set(PORTB, bit); break;
     case 2: bit_set(PORTC, bit); break;
     case 3: bit_set(PORTD, bit); break;
#ifdef PORTE
     case 4: bit_set(PORTE, bit); break;
#endif
#ifdef PORTF
     case 5: bit_set(PORTF, bit); break;
#endif     
     }
 return 0;
}




uint8_t pbit_clr(uint8_t portbit)

{
 uint8_t bit;

 bit=1<< (portbit & 0x07);
// uart_printf ("clr port %c %d\r\n",'a'+(portbit>>4), portbit & 0x07); 
 switch (portbit>>4) {
     case 0: bit_clr(PORTA, bit); break;
     case 1: bit_clr(PORTB, bit); break;
     case 2: bit_clr(PORTC, bit); break;
     case 3: bit_clr(PORTD, bit); break;
#ifdef PORTE
     case 4: bit_clr(PORTE, bit); break;
#endif
#ifdef PORTF
     case 5: bit_clr(PORTF, bit); break;
#endif     
     }
 return 0;
}
