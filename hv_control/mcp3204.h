#ifndef _MCP3204_
#define _MCP3204_



#define ADC_SE 0x1
#define ADC_DIFF 0x0


void init_mcp3204(uint8_t chip, uint8_t cs, uint8_t clk,  uint8_t dout,  uint8_t din);
unsigned int mcp3204_getw (uint8_t chip, uint8_t adc, uint8_t mode);
unsigned int mcp3204_spiw (uint8_t chip, uint8_t adc, uint8_t mode);

#endif
