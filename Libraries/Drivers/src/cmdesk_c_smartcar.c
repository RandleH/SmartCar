#include "cmdesk_c_smartcar.h"

uint8 HAS_COMMAND = 0;

uint8_t CMD_Check(uint8* CMD_Buf,CarPara_Typedef* CAR_Parameter_P)
{
  uint8 result = 0;
  if(CMD_Buf[0] == C_SmartCar_ADDR)
  {
    switch(CMD_Buf[1])
    {
case 0x00:
{
  CAR_Parameter_P->MOTOR.l_duty = CMD_Buf[3];
  CAR_Parameter_P->MOTOR.r_duty = CMD_Buf[4];
  MOTOR_Duty(LEFT_ACTIVE_CH,CAR_Parameter_P->MOTOR.l_duty);
  MOTOR_Duty(RIGHT_ACTIVE_CH,CAR_Parameter_P->MOTOR.r_duty);
  break;
}
case 0x01:
{
  GET_Config();
  NRF_TX_Buff[0] = C_SmartCar_ADDR;//------Address
  NRF_TX_Buff[1] = 0x01;//-----------------Function Index
  NRF_TX_Buff[2] = 8;//--------------------Number of Camera Parameters
  NRF_TX_Buff[3] = Car_Parameter.CAMERA.auto_exp;
  NRF_TX_Buff[4] = Car_Parameter.CAMERA.exp_time;
  NRF_TX_Buff[5] = Car_Parameter.CAMERA.fps;
  NRF_TX_Buff[6] = Car_Parameter.CAMERA.col;
  NRF_TX_Buff[7] = Car_Parameter.CAMERA.row;
  NRF_TX_Buff[8] = Car_Parameter.CAMERA.lr_offset;
  NRF_TX_Buff[9] = Car_Parameter.CAMERA.ud_offset;
  NRF_TX_Buff[10] = Car_Parameter.CAMERA.gain;
  systick_delay_ms(1);
  while(NRF_Send(NRF_TX_Buff) != TX_OK);
  NRF_RecvMode();
  break;
}
case 0x02:
{
  MT9V032_CFG[0][1] = Car_Parameter.CAMERA.auto_exp  = CMD_Buf[3];
  MT9V032_CFG[1][1] = Car_Parameter.CAMERA.exp_time  = CMD_Buf[4];
  MT9V032_CFG[2][1] = Car_Parameter.CAMERA.fps       = CMD_Buf[5];
  MT9V032_CFG[3][1] = Car_Parameter.CAMERA.col       = CMD_Buf[6];
  MT9V032_CFG[4][1] = Car_Parameter.CAMERA.row       = CMD_Buf[7];
  MT9V032_CFG[5][1] = Car_Parameter.CAMERA.lr_offset = CMD_Buf[8];
  MT9V032_CFG[6][1] = Car_Parameter.CAMERA.ud_offset = CMD_Buf[9];
  MT9V032_CFG[7][1] = Car_Parameter.CAMERA.gain      = CMD_Buf[10];
  Car_Parameter.SCREEN.display = Enable;
  SET_Config();
  break;
}
case 0x03:
{
  GET_Config();
  NRF_TX_Buff[0]  = C_SmartCar_ADDR;//------Address
  NRF_TX_Buff[1]  = 0x03;//-----------------Function Index
  NRF_TX_Buff[2]  = 3;//--------------------Number of Camera Parameters
  NRF_TX_Buff[3]  = Car_Parameter.SERVO.freq>>8;
  NRF_TX_Buff[4]  = (uint8)Car_Parameter.SERVO.freq;
  NRF_TX_Buff[5]  = Car_Parameter.SERVO.duty;
  NRF_TX_Buff[6]  = Car_Parameter.SERVO.max_duty;
  NRF_TX_Buff[7]  = Car_Parameter.SERVO.min_duty;
  NRF_TX_Buff[8]  = (uint8)Car_Parameter.SERVO.angle;
  NRF_TX_Buff[9]  = (uint8)Car_Parameter.SERVO.max_angle;
  NRF_TX_Buff[10] = (uint8)Car_Parameter.SERVO.min_angle;
  systick_delay_ms(1);
  while(NRF_Send(NRF_TX_Buff) != TX_OK);
  NRF_RecvMode();
  break;
}
case 0x04:
{
  Car_Parameter.SERVO.freq        = (CMD_Buf[3]<<8)|CMD_Buf[4];
  Car_Parameter.SERVO.duty        = CMD_Buf[5];
  Car_Parameter.SERVO.max_duty    = CMD_Buf[6];
  Car_Parameter.SERVO.min_duty    = CMD_Buf[7];
  Car_Parameter.SERVO.angle       = (int8)CMD_Buf[8];
  Car_Parameter.SERVO.max_angle   = (int8)CMD_Buf[9];
  Car_Parameter.SERVO.min_angle   = (int8)CMD_Buf[10];
  Car_Parameter.SERVO.servo_param = (ServoParam_Type)CMD_Buf[11];
  break;
}
case 0x10://Manual Drive
case 0x13://Manual Stop
{
  Car_Parameter.SCREEN.display = Disable;
  Car_Parameter.SERVO.servo_param = use_angle;
  Car_Parameter.SERVO.duty   = 32;
  Car_Parameter.SERVO.angle  = 0;
  Car_Parameter.MOTOR.l_duty = 0;
  Car_Parameter.MOTOR.r_duty = 0;
  break;
}

case 0x11://Start Driving
{
  uint16 JoyStick_X = CMD_Buf[5]<<8 | CMD_Buf[6];
  uint16 JoyStick_Y = CMD_Buf[3]<<8 | CMD_Buf[4];
  
  Car_Parameter.SERVO.angle= (int8)map_linear(JoyStick_X,
                                  0,
                                  4095,
                                  Car_Parameter.SERVO.min_angle,
                                  Car_Parameter.SERVO.max_angle);
  
  if(JoyStick_Y > 2048+200)
  {
    Car_Parameter.MOTOR.dir    = forward;
    Car_Parameter.MOTOR.l_duty= (uint8)map_linear(JoyStick_Y,
                                     4095/2+200,
                                     4095,
                                     0,
                                     Car_Parameter.MOTOR.max_duty);
  }
  else if(JoyStick_Y < 2048-200)
  {
    Car_Parameter.MOTOR.dir    = backward;
    Car_Parameter.MOTOR.l_duty = Car_Parameter.MOTOR.max_duty-
                                  (uint8)map_linear(JoyStick_Y,
                                     0,
                                     4095/2-200,
                                     0,
                                     Car_Parameter.MOTOR.max_duty);
  }
  else
  {
    Car_Parameter.MOTOR.dir    = forward;
    Car_Parameter.MOTOR.l_duty = 0;
  }
  Car_Parameter.MOTOR.r_duty = Car_Parameter.MOTOR.l_duty; 
  //TFT_Printf("%d,%d",Car_Parameter.SERVO.angle,Car_Parameter.MOTOR.l_duty);
  break;
}

case 0x20://Test Module
{
  Car_Parameter.SCREEN.display = Enable;
  Car_Parameter.MOTOR.l_duty   = 0;
  Car_Parameter.MOTOR.r_duty   = 0;
  break;
}
case 0x31:// C_SmartCar -> NRF -> ALGORITHM -> Image Processor -> Gussian Blur(Read)
{
  //In this case, CMD_Desk require collecting data of Gussian Blur
  NRF_TX_Buff[0] = C_SmartCar_ADDR;//------Address
  NRF_TX_Buff[1] = 0x31;//-----------------Function Index
  NRF_TX_Buff[2] = 2;//--------------------Number of Camera Parameters
  NRF_TX_Buff[3] = (uint8)(Car_Parameter.IMAGE.gus_cfg.delta*100);
  NRF_TX_Buff[4] = Car_Parameter.IMAGE.gus_cfg.order;
  systick_delay_ms(1);
  while(NRF_Send(NRF_TX_Buff) != TX_OK);
  NRF_RecvMode();
  break;
}
case 0x32:// C_SmartCar -> NRF -> ALGORITHM -> Image Processor -> Gussian Blur(Write)
{
  //In this case, CMD_Desk is going to config data for Gussian Blur
  Car_Parameter.IMAGE.gus_cfg.delta  = CMD_Buf[3]/100.0;
  Car_Parameter.IMAGE.gus_cfg.order  = CMD_Buf[4];
  Car_Parameter.IMAGE.filter         = gus;
  break;
}
case 0x33:// C_SmartCar -> NRF -> ALGORITHM -> Image Processor -> Binary Image(Read)
{
  //In this case, CMD_Desk require collecting data of Binary Configuration
  NRF_TX_Buff[0] = C_SmartCar_ADDR;//------Address
  NRF_TX_Buff[1] = 0x33;//-----------------Function Index
  NRF_TX_Buff[2] = 2;//--------------------Number of Camera Parameters
  NRF_TX_Buff[3] = Car_Parameter.IMAGE.bin_cfg.threshold;
  NRF_TX_Buff[4] = (uint8)Car_Parameter.IMAGE.bin_cfg.lumin*100;
  systick_delay_ms(1);
  while(NRF_Send(NRF_TX_Buff) != TX_OK);
  NRF_RecvMode();
  break;
}
case 0x34:// C_SmartCar -> NRF -> ALGORITHM -> Image Processor -> Binary Image(Write)
{
  //In this case, CMD_Desk is going to config data for Binary Image
  Car_Parameter.IMAGE.bin_cfg.threshold  = CMD_Buf[3];
  Car_Parameter.IMAGE.bin_cfg.lumin      = CMD_Buf[4]/100.0;
  Car_Parameter.IMAGE.filter             = bin;
  break;
}
case 0x35:// C_SmartCar -> BLT -> ALGORITHM -> Image Processor -> Canny Edge
{
  //In this case, CMD_Desk require collecting data of Canny Edge
  NRF_TX_Buff[0] = C_SmartCar_ADDR;//------Address
  NRF_TX_Buff[1] = 0x35;//-----------------Function Index
  NRF_TX_Buff[2] = 6;//--------------------Number of Camera Parameters
  NRF_TX_Buff[3+0] = (uint8)(Car_Parameter.IMAGE.canny_cfg.gus_delta*100);
  NRF_TX_Buff[3+1] = Car_Parameter.IMAGE.canny_cfg.gus_order;
  NRF_TX_Buff[3+2] = Car_Parameter.IMAGE.canny_cfg.gus_cmd;
  NRF_TX_Buff[3+3] = (uint8)(Car_Parameter.IMAGE.canny_cfg.kernal);
  NRF_TX_Buff[3+4] = Car_Parameter.IMAGE.canny_cfg.h_threshold;
  NRF_TX_Buff[3+5] = Car_Parameter.IMAGE.canny_cfg.l_threshold;
  systick_delay_ms(1);
  while(NRF_Send(NRF_TX_Buff) != TX_OK);
  NRF_RecvMode();
  break;
}
case 0x36:
{
  Car_Parameter.IMAGE.canny_cfg.gus_delta   = CMD_Buf[3+0]/100.0; 
  Car_Parameter.IMAGE.canny_cfg.gus_order   = CMD_Buf[3+1];
  Car_Parameter.IMAGE.canny_cfg.gus_cmd     = CMD_Buf[3+2];
  Car_Parameter.IMAGE.canny_cfg.kernal      = (cannykernal_type)CMD_Buf[3+3];
  Car_Parameter.IMAGE.canny_cfg.h_threshold = CMD_Buf[3+4];
  Car_Parameter.IMAGE.canny_cfg.l_threshold = CMD_Buf[3+5];
  Car_Parameter.IMAGE.filter                = canny;
  break;
}
case 0x37:
{
  //In this case, CMD_Desk require collecting data of Average Filter
  NRF_TX_Buff[0] = C_SmartCar_ADDR;//------Address
  NRF_TX_Buff[1] = 0x37;//-----------------Function Index
  NRF_TX_Buff[2] = 1;//--------------------Number of Camera Parameters
  NRF_TX_Buff[3+0] = Car_Parameter.IMAGE.ger_cfg.order;
  systick_delay_ms(1);
  while(NRF_Send(NRF_TX_Buff) != TX_OK);
  NRF_RecvMode();
  break;
}
case 0x38:
{
  Car_Parameter.IMAGE.ger_cfg.order = CMD_Buf[3+0];
  Car_Parameter.IMAGE.filter        = avg;
  break;
}
case 0x39:
{
  //In this case, CMD_Desk require collecting data of Average Filter
  NRF_TX_Buff[0] = C_SmartCar_ADDR;//------Address
  NRF_TX_Buff[1] = 0x39;//-----------------Function Index
  NRF_TX_Buff[2] = 1;//--------------------Number of Camera Parameters
  NRF_TX_Buff[3+0] = Car_Parameter.IMAGE.ger_cfg.order;
  systick_delay_ms(1);
  while(NRF_Send(NRF_TX_Buff) != TX_OK);
  NRF_RecvMode();
  break;
}
case 0x3A:
{
  Car_Parameter.IMAGE.ger_cfg.order = CMD_Buf[3+0];
  Car_Parameter.IMAGE.filter        = mid;
  break;
}

    

}


  }
  else
    result = 1;
  HAS_COMMAND = 0;
  return result;
}

