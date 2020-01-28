/*--------------------------------------------------------------------------
* @file         c_smartcar.h
* @author       Randle_H
* @version      V1.0.1
* @date         2019/11/24
* @Software     IAR 7.7 or MDK 5.17
* @Target core  MK66FX1M0
* @brief        This file provides all declarations for smartcar project.
* COPYRIGHT NOTICE
 Copyright (c) 2019 Randle_H. All rights reserved.
----------------------------------------------------------------------------*/
#include "headfiles.h"
#include "image.h"
#include "MT9V032.h"
#ifndef _SMART_CAR_H
#define _SMART_CAR_H

#define Disable 0
#define Enable  1
typedef enum
{
  avg=0,gus,mid,canny,bin,no_filter,stat
}ImgFilterType;

typedef enum
{
  use_cam,use_meg
}Sensor_Type;

typedef enum
{
  use_angle,use_duty
}ServoParam_Type;

typedef enum
{
  linear=0,quadra,sqr
}MapAlgoType;

typedef enum
{
  forward=0,backward
}MotorDirType;

typedef struct          //Configuration for Servo
{
  int8   max_angle;
  int8   angle;
  int8   min_angle;
  uint8  max_duty;
  uint8  duty;
  uint8  min_duty;
  ServoParam_Type servo_param;
  uint16 freq;
  uint16 ftm_mod;
  uint16 ftm_cv;
}Servo_Typedef;

typedef struct          //Configuration for gray scale camera
{
  int auto_exp;
  int exp_time;
  int fps;
  int col;
  int row;
  int lr_offset;
  int ud_offset;
  int gain;
}Camera_Typedef;

typedef struct          //Configuration for Motor
{
  uint16 r_duty;
  uint16 l_duty;
  uint16 max_duty;
  uint32 freq;
  uint16 ftm_mod;
  uint16 ftm_cv;
  MotorDirType dir;
}Motor_Typedef;

typedef struct          //Configuration for TFT Screen
{
  bool display;
  uint8 brightness;
  uint8 direction;
  void (*ops)(void);
}Screen_Typedef;

typedef struct
{
  
  MapAlgoType    map;
}Algorithm_Typedef;

typedef struct          //Configuration for image process
{
  ImgFilterType        filter;
  GussianConfigTypedef gus_cfg;
  CannyConfigTypedef   canny_cfg;
  BinConfigTypedef     bin_cfg;
  GeneralFilterTypedef ger_cfg;
  GrayStatTypedef      statistic;
  uint8*               source_img;
  uint8*               output_img;
  void* filter1_cfg;
  void* filter2_cfg;
  void (*filter1)(uint8,uint8,uint8,uint8,void* filter1_cfg);
  void (*filter2)(uint8,uint8,uint8,uint8,void* filter2_cfg);

  uint8 row_start;
  uint8 row_end;
  uint8 col_start;
  uint8 col_end;
}ImgProcess_Typedef;

typedef struct
{
  uint8 width;
  uint8 center[ROW];
  uint8 left_edge[ROW];
  uint8 right_edge[ROW];
  int8  servo_angle;
  uint8 threshold;
  uint8 row_start;
  uint8 row_end;
  uint8 col_start;
  uint8 col_end;
}RoadInfoTypedef;

typedef struct          //Configuration for Automobile
{
  //====Hardware====//
  Servo_Typedef         SERVO;
  Camera_Typedef        CAMERA;
  Motor_Typedef         MOTOR;
  Screen_Typedef        SCREEN;
  //====Software====//
  ImgProcess_Typedef    IMAGE; 
  RoadInfoTypedef       ROAD;
  Sensor_Type           SENSOR;
}CarPara_Typedef;

typedef struct          //Address of Image Data Buffer
{
  vuint8  CAM[ROW][COL];
  uint8  BUF1[ROW][COL];
  uint8  BUF2[ROW][COL];
}ImageBuf_Typedef;

void CAR_Parameter_Reset(CarPara_Typedef* Car_Parameter_P);
void CAR_ParaApply(CarPara_Typedef* Car_Parameter_P);
bool Image_IsValid(ImgProcess_Typedef* cfg);
extern CarPara_Typedef         Car_Parameter;
extern ImageBuf_Typedef        Car_Image;
#endif



