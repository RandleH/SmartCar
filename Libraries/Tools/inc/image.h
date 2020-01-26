/*--------------------------------------------------------------------------
* @File         image.h
* @Author       Randle_H
* @Version      V1.0.1
* @Date         2020/01/08
* @Software     IAR 8.32 or MDK 5.17
* @Target core  MK66FX1M0
* @Brief        This file provides all the declarations for image.c .
* @Instruction:
*                      __________________________
*            [0][0] ->|                          |<- [0][COL-1]
*                     |                          |
*                     |                          |
*                     | (Image Diagram in array) |
*                     |                          |
*                     |                          |
*        [ROW-1][0] ->|__________________________|<- [ROW-1][COL-1]
*
* COPYRIGHT NOTICE
 Copyright (c) 2019 Randle_H. All rights reserved.
----------------------------------------------------------------------------*/

#include "headfiles.h"
#include <math.h>
#include <stdlib.h>
#include "img_canny.h"
#include "img_bin.h"
#include "img_gussian.h"
#include "img_filter.h"
#include "img_stat.h"
#ifndef _IMAGE_TOOL
#define _IMAGE_TOOL


#define pi 3.14159265
#define PI pi

#define __PARAM_SAFETY_3X3(x,y)     if(y-1<0||x-1<0||y+1>ROW-1||x+1>COL-1)continue
#define __PARAM_SAFETY_5X5(x,y)     if(y-2<0||x-2<0||y+2>ROW-1||x+2>COL-1)continue    

void Image_SnapShot(uint8 row,uint8 col,uint8* source_img,uint8* output_img);

void Image_Bin          (uint8 row_start,uint8 col_start,uint8 row_end,uint8 col_end  ,BinConfigTypedef*     Bin_CFG);
void Image_AvgFilter    (uint8 row_start,uint8 col_start,uint8 row_end,uint8 col_end  ,GeneralFilterTypedef* Ger_CFG);
void Image_MidFilter    (uint8 row_start,uint8 col_start,uint8 row_end,uint8 col_end  ,GeneralFilterTypedef* Ger_CFG);
void Image_CannyEdge    (uint8 row_start,uint8 col_start,uint8 row_end,uint8 col_end  ,CannyConfigTypedef*   Canny_CFG);
void Image_GusBlur      (uint8 row_start,uint8 col_start,uint8 row_end,uint8 col_end  ,GussianConfigTypedef* Gus_CFG);
void Image_GrayStat     (uint8 row_start,uint8 col_start,uint8 row_end,uint8 col_end  ,GrayStatTypedef*      GrayStat_Data);


#endif
