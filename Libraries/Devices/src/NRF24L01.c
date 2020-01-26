#include "NRF24L01.h"

uint8 NRF_RF_Addr[NRF_ADDR_WIDTH]={0xff,0xff,0xff,0xff,0xff};   // TX,RX地址,可自行更改
uint8 NRF_RX_Buff[NRF_DATA_WIDTH]={0};                      	// 接收数据缓冲区
uint8 NRF_TX_Buff[NRF_DATA_WIDTH]={0};                      	// 发送数据缓冲区

void NRF_Init(void)
{
  NRF_PORT_Init();
  NRF_RecvMode();
}

void NRF_SPI_Configuration(void)
{
  
}

void NRF_PORT_Init(void)
{
#if NRF_SOFTWARE
  SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK
                |SIM_SCGC5_PORTB_MASK
                |SIM_SCGC5_PORTC_MASK);
  PORTB->PCR[NRF_MOSI_PIN] &= ~((uint32)(PORT_PCR_MUX_MASK));
  PORTB->PCR[NRF_MOSI_PIN] |= ALT1|PULLUP;
  PORTB->PCR[NRF_SCK_PIN]  &= ~((uint32)(PORT_PCR_MUX_MASK));
  PORTB->PCR[NRF_SCK_PIN] |= ALT1|PULLUP;
  PORTB->PCR[NRF_MISO_PIN] &= ~((uint32)(PORT_PCR_MUX_MASK));
  PORTB->PCR[NRF_MISO_PIN] |= ALT1|PULLUP;
  PORTB->PCR[NRF_IRQ_PIN]  &= ~((uint32)(PORT_PCR_MUX_MASK));
  PORTB->PCR[NRF_IRQ_PIN] |= ALT1|PULLUP|IRQ_ZERO;
  GPIOB->PDDR |=  (uint32)((1<<NRF_MOSI_PIN)|(1<<NRF_SCK_PIN));
  GPIOB->PDDR &= ~(uint32)((1<<NRF_MISO_PIN)|(1<<NRF_IRQ_PIN));
 
  PORTA->PCR[NRF_CSN_PIN] &= ~((uint32)(PORT_PCR_MUX_MASK));
  PORTA->PCR[NRF_CSN_PIN] |= ALT1|PULLUP;
  GPIOA->PDDR |= (uint32)(1<<NRF_CSN_PIN);
  
  PORTC->PCR[NRF_CE_PIN] &= ~((uint32)(PORT_PCR_MUX_MASK));
  PORTC->PCR[NRF_CE_PIN] |= ALT1|PULLUP;
  GPIOC->PDDR |= (uint32)(1<<NRF_CE_PIN);
#else
#endif
  
  NVIC_SetPriority(PORTB_IRQn,0);
  NVIC_EnableIRQ(PORTB_IRQn);
}

uint8 NRF_WriteReg(uint8 cmd, uint8 val)
{
  uint8 status;
  NRF_CSN(0);                    // NRF_CSN_PIN置低,开始传输数据
  status = NRF_WriteByte(cmd);    // 写入寄存器地址,同时返回状态字
  NRF_WriteByte(val);          //然后写数据到该寄存器
  NRF_CSN(1);              // NRF_CSN_PIN拉高,结束数据传输
  return(status);          // 返回状态寄存器    
}

uint8 NRF_ReadReg(uint8 cmd)
{
  uint8 val;
  NRF_CSN (0);                                    		// NRF_CSN_PIN置低,开始传输数据
  NRF_WriteByte(cmd);                               	// 写入寄存器地址
  val = NRF_WriteByte(0);                           	// 然后从该寄存器读数据
  NRF_CSN (1);                                    		// NRF_CSN_PIN拉高,结束数据传输
  return(val);                                        	// 返回寄存器数据
}

uint8 NRF_WriteCmd(uint8 cmd)
{
  uint8 status;
  NRF_CSN(0);                                    		// NRF_CSN_PIN置低,开始传输数据
  status = NRF_WriteByte(cmd);                      	// 写入寄存器地址,同时返回状态字
  NRF_CSN(1);                                    		// NRF_CSN_PIN拉高,结束数据传输
  return(status);                                     	// 返回状态寄存器    
}

