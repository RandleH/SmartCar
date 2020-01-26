/*--------------------------------------------------------------------------
* @file         c_smartcar.c
* @author       Randle_H
* @version      V1.0.1
* @date         2019/11/24
* @Software     IAR 8.32 or MDK 5.17
* @Target core  MK66FX1M0
* @brief        This file provides all parameter function for smartcar.
* COPYRIGHT NOTICE
 Copyright (c) 2019 Randle_H. All rights reserved.
----------------------------------------------------------------------------*/
#include "c_smartcar.h"

CarPara_Typedef         Car_Parameter;
ImageBuf_Typedef        Car_Image;
/*============================================================================
@ brief:        Initial settings for smart car
@ param:        CarPara_Typedef structure
@ return:       Void
Sample Usage:   CAR_Parameter_Init(&structure);
============================================================================*/
void CAR_Parameter_Reset(CarPara_Typedef* Car_Parameter_P)
{
//============= Default Configuration for Hardware =============//
  
  Car_Parameter_P->MOTOR.freq     = 10000;  // 10KHz
  Car_Parameter_P->MOTOR.max_duty = 30;     // 50%
  Car_Parameter_P->MOTOR.r_duty   = 20;     // 20%
  Car_Parameter_P->MOTOR.l_duty   = 20;     // 20%
  Car_Parameter_P->MOTOR.dir      = forward;
  
  Car_Parameter_P->SERVO.servo_param = use_angle; 
  Car_Parameter_P->SERVO.freq        = 333;    // 333Hz
  Car_Parameter_P->SERVO.min_duty    = 24;     // 24%
  Car_Parameter_P->SERVO.max_duty    = 40;     // 40%
  Car_Parameter_P->SERVO.min_angle   = -45;
  Car_Parameter_P->SERVO.max_angle   = 30;
  Car_Parameter_P->SERVO.angle       = 0;
  Car_Parameter_P->SERVO.duty        = 32;     // 32%
  
  Car_Parameter_P->CAMERA.auto_exp  = 0;
  Car_Parameter_P->CAMERA.exp_time  = 224;
  Car_Parameter_P->CAMERA.fps       = 141;
  Car_Parameter_P->CAMERA.col       = COL;
  Car_Parameter_P->CAMERA.row       = ROW;
  Car_Parameter_P->CAMERA.lr_offset = 39;
  Car_Parameter_P->CAMERA.ud_offset = 0;
  Car_Parameter_P->CAMERA.gain      = 64;
  
  Car_Parameter_P->SCREEN.display = Enable;
  Car_Parameter_P->SCREEN.ops     = SHOW_Image; 
  
  Car_Parameter_P->SENSOR         = use_cam;
#ifdef _IMAGE_TOOL
//============= Default Configuration for Image =============//
  Car_Parameter_P->IMAGE.source_img            = (uint8*)Car_Image.CAM[0];
  Car_Parameter_P->IMAGE.output_img            = Car_Image.BUF1[0];
  Car_Parameter_P->IMAGE.filter                = canny;
  Car_Parameter_P->IMAGE.row_start             = 0;
  Car_Parameter_P->IMAGE.row_end               = ROW-1;
  Car_Parameter_P->IMAGE.col_start             = 0;
  Car_Parameter_P->IMAGE.col_end               = COL-1;
#ifdef _CANNY_H 
  //Default Config for Canny:
  Car_Parameter_P->IMAGE.canny_cfg.h_threshold = 180;
  Car_Parameter_P->IMAGE.canny_cfg.l_threshold = 60;
  Car_Parameter_P->IMAGE.canny_cfg.kernal      = sobel;
  Car_Parameter_P->IMAGE.canny_cfg.gus_delta   = 1.4;
  Car_Parameter_P->IMAGE.canny_cfg.gus_order   = 3;
  Car_Parameter_P->IMAGE.canny_cfg.gus_cmd     = Enable;
  Car_Parameter_P->IMAGE.canny_cfg.source_img  = Car_Parameter_P->IMAGE.source_img;
  Car_Parameter_P->IMAGE.canny_cfg.output_img  = Car_Parameter_P->IMAGE.output_img;
#endif  
#ifdef _BINARY_H
  //Default Config for Bin:
  Car_Parameter_P->IMAGE.bin_cfg.threshold     = 80;
  Car_Parameter_P->IMAGE.bin_cfg.lumin         = 0.1;
  Car_Parameter_P->IMAGE.bin_cfg.output_img    = Car_Parameter_P->IMAGE.output_img;
#endif
#ifdef _GUSSIAN_H  
  //Default Config for gusblur:
  Car_Parameter_P->IMAGE.gus_cfg.order         = 3;
  Car_Parameter_P->IMAGE.gus_cfg.delta         = 1.4;
  Car_Parameter_P->IMAGE.gus_cfg.source_img    = Car_Parameter_P->IMAGE.source_img;
  Car_Parameter_P->IMAGE.gus_cfg.output_img    = Car_Parameter_P->IMAGE.output_img;
#endif
#ifdef _FILTER_H
  //Default Config for General Filters
  Car_Parameter_P->IMAGE.ger_cfg.order         = 3;
  Car_Parameter_P->IMAGE.ger_cfg.source_img    = Car_Parameter_P->IMAGE.source_img;
  Car_Parameter_P->IMAGE.ger_cfg.output_img    = Car_Parameter_P->IMAGE.output_img;
#endif
#ifdef _STAT_H
  Car_Parameter_P->IMAGE.statistic.source_img    = Car_Parameter_P->IMAGE.source_img;
  Car_Parameter_P->IMAGE.statistic.Histogram_CMD = true;
  Car_Parameter_P->IMAGE.statistic.Histogram_backcolor = BLACK;
  Car_Parameter_P->IMAGE.statistic.Histogram_textcolor = WHITE;
#endif
#endif
  
#ifdef _CONTEST_H
  Car_Parameter_P->ROAD.row_start = 40;
  Car_Parameter_P->ROAD.row_end   = ROW-1;
  Car_Parameter_P->ROAD.col_start = 0;
  Car_Parameter_P->ROAD.col_end   = COL-1;
  INIT_RoadData(&Car_Parameter_P->ROAD);  
#endif
}


