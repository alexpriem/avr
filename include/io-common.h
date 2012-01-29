#ifndef __IO_COMMON_
#define __IO_COMMON_

#define NUM_DEVICES 20

extern volatile uint8_t *p_cs[NUM_DEVICES];
extern volatile uint8_t *p_data[NUM_DEVICES];
extern volatile uint8_t *p_clk[NUM_DEVICES];

extern uint8_t b_cs[NUM_DEVICES];
extern uint8_t b_data[NUM_DEVICES];
extern uint8_t b_clk[NUM_DEVICES];
#endif