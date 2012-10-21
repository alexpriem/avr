#include <inttypes.h>
#include <avr/io.h>
 
#include "uart.h"
#include "hc595p.h"
#include "led595p.h"



// 0, 1  , 2 , 3 ,
		// 4, 5, 6, 7, 
		// 8, 9k


unsigned char numbers[11]={
   0xbb,  // 0
   0xa0,  // 1
   0x37,  // 2
   0xb5,  // 3
   0xac,  // 4
   0x9d,  // 5
   0x9f,  // 6
   0xb0,  // 7
   0xbf,  // 8
   0xbd,  // 9
   0x00     // blank
   };

	 

unsigned char hex_numbers[16]={
   0xbb,  // 0
   0xa0,  // 1
   0x37,  // 2
   0xb5,  // 3
   0xac,  // 4
   0x9d,  // 5
   0x9f,  // 6
   0xb0,  // 7
   0xbf,  // 8
   0xbd,  // 9
   
   0x5f,  // a
   0x7c,  // b
   0x58,  // c
   0x5e,  // d
   0x7b,  // e
   0x71,  // f
   };
   

   unsigned char letters[27]={
   0x5f,  // a
   0x7c,  // b
   0x58,  // c
   0x5e,  // d
   0x7b,  // e
   0x71,  // f
    
   0x6f,  // g
   0x74,  // h
   0x04,  // i
   0x0c,  // j
   0x76,  // k
   0x38,  // l
   0x54,  // m
   0x54,  // n
   0x5c,  // o
   0x73,  // p
   0x67,  // q
   0x50,  // r
   0x6d,  // s
   0x78,  // t
   0x1c,  // u
   0x3e,  // v
   0x7e,  // w
   0x76,  // x
   0x6e, // y
   0x5b, // z
   0x00 // blank
   };
   
   
   unsigned char special_up[27]={
   0x61,  // 
   0x23,  // 
   0x43,  // 
   0x62,  // 
   };  // 
   
   unsigned char special_down[27]={
   0x4c,  // 
   0x54,  // 
   0x58,  // 
   0x1c,  // 
   };  // 
   
   unsigned char special_updown[6]={
   0x3e,  // 
   0x3d,  // c
   0x3b,  //    
   0x37,  // d
   0x2f, 
   0x1f,
   };  // j





void led595p_put4 (uint8_t chip, unsigned long c)

{
 hc595p_putc(chip,c>>24, (c>>16) & 0xff, (c>>8) & 0xff, c & 0xff); 
}




void led595p_put_unumber (uint8_t chip, unsigned int num)
{
 uint8_t digit;
 int orignum;
 unsigned long w;
 
 w=0;
 if (num>10000)  {
	w=0x5c38715c;  /* 'oflo' */
	led595p_put4 (chip, w);
	return;
	}
 orignum=num;
 
 if (orignum>=1000) {
	digit=0;
	while (num>=1000) { digit++; num-=1000;}
	w=numbers[digit];
	}
 if (orignum>=100) {
	digit=0;
	while (num>=100) { digit++; num-=100;}
	w|=(unsigned long)numbers[digit]<<8;
 }
 if (orignum>=10) {
	digit=0;
	while (num>=10) { digit++; num-=10;}
	w|=(unsigned long)numbers[digit]<<16;
 }
 w|=(unsigned long)numbers[num]<<24;
 
 led595p_put4 (chip, w);

}

void led595p_put_snumber (uint8_t chip, int num)
{
 unsigned long w;
 int orignum;
 uint8_t digit;
 
 w=0x00;
 orignum=num;
 if (num>=10000)  {
	w=0x5c38715c;  /* 'oflo' */
	led595p_put4 (chip, w);
	return;
	}
if (num<=-1000)  {
	w=0x5c38711c;  /* 'uflo' */
	led595p_put4 (chip, w);
	return;
	}
	
 if (num<=0) { 
	num=-num;
	w=0x40;
	}
 if (num>=1000) {
	digit=0;
	while (num>=1000) { digit++; num-=1000;}
	w=numbers[digit];
 }	

 
 if (orignum>=100) {
	digit=0;
	while (num>=100) { digit++; num-=100;}
	w|=(unsigned long)numbers[digit]<<8;
 }
 
 if (orignum>=10) {
	digit=0;
	while (num>=10) { digit++; num-=10;}
	w|=(unsigned long)numbers[digit]<<16;
 }
 w|=(unsigned long)numbers[num]<<24;

 led595p_put4 (chip, w);
}


void led595p_put_txt (uint8_t chip, char *s)
{
 unsigned long w;
 uint8_t c;

 c=s[0];
 if ((c & 0xf0)==0x30) {
	c-='0';
	w=(unsigned long) numbers[c];	
  } else {
	c=(c | 0x20) -'a'; 
	w=(unsigned long)letters[c];
 }
 
 c=s[1];
 if ((c & 0xf0)==0x30) {
	c-='0';
	w=(unsigned long) numbers[c]<<8;	
  } else {
	c=(c | 0x20) -'a'; 
	w|=(unsigned long)letters[c]<<8;
  }
  
  c=s[2];
  if ((c & 0xf0)==0x30) {
	c-='0';
	w=(unsigned long) numbers[c]<<16;	
  } else {
	c=(c | 0x20) -'a'; 
	w|=(unsigned long)letters[c]<<16;
  }
  
  c=s[3];
  if ((c & 0xf0)==0x30) {
	c-='0';
	w=(unsigned long) numbers[c]<<24;	
  } else {
	c=(c | 0x20) -'a'; 
	w|=(unsigned long)letters[c]<<24;
 }
 
 led595p_put4 (chip, w);
}

