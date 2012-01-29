/*************************************************************************
Title:    et-lab interface
Author:   alex priem  (apriem@tubeness.nl)
File:     $Id: test_lcd.c,v 1.6 2004/12/10 13:53:59 peter Exp $
Software: AVR-GCC 3.3
Hardware: ATmega32
          
**************************************************************************/
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "lcd.h"
#include "scpi.h"
#include "scpi_cmds.h"
#include "uart.h"


char return_buf[80];



 #define _DEBUG_ 1 


/* ultra-light-variant, tot we beter begrijpen hoe scpi werkt */






int scpi_parse_command (char *s, char **new_s)


{
 uint8_t i,j,c,l,sl, keynr=0, keyval;
 char key[32], shortkey[32], *t;

 
 keyval=-1;
 t=(char *)pgm_read_word (&(keywords[keynr]));
 while (t!=NULL) {
    
    strcpy_P(key, (PGM_P) t );
    /* lange en korte variant isoleren */
    l=strlen(key);

    j=0;
    for (i=0; i<l; i++){
         c=key[i];
         
         if (islower(c))
             continue;
         shortkey[j]=c;
         j++;
    }
    sl=j;
    shortkey[j]=' ';
    shortkey[j+1]=0;

#ifdef _DEBUGD_             
    uart_puts(shortkey);
#endif

    /* de lange variant staat in buf; de korte variant staat in buf_upc */
    if (strncasecmp(key, s,l)==0) {
        keyval=keynr;
        *new_s=s+l;
        break;
        }
    if  (strncasecmp(shortkey, s,sl)==0) {
        keyval=keynr;
        *new_s=s+sl;
        break;
        }
    
    keynr++;
    t=(char *)pgm_read_word (&(keywords[keynr]));
    
}

 return keyval;
}





int scpi_parse_unit (char *s, uint8_t unittype, char **s2)

{
 int val;
 uint8_t unitnr,l;
 char *t, *u;
 str_units *unit;
 

 t=s;

 while (*t==' ') t++;
 lcd_putc ('X');
 lcd_puts (t);
 val=0;

 unitnr=0;
 lcd_clrscr();
 for (;;) {
     unit=&units[unitnr];
     u=unit->prefix;     
     l=strlen(u);
     lcd_putc('=');
     lcd_puts(u);
     
     
     if (unit->unittype==UNIT_END){
         *s2=NULL;
         return 0;
         }
     if (unit->unittype!=unittype) {
         unitnr++;
         }
     else {
         
         
         if (strncasecmp(u, t,l)==0) {
             *s2=t+l;
             return unit->factor;
            }
         unitnr++;
         }
     }
         
 return val;
}


double scpi_apply_unit (double val, int unit)

{
 if (unit==0) return val;
 if (unit==-3) return val/1000;
 if (unit==-6) return val/1000000;
 if (unit==3) return val*1000;
 if (unit==6) return val*1000000;
 
 return val;
}






int scpi_parse_line (char *s)
{
 uint8_t retval,key;
 char *s2;
#ifdef _DEBUG_
 char  buf[10];
#endif
    
 key=scpi_parse_command (s,&s2);
#ifdef _DEBUG_
 uart_puts ("cmd:");
 itoa (key,buf, 10);
 uart_puts (buf);
 uart_puts ("[");
 uart_puts (s2);
#endif 
 retval=scpi_function (key, s2);
 if (return_buf[0]!=0) {
     #ifdef _DEBUG_
         uart_putc ('\n');
         uart_puts (return_buf);
     #endif 
     uart_puts (return_buf);
    }

 return retval;
}



int  scpi_get_double_unit (char *s, double *val)

{double tmp;
 int unit;
 char *s2, *s3;
 
 tmp=strtod(s,&s2);
 if ((s==s2) || (errno==ERANGE)) return ERROR;
 unit=scpi_parse_unit(s2, UNIT_VOLT, &s3 );
 *val=scpi_apply_unit (tmp, unit);
 return OK;
}
