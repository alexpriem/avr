#ifndef _MCP3201_
#define _MCP3201_



#define ADC_A 0x1
#define ADC_B 0x0
#define POS 0x0
#define NEG 0x1
#define ADC_SE 0x1
#define ADC_DIFF 0x0


void init_mcp3202(uint8_t chip, uint8_t cs, uint8_t clk,  uint8_t dout,  uint8_t din);
unsigned int mcp3202_getw (uint8_t chip, uint8_t adc, uint8_t mode);
unsigned int mcp3202_spiw (uint8_t chip, uint8_t adc, uint8_t mode);

#endif
