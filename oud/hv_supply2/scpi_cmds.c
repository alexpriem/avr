
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include <stdlib.h>
#include "lcd.h"
#include "scpi.h" 
#include "scpi_cmds.h"

#include "hv_supply.h"

/* alles in deze file wordt tzt getemplated mbv een python-script. */




char key00[] PROGMEM = "*CLS";
char key01[] PROGMEM = "*ESE";
char key02[] PROGMEM = "*ESE?";
char key03[] PROGMEM = "*IDN";
char key04[] PROGMEM = "*OPC";
char key05[] PROGMEM = "*OPC?";
char key06[] PROGMEM = "*RST";
char key07[] PROGMEM = "*SRE";
char key08[] PROGMEM = "*SRE?";
char key09[] PROGMEM = "*STB";
char key10[] PROGMEM = "*TST?";
char key11[] PROGMEM = "*WAI";

char key12[] PROGMEM = "VOLTage?";
char key13[] PROGMEM = "VOLTage";
char key14[] PROGMEM = "CURRent?";
char key15[] PROGMEM = "CURRent";
char key16[] PROGMEM = "SWEep:STArt?";
char key17[] PROGMEM = "SWEep:STArt";
char key18[] PROGMEM = "SWEeP:STOP?";
char key19[] PROGMEM = "SWEep:STOP";

char key20[] PROGMEM = "SWEep:STEP?";
char key21[] PROGMEM = "SWEep:STEP";

char key22[] PROGMEM = "SWEep:TIME?";
char key23[] PROGMEM = "SWEep:TIME";


char key24[] PROGMEM = "SWEep:GO";
char key25[] PROGMEM = "OUTPut";
char key26[] PROGMEM = "INSTrument:NSEL?";
char key27[] PROGMEM = "INSTrument:NSEL";



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
    key21,
    key22,
    key23,
    key24,
    key25,
    key26,
    key27,
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
/*        case 0: retval=scpi_dummy(s);
                break;
        case 1: retval=scpi_dummy(s);
                break;
        case 2: retval=scpi_dummy(s);
                break;
        case 3: retval=scpi_dummy(s);
                break;
        case 4: retval=scpi_dummy(s);
                break;
        case 5: retval=scpi_dummy(s);
                break;
        case 6: retval=scpi_dummy(s);
                break;
        case 7: retval=scpi_dummy(s);
                break;
        case 8: retval=scpi_dummy(s);
                break;
        case 9: retval=scpi_dummy(s);
                break;

        case 10: retval=scpi_dummy(s);
                break;
        case 11: retval=scpi_dummy(s);
                break;

*/
        case 12: retval=get_voltage(s);
                break;
        case 13: retval=set_voltage(s);
                break;

        case 14: retval=get_current(s);
                break;
        case 15: retval=set_current(s);
                break;

        case 16: retval=get_sweep_start(s);
                break;
        case 17: retval=set_sweep_start(s);
                break;
        case 18: retval=get_sweep_stop(s);
                break;
        case 19: retval=set_sweep_stop(s);
                break;
        case 20: retval=get_sweep_step(s);
                break;
        case 21: retval=set_sweep_step(s);
                break;
        case 22: retval=get_sweep_time(s);
                break;
        case 23: retval=set_sweep_time(s);
                break;

        case 24: retval=do_sweep(s);
                break;
        case 25: retval=set_output(s);
                break;
        case 26: retval=set_instrument_nsel(s);
                break;
        case 27: retval=get_instrument_nsel(s);
                break;
        default: retval=-2;
                break;
    }
   
    return retval;
}
    
