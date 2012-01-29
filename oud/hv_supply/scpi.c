/*************************************************************************

**************************************************************************/
#include <ctype.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include "lcd.h"
#include "scpi.h"
#include "scpi_cmds.h"
#include "uart.h"


char return_buf[80];


#define NUMDIGITS 6
#define NUMFAC 1000000

#define DEBUG 1 


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
 val=0;

 unitnr=0;
 for (;;) {
     unit=&units[unitnr];
     u=unit->prefix;     
     l=strlen(u);
     
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


long scpi_apply_unit (long val, int unit)

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
 char  buf[10];

 *return_buf=0;
 key=scpi_parse_command (s,&s2);

  uart_puts ("\n\rcmd:");
 itoa (key,buf, 10);
 uart_puts (buf);
 uart_puts ("[");
 uart_puts (s2);

 retval=scpi_function (key, s2);
 if (*return_buf) {
     #ifdef DEBUG
         lcd_putc ('\n');
         lcd_puts (return_buf);
     #endif 
     uart_puts (return_buf);
    }

 return retval;
}









char *ltostr (long l, char *s)

{uint8_t fl;
 long l_msb;
 char v[32];
 char *t, *s2;

 
 l_msb=l/NUMFAC;
 ltoa (l_msb,s,10);

 s2=s;                   // kopieer s naar v
 t=v;
 while ((*t++=*s2++)!=0)
     continue;
 t--;
 *t++='.';              // decimale punt toevoegen
 
 ltoa (l-l_msb*NUMFAC,s,10);  // gedeelte achter de punt afhandelen.

 fl=strlen(s);
 while (fl<NUMDIGITS) {  // '1' voorzien van voorloopnullen (->'001')
     *t++='0';  fl++;
     }
 s2=s;
 while ((*t++=*s2++)!=0)      // fractie toevoegen.
     continue;
 *t=0;
 
 strcpy (s,v);
 
 return s; 
}



int  scpi_get_long_unit (char *s, long *val)

{int unit;
 uint8_t l;
 long nr,nr2, factor;
 char *s2, *s3, *s4;

 unit=atoi (s);
 nr=strtol(s,&s2,0);

 nr=nr*NUMFAC;
 s3=s2;
 if(*s2++=='.'){
     nr2=strtol(s2,&s3,0);
     l=s3-s2;
     factor=1;
     if (l<NUMDIGITS) {
         while (l<NUMDIGITS) {
             l++;  factor*=10;
             }
         nr2*=factor;
         }
     
     if (l>NUMDIGITS) {
         while (l>NUMDIGITS) {
             l--; factor*=10;
         }
         nr2=nr2/factor;
        }
     nr+=nr2;
     }

 unit=scpi_parse_unit(s3, UNIT_VOLT, &s4 );
 *val=scpi_apply_unit (nr, unit);
 
 return OK;
}


int  scpi_get_boolean (char *s, uint8_t *val)

{
// char *s2, *s3;
 
 return OK;
}
