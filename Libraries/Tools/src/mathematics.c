#include "mathematics.h"


float map_linear(float val,float input_min,float input_max,float output_min,float output_max)
{
  if(	  input_max<input_min
          ||output_max<output_min
          ||val<input_min
          ||val>input_max	)
    exit(-1);
  
  return (output_min+(output_max-output_min)*(val-input_min)/(input_max-input_min)); 
}