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
#include <ctype.h>
#include <math.h>
#include <errno.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
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





int scpi_parse_line (char *s)
{
 uint8_t retval,key;
 char *s2;
    
 key=scpi_parse_command (s,&s2);
#ifdef _DEBUG_
 uart_printf ("scpi_parse_line:%d s2:[%s]\r\n", key, s2);
#endif 
 if (key==255)  {
	uart_printf ("ERROR: unknown command");
	return E_CMD;
}
	
 if (key!=255)
	retval=scpi_function (key, s2);
 
 if (retval!=0) {
	uart_printf ("ERROR excuting cmd: %d\r\n", key, retval);
	switch (retval) {
		case E_UNTERMINATED: uart_printf ("line unterminated");
		break;
		case E_INTRANGE: uart_printf ("integer overrange");
		break;
		case E_LONGRANGE: uart_printf ("long overrange");
		break;
		case E_VOLTAGERANGE : uart_printf ("voltage out of range (0..512)");
		break;
		case E_CURRENTRANGE: uart_printf ("current out of range (0..2.0)");
		break;
		case E_OVPRANGE: uart_printf ("ovp out of range (0..512)");
		break;
		case E_STANDBYRANGE: uart_printf ("standby: only 1 or 0");		
		break;
		default: uart_printf ("unknown error (code %d)", retval);
		}
	uart_printf ("\r\n");		
 }
 return retval;
}




 //if (!((c=='\n') || (c==0) || (c==' '))) *err=1;
 
 // kijkt of er een digit in de buffer is. Spaties worden genegeerd.
 
uint8_t scpi_get_digit (char *s, int *err, char **s2) 

{char *t;
 uint8_t val=0;
 
 t=s;
#ifdef _DEBUG_
 uart_printf ("get_digit: [%s]\r\n", t);
#endif 
 while (*t==' ') t++;
 
 if (isdigit(*t)) {
  val=*t-'0';
  t++;
  }

 *s2=t;
 *err=0;
 return val;  
}

int scpi_check_buf_empty(char *s) 

{
 char *t;
 
 t=s;
 while (*t==' ') t++; 
 if ((*t=='\n') || (*t==0)) return TRUE;
 return FALSE;
}



long scpi_get_value (char *s, int *err, char **s2, uint8_t maxlen, int errorcode, long maxval)
{
 char *t;
 long tmp=0;
 uint8_t len=0;
 int8_t sign=1;
 
 *err=0;
 
 t=s;
 while (*t==' ') t++;
 
 sign=1;
 if (*t=='-') { sign=-1; t++; }
 if (*t=='+') { t++; }
 
  while (isdigit(*t)) {
	tmp=tmp*10+(*t-'0');
	len++;
	t++;
	if (len>maxlen) { 
		*err=errorcode;
		break;
		}
	}
  
  if ((maxval!=0) && (tmp>maxval)) *err=errorcode;
  if (sign==-1) tmp=-tmp;

 *s2=t;
#ifdef _debug_
 uart_printf ("get_val: %l %d\r\n", tmp, sign);
#endif 
 return tmp;
}



int scpi_get_int (char *s, int *err, char **s2)

{
 return (int) scpi_get_value (s, err, s2, 5, E_INTRANGE, 32767L);
}

long scpi_get_long (char *s, int *err, char **s2)
{
 return scpi_get_value (s, err, s2,10, E_LONGRANGE, 0);
}


long scpi_get_longfrac (char *s, int *err, char **s2)
{
 char *s3;
 long tmp,tmp2;
 
 tmp=scpi_get_value (s, err, s2,10, E_LONGRANGE, 0);
 uart_printf ("error:%d\r\n", *err);
 if (*err!=0) return tmp;
 
 tmp*=1000;
 if (**s2=='.') {
	(*s2)++;
	tmp2=scpi_get_value (*s2, err, &s3, 10, E_LONGRANGE, 0);
	if (tmp2!=0) {
		while (tmp2>1000)  tmp2=tmp2/10;
		while (tmp2<100) tmp2*=10;  	
		if (tmp>=0) 
			tmp+=tmp2;
		else
			tmp-=tmp2;
	}
 }
  
#ifdef _DEBUG_
  uart_printf ("longfrac: val:%l, error:%d\r\n", tmp, *err);
#endif
 *s2=s3;
 return tmp; 
}


