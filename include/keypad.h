#ifndef _KEYPAD_H_
#define _KEYPAD_H_

void keypad_3x4_setup ( uint8_t in1, uint8_t in2, uint8_t in3, uint8_t in4, 
					   uint8_t out1, uint8_t out2, uint8_t out3, uint8_t out4);
uint8_t keypad_3x4_getc (void);
void keypad_4x4_setup ( uint8_t in1, uint8_t in2, uint8_t in3, uint8_t in4, 
					   uint8_t out1, uint8_t out2, uint8_t out3, uint8_t out4);
uint8_t keypad_4x4_getc (void);

#endif /* _BITOPS_H_ */

