#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

extern void encoder_setup (uint8_t encoder, uint8_t bit0, uint8_t bit1);

extern int8_t encoder_poll (uint8_t encoder);
extern int8_t encoder_poll_range (uint8_t encoder, int8_t *var, int8_t min, int8_t max, int8_t step);


#endif //ROTARY_ENCODER_H
