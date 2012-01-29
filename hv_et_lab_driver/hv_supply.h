#ifndef HV_SUPPLY_H
#define HV_SUPPLY_H


#define NUM_SUPPLIES 4    /* 0-400, 0-400, -100-0, 0-6  */


extern char return_buf[80];


typedef struct {
    long voltage;
	long ovp_voltage;
    long current;
	uint8_t standby;
    uint8_t mode;
    
    long swstart;
    long swstop;
    long swstep;
    uint8_t swtype;
    long swtime;
    uint8_t enable;    
} str_powersupply;


extern uint8_t dev;
extern str_powersupply powersupply[NUM_SUPPLIES];


/* functiedefinities */
extern int get_voltage(char *s);
extern int set_voltage(char *s);
extern int get_current(char *s);
extern int set_current(char *s);
extern int get_ovp_voltage(char *s);
extern int set_ovp_voltage(char *s);
extern int get_standby(char *s);
extern int set_standby(char *s);


extern int get_sweep_start(char *s);
extern int set_sweep_start(char *s);
extern int get_sweep_stop(char *s);
extern int set_sweep_stop(char *s);
extern int get_sweep_step(char *s);
extern int set_sweep_step(char *s);
extern int get_sweep_time(char *s);
extern int set_sweep_time(char *s);
extern int do_sweep(char *s);
extern int set_output(char *s);
extern int set_instrument_nsel(char *s);
extern int get_instrument_nsel(char *s);

extern int device_reset (void);


#endif /* HV_SUPPLY_H */
