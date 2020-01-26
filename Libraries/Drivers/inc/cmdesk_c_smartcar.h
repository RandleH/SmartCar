#include "NRF24L01.h"
#include "c_smartcar.h"
#include "mathematics.h"
#ifndef _CMD_H
#define _CMD_H
#define __CMD_DESK Enable
#define C_SmartCar_ADDR 0x39
extern uint8 HAS_COMMAND;
uint8_t CMD_Check(uint8* CMD_Buf,CarPara_Typedef* CAR_Parameter_P);

#endif


