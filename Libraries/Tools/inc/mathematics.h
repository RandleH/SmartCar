#include "math.h"

#include "stdlib.h"
#ifndef _MATHEMATICS_H
#define _MATHEMATICS_H


float map_linear(float val,float input_min,float input_max,float output_min,float output_max);
float map_log   (float val,float input_min,float input_max,float output_min,float output_max);
float map_exp   (float val,float input_min,float input_max,float output_min,float output_max);
#endif

