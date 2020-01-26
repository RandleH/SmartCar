#include "nxp_contest.h"


void INIT_RoadData(RoadInfoTypedef* road_p)
{
  memset(road_p->center,COL/2,sizeof(road_p->center));
  memset(road_p->right_edge,COL,sizeof(road_p->right_edge));
  memset(road_p->left_edge,0,sizeof(road_p->left_edge));
}

void GET_RoadFeature(RoadInfoTypedef* road_p,uint8* img)
{
  uint8 threshold = road_p->threshold;
  for(int i=road_p->row_end;i>road_p->row_start;i--)
  {
    road_p->right_edge[i] = M2R_scan(i,road_p,img);
    road_p->left_edge[i]  = M2L_scan(i,road_p,img);
  }
}

void GET_MagnetFeature()
{
  
}

void CTRL_Servo(RoadInfoTypedef* road_p,Servo_Typedef* servo)
{
  
}

void EDGE_Smooth(RoadInfoTypedef* road_p)
{
  
}



static inline uint8 M2R_scan(uint8 row,RoadInfoTypedef* road_p,uint8* img)
{
  uint8 j;
  for(j=road_p->center[row+1];j<road_p->col_end;j++)
  {
    if(*(img+row*COL+j)==0&&*(img+row*COL+j-1)==0xff)
      break;
  }
  return j;
}
static inline uint8 M2L_scan(uint8 row,RoadInfoTypedef* road_p,uint8* img)
{
  uint8 j;
  for(j=road_p->center[row+1];j>road_p->col_start;j--)
  {
    if(*(img+row*COL+j)==0&&*(img+row*COL+j+1)==0xff)
      break;
  }
  return j;
}
static inline uint8 L2R_scan(uint8 row,RoadInfoTypedef* road_p,uint8* img)
{
  
}
static inline uint8 R2L_scan(uint8 row,RoadInfoTypedef* road_p,uint8* img)
{
  
}
