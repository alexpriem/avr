#ifndef _MCP4922_
#define _MCP4922_



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


void init_mcp4922(uint8_t chip, uint8_t cs, uint8_t clk,  uint8_t data,uint8_t ldac);
void init_mcp4922_spi(uint8_t chip, uint8_t cs, uint8_t ldac);
void mcp4922_putw (uint8_t chip,  int16_t word, uint8_t dac);
void mcp4922_spiw (uint8_t chip,  int16_t word, uint8_t dac);

#endif
