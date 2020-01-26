#include "headfiles.h"
#ifndef _CONTEST_H
#define _CONTEST_H


/*
typedef struct
{
  uint8 width;
  uint8 center[ROW];
  uint8 left_edge[ROW];
  uint8 right_edge[ROW];
  int8  servo_angle;
  uint8 threshold;
  uint8 row_start;
  uint8 row_end;
  uint8 col_start;
  uint8 col_end;
}RoadInfoTypedef;*/

static inline uint8 M2R_scan(uint8 row,RoadInfoTypedef* road_p,uint8* img);
static inline uint8 M2L_scan(uint8 row,RoadInfoTypedef* road_p,uint8* img);
static inline uint8 L2R_scan(uint8 row,RoadInfoTypedef* road_p,uint8* img);
static inline uint8 R2L_scan(uint8 row,RoadInfoTypedef* road_p,uint8* img);

void INIT_RoadData(RoadInfoTypedef* road_p);
void GET_RoadFeature(RoadInfoTypedef* road_p,uint8* img);
void GET_MagnetFeature();
void CTRL_Servo(RoadInfoTypedef* road_p,Servo_Typedef* servo);
void EDGE_Smooth(RoadInfoTypedef* road_p);


#endif

