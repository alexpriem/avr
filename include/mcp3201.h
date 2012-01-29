#ifndef _MCP3201_
#define _MCP3201_



#define DAC_A 0x8000
#define DAC_B 0
#define BUF   0x4000
#define UNBUF 0
#define GAIN  0
#define NOGAIN 0x2000
#define SHDN  0
#define NOSHDN 0x1000

#define DEFAULT (BUF| NOGAIN| NOSHDN)
#define CNTRL 0xF000


void init_mcp3201(uint8_t chip, uint8_t cs, uint8_t clk,  uint8_t data);
unsigned int mcp3201_getw (uint8_t chip);

#endif
