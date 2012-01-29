#ifndef _AD_DA_H_
#define _AD_DA_H_
#include <stdint.h> 


extern void init_adc (uint8_t adc, uint8_t cs, uint8_t clk, uint8_t dout);
extern unsigned int  get_adc (uint8_t adc);
extern void delay (void);

extern void init_tda1543 (uint8_t dac1, uint8_t dac2, uint8_t bck, uint8_t ws, uint8_t data);

extern void set_tda1543 (uint8_t dac, uint16_t val);
extern int set_tda1543_2 (uint8_t dac1,  uint16_t val1,  uint8_t dac2, uint16_t val2);

#endif  /* _AD_DA_H_ */