uint8 NRF_WriteRegBurst(uint8 cmd, uint8 *p, uint8 len)
{
  uint8 status, i;
  NRF_CSN (0);                                    		// NRF_CSN_PIN置低,开始传输数据
  status = NRF_WriteByte(cmd);                      	// 选择寄存器,同时返回状态字
  for(i=0; i<len; i++)	
      NRF_WriteByte(p[i]);                          	// 逐个字节写入nRF24L01+
  NRF_CSN(1);                                    		// NRF_CSN_PIN拉高,结束数据传输
  return(status);                                     	// 返回状态寄存器    
}

uint8 NRF_ReadRegBurst(uint8 cmd, uint8 *p, uint8 len)
{
  uint8 status, i;
  NRF_CSN (0);                                    		// NRF_CSN_PIN置低,开始传输数据
  status = NRF_WriteByte(cmd);                      	// 选择寄存器,同时返回状态字
  for(i=0; i<len; i++)	
      p[i] = NRF_WriteByte(0);                      	// 逐个字节从nRF24L01+读出
  NRF_CSN (1);                                    		// NRF_CSN_PIN拉高,结束数据传输
  return(status);                                     	// 返回状态寄存器    
}

uint8 NRF_WriteByte(uint8 byte)
{
  uint8 i;
  for(i=0; i<8; i++)
  {                            	
    NRF_MOSI((byte&0x80)>>7);               			// 将byte最高位输出到NRF_MOSI_PIN
    byte <<= 1;                                		// 低一位移位到最高位
    NRF_SCK (1);                            		// 拉高NRF_SCK_PIN,nRF24L01从NRF_MOSI_PIN读入1位数据,同时从NRF_MISO_PIN输出1位数据
    byte |= NRF_MISO;                       		// 读NRF_MISO_PIN到byte最低位
    NRF_SCK (0);                            		// NRF_SCK_PIN置低
  }	
  return(byte);                                      		// 返回读出的一字节
}

void NRF_RecvMode(void)
{
  NRF_CE  (0);                                    		// 进入待机模式
  NRF_CSN (1);                                    		// SPI禁止
  NRF_SCK (0);                                    		// SPI时钟置低
  
  NRF_CE (0);                                                                     	// NRF_CE_PIN置低,进入待机模式
  NRF_WriteReg(NRF_CMD_W_REGISTER | NRF_REG_CONFIG,     NRF_RCD_TX_DS_DI        // 禁用发送完成中断
                                  | NRF_RCD_MAX_RT_DI       // 禁用发送超时中断
                                  | NRF_RCD_CRC_EN          // 启用CRC
                                  | NRF_RCD_CRC16           // CRC16
                                  | NRF_RCD_PWR_UP          // 上电模式
                                  | NRF_RCD_PRIM_PRX);      // RX模式
  NRF_WriteReg(NRF_CMD_W_REGISTER | NRF_REG_EN_AA,      NRF_RCD_PIPE0_AA_EN);   // 使能通道0自动应答
  NRF_WriteReg(NRF_CMD_W_REGISTER | NRF_REG_EN_RXADDR,  NRF_RCD_PIPE0_RX_EN);   // 使能接收通道0
  NRF_WriteReg(NRF_CMD_W_REGISTER | NRF_REG_SETUP_AW,   NRF_RCD_AW_5byte);      // 设置RX地址宽度
  NRF_WriteReg(NRF_CMD_W_REGISTER | NRF_REG_SETUP_RETR, NRF_RCD_ARC_10|NRF_RCD_ARD_250us);        // 使能自动重发  间隔时间250US
  NRF_WriteReg(NRF_CMD_W_REGISTER | NRF_REG_RF_CH,      NRF_RF_CH);             // 设置射频通道
  NRF_WriteReg(NRF_CMD_W_REGISTER | NRF_REG_RF_SETUP,   NRF_RCD_RF_DR_2Mbps     // 设置传输速率为2Mbps
                                                            | NRF_RCD_RF_PWR_0dBm);   // 发射功率为0dBm
  NRF_WriteReg(NRF_CMD_W_REGISTER | NRF_REG_RX_PW_P0,   NRF_DATA_WIDTH);        // 设置RX通道0数据宽度

  NRF_WriteRegBurst(NRF_CMD_W_REGISTER | NRF_REG_RX_ADDR_P0, NRF_RF_Addr, NRF_ADDR_WIDTH);   // 设置RX通道0地址
  NRF_WriteCmd(NRF_CMD_FLUSH_RX);                                               // 清空RXFIFO
  NRF_CE (1);
}

