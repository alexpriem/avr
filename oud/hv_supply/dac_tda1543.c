#include <stdlib.h>
#include "lcd.h"
#include "uart.h"

#include <stdlib.h>
#include "ad_da.h"
#include "uart.h"
#include "bitops.h"
#include "scpi.h"    // ERROR/ OK

#define NUM_CHIPS 4
#define NUM_DACS 8

uint8_t chipws[NUM_CHIPS];
uint8_t chipbck[NUM_CHIPS];
uint8_t chipdata[NUM_CHIPS];
uint8_t chip[NUM_DACS];
uint8_t chipdac1[NUM_CHIPS];
uint8_t chipdac2[NUM_CHIPS];
uint8_t dacval[NUM_DACS]={0,0, 0,0, 0,0, 0,0};

static int nrchips=0;

//            uint8_t cs, uint8_t clk, uint8_t data)
void init_tda1543 (uint8_t dac1,  uint8_t dac2,
                   uint8_t bck, uint8_t ws, uint8_t data)
{
 uint8_t temp;

  chip[dac1]=nrchips;
  chip[dac2]=nrchips;
  if (dac1>dac2) { temp=dac1; dac1=dac2; dac2=temp; }
  
  chipdac1[nrchips]=dac1;
  chipdac2[nrchips]=dac2;
 
  
  bck = 1<<bck;
  ws = 1<<ws;
  data=1<<data;

  chipws[nrchips]  = ws;
  chipbck[nrchips] = bck;
  chipdata[nrchips]= data;

  nrchips++;
  bit_set (DDRA, bck|ws|data); 
}
    



 



void set_tda1543 (uint8_t dac,  uint16_t val)

{
 uint8_t dac1, dac2, val1, val2, chipnr;
    
 chipnr=chip[dac];
 dac1=chipdac1[chipnr];
 dac2=chipdac2[chipnr];

 val1=dacval[dac1];
 val2=dacval[dac2];
 if (dac1==dac) val1=val; 
 if (dac2==dac) val2=val;
 
 set_tda1543_2 (dac1, val1, dac2, val2);
}


int set_tda1543_2 (uint8_t dac1,  uint16_t val1,  uint8_t dac2, uint16_t val2)

{
 uint8_t  chipnr, ws, bck, data, i;
 uint16_t val;

 chipnr=chip[dac1];
 if (chip[dac2]!=chipnr) return ERROR;
 dacval[dac1]=val1;
 dacval[dac2]=val2;


 ws  = chipws[chipnr];
 bck = chipbck[chipnr];
 data= chipdata[chipnr];
 
 

 bit_clr (PORTA, ws);      // ws=0
 bit_clr (PORTA, bck);     // klok laag

 val=val1;
 for (i=0; i<17; i++) {
     if (val & 0x8000) bit_set (PORTA, data);
     else bit_clr (PORTA, data);
     val=val<<1;
     bit_set (PORTA, bck); delay ();    // uitklokken.
     bit_clr(PORTA, bck); delay();
 }
 bit_set (PORTA, ws);         

 if (val & 0x8000) bit_set (PORTA, data);  // lsb uitklokken.
 else bit_clr (PORTA, data);
 bit_set (PORTA, bck); delay ();    // uitklokken.
 bit_clr(PORTA, bck); delay(); 
 

 val=val2;
 for (i=0; i<17; i++) {
//     if (val & 0x8000) bit_set (PORTA, data);
//     else bit_clr (PORTA, data);
     bit_clr(PORTA, data);
     val=val<<1;
     bit_set (PORTA, bck); delay ();    // uitklokken.
     bit_clr(PORTA, bck); delay(); 
 }
 bit_clr (PORTA, ws);         

 bit_set (PORTA, bck); delay ();    // uitklokken.
 bit_clr(PORTA, bck); delay(); 
 
 return OK;
}
