/*--------------------------------------------------------------------------
* @File         image.c
* @Author       Randle_H
* @Version      V1.0.1
* @Date         2019/11/24
* @Software     IAR 8.32 or MDK 5.17
* @Target core  MK66FX1M0
* @Brief        This file provides all the algorithms for image processing.
* @Instruction:
*                      __________________________
*            [0][0] ->|                          |<- [0][COL-1]
*                     |                          |
*                     |                          |
*                     | (Image Diagram in array) |
*                     |                          |
*                     |                          |
*        [ROW-1][0] ->|__________________________|<- [ROW-1][COL-1]| 
*
* COPYRIGHT NOTICE
 Copyright (c) 2019 Randle_H. All rights reserved.
----------------------------------------------------------------------------*/
#include "image.h"

/*============================================================================
@ brief:        Copy an image from camera buffer
@ param:        ImageSize[row,col], ImageSourceAddress, ImageStoreAddress
@ return:       Void
Sample Usage:   Image_SnapShot(0,0,120,160,(uint8*)Car_Image.CAM[0],Car_Image.BUF1[0]);
============================================================================*/
void Image_SnapShot(uint8 row,uint8 col,uint8* source_img,uint8* output_img)
{
  uint8 x,y;
  for(y=0;y<row;y++)
  {
    for(x=0;x<col;x++)
    {
      *(output_img+y*COL+x) = *(source_img+y*COL+x);
    }
  }
}

/*============================================================================
@ brief:        Analyze a gray-scale image such as 
@ param:        ImageWindows{}, Gray Scale Statistics Structure(See at "img_stat.h")
@ return:       Void
Sample Usage:   Image_GrayStat(0,0,120,160,&GrayStat_CFG);
============================================================================*/
void Image_GrayStat(uint8 row_start, uint8 col_start,
                     uint8 row_end ,  uint8 col_end  ,GrayStatTypedef* GrayStat_Data)
{
  float gray_avg=0.0;
  float gray_avg_tmp = 0.0;
  memset(GrayStat_Data->gray_Count,0,sizeof(GrayStat_Data->gray_Count));
  
  for(uint8 y=row_start;y<row_end;y++)
  {
    gray_avg_tmp = 0.0;
    for(uint8 x=col_start;x<col_end;x++)
    {
      GrayStat_Data->gray_Count[*(GrayStat_Data->source_img+y*COL+x)]++;
      gray_avg_tmp += *(GrayStat_Data->source_img+y*COL+x);
    }
    gray_avg += gray_avg_tmp/((row_end-row_start)*(col_end-col_start));
  }
  GrayStat_Data->gray_Avg = (uint8)(gray_avg);
  GrayStat_Data->gray_Max = 0;
  for(uint8 i=1;i<255;i++)
  {
    if(GrayStat_Data->gray_Count[i] > GrayStat_Data->gray_Max)       
      GrayStat_Data->gray_Max = GrayStat_Data->gray_Count[i];
  }
}

/*============================================================================
@ brief:        Convert a gray scale image into binary 
@ param:        ImageWindows{}, Binary Config Structure(See at "img_bin.h")
@ return:       Void
Sample Usage:   Image_Bin(0,0,120,160,&BinaryCFG_Structure);
============================================================================*/
void Image_Bin(uint8 row_start, uint8 col_start,
                     uint8 row_end ,  uint8 col_end  ,BinConfigTypedef* Bin_CFG)
{
  for(uint8 y=row_start;y<row_end;y++)
  {
    for(uint8 x=col_start;x<col_end;x++)
    {
      if(*(Car_Parameter.IMAGE.source_img+y*COL+x)>Bin_CFG->threshold)
        *(Bin_CFG->output_img+y*COL+x)=255;
      else
        *(Bin_CFG->output_img+y*COL+x)=0;
    }
  }
}

/*============================================================================
@ brief:        Apply average filter on a gray-scale image
@ param:        ImageWindows{}, General Filter Config Structure(See at "img_filter.h")
@ return:       Void
Sample Usage:   Image_AvgFilter(0,0,120,160,&GeneralFilter_Structure);
============================================================================*/
void Image_AvgFilter(uint8 row_start,uint8 col_start,
                     uint8 row_end  ,uint8 col_end  ,GeneralFilterTypedef* Ger_CFG)
{
  uint8 x,y;
  uint16 pixel_tmp;
  switch(Ger_CFG->order)
  {
  case 3:
    for(y=row_start;y<row_end;y++)
      for(x=col_start;x<col_end;x++)
      {
        __PARAM_SAFETY_3X3(x,y);
        pixel_tmp=0;
        pixel_tmp+= *(Ger_CFG->source_img+(y-1)*COL+(x-1));
        pixel_tmp+= *(Ger_CFG->source_img+(y-1)*COL+(x));
        pixel_tmp+= *(Ger_CFG->source_img+(y-1)*COL+(x+1));
        pixel_tmp+= *(Ger_CFG->source_img+(y)*COL+(x-1));
        pixel_tmp+= *(Ger_CFG->source_img+(y)*COL+(x));
        pixel_tmp+= *(Ger_CFG->source_img+(y)*COL+(x+1));
        pixel_tmp+= *(Ger_CFG->source_img+(y+1)*COL+(x-1));
        pixel_tmp+= *(Ger_CFG->source_img+(y+1)*COL+(x));
        pixel_tmp+= *(Ger_CFG->source_img+(y+1)*COL+(x+1));
        pixel_tmp/= 9;
        *(Ger_CFG->output_img+y*COL+x) = (uint8)pixel_tmp;
      }
    break;
  case 5:
    for(y=row_start;y<row_end;y++)
      for(x=col_start;x<col_end;x++)
      {
        __PARAM_SAFETY_5X5(x,y);
        pixel_tmp=0;
        for(int8 i=-2;i<=2;i++)
          for(int8 j=-2;j<=2;j++)
            pixel_tmp+= *(Ger_CFG->source_img+(y+j)*COL+(x+i));
        pixel_tmp/= 25;
        *(Ger_CFG->output_img+y*COL+x)=(uint8)pixel_tmp;
      }
    break;
  default: return;
  }
}

/*============================================================================
@ brief:        Apply median filter on a gray-scale image 
@ param:        ImageWindows{}, General Filter Config Structure(See at "img_filter.h")
@ return:       Void
Sample Usage:   Image_MidFilter(0,0,120,160,&GeneralFilter_Structure);
============================================================================*/
void Image_MidFilter(uint8 row_start,uint8 col_start,
                     uint8 row_end  ,uint8 col_end  ,GeneralFilterTypedef* Ger_CFG)
{
  uint8  x,y;
  switch(Ger_CFG->order)
  {
  case 3:
    {
      uint8 window[3*3]={0};
      uint8 temp;
      for(y=row_start;y<row_end;y++)
        for(x=col_start;x<col_end;x++)
        {
          __PARAM_SAFETY_3X3(x,y);
          window[0] = *(Ger_CFG->source_img+(y-1)*COL+(x-1));
          window[1] = *(Ger_CFG->source_img+(y-1)*COL+(x  ));
          window[2] = *(Ger_CFG->source_img+(y-1)*COL+(x+1));
          window[3] = *(Ger_CFG->source_img+(y  )*COL+(x-1));
          window[4] = *(Ger_CFG->source_img+(y  )*COL+(x  ));
          window[5] = *(Ger_CFG->source_img+(y  )*COL+(x+1));
          window[6] = *(Ger_CFG->source_img+(y+1)*COL+(x-1));
          window[7] = *(Ger_CFG->source_img+(y+1)*COL+(x  ));
          window[8] = *(Ger_CFG->source_img+(y+1)*COL+(x+1));
          for(uint8 i=1;i<9;i++)
            for(uint8 j=i+1;j<9;j++)
            {
              if(window[j]>window[i])
              {
                temp = window[j];
                window[j] = window[i];
                window[i] = temp;
              }
            }
          *(Ger_CFG->output_img+y*COL+x)=window[5];
        }
      break;
    }
  case 5:
    {
      uint8 window[5*5]={0};
      uint8 temp;
      for(y=row_start;y<row_end;y++)
        for(x=col_start;x<col_end;x++)
        {
          __PARAM_SAFETY_5X5(x,y);
          window[0]  = *(Ger_CFG->source_img+(y-2)*COL+(x-2));
          window[1]  = *(Ger_CFG->source_img+(y-2)*COL+(x-1));
          window[2]  = *(Ger_CFG->source_img+(y-2)*COL+(x  ));
          window[3]  = *(Ger_CFG->source_img+(y-2)*COL+(x+1));
          window[4]  = *(Ger_CFG->source_img+(y-2)*COL+(x+2));
          window[5]  = *(Ger_CFG->source_img+(y-1)*COL+(x-2));
          window[6]  = *(Ger_CFG->source_img+(y-1)*COL+(x-1));
          window[7]  = *(Ger_CFG->source_img+(y-1)*COL+(x  ));
          window[8]  = *(Ger_CFG->source_img+(y-1)*COL+(x+1));
          window[9]  = *(Ger_CFG->source_img+(y-1)*COL+(x+2));
          window[10] = *(Ger_CFG->source_img+(y  )*COL+(x-2));
          window[11] = *(Ger_CFG->source_img+(y  )*COL+(x-1));
          window[12] = *(Ger_CFG->source_img+(y  )*COL+(x  ));
          window[13] = *(Ger_CFG->source_img+(y  )*COL+(x+1));
          window[14] = *(Ger_CFG->source_img+(y  )*COL+(x+2));
          window[15] = *(Ger_CFG->source_img+(y+1)*COL+(x-2));
          window[16] = *(Ger_CFG->source_img+(y+1)*COL+(x-1));
          window[17] = *(Ger_CFG->source_img+(y+1)*COL+(x  ));
          window[18] = *(Ger_CFG->source_img+(y+1)*COL+(x+1));
          window[19] = *(Ger_CFG->source_img+(y+1)*COL+(x+2));
          window[20] = *(Ger_CFG->source_img+(y+2)*COL+(x-2));
          window[21] = *(Ger_CFG->source_img+(y+2)*COL+(x-1));
          window[22] = *(Ger_CFG->source_img+(y+2)*COL+(x  ));
          window[23] = *(Ger_CFG->source_img+(y+2)*COL+(x+1));
          window[24] = *(Ger_CFG->source_img+(y+2)*COL+(x+2));
          for(uint8 i=1;i<25;i++)
            for(uint8 j=i+1;j<25;j++)
            {
              if(window[j]>window[i])
              {
                temp = window[j];
                window[j] = window[i];
                window[i] = temp;
              }
            }
          *(Ger_CFG->output_img+y*COL+x)=window[12];
        }
      break;
    }
  default: return;
  }
}

