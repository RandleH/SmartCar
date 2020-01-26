/*--------------------------------------------------------------------------
* @file         img_canny.h
* @author       Randle_H
* @version      V1.0.1
* @date         2019/11/24
* @Software     IAR 8.32 or MDK 5.17
* @Target core  MK66FX1M0
* @brief        This file provides the declarations for canny edge configuration
* COPYRIGHT NOTICE
 Copyright (c) 2019 Randle_H. All rights reserved.
----------------------------------------------------------------------------*/
#ifndef _CANNY_H
#define _CANNY_H
#define VERY_BIG_NUMBER 999
#include "MT9V032.h"
typedef enum
{
  sobel,
  prewitt,
  scharr,
  laplace,
}cannykernal_type;

typedef struct
{
  uint8 h_threshold;
  uint8 l_threshold;
  double gus_delta;
  uint8  gus_order;
  bool   gus_cmd;
  cannykernal_type kernal;
  uint8* source_img;
  uint8* output_img;
}CannyConfigTypedef;

 

inline void Canny_NonMaxSupression(uint8* canny_img,uint8 angle);



#endif


