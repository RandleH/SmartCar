/*--------------------------------------------------------------------------
* @file         fourier.c
* @author       Randle_H & Baron Jean Baptiste Joseph Fourier(1768 ~ 1830)
* @version      V1.0.1
* @date         2019/12/28
* @Software     IAR 8.32.1 or MDK 5.17
* @Target core  MK66FX1M0
* @brief        This file provides basic fourier transform functions algorithm
* COPYRIGHT NOTICE
 Copyright (c) 2019 Randle_H. All rights reserved.
----------------------------------------------------------------------------*/
#include "fourier.h"
/*
=============================================
            N-1	       -j(2*π*n*k)/N
    X(k) =   ∑  x(n)*exp
            n=0
=============================================
*/
void DFT_1D(double* x,COMPLEX_Typedef* X,int length)
{
  double* x_tmp;
  int k,n;
  for(k=0;k<length;k++)
  {
    x_tmp=x;
    X[k].real = 0;
    X[k].img  = 0;
    for(n=0;n<length;n++)
    {
      X[k].real+= (*x_tmp)*cos(2.0*PI*k*n/(length));
      X[k].img += -(*x_tmp)*sin(2.0*PI*k*n/(length));
      x_tmp++;
    }
    X[k].real /= (double)length;
    X[k].img  /= (double)length;
    //printf("X[%d]=\t%lf\t+\tj*%lf\n",k,X[k].real,X[k].img);
  }
}

void DFT_2D(double* x,COMPLEX_Typedef* Y,const unsigned char row,const unsigned char col)
{
  unsigned int k1,k2;
  unsigned int n1,n2;
  COMPLEX_Typedef* Y_Tmp=Y;
  for(k1=0;k1<row;k1++)
  {
    for(k2=0;k2<col;k2++)
    {
      Y_Tmp=Y+k1*col+k2;
      Y_Tmp->real = 0;	//Y[k1][k2]
      Y_Tmp->img  = 0;
      for(n1=0;n1<row;n1++)
      {
        for(n2=0;n2<col;n2++)
        {
          Y_Tmp->real+=(*(x+n1*col+n2))*cos(2.0*pi*((double)k1*n1/row+(double)k2*n2/col));
          Y_Tmp->img -=(*(x+n1*col+n2))*sin(2.0*pi*((double)k1*n1/row+(double)k2*n2/col));
          *(Car_Image.BUF1[0]+k1*COL+k2)=(uint8)round(sqrt((Y_Tmp->real)*(Y_Tmp->real)+(Y_Tmp->img)*(Y_Tmp->img)));
        }
      }
      Y_Tmp->real /= (double)(row*col);
      Y_Tmp->img  /= (double)(row*col);
    }
  }
}