/*============================================================================
@ brief:        Filter based on Gussian kernals
@ param:        ImageWindows, Gussian Config Structure(See at "img_gussian.h")
@ return:       Void
Sample Usage:   Image_GusBlur(0,0,120,160,&GusCFG_Structure);
============================================================================*/
void Image_GusBlur(uint8 row_start, uint8 col_start,
                   uint8 row_end ,  uint8 col_end  ,GussianConfigTypedef* Gus_CFG)
{
  uint16  kernal[6];
  uint16  sigma=0;
  uint8   x,y;
  uint8*  src = Gus_CFG->source_img;
  uint8*  out = Gus_CFG->output_img;
  uint8  pixel_temp;
  static uint8 local_order = 0;
  static float local_delta = 0.0;
  float temp = 2*(Gus_CFG->delta)*(Gus_CFG->delta);
  
  //Only when the order or delta changed, recalculating kernal
  if(local_order != Gus_CFG->order || local_delta != Gus_CFG->delta)
  {
    if(Gus_CFG->order==3)
    {
      kernal[0] = (uint8)(100/(pi*temp));
      kernal[1] = (uint8)(100/(pi*temp)*exp(-1/(temp)));
      kernal[2] = (uint8)(100/(pi*temp)*exp(-2/(temp)));
      sigma += kernal[0];
      sigma += (kernal[1])<<2;
      sigma += (kernal[2])<<2;
    }
    else if(Gus_CFG->order==5)
    {
      kernal[0] = (uint8)(100/(pi*temp));
      kernal[1] = (uint8)(100/(pi*temp)*exp(-1/(temp)));
      kernal[2] = (uint8)(100/(pi*temp)*exp(-2/(temp)));
      kernal[3] = (uint8)(100/(pi*temp)*exp(-4/(temp)));
      kernal[4] = (uint8)(100/(pi*temp)*exp(-5/(temp)));
      kernal[5] = (uint8)(100/(pi*temp)*exp(-8/(temp)));
    }
    else return;
  }
  
  if(Gus_CFG->order==3)
  {
    for(y=row_start;y<row_end;y++)
    {
      for(x=col_start;x<col_end;x++)
      {
        if(y-1<0||x-1<0||y+1>ROW-1||x+1>COL-1)    continue;
        pixel_temp=0;
        pixel_temp+=kernal[0]*(*(src+y*COL+x))/sigma;
        pixel_temp+=kernal[1]*(*(src+(y-1)*COL+x)+*(src+(y+1)*COL+x)+*(src+y*COL+x-1)+*(src+y*COL+x+1))/sigma;
        pixel_temp+=kernal[2]*(*(src+(y-1)*COL+x-1)+*(src+(y-1)*COL+x+1)+*(src+(y+1)*COL+x-1)+*(src+(y+1)*COL+x+1))/sigma;
        *(out+y*COL+x) = pixel_temp;
      }
    }
  }
  else
  {
    for(y=row_start;y<row_end;y++)
    {
      for(x=col_start;x<col_end;x++)
      {
        
      }
    }
  }
}

