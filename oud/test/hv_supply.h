#ifndef HV_SUPPLY_H
#define HV_SUPPLY_H


#define NUM_SUPPLIES 4    /* 0-400, 0-400, -100-0, 0-6  */


typedef struct {
    double voltage;
    double current;
    uint8_t mode;
    
    double swstart;
    double swstop;
    double swstep;
    uint8_t swtype;
    double swtime;
    uint8_t enable;    
} str_powersupply;


extern str_powersupply powersupply[NUM_SUPPLIES];


/* functiedefinities */
extern int get_voltage(char *s);
extern int set_voltage(char *s);
extern int get_current(char *s);
extern int set_current(char *s);
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


#endif /* HV_SUPPLY_H */
