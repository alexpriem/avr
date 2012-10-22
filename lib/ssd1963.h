#ifndef _SSD1963_H_
#define  _SSD1963_H_
extern void Set_Reg(uint8_t index , uint8_t value);
extern void TFT_M_SSD1963_Set_Index(uint8_t index);
extern void TFT_M_SSD1963_Write_Command(uint8_t cmd);
extern void TFT_M_SSD1963_Write_Data(uint16_t _data);
extern void TFT_M_SSD1963_Window_Set_320_240 ();
extern void TFT_M_SSD1963_Write_memory_start (); 
extern void TFT_M_SSD1963_Init_8bit_320_240 ();
#endif 