#include "headfiles.h"
#ifndef _MOTOR_H
#define _MOTOR_H

#define PWM_PRECISON 100U
#define LEFT_ACTIVE_CH    0
#define RIGHT_ACTIVE_CH   2
#define LEFT_INACTIVE_CH    1
#define RIGHT_INACTIVE_CH   3
#define MOTOR_FREEZE FTM0->CONTROLS[LEFT_ACTIVE_CH].CnV=0;FTM0->CONTROLS[RIGHT_ACTIVE_CH].CnV=0


void MOTOR_Port_Configuration(void);
void MOTOR_FTM_Configuration(uint32 freq,uint32 duty);
void MOTOR_Init(Motor_Typedef* motor_p);
void MOTOR_Duty(uint8 ch,uint32 duty);
void MOTOR_Direction(MotorDirType dir);
inline void MOTOR_CheckParam(void);


#endif



