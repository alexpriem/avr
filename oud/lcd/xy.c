/****************************************************************************
 *  Title  :   rotary encoder library (polling and interrupt-driven version)
 *
 *  
 *
 ****************************************************************************/
 


#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>


unsigned char menuitems[6][20] PROGMEM={
		{"Heater turn on:"},
		{"HV turn on:"},
		{"HV ramp up:"},
	    {"HV turn off:"},
        {"Heater turn off:"}};
          
		


