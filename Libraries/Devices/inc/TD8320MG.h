#include "headfiles.h"
#ifndef _TD8320MG_H
#define _TD8320MG_H

#define SERVO_FREEZE SERVO_Duty(31)

void TD8320MG_Init(Servo_Typedef* servo_p);
void TD8320MG_Port_Configuration(void);
void TD8320MG_FTM_Configuration(uint32 freq,uint32 duty);
static inline void TD8320MG_Duty(uint32 duty);

void SERVO_Angle(int8 angle);
void SERVO_Duty(uint32 duty);

#endif




