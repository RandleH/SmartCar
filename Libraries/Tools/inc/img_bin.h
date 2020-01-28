/*--------------------------------------------------------------------------
* @file         img_bin.h
* @author       Randle_H
* @version      V1.0.1
* @date         2019/11/24
* @Software     IAR 8.32 or MDK 5.17
* @Target core  MK66FX1M0
* @brief        This file provides the declarations for binary image configuration
* COPYRIGHT NOTICE
 Copyright (c) 2019 Randle_H. All rights reserved.
----------------------------------------------------------------------------*/
#ifndef _BINARY_H
#define _BINARY_H


typedef struct
{
  uint8 threshold;
  float lumin;
  uint8* source_img;
  uint8* output_img;
  bool cmd;
}BinConfigTypedef;




#endif