/*============================================================================
@ brief:        Apply parameter to the smartcar
@ param:        CarPara_Typedef structure
@ return:       Void
Sample Usage:   CAR_ParaSet(&structure);
============================================================================*/
void CAR_ParaApply(CarPara_Typedef* Car_Parameter_P)
{
  
  if(Car_Parameter_P->MOTOR.l_duty > Car_Parameter_P->MOTOR.max_duty)
    Car_Parameter_P->MOTOR.l_duty = Car_Parameter_P->MOTOR.max_duty;
  if(Car_Parameter_P->MOTOR.r_duty > Car_Parameter_P->MOTOR.max_duty)
    Car_Parameter_P->MOTOR.r_duty = Car_Parameter_P->MOTOR.max_duty;
  MOTOR_Direction(Car_Parameter_P->MOTOR.dir);
  if(Car_Parameter_P->MOTOR.dir==forward)
  {
    
    MOTOR_Duty(LEFT_ACTIVE_CH,Car_Parameter_P->MOTOR.l_duty);
    MOTOR_Duty(RIGHT_ACTIVE_CH,Car_Parameter_P->MOTOR.r_duty);
  }
  else
  {
    MOTOR_Duty(LEFT_ACTIVE_CH,PWM_PRECISON-Car_Parameter_P->MOTOR.l_duty);
    MOTOR_Duty(RIGHT_ACTIVE_CH,PWM_PRECISON-Car_Parameter_P->MOTOR.r_duty);
  }
  
  if(Car_Parameter_P->SERVO.servo_param == use_angle)
    SERVO_Angle(Car_Parameter_P->SERVO.angle);
  else if(Car_Parameter_P->SERVO.servo_param == use_duty)
    SERVO_Duty(Car_Parameter_P->SERVO.duty);
  else
    SERVO_FREEZE;
  
  switch(Car_Parameter_P->IMAGE.filter)
  {
  case gus:
    Image_GusBlur(Car_Parameter_P->IMAGE.row_start,Car_Parameter_P->IMAGE.col_start,
                  Car_Parameter_P->IMAGE.row_end  ,Car_Parameter_P->IMAGE.col_end,
                  &Car_Parameter_P->IMAGE.gus_cfg);
    break;
  case canny:
    Image_CannyEdge(Car_Parameter_P->IMAGE.row_start,Car_Parameter_P->IMAGE.col_start,
                    Car_Parameter_P->IMAGE.row_end  ,Car_Parameter_P->IMAGE.col_end,
                    &Car_Parameter_P->IMAGE.canny_cfg);
    break;
  case bin:
    Image_Bin(Car_Parameter_P->IMAGE.row_start,Car_Parameter_P->IMAGE.col_start,
              Car_Parameter_P->IMAGE.row_end  ,Car_Parameter_P->IMAGE.col_end,&Car_Parameter_P->IMAGE.bin_cfg);
    break;
  case avg:
    Image_AvgFilter(Car_Parameter_P->IMAGE.row_start,Car_Parameter_P->IMAGE.col_start,
                    Car_Parameter_P->IMAGE.row_end  ,Car_Parameter_P->IMAGE.col_end,
                    &Car_Parameter_P->IMAGE.ger_cfg);
    break;
  case mid:
    Image_MidFilter(Car_Parameter_P->IMAGE.row_start,Car_Parameter_P->IMAGE.col_start,
                    Car_Parameter_P->IMAGE.row_end  ,Car_Parameter_P->IMAGE.col_end,
                    &Car_Parameter_P->IMAGE.ger_cfg);
    break;
  case no_filter:
    Image_SnapShot(ROW,COL,Car_Parameter_P->IMAGE.source_img,Car_Parameter_P->IMAGE.output_img);
    break;
  case stat:
    Image_GrayStat(Car_Parameter_P->IMAGE.row_start,Car_Parameter_P->IMAGE.col_start,
                   Car_Parameter_P->IMAGE.row_end  ,Car_Parameter_P->IMAGE.col_end,
                   &Car_Parameter_P->IMAGE.statistic);
    break;
  }
  
#ifdef _CONTEST_H
  GET_RoadFeature(&Car_Parameter_P->ROAD,Car_Parameter_P->IMAGE.bin_cfg.output_img);
  
#endif
  
  if(Car_Parameter_P->SCREEN.display==Enable)
    Car_Parameter_P->SCREEN.ops();
    //SHOW_Image(Car_Parameter_P->IMAGE.output_img);
}
