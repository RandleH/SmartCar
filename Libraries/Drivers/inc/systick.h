
#ifndef _systick_h
#define _systick_h


#include "common.h"

void systick_delay(uint32 time);
void    systick_delay_ms(uint32 ms);
void    systick_timing(uint32 time);
uint32  systick_getval(void);


#endif
