
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include <stdlib.h>
#include "scpi.h" 
#include "scpi_cmds.h"

#include "hv_supply.h"

/* alles in deze file wordt tzt getemplated mbv een python-script. */




char key00[] PROGMEM = "OUTPut";
char key01[] PROGMEM = "INSTrument:NSEL?";
char key02[] PROGMEM = "INSTrument:NSEL";

char key03[] PROGMEM = "VOLTage?";
char key04[] PROGMEM = "VOLTage";
char key05[] PROGMEM = "CURRent?";
char key06[] PROGMEM = "CURRent";
char key07[] PROGMEM = "OVProt?";
char key08[] PROGMEM = "OVProt";
char key09[] PROGMEM = "STandby?";
char key10[] PROGMEM = "STandby";


char key11[] PROGMEM = "SWEep:STArt?";
char key12[] PROGMEM = "SWEep:STArt";
char key13[] PROGMEM = "SWEeP:STOP?";
char key14[] PROGMEM = "SWEep:STOP";
char key15[] PROGMEM = "SWEep:STEP?";
char key16[] PROGMEM = "SWEep:STEP";
char key17[] PROGMEM = "SWEep:TIME?";
char key18[] PROGMEM = "SWEep:TIME";

char key19[] PROGMEM = "SWEep:GO";
char key20[] PROGMEM = "RST";


PGM_P keywords[] PROGMEM={
    key00,
    key01,
    key02,
    key03,
    key04,
    key05,
    key06,
    key07,
    key08,
    key09,

    key10,
    key11,
    key12,
    key13,
    key14,
    key15,
 
    key16,
    key17,
    key18,
    key19,
	
	key20,

    NULL
    };




str_units  units[]={
    {"mV", UNIT_VOLT, -3},
    {"V",  UNIT_VOLT,  0},
    {"KV", UNIT_VOLT,  3},
    {"uA", UNIT_AMP, -6},
    {"mA", UNIT_AMP, -3},
    {"A",  UNIT_AMP,0},
    {"MS", UNIT_SEC, -3},
    {"S",  UNIT_SEC,  0},
    {"",   UNIT_END,0}
};


int scpi_dummy (char *s)
{
 return 0;
}





int scpi_function (uint8_t keynr, char *s)

{
   uint8_t retval;
    
    switch (keynr) {        
        case 1: retval=set_instrument_nsel(s);
                break;
        case 2: retval=get_instrument_nsel(s);
                break;

		case 3:  retval=get_voltage(s);
                break;
        case 4: retval=set_voltage(s);
                break;

        case 5: retval=get_current(s);
                break;
        case 6: retval=set_current(s);
                break;

        case 7: retval=get_ovp_voltage(s);
                break;
        case 8: retval=set_ovp_voltage(s);
                break;

        case 9: retval=get_standby(s);
                break;
        case 10: retval=set_standby(s);
                break;
				
				
				/*
        case 7: retval=get_sweep_start(s);
                break;
        case 8: retval=set_sweep_start(s);
                break;
        case 9: retval=get_sweep_stop(s);
                break;
        case 10: retval=set_sweep_stop(s);
                break;
        case 11: retval=get_sweep_step(s);
                break;
        case 12: retval=set_sweep_step(s);
                break;
        case 13: retval=get_sweep_time(s);
                break;
        case 14: retval=set_sweep_time(s);
                break;

        case 15: retval=do_sweep(s);
                break;
				*/
		case 20: retval=device_reset ();
				 break;
        default: retval=-2;
                break;
    }
   
    return retval;
}
    
