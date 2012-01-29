
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "bitops.h"
#include "uart.h"
#include "delay.h"

#include "uc_info.h"



#define NUM_74HC595 4






uint8_t clk[NUM_SPI];
uint8_t data[NUM_SPI];
uint8_t cs[NUM_SPI];





void init_mcp4922(uint8_t chip, uint8_t clk, uint8_t cs, uint8_t data, uint8_t ldac)

{
 clk[chip]= 1<< clk;
 data[chip]= 1<< data;
 cs[chip]=1 <<cs;
 ldac[chip]=1<< ldac;
}





void mcp4922_putc (uint8_t chip, unsigned char c)


{
 uint8_t clk, enable, data;
 uint8_t i;
 int word;

 clk=clk[chip];
 data=data[chip];
 enable=cs[chip];

 bit_clr(PORTC, clk);
 bit_clr(PORTC, enable);
 bit_clr (PORTC, data);
 


 for (i=0; i<16; i++) {    
    if ((c & 0x80) ==0) {
        bit_clr (PORTC, data);
    } else {
        bit_set (PORTC, data);
    }          
    udelay(10);
    bit_set(PORTC, clk);
    udelay(10);
    bit_clr(PORTC, clk);
    udelay(20);

    
    }
     
}




