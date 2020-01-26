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

#include "ST7735.h"
#include "stdarg.h"
#include "stdio.h"
#include "tft_font.h"
static char sprint_buf[32];
static int x_pos;
static int y_pos;
static void TFT_PORT_Configuration(void)
{
  SIM->SCGC5 |= (SIM_SCGC5_PORTB_MASK);
  //Signal Multiplexing and Pin Assignments  [Chapter10.3.1  Page:243]
  PORTB->PCR[11] = ALT2;        //SPI1_SCK
  PORTB->PCR[16] = ALT2;        //SPI1_SOUT
  PORTB->PCR[10] = ALT2;        //SPI1_PCS0
  PORTB->ISFR    = (uint32)((1<<10)|(1<<11)|(1<<16));
  PTB->PDDR     &= ~(uint32)((1<<10)|(1<<11)|(1<<16));
  PORTB->PCR[8]  = ALT1;        //GPIO_RES
  PORTB->PCR[9]  = ALT1;        //GPIO_DC
  PTB->PDDR     |= (uint32)((1<<8)|(1<<9));
}

static void TFT_SPI_Configuration(void)
{
  SIM->SCGC6 |= SIM_SCGC6_SPI1_MASK;
  //Module Configuration Register       [Chapter50.3.1  Page:1413]
  SPI1->MCR = ( 0
                | SPI_MCR_CLR_TXF_MASK    //Clear the TX FIFO counter
                | SPI_MCR_CLR_RXF_MASK    //Clear the RX FIFO counter
                | SPI_MCR_HALT_MASK);     //Stop transfers
  SPI1->MCR = ( 0
                | SPI_MCR_MSTR_MASK         //Master Mode Select
                | SPI_MCR_PCSIS(1<<0)        //PCS0
                | SPI_MCR_PCSIS_MASK);    //The inactive state of PCSx is high
  //DSPI Status Register                [Chapter50.3.5  Page:1420]
  SPI1->SR = (0
                | SPI_SR_EOQF_MASK        //End of Queue Flag
                | SPI_SR_TFUF_MASK        //Transmit FIFO Underflow Flag
                | SPI_SR_TFFF_MASK        //Transmit FIFO Fill Flag
                | SPI_SR_RFOF_MASK        //Receive FIFO Overflow Flag
                | SPI_SR_RFDF_MASK);      //Receive FIFO Drain Flag
  //Clock Transfer Attributes Register  [Chapter50.3.3  Page:1413]
  SPI1->CTAR[0] = ( 0
                | SPI_CTAR_PBR(0)         //Baud Rate Prescaler
                | SPI_CTAR_BR(0)          //Baud Rate Scaler
                | SPI_CTAR_PDT(0)         //Delay after Transfer Prescaler
                | SPI_CTAR_FMSZ(7)        //Frame Size = 7 bits
                | SPI_CTAR_PCSSCK(0)      //PCS to SCK Delay Prescaler
                | SPI_CTAR_CSSCK(0));      //PCS to SCK Delay Scaler
  //Start Transfer
  SPI1->MCR &= (~(uint32)(SPI_MCR_HALT_MASK));
}

static void TFT_MOSI(uint8 data)
{
  do
  {
    SPI1->SR   = (0
                     | SPI_SR_EOQF_MASK    
                     | SPI_SR_TFUF_MASK    
                     | SPI_SR_TFFF_MASK    
                     | SPI_SR_RFOF_MASK    
                     | SPI_SR_RFDF_MASK);
    SPI1->MCR |= (0
                     | SPI_MCR_CLR_TXF_MASK  
                     | SPI_MCR_CLR_RXF_MASK);
  }while((SPI1->SR & SPI_SR_RFDF_MASK)); 
  SPI1->PUSHR  = (0
                     | SPI_PUSHR_CTAS(0)        
                     | SPI_PUSHR_PCS(1<<0)
                     | SPI_PUSHR_EOQ_MASK       
                     | SPI_PUSHR_TXDATA(data));
  while(!(SPI1->SR & SPI_SR_EOQF_MASK));//Wait for transfer completed
  while(!(SPI1->SR & SPI_SR_RFDF_MASK));
  SPI1->SR |= SPI_SR_RFDF_MASK;   
}

static void TFT_WriteDat(uint8 data)
{
  TFT_DC(1);
  TFT_MOSI(data);
}

void TFT_WriteDat16Bits(uint16 data)
{
  uint8 H,L;
  H=(uint8)(data>>8);
  L=(uint8)(data&0x00ff);
  TFT_DC(1);
  TFT_MOSI(H);
  TFT_MOSI(L);
}

static void TFT_WriteCmd(uint8 cmd)
{
  TFT_DC(0);
  TFT_MOSI(cmd);
}

