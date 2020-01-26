
#include "information.h"

void SHOW_Image(void)
{
  uint8* image = Car_Parameter.IMAGE.output_img;
  int i,j;
  uint16 color,temp;
  for(i=(TFT_X_MAX-ROW)/2+4;i<ROW;i++)
  {
    TFT_SetRegion(i,0,i,TFT_Y_MAX-1);
    for(j=0;j<TFT_Y_MAX;j++)
    {
      temp = *image;
      color  = (0x001f&((temp)>>3))<<11;
      color |= (((0x003f)&((temp)>>2))<<5);
      color |= (0x001f&((temp)>>3));
      TFT_WriteDat16Bits(color);
      image++;
    }
  }
}

void SHOW_ImageStat(void)
{
  if(Car_Parameter.IMAGE.statistic.Histogram_CMD==true)
  {
    TFT_SetRegion(0,0,TFT_X_MAX-1,TFT_Y_MAX-1);
    for (uint8 i=0;i<TFT_Y_MAX;i++)
      for (uint8 j=0;j<TFT_X_MAX;j++)
      {
        if((Car_Parameter.IMAGE.statistic.gray_Count[i*255/TFT_Y_MAX]/3)>=TFT_X_MAX-j)
          TFT_WriteDat16Bits(Car_Parameter.IMAGE.statistic.Histogram_textcolor);
        else
          TFT_WriteDat16Bits(Car_Parameter.IMAGE.statistic.Histogram_backcolor);
      }
  }
}

void SHOW_RoadInfo(void)
{
  uint8 i;
  const uint8 width = 4;
  for(i=Car_Parameter.ROAD.row_end;i>Car_Parameter.ROAD.row_start;i--)
  {
    if(i+8>=ROW) continue;
    
    TFT_SetRegion(i+8,Car_Parameter.ROAD.left_edge[i],
                  i+8,Car_Parameter.ROAD.left_edge[i]+width);
    for(uint8 j=0;j<width;j++)
      TFT_WriteDat16Bits(GREEN);
    
    TFT_SetRegion(i+8,Car_Parameter.ROAD.right_edge[i]-width,
                  i+8,Car_Parameter.ROAD.right_edge[i]);
    for(uint8 j=0;j<width;j++)
      TFT_WriteDat16Bits(BLUE);
  }
}

void SHOW_Calibration(void)
{
  
}






