//
// Accesses register space of the controller, write command + data (8 bit)
//
void Set_Reg(uint8_t index , uint8_t value)
{
   TFT_RS = 0;
   TFT_DataPort = index;
   TFT_WR = 0;
   Delay_us(1);    // <-------------- only required during initialisation
   TFT_WR = 1;
   TFT_RS = 1;
   TFT_DataPort = value;
   TFT_WR = 0;
   Delay_us(1);    // <-------------- only required during initialisation
   TFT_WR = 1;
}

//
// Accesses register space of the controller and write command (8 bit)
//
void TFT_M_SSD1963_Set_Index(uint8_t index) 
{
   TFT_RS = 0;
   TFT_DataPort = index;
   TFT_WR = 0;
   Delay_us(1);    // <-------------- only required during initialisation
   TFT_WR = 1;
}


//
// Actually writting data (8 bit) after a "Set_Index"
//
void TFT_M_SSD1963_Write_Command(uint8_t cmd)
{
   TFT_RS = 1;
   TFT_DataPort = cmd;
   TFT_WR = 0;
   Delay_us(1);    // <-------------- only required during initialisation
   TFT_WR = 1;
}

//
// Write data from word
//
void TFT_M_SSD1963_Write_Data(uint16_t _data)
{
   TFT_RS = 1;
   TFT_DataPort = Lo(_data);
   TFT_WR = 0;
   Delay_us(1);    // <-------------- only required during initialisation
   TFT_WR = 1;
}

//
// Initialise full screen window 0,0 to 320,240 i.e. landscape
//
void TFT_M_SSD1963_Window_Set_320_240 ()
{
   TFT_M_SSD1963_Set_Index(0x2A);        // SET column address
   TFT_M_SSD1963_Write_Command(0x00);    // SET start column address=0
   TFT_M_SSD1963_Write_Command(0x00);
   TFT_M_SSD1963_Write_Command(0x01);    // SET end column address=320
   TFT_M_SSD1963_Write_Command(0x3F);

   TFT_M_SSD1963_Set_Index(0x2B);        // SET page address
   TFT_M_SSD1963_Write_Command(0x00);    // SET start page address=0
   TFT_M_SSD1963_Write_Command(0x00);
   TFT_M_SSD1963_Write_Command(0x00);    // SET end page address=240
   TFT_M_SSD1963_Write_Command(0xEF);
}

//
// Transfer image information from the host processor interface to the SSD1963
// starting at the location set by "TFT_SED_Window_Set".
//
void TFT_M_SSD1963_Write_memory_start () 
{
   TFT_M_SSD1963_Set_Index(0x2C);
}

//
// Initialise Winstar WF35CTIBCDB#000, SSD1963 with 10MHz crystal,
// 8bit interface and the orientation is LANDSCAPE
//
void TFT_M_SSD1963_Init_8bit_320_240 ()
{
   TFT_CS_Direction = 0;  // set this pin as output
   TFT_CS = 1;            // CS = 1

   TFT_RST_Direction = 0; // set this pin as output
   TFT_RST = 1;           // RST = 1
   
   TFT_RS_Direction  = 0; // set this pin as output
   TFT_RS = 1;            // RS = 1
   
   TFT_WR_Direction  = 0; // set this pin as output
   TFT_WR = 1;            // WR = 1
   
   TFT_RD_Direction  = 0; // set this pin as output
   TFT_RD = 1;            // RD = 1

   Delay_ms(100);
   
   TFT_RST = 0;           // H/W Reset >= 100us
   Delay_us(120);
   TFT_RST = 1;
   Delay_us(120);
   
   TFT_CS = 0;
   TFT_M_SSD1963_Set_Index(0x01);        //Software Reset
   Delay_ms(5);                          // Delay >= 5ms

   TFT_M_SSD1963_Set_Index(0x01);        //Software Reset
   Delay_ms(5);                          // Delay >= 5ms
   
   TFT_M_SSD1963_Set_Index(0x01);        //Software Reset
   Delay_ms(5);                          // Delay >= 5ms
   
   Set_Reg(0xE0, 0x01);    // START PLL

   Set_Reg(0xE0, 0x03);    // LOCK PLL

   TFT_M_SSD1963_Set_Index(0xB0);        // SET LCD MODE SET TFT 18Bits MODE

   TFT_M_SSD1963_Write_Command(0x0C);    // SET TFT MODE & hsync+Vsync+DEN MODE
   TFT_M_SSD1963_Write_Command(0x00);    // SET TFT MODE & hsync+Vsync+DEN MODE
   TFT_M_SSD1963_Write_Command(0x01);    // SET horizontal size=320-1 HightByte
   TFT_M_SSD1963_Write_Command(0x3F);    // SET horizontal size=320-1 LowByte
   TFT_M_SSD1963_Write_Command(0x00);    // SET vertical size=240-1 HightByte
   TFT_M_SSD1963_Write_Command(0xEF);    // SET vertical size=240-1 LowByte
   TFT_M_SSD1963_Write_Command(0x00);    // SET even/odd line RGB seq.=RGB

   Set_Reg(0xF0, 0x00);    // SET pixel data I/F format=8bit

   Set_Reg(0x3A, 0x60);    // SET R G B format = 6 6 6

   TFT_M_SSD1963_Set_Index(0xE2);        // SET PLL freq=113.33MHz
   TFT_M_SSD1963_Write_Command(0x22);
   TFT_M_SSD1963_Write_Command(0x03);
   TFT_M_SSD1963_Write_Command(0x04);

   TFT_M_SSD1963_Set_Index(0xE6);        // SET PCLK freq=6.5MHz/19MHz, pixel clk freq
   TFT_M_SSD1963_Write_Command(0x00);
   TFT_M_SSD1963_Write_Command(0xEA);
   TFT_M_SSD1963_Write_Command(0xEC);

   TFT_M_SSD1963_Set_Index(0xB4);        // SET HBP
   TFT_M_SSD1963_Write_Command(0x01);    // SET HSYNC Total = 440
   TFT_M_SSD1963_Write_Command(0xB8);
   TFT_M_SSD1963_Write_Command(0x00);    // SET HBP = 68
   TFT_M_SSD1963_Write_Command(0x44);
   TFT_M_SSD1963_Write_Command(0x0F);    // SET VBP 16 = 15 + 1
   TFT_M_SSD1963_Write_Command(0x00);
   TFT_M_SSD1963_Write_Command(0x00);    // SET Hsync pulse start position
   TFT_M_SSD1963_Write_Command(0x00);    // SET Hsync pulse subpixel start position

   TFT_M_SSD1963_Set_Index(0xB6);        // SET VBP
   TFT_M_SSD1963_Write_Command(0x01);    //SET Vsync total 265 = 264 + 1
   TFT_M_SSD1963_Write_Command(0x08);
   TFT_M_SSD1963_Write_Command(0x00);    // SET VBP = 18
   TFT_M_SSD1963_Write_Command(0x12);
   TFT_M_SSD1963_Write_Command(0x07);    // SET Vsync pulse 8 = 7 + 1
   TFT_M_SSD1963_Write_Command(0x00);    // SET Vsync pulse start position
   TFT_M_SSD1963_Write_Command(0x00);

   TFT_M_SSD1963_Window_Set_320_240;  // SET DISPLAY SIZE 320x240

   TFT_M_SSD1963_Set_Index(0x13); // SET normal mode

   TFT_M_SSD1963_Set_Index(0x29); // SET display on
}