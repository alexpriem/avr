#ifndef _PGA2311_H_
#define _PGA2311_H_


extern void pga2311_setup(uint8_t chip, uint8_t zcen, uint8_t cs, uint8_t sdi, uint8_t clk, uint8_t mute);
extern void pga2311_init (uint8_t chip);
extern void pga2311_mute (uint8_t chip);
extern void pga2311_set_gain (uint8_t chip, uint8_t vol_l, uint8_t vol_r );


#endif // _PGA2311_H_
