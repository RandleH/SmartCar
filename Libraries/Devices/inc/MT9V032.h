#include "headfiles.h"
#ifndef _MT9V032_H
#define _MT9V032_H

#define COL     160//图像宽度   范围1-752     K60采集不允许超过188
#define ROW     120//图像高度	范围1-480


typedef enum
{
    INIT = 0,               //摄像头初始化命令
    AUTO_EXP,               //自动曝光命令
    EXP_TIME,               //曝光时间命令
    FPS,                    //摄像头帧率命令
    SET_COL,                //图像列命令
    SET_ROW,                //图像行命令
    LR_OFFSET,              //图像左右偏移命令
    UD_OFFSET,              //图像上下偏移命令
    GAIN,                   //图像偏移命令
    CONFIG_FINISH,          //非命令位，主要用来占位计数
    
    SET_EXP_TIME = 0XF0,    //单独设置曝光时间命令
    GET_STATUS,             //获取摄像头配置命令
    GET_VERSION,            //固件版本号命令
    
    SET_ADDR = 0XFE,        //寄存器地址命令
    SET_DATA                //寄存器数据命令
}CMD;

#define  DMA_CH0 0
#define  DMA_IRQ_DIS(DMA_CHn)   disable_irq((IRQn_Type)((IRQn_Type)DMA_CHn + DMA0_IRQn))              //禁止DMA通道传输完成中断

#define  DMA_IRQ_CLEAN(DMA_CHn) DMA0->INT|=(DMA_INT_INT0_MASK<<DMA_CHn)           //清除通道传输中断标志位

#define  DMA_EN(DMA_CHn)        DMA0->ERQ |= (DMA_ERQ_ERQ0_MASK<<(DMA_CHn))       //使能通道硬件DMA请求
#define  DMA_DIS(DMA_CHn)       DMA0->ERQ &=~(DMA_ERQ_ERQ0_MASK<<(DMA_CHn))       //禁止通道硬件DMA请求

#define  DMA_CHn_DIS(DMA_CHn)   DMAMUX->CHCFG[DMA_CHn] &= ~DMAMUX_CHCFG_SOURCE(Channel_Disabled)  //禁用通道

void Camera_Init(void);
void CAM_PORT_Configuration(void);
void CAM_UART_Configuration(void);
void CAM_DMA_Configuration(void);
void CAM_IRQ_Configuration(void);
void CAM_UART_Recv(uint8* buf);
void CAM_UART_Send(uint8* buf,uint32 lenth);
void GET_Config(void);
void SET_Config(void);
extern uint8 RECV_Num;
extern uint8 CAM_RX[3];
extern uint8 CAM_RX_Flag;
extern uint8 CAM_Finished;
extern int16 MT9V032_CFG[CONFIG_FINISH][2];

#endif



