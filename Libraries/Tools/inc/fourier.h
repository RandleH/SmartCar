/*--------------------------------------------------------------------------
* @file         fourier.h
* @author       Randle_H & Baron Jean Baptiste Joseph Fourier(1768 ~ 1830)
* @version      V1.0.1
* @date         2019/11/24
* @Software     IAR 7.7 or MDK 5.17
* @Target core  MK66FX1M0
* @brief        This file provides all the declarations for fourier.c
* COPYRIGHT NOTICE
 Copyright (c) 2019 Randle_H. All rights reserved.
----------------------------------------------------------------------------*/
#include "headfiles.h"
#include <math.h>

#ifndef _FOURIER_H
#define _FOURIER_H


typedef struct
{
	double real;
	double img;
}COMPLEX_Typedef; 

void DFT_1D(double* x,COMPLEX_Typedef* X,int length);
void DFT_2D(double* x,COMPLEX_Typedef* Y,const unsigned char row,const unsigned char col);

#endif


