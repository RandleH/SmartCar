/*--------------------------------------------------------------------------
* @file         img_gussian.h
* @author       Randle_H & Johann Carl Friedrich Gauß(1777 ~ 1855)
* @version      V1.0.1
* @date         2019/11/24
* @Software     IAR 8.32 or MDK 5.17
* @Target core  MK66FX1M0
* @brief        This file provides the declarations for canny edge configuration
* COPYRIGHT NOTICE
 Copyright (c) 2019 Randle_H. All rights reserved.
----------------------------------------------------------------------------*/
#ifndef _GUSSIAN_H

#define _GUSSIAN_H

typedef struct
{
  uint8  order;
  float  delta;
  uint8* source_img;
  uint8* output_img;
  bool cmd;
}GussianConfigTypedef;


#endif


