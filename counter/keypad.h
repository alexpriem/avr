#ifndef _KEYPAD_H_
#define _KEYPAD_H_

uint8_t getc_3x4_keypad (void);
void init_4x4_keypad ( uint8_t in1, uint8_t in2, uint8_t in3, uint8_t in4, 
					   uint8_t out1, uint8_t out2, uint8_t out3, uint8_t out4);
uint8_t getc_4x4_keypad (void);

#endif /* _BITOPS_H_ */