static void TFT_Delay(long time)
{
  while(time--);
}

void TFT_SetRegion(unsigned int x_start,unsigned int y_start,unsigned int x_end,unsigned int y_end)
{	
  TFT_WriteCmd(0x2a);
  TFT_WriteDat(0x02);
  TFT_WriteDat(x_start);
  TFT_WriteDat(0x02);
  TFT_WriteDat(x_end);

  TFT_WriteCmd(0x2b);
  TFT_WriteDat(0x01);
  TFT_WriteDat(y_start);
  TFT_WriteDat(0x01);
  TFT_WriteDat(y_end);	

  TFT_WriteCmd(0x2c);
}

void TFT_Fill(int color)
{
  uint8 i,j;
  TFT_SetRegion(0,0,TFT_X_MAX-1,TFT_Y_MAX-1);
  for (i=0;i<TFT_Y_MAX;i++)
    for (j=0;j<TFT_X_MAX;j++)
      TFT_WriteDat16Bits(color);
}

void TFT_ClearPage8X16(uint8 Page,uint8 x_start,uint8 x_end,int color)
{
  uint8 i,j;
  TFT_SetRegion(Page*16,x_start,Page*16+16-1,x_end);
  for (i=x_start;i<x_end;i++)
  {
    for (j=0;j<16;j++)
      TFT_WriteDat16Bits(color);
  }
}

void TFT_AllColor(void)
{
  uint8 i,j;
  float R=255.0,G=0.0,B=0.0;//R[0:255],G[0:255],B[0:255]
  uint16 color;
  TFT_SetRegion(0,0,TFT_X_MAX-1,TFT_Y_MAX-1);
  for(i=0;i<TFT_Y_MAX;i++)
  {
    for(j=0;j<TFT_X_MAX;j++)
    {
      switch((int)(j/(TFT_X_MAX/6.0)))
      {
      case 6:
      case 0:R=255.0;G+=(254/(TFT_X_MAX/6.0));B=0.0;break;
      case 1:R-=(254/(TFT_X_MAX/6.0));G=255.0;B=0.0;break;
      case 2:R=0.0;G=254.0;B+=(254/(TFT_X_MAX/6.0));break;
      case 3:R=0.0;G-=(254/(TFT_X_MAX/6.0));B=255.0;break;
      case 4:R+=(254/(TFT_X_MAX/6.0));G=0.0;B=255.0;break;
      case 5:R=255.0;G=0.0;B-=(254/(TFT_X_MAX/6.0));break;
      }
      if(R>255.0)       R=255.0;
      if(G>255.0)       G=255.0;
      if(B>255.0)       B=255.0;
      if(R<0.0)         R=0.0;
      if(G<0.0)         G=0.0;
      if(B<0.0)         B=0.0;
      color = ((uint16)(R*31/255))<<11;
      color|= ((uint16)(G*63/255))<<5;
      color|= (uint16)(B*31/255);
      TFT_WriteDat16Bits(color);
    }
  }
}

