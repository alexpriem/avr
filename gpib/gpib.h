#ifndef _GPIB_
#define _GPIB_

#define SRQ  0x01
#define ATN  0x02
#define EOI  0x04
#define DAV  0x08
#define NRFD 0x10
#define NDAC 0x20
#define IFC  0x40
#define REN  0x80

#define GPIB_TEBITS 0x08
#define GPIB_PEBITS 0x10
#define GPIB_VCC    0x20
#define GPIB_TEPROG 0x40
#define GPIB_DCPROG 0x80


#define TALK_MASK  0x40
#define TALK_BITS  0xff
#define LISTEN_BITS 0x00
#define LISTEN_MASK 0x20

#define TALK_DDR   (REN | IFC | DAV | EOI | ATN | SRQ) 
#define LISTEN_DDR (REN | IFC | NDAC | NRFD) 


#endif
