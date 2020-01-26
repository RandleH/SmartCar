/*
TFT Screen Pins Defination:
------------------------------------ 
      TFT Pin             MCU Pin
------------------------------------
        SCL                 B11
        SDA                 B16
        RES                 B8
        DC                  B9
        CS                  B10
------------------------------------ 
*/

#include "headfiles.h"
#ifndef _ST7735_H
#define _ST7735_H

#define TFT_DC(x)  ((x==0)?(PTB->PDOR&=~(uint32)(1<<9)):(PTB->PDOR|=(uint32)(1<<9)))
#define TFT_RES(x) ((x==0)?(PTB->PDOR&=~(uint32)(1<<8)):(PTB->PDOR|=(uint32)(1<<8)))

#define TFT_X_MAX 128
#define TFT_Y_MAX 160
          
#define BACK_COLOR      GRAY
#define PEN_COLOR       RED

#define WHITE         	0xFFFF
#define BLACK         	0x0000	  
#define BLUE         	0x001F  
#define BRED             0XF81F
#define GRED 		0XFFE0
#define GBLUE		0X07FF
#define RED           	0xF800
#define MAGENTA       	0xF81F
#define GREEN         	0x07E0
#define CYAN          	0x7FFF
#define YELLOW           0xFFE0
#define BROWN 	        0XBC40 
#define BRRED 		0XFC07 
#define GRAY  	        0X8430 

#define CLEAN_TYPE      y_pos = 0

void TFT_Init(void);
void TFT_AllColor(void);
static void TFT_PORT_Configuration(void);
static void TFT_SPI_Configuration(void);
static void TFT_MOSI(uint8 data);
static void TFT_WriteDat(uint8 data);
void TFT_WriteDat16Bits(uint16 data);
static void TFT_WriteCmd(uint8 cmd);
void TFT_SetRegion(unsigned int x_start,unsigned int y_start,unsigned int x_end,unsigned int y_end);
static void TFT_Delay(long time);
void TFT_Fill(int color);
void TFT_ClearPage8X16(uint8 Page,uint8 x_start,uint8 x_end,int color);
void TFT_Char(uint8 x_start,uint8 y_start,uint8 letter);
void TFT_Printf(const char* format,...);
void TFT_Format(int x,int y);
#endif