void TFT_Init(void)
{
  TFT_PORT_Configuration();
  TFT_SPI_Configuration();
    
  TFT_RES(0);
  TFT_Delay(999999);
  TFT_RES(1);
  TFT_Delay(999999);
  TFT_WriteCmd(0x11);
  TFT_Delay(999999);

  TFT_WriteCmd(0xB1); 
  TFT_WriteDat(0x01); 
  TFT_WriteDat(0x2C); 
  TFT_WriteDat(0x2D); 
  
  TFT_WriteCmd(0xB2); 
  TFT_WriteDat(0x01); 
  TFT_WriteDat(0x2C); 
  TFT_WriteDat(0x2D); 
  
  TFT_WriteCmd(0xB3); 
  TFT_WriteDat(0x01); 
  TFT_WriteDat(0x2C); 
  TFT_WriteDat(0x2D); 
  TFT_WriteDat(0x01); 
  TFT_WriteDat(0x2C); 
  TFT_WriteDat(0x2D); 
  
  TFT_WriteCmd(0xB4);
  TFT_WriteDat(0x07); 

  TFT_WriteCmd(0xC0); 
  TFT_WriteDat(0xA2); 
  TFT_WriteDat(0x02); 
  TFT_WriteDat(0x84); 
  TFT_WriteCmd(0xC1); 
  TFT_WriteDat(0xC5); 
  
  TFT_WriteCmd(0xC2); 
  TFT_WriteDat(0x0A); 
  TFT_WriteDat(0x00); 
  
  TFT_WriteCmd(0xC3); 
  TFT_WriteDat(0x8A); 
  TFT_WriteDat(0x2A); 
  TFT_WriteCmd(0xC4); 
  TFT_WriteDat(0x8A); 
  TFT_WriteDat(0xEE); 
  
  TFT_WriteCmd(0xC5);
  TFT_WriteDat(0x0E); 
  
  TFT_WriteCmd(0x36);
  //TFT_WriteDat(0xC0);
  //TFT_WriteDat(0x00);
  TFT_WriteDat(0x40);

  TFT_WriteCmd(0xe0); 
  TFT_WriteDat(0x0f); 
  TFT_WriteDat(0x1a); 
  TFT_WriteDat(0x0f); 
  TFT_WriteDat(0x18); 
  TFT_WriteDat(0x2f); 
  TFT_WriteDat(0x28); 
  TFT_WriteDat(0x20); 
  TFT_WriteDat(0x22); 
  TFT_WriteDat(0x1f); 
  TFT_WriteDat(0x1b); 
  TFT_WriteDat(0x23); 
  TFT_WriteDat(0x37); 
  TFT_WriteDat(0x00); 	
  TFT_WriteDat(0x07); 
  TFT_WriteDat(0x02); 
  TFT_WriteDat(0x10); 
  
  TFT_WriteCmd(0xe1); 
  TFT_WriteDat(0x0f); 
  TFT_WriteDat(0x1b); 
  TFT_WriteDat(0x0f); 
  TFT_WriteDat(0x17); 
  TFT_WriteDat(0x33); 
  TFT_WriteDat(0x2c); 
  TFT_WriteDat(0x29); 
  TFT_WriteDat(0x2e); 
  TFT_WriteDat(0x30); 
  TFT_WriteDat(0x30); 
  TFT_WriteDat(0x39); 
  TFT_WriteDat(0x3f); 
  TFT_WriteDat(0x00); 
  TFT_WriteDat(0x07); 
  TFT_WriteDat(0x03); 
  TFT_WriteDat(0x10);  
  
  TFT_WriteCmd(0x2a);
  TFT_WriteDat(0x00);
  TFT_WriteDat(0x00+2);
  TFT_WriteDat(0x00);
  TFT_WriteDat(0x80+2);
  
  TFT_WriteCmd(0x2b);
  TFT_WriteDat(0x00);
  TFT_WriteDat(0x00+3);
  TFT_WriteDat(0x00);
  TFT_WriteDat(0x80+3);
  
  TFT_WriteCmd(0xF0); 
  TFT_WriteDat(0x01); 
  TFT_WriteCmd(0xF6);
  TFT_WriteDat(0x00); 
  
  TFT_WriteCmd(0x3A);
  TFT_WriteDat(0x05); 
  
  
  TFT_WriteCmd(0x29);
  TFT_Fill(WHITE);
}

void TFT_Printf(const char* format,...)
{
  va_list args;
  int length,cnt;
  x_pos=0;
  va_start(args,format);
  length = vsprintf(sprint_buf,format,args);
  for(cnt=0;cnt<length;cnt++)
  {
    if(sprint_buf[cnt]=='\n')
    {
      TFT_ClearPage8X16(y_pos,(cnt-x_pos)*8,160,BACK_COLOR);
      y_pos++;
      x_pos = cnt;
      x_pos++;
      continue;
    }
    TFT_Char((cnt-x_pos)*8,y_pos*16,sprint_buf[cnt]);
  }
  TFT_ClearPage8X16(y_pos,(cnt-x_pos)*8,160,BACK_COLOR);
  va_end(args);
}

void TFT_Format(int x,int y)
{
  x_pos = x;
  y_pos = y;
}

void TFT_Char(uint8 x_start,uint8 y_start,uint8 letter)
{
  uint8 i,j;
  uint8 temp_h,temp_l;
  TFT_SetRegion(y_start,x_start,y_start+16-1,x_start+8-1);
  for(i=0;i<8;i++)
  {
    temp_h = F8X16[(letter-32)*16+i];   //Show Up part of letter
    temp_l = F8X16[(letter-32)*16+8+i]; //Show Down part of letter
    for(j=0;j<8;j++)
    {
      if(temp_h&0x01)
      {
        //TFT_SetRegion(y_start+j,x_start+i,y_start+j+1,x_start+i);
        TFT_WriteDat16Bits(PEN_COLOR);
      }
      else
        TFT_WriteDat16Bits(BACK_COLOR);
      temp_h>>=1;
    }
    for(j=0;j<8;j++)
    {
      if(temp_l&0x01)
      {
        //TFT_SetRegion(y_start+j+8,x_start+i,y_start+j+8+1,x_start+i);
        TFT_WriteDat16Bits(PEN_COLOR);
      }
      else
        TFT_WriteDat16Bits(BACK_COLOR);
      temp_l>>=1;
    }
  }
}


