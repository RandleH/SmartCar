/*--------------------------------------------------------------------------
* @file         main.cpp
* @author       Randle_H
* @version      V1.0.1
* @date         2019/11/24
* @Software     IAR 7.7 or MDK 5.17
* @Target core  MK66FX1M0
* @brief        This file provides entrance for projects.
* COPYRIGHT NOTICE
 Copyright (c) 2019 Randle_H. All rights reserved.
----------------------------------------------------------------------------*/
#include "headfiles.h"
static inline void PRJ_Init(void);
int main()
{
  PRJ_Init();
  while(1)
  {
#if __CMD_DESK    
    if(HAS_COMMAND)
      CMD_Check(NRF_RX_Buff,&Car_Parameter);
    CAR_ParaApply(&Car_Parameter);
                                   SHOW_RoadInfo();
#endif
  }
}

static inline void PRJ_Init(void)
{
  get_clk();
  CAR_Parameter_Reset(&Car_Parameter);
  MOTOR_Init(&Car_Parameter.MOTOR);
  TD8320MG_Init(&Car_Parameter.SERVO);
  TFT_Init();
  Camera_Init();
  NRF_Init();
}



