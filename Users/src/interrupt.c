#include "interrupt.h"

void UART3_RX_TX_IRQHandler(void)
{
  if(UART3->S1 & UART_S1_RDRF_MASK)
  {
    CAM_UART_Recv(&CAM_RX[RECV_Num]);
    RECV_Num++;
    
    if(RECV_Num==1 && CAM_RX[0]!=0xA5)  
      RECV_Num = 0;
    if(RECV_Num==3)
    {
      RECV_Num = 0;
      CAM_RX_Flag = 1;
    }
  }
}

void PORTC_IRQHandler(void)
{
  PORTC->ISFR = 0xffffffff;
  DMA_IRQ_CLEAN(DMA_CH0);
  DMA0->TCD[DMA_CH0].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(ROW*COL);  //当前主循环次数 当前循环次数耗尽后再次加载起始主循环次数
  DMA0->TCD[DMA_CH0].BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(ROW*COL);  //起始主循环次数

  DMA0->TCD[DMA_CH0].SADDR = (uint32)(uint8 *)(PTC_BASE+0x10+1);     // 设置  源地址
  DMA0->TCD[DMA_CH0].DADDR = (uint32)Car_Parameter.IMAGE.source_img;                           // 设置目的地址
  DMA_EN(DMA_CH0);
}

void DMA0_IRQHandler(void)
{
  DMA_IRQ_CLEAN(DMA_CH0);
  CAM_Finished =1;
}

void PORTB_IRQHandler(void)
{
  NRF_Recv(NRF_RX_Buff);
  HAS_COMMAND = 1;
  PORTB->ISFR = 0xffffffff;
}