/*============================================================================
@ brief:        Canny Edge Detection including sobel,prewitt,scharr and laplace kernals
@ param:        ImageWindows, Canny Config Structure(See at "img_canny.h")
@ return:       Void
Sample Usage:   Image_CannyEdge(0,0,120,160,&CannyCFG_Structure);
============================================================================*/
void Image_CannyEdge(uint8 row_start, uint8 col_start,
                       uint8 row_end ,  uint8 col_end  ,CannyConfigTypedef* Canny_CFG)
{
  uint8         x,y;                    //select a pixel
  int           grad_x,grad_y;          //position of the srounding pixel
  uint8*        img_temp;               //store a gussian filter image 
  uint8*        grad_temp;              //store grad angle data
  float         tan_val;                
  
  //  ↓↓↓↓↓↓↓↓↓↓  Config Data  ↓↓↓↓↓↓↓↓↓↓  //
  if(Canny_CFG->gus_cmd==Enable)
  {
    GussianConfigTypedef Gus_CFG;        
    if(Canny_CFG->output_img==Car_Image.BUF1[0])   
    {
      img_temp  = Car_Image.BUF2[0];
      grad_temp = Car_Image.BUF2[0];
    }
    else
    {
      img_temp  = Car_Image.BUF1[0];
      grad_temp = Car_Image.BUF1[0];
    }
    Gus_CFG.order      = Canny_CFG->gus_order;
    Gus_CFG.delta      = Canny_CFG->gus_delta;
    Gus_CFG.source_img = Canny_CFG->source_img;
    Gus_CFG.output_img = img_temp;
    Image_GusBlur(row_start,col_start,row_end,col_end,&Gus_CFG);
  }
  else
  {
    img_temp = Canny_CFG->source_img;
    if(Canny_CFG->output_img==Car_Image.BUF1[0])
      grad_temp = Car_Image.BUF2[0];
    else
      grad_temp = Car_Image.BUF1[0];
  }
  //  ↓↓↓↓↓↓↓↓↓↓  Start Edge Detection  ↓↓↓↓↓↓↓↓↓↓  //
  for(y=row_start;y<row_end;y++)
  {
    for(x=col_start;x<col_end;x++)
    {
      grad_x=0;grad_y=0;
      if(y-2<0||x-2<0||y+1>ROW-1||x+1>COL-1)    continue;
      switch(Canny_CFG->kernal)
      {
        case sobel:
          grad_x += (int)(*(img_temp+(y-1)*COL+(x+1))-*(img_temp+(y-1)*COL+(x-1)));
          grad_x += (int)(*(img_temp+y*COL+(x+1))<<1-*(img_temp+y*COL+(x-1))<<1);
          grad_x += (int)(*(img_temp+(y+1)*COL+(x+1))-*(img_temp+(y+1)*COL+(x-1)));
          grad_y += (int)(*(img_temp+(y-1)*COL+(x-1))-*(img_temp+(y+1)*COL+(x-1)));
          grad_y += (int)(*(img_temp+(y-1)*COL+x)<<1-*(img_temp+(y+1)*COL+x)<<1);
          grad_y += (int)(*(img_temp+(y-1)*COL+(x+1))-*(img_temp+(y+1)*COL+(x+1)));break;
        case prewitt:
          grad_x += (int)(*(img_temp+(y-1)*COL+(x+1))-*(img_temp+(y-1)*COL+(x-1)));
          grad_x += (int)(*(img_temp+(y  )*COL+(x+1))-*(img_temp+(y  )*COL+(x-1)));
          grad_x += (int)(*(img_temp+(y+1)*COL+(x+1))-*(img_temp+(y+1)*COL+(x-1)));
          grad_y += (int)(*(img_temp+(y-1)*COL+(x-1))-*(img_temp+(y+1)*COL+(x-1)));
          grad_y += (int)(*(img_temp+(y-1)*COL+(x  ))-*(img_temp+(y+1)*COL+(x  )));
          grad_y += (int)(*(img_temp+(y-1)*COL+(x+1))-*(img_temp+(y+1)*COL+(x+1)));break;
        case scharr:
          grad_x += (int)(*(img_temp+(y-1)*COL+(x+1))-*(img_temp+(y-1)*COL+(x-1)))*3;
          grad_x += (int)(*(img_temp+(y  )*COL+(x+1))-*(img_temp+(y  )*COL+(x-1)))*10;
          grad_x += (int)(*(img_temp+(y+1)*COL+(x+1))-*(img_temp+(y+1)*COL+(x-1)))*3;
          grad_y += (int)(*(img_temp+(y-1)*COL+(x-1))-*(img_temp+(y+1)*COL+(x-1)))*3;
          grad_y += (int)(*(img_temp+(y-1)*COL+(x  ))-*(img_temp+(y+1)*COL+(x  )))*10;
          grad_y += (int)(*(img_temp+(y-1)*COL+(x+1))-*(img_temp+(y+1)*COL+(x+1)))*3;break;
        case laplace:
          grad_x -= *(img_temp+y*COL+x)*4;
          grad_x += *(img_temp+(y-1)*COL+x);
          grad_x += *(img_temp+(y+1)*COL+x);
          grad_x += *(img_temp+y*COL+(x-1));
          grad_x += *(img_temp+y*COL+(x+1));
          grad_y  = 0;break;
      default:return;
      }
      if(abs(grad_x)+abs(grad_y)>255)
        *(Canny_CFG->output_img+y*COL+x) = 255;
      else
        *(Canny_CFG->output_img+y*COL+x) = abs(grad_x)+abs(grad_y);


      if(grad_x != 0)
        tan_val = grad_y/grad_x;
      else
        tan_val = VERY_BIG_NUMBER;
      if(tan_val>=0)
      {
        if(tan_val<0.414)	*(grad_temp+(y-1)*COL+x-1)=0;
        else if(tan_val<2.414)	*(grad_temp+(y-1)*COL+x-1)=45;
        else			*(grad_temp+(y-1)*COL+x-1)=90;
      }
      else
      {
        if(tan_val<-2.414)	*(grad_temp+(y-1)*COL+x-1)=90;
        else if(tan_val<-0.414)  *(grad_temp+(y-1)*COL+x-1)=135;
        else			*(grad_temp+(y-1)*COL+x-1)=180;
      }

      Canny_NonMaxSupression(Canny_CFG->output_img+(y-1)*COL+x-1,*(grad_temp+(y-2)*COL+x-2));     

      if(*(Canny_CFG->output_img+(y-1)*COL+x-1)>Canny_CFG->h_threshold)
        *(Canny_CFG->output_img+(y-1)*COL+x-1)=255;
      if(*(Canny_CFG->output_img+(y-1)*COL+x-1)<Canny_CFG->l_threshold)
        *(Canny_CFG->output_img+(y-1)*COL+x-1)=0;
    }
  }
}

/*============================================================================
@ brief:        None max suppression for canny algorithm
@ param:        a pointer to a pixel, grad angle
@ return:       void
Sample Usage:   
============================================================================*/
static inline void Canny_NonMaxSupression(uint8* canny_img,uint8 angle)
{
  uint8* L  = canny_img-1;      //      ←
  uint8* R  = canny_img+1;      //      →
  uint8* U  = canny_img-COL;    //      ↑
  uint8* D  = canny_img+COL;    //      ↓
  uint8* UL = canny_img-1-COL;  //     ↖
  uint8* UR = canny_img+1-COL;  //     ↗
  uint8* DL = canny_img-1+COL;  //     ↙
  uint8* DR = canny_img+1+COL;  //     ↘
  switch(angle)
  {
  case 0:  
  case 180:if(*canny_img<*L ||*canny_img<*R)     *canny_img=0;break;
    
  case 45: if(*canny_img<*UR||*canny_img<*DL)    *canny_img=0;break;
    
  case 90: if(*canny_img<*U ||*canny_img<*D)     *canny_img=0;break;
  case 135:if(*canny_img<*UL||*canny_img<*DR)    *canny_img=0;break;
  
  default:return;
  }
}