uint8 NRF_Recv(uint8 *p)
{
  uint8 sta=0;
  if(NRF_IRQ==0)
  {                                                                 	// 等待NRF_IRQ_PIN置低
      sta = NRF_ReadReg(NRF_CMD_R_REGISTER | NRF_REG_STATUS);                  // 读取STATUS寄存器
      if(sta & NRF_RCD_RX_DR)
      {                                                        // RX_DS为1时表明接收到数据
          NRF_WriteReg(NRF_CMD_W_REGISTER | NRF_REG_STATUS, NRF_RCD_RX_DR);     // 写回RX_DS,重置NRF_IRQ_PIN中断
          NRF_ReadRegBurst(NRF_CMD_R_RX_PAYLOAD, p, NRF_DATA_WIDTH);               // 读取nRF24L01+中的接收数据
          return(1);                                                                  // 返回接收完成标志
      }
  }
  return(0);                                     // 返回接收检查失败标志
}

uint8 NRF_Send(uint8* TX_Buff)
{
    uint8 sta=0;
    NVIC_DisableIRQ(PORTB_IRQn);
    NRF_CE (0);                                                                     	// NRF_CE_PIN置低,进入待机模式
    
    NRF_WriteReg(NRF_CMD_W_REGISTER | NRF_REG_CONFIG, NRF_RCD_RX_DR_DI            // 禁用发送完成中断
                                                          | NRF_RCD_MAX_RT_DI           // 禁用发送超时中断
                                                          | NRF_RCD_CRC_EN              // 启用CRC
                                                          | NRF_RCD_CRC16               // CRC16
                                                          | NRF_RCD_PWR_UP              // 上电模式
                                                          | NRF_RCD_PRIM_PTX);          // TX模式
    NRF_WriteReg(NRF_CMD_W_REGISTER | NRF_REG_EN_AA, NRF_RCD_PIPE0_AA_EN
                                                          | NRF_RCD_PIPE1_AA_EN
                                                          | NRF_RCD_PIPE2_AA_EN
                                                          | NRF_RCD_PIPE3_AA_EN
                                                          | NRF_RCD_PIPE4_AA_EN
                                                          | NRF_RCD_PIPE5_AA_EN);
    NRF_WriteReg(NRF_CMD_W_REGISTER | NRF_REG_SETUP_RETR,NRF_RCD_ARD_750us
                                                          | NRF_RCD_ARC_15);

    NRF_WriteRegBurst(NRF_CMD_W_REGISTER | NRF_REG_TX_ADDR, NRF_RF_Addr, NRF_ADDR_WIDTH);  // 设置TX地址
    NRF_WriteRegBurst(NRF_CMD_W_TX_PAYLOAD, TX_Buff, NRF_DATA_WIDTH);                       // 写入发送数据
    NRF_CE (1);                                                                     	// NRF_CE_PIN置高,进入运行模式
    systick_delay_ms(1);
    while(1)
    {
      sta = NRF_ReadReg(NRF_CMD_R_REGISTER | NRF_REG_STATUS);
      if(sta&0x10)//发送失败
      {
          NRF_WriteReg(NRF_CMD_W_REGISTER | NRF_REG_STATUS, 0x10); 				// 写回TX_DR,重置NRF_IRQ_PIN中断
          NRF_WriteReg(NRF_CMD_FLUSH_TX,0xff);
          break;
      }
      if(sta&0x20)//发送成功
      {
          NRF_WriteReg(NRF_CMD_W_REGISTER | NRF_REG_STATUS, NRF_RCD_TX_DR); 	// 写回TX_DR,重置NRF_IRQ_PIN中断
          sta = TX_OK;
          NRF_RecvMode();  
          NVIC_EnableIRQ(PORTB_IRQn);
          break;   
      }
    }
    return sta;
}



