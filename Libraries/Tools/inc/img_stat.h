# include "string.h"
#ifndef _STAT_H
#define _STAT_H
typedef struct
{
  uint16 gray_Max;
  bool   Histogram_CMD;
  uint16 Histogram_backcolor;
  uint16 Histogram_textcolor;
  uint16 gray_Count[256];
  uint8  gray_Avg;
  uint8* source_img;
}GrayStatTypedef;

#endif