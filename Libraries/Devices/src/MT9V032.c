/*
MT9V032 Camera Pins Defination:
------------------------------------ 
        Camera Pin       MCU Pin
------------------------------------
        SDA(51RX)         C17
        SCL(51TX)         C16
          VSCYN           C6
          PSYCN           C18           
        Data Port        C[8:15] 
------------------------------------ 
*/


#include "MT9V032.h"

uint8 CAM_RX[3] = {0};
uint8 CAM_TX[4] = {0};
uint8 RECV_Num = 0;
uint8 CAM_RX_Flag  = 0;
uint8 CAM_Finished = 0;

#ifdef _SMART_CAR_H
static inline void RecordData(void);
#endif
//需要配置到摄像头的数据
int16 MT9V032_CFG[CONFIG_FINISH][2]=
{
    {AUTO_EXP,          0},   //自动曝光设置      范围1-63 0为关闭 如果自动曝光开启  EXP_TIME命令设置的数据将会变为最大曝光时间，也就是自动曝光时间的上限
                              //一般情况是不需要开启这个功能，因为比赛场地光线一般都比较均匀，如果遇到光线非常不均匀的情况可以尝试设置该值，增加图像稳定性
    {EXP_TIME,          300}, //曝光时间          摄像头收到后会自动计算出最大曝光时间，如果设置过大则设置为计算出来的最大曝光值
    {FPS,               140},  //图像帧率          摄像头收到后会自动计算出最大FPS，如果过大则设置为计算出来的最大FPS
    {SET_COL,           COL}, //图像列数量        范围1-752     K60采集不允许超过188
    {SET_ROW,           ROW}, //图像行数量        范围1-480
    {LR_OFFSET,         39},   //图像左右偏移量    正值 右偏移   负值 左偏移  列为188 376 752时无法设置偏移    摄像头收偏移数据后会自动计算最大偏移，如果超出则设置计算出来的最大偏移
    {UD_OFFSET,         0},   //图像上下偏移量    正值 上偏移   负值 下偏移  行为120 240 480时无法设置偏移    摄像头收偏移数据后会自动计算最大偏移，如果超出则设置计算出来的最大偏移
    {GAIN,              64},  //图像增益          范围16-64     增益可以在曝光时间固定的情况下改变图像亮暗程度

    {INIT,              0}    //摄像头开始初始化
};

int16 GET_CFG[CONFIG_FINISH-1][2]=
{
    {AUTO_EXP,          0},   //自动曝光设置      
    {EXP_TIME,          0},   //曝光时间          
    {FPS,               0},   //图像帧率          
    {SET_COL,           0},   //图像列数量        
    {SET_ROW,           0},   //图像行数量        
    {LR_OFFSET,         0},   //图像左右偏移量    
    {UD_OFFSET,         0},   //图像上下偏移量    
    {GAIN,              0},   //图像增益          
};


void Camera_Init(void)
{
  CAM_PORT_Configuration();
  CAM_UART_Configuration();
  systick_delay_ms(1000);
  SET_Config();
  GET_Config();
  CAM_IRQ_Configuration();
  CAM_DMA_Configuration();
  
}

void CAM_PORT_Configuration(void)
{
  __disable_irq();
  uint8 cnt =16;
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  PORTC->PCR[16] = ALT3;        //UART3_RX
  PORTC->PCR[17] = ALT3;        //UART3_TX
  PORTC->ISFR |= (1<<16)|(1<<17);
  PTC->PDDR &= (~(uint32)((1<<16)|(1<<17)));
  
  PORTC->PCR[18] = ALT1| DMA_RISING;
  PTC->PDDR &= ~((uint32)(1<<18));
  while((cnt--)>8)
  {
    PORTC->PCR[cnt] = ALT1| PULLDOWN;
    PTC->PDDR &= (~(uint32)(1<<cnt));
  }
  
  PORTC->PCR[6] = ALT1|PULLDOWN|IRQ_FALLING;
  PTC->PDDR &= ~((uint32)(1<<6));
  __enable_irq();
}

void CAM_UART_Configuration(void)
{
  SIM->SCGC4  |= SIM_SCGC4_UART3_MASK;
  vuint32 UBD  = (bus_clk_mhz * 1000000)/(9600 * 16);
  uint32 BRFA = (bus_clk_mhz * 1000000 * 2 / 9600)  - (UBD * 32);
  if(UBD > 0x1FFF)      UBD = 0x1FFF;
  if(BRFA>0x1F)         BRFA = 0x1F;
  
  //UART Memory Map and Register        [Chapter50  Page:1497]
  //Control Register2                   [Chapter50.3.1  Page:1505]
  UART3->C2 &= ~(0
                 |UART_C2_TE_MASK       //Transimitter Disable
                 |UART_C2_RE_MASK);     //Receiver Disable
  UART3->C1 |= 0;
  
  //Baud Rate Registers: High & Low     [Chapter50.3.2  Page:1505]
  UART3->BDH &= ~UART_BDH_SBR_MASK;     //Clear Previous Baud Rate Set
  UART3->BDH = UBD>>8;              //Write the 8Bits of the Baud rate
  UART3->BDL = (uint8)(UBD);
  //Control Register4                   [Chapter50.3.11  Page:1517]
  UART3->C4 &= ~UART_C4_BRFA_MASK;
  UART3->C4 |= UART_C4_BRFA(BRFA);         //Baud Rate Fine Adjust  ?
  //FIFO Parameters                     [Chapter52.3.6  Page:1522]
  UART3->PFIFO |= (0
                | UART_PFIFO_TXFE_MASK
                | UART_PFIFO_RXFE_MASK);
  UART3->C2 |= (0
                |UART_C2_RIE_MASK       //Receive Interrupt Enable
                |UART_C2_TE_MASK        //Transimitter Enable
                |UART_C2_RE_MASK);      //Receiver Enable
  NVIC_EnableIRQ(UART3_RX_TX_IRQn);
}

void CAM_DMA_Configuration(void)
{
  SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;        
  SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
  
  //Transfer Control Descriptor 
  DMA0->TCD[0].SADDR = 0;
  DMA0->TCD[0].SADDR = (uint32)(uint8 *)(PTC_BASE+0x10+1);                   // 设置  源地址
  DMA0->TCD[0].DADDR = 0;                               
  DMA0->TCD[0].DADDR = (uint32)Car_Parameter.IMAGE.source_img;                   // 设置目的地址
  DMA0->TCD[0].SOFF = 0;                                
  DMA0->TCD[0].SOFF = 0x00u;                            // 设置源地址偏移 = 0x0, 即不变
  DMA0->TCD[0].DOFF = 0;                                
  DMA0->TCD[0].DOFF = 1;                            // 每次传输后，目的地址加 BYTEs
  DMA0->TCD[0].ATTR = 0;
  DMA0->TCD[0].ATTR = (0
                       | DMA_ATTR_SMOD(0x0)               // 源地址模数禁止  Source address modulo feature is disabled
                       | DMA_ATTR_SSIZE(0)            // 源数据位宽 ：DMA_BYTEn  。    SSIZE = 0 -> 8-bit ，SSIZE = 1 -> 16-bit ，SSIZE = 2 -> 32-bit ，SSIZE = 4 -> 16-byte
                       | DMA_ATTR_DMOD(0x0)               // 目标地址模数禁止
                       | DMA_ATTR_DSIZE(0)            // 目标数据位宽 ：DMA_BYTEn  。  设置参考  SSIZE
                      );
  DMA0->TCD[0].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(ROW*COL);   //当前主循环次数 当前循环次数耗尽后再次加载起始主循环次数
  DMA0->TCD[0].BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(ROW*COL);   //起始主循环次数
  DMA0->CR &= ~DMA_CR_EMLM_MASK;                                // CR[EMLM] = 0
  DMA0->TCD[0].NBYTES_MLNO = DMA_NBYTES_MLNO_NBYTES(1); // 通道每次传输字节数，这里设置为BYTEs个字节。注：值为0表示传输4GB 
  
  
  // 配置 DMA 传输结束后的操作 
  DMA0->TCD[0].SLAST = 0;//调整  源地址的附加值,主循环结束后恢复  源地址
  DMA0->TCD[0].DLAST_SGA = 0; //调整目的地址的附加值,主循环结束后恢复目的地址或者保持地址;
  DMA0->TCD[0].CSR = 0;
  DMA0->TCD[0].CSR = (0
                           | DMA_CSR_BWC(3)               //带宽控制,每读一次，eDMA 引擎停止 8 个周期（0不停止；1保留；2停止4周期；3停止8周期）
                           | DMA_CSR_DREQ_MASK            //主循环结束后停止硬件请求
                           | DMA_CSR_INTMAJOR_MASK        //主循环结束后产生中断
                          );
  DMA0->TCD[0].SLAST = 0;
  //DMA Source
  //Channel Configuration register
  DMAMUX0->CHCFG[0] = (0
                          | DMAMUX_CHCFG_ENBL_MASK       // Enable routing of DMA request 
                          | DMAMUX_CHCFG_SOURCE(kDmaRequestMux0PortC));
  //DMA Enable
  DMA0->ERQ &= ~(DMA_ERQ_ERQ0_MASK);       //Disable DMA0_CH0
  DMA0->INT |= (DMA_INT_INT0_MASK);
}

void CAM_IRQ_Configuration(void)
{ 
  
  DMA0->ERQ &=~(DMA_ERQ_ERQ0_MASK<<(DMA_CH0));
  DMA0->INT |=(DMA_INT_INT0_MASK<<DMA_CH0);
  NVIC_EnableIRQ(DMA0_IRQn);
  DMA0->ERQ |= (DMA_ERQ_ERQ0_MASK<<(DMA_CH0));
  
  NVIC_SetPriority(PORTC_IRQn,1);
  NVIC_EnableIRQ(PORTC_IRQn);
}

void CAM_UART_Send(uint8* buf,uint32 lenth)
{
  while(lenth--)
  {
    while( !((UART3->S1) & UART_S1_TDRE_MASK) );
    UART3->D = (uint8)(*buf); 
    buf++;
  }
}

void CAM_UART_Recv(uint8* buf)
{
  while(!(UART3->S1 & UART_S1_RDRF_MASK));
  *buf = UART3->D;
}

void GET_Config(void)
{
  uint16 temp, cnt;
  RECV_Num = 0;
  for(cnt=0; cnt<CONFIG_FINISH-1; cnt++)
  {
    CAM_TX[0] = 0xA5;
    CAM_TX[1] = GET_STATUS;
    temp = GET_CFG[cnt][0];
    CAM_TX[2] = temp>>8;
    CAM_TX[3] = (uint8)temp;
    
    CAM_UART_Send(CAM_TX,4);
    
    //等待接受回传数据
    while(!CAM_RX_Flag);
    CAM_RX_Flag = 0;
    GET_CFG[cnt][1] = CAM_RX[1]<<8 | CAM_RX[2];
  }
#ifdef _SMART_CAR_H
  RecordData();
#endif
}

void SET_Config(void)
{
  uint8 cnt;
  uint16 temp;
  for(cnt=0; cnt<CONFIG_FINISH; cnt++)
    {
      CAM_TX[0] = 0xA5;
      CAM_TX[1] = MT9V032_CFG[cnt][0];
      temp = MT9V032_CFG[cnt][1];
      CAM_TX[2] = temp>>8;
      CAM_TX[3] = (uint8)temp;
      
      CAM_UART_Send(CAM_TX,4);
      systick_delay_ms(2);
    }
    //等待摄像头初始化成功
    while(!CAM_RX_Flag);
    CAM_RX_Flag = 0;
    while((CAM_RX[1]!=0xff) || (CAM_RX[2]!=0xff));
}

#ifdef _ST7735_H

void DSP_CameraPara()
{
  TFT_Printf("");
}

#endif

#ifdef _SMART_CAR_H
static inline void RecordData(void)
{
  Car_Parameter.CAMERA.auto_exp  = GET_CFG[0][1];
  Car_Parameter.CAMERA.exp_time  = GET_CFG[1][1];
  Car_Parameter.CAMERA.fps       = GET_CFG[2][1];
  Car_Parameter.CAMERA.col       = GET_CFG[3][1];
  Car_Parameter.CAMERA.row       = GET_CFG[4][1];
  Car_Parameter.CAMERA.lr_offset = GET_CFG[5][1];
  Car_Parameter.CAMERA.ud_offset = GET_CFG[6][1];
  Car_Parameter.CAMERA.gain      = GET_CFG[7][1];
}
#endif

