


#include "systick.h"
#define EX_REF_CLK  50 //(定义外部参考时钟为50MHZ)
uint32 mcgout_clk_mhz = 100;
uint32 core_clk_mhz = 100;
uint32 bus_clk_mhz = 100;

void get_clk(void)
{
  uint32 temp1,temp2;
  temp1 = MCG->C6 & MCG_C6_VDIV0_MASK;
  temp2 = MCG->C5 & MCG_C5_PRDIV0_MASK; 
  mcgout_clk_mhz = EX_REF_CLK * ((temp1) + 16) / ((temp2) + 1)/2;
  core_clk_mhz = mcgout_clk_mhz / ((SIM->CLKDIV1 >> SIM_CLKDIV1_OUTDIV1_SHIFT) + 1);
  bus_clk_mhz = mcgout_clk_mhz / (((SIM->CLKDIV1 >> SIM_CLKDIV1_OUTDIV2_SHIFT) & 0x0f) + 1);
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      systick延时函数
//  @param      time            需要延时的时间
//  @return     void
//  @since      v1.0
//  Sample usage:               systick_delay(1000);   //延时1000个内核时钟周期
//-------------------------------------------------------------------------------------------------------------------
void systick_delay(uint32 time)
{
    if(time == 0)   return;

    SysTick->CTRL = 0x00;//先关了 systick ,清标志位
    SysTick->LOAD = time;//设置延时时间
    SysTick->VAL = 0x00;//清空计数器
    SysTick->CTRL = ( 0 | SysTick_CTRL_ENABLE_Msk         //使能 systick
                  //| SysTick_CTRL_TICKINT_Msk        //使能中断 (注释了表示关闭中断)
                    | SysTick_CTRL_CLKSOURCE_Msk      //时钟源选择 (core clk) K60只能选择(core clk)
                );
     while( !(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));//等待时间到
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      毫秒级systick延时函数
//  @param      ms              延时多少毫秒
//  @return     void
//  @since      v1.0
//  Sample usage:               systick_delay_ms(1000);   //延时1000毫秒
//-------------------------------------------------------------------------------------------------------------------
void systick_delay_ms(uint32 ms)
{
    //get_clk();//获取内核时钟便于后面设置
	while(ms--) systick_delay(core_clk_mhz*1000);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      systick定时器
//  @param      time            定时时间(0-0x00ffffff)
//  @return     void
//  @since      v1.0
//  Sample usage:               systick_timing(1000);   //定时1000个内核时钟周期
//-------------------------------------------------------------------------------------------------------------------
void systick_timing(uint32 time)
{
    time &= 0x00ffffff;     	//数据强制变换为24位 
    SysTick->LOAD = time;       //设置延时时间
    SysTick->VAL = 0x00;       	//清空计数器
    SysTick->CTRL = ( 0 
					| SysTick_CTRL_ENABLE_Msk       //使能 systick
                    | SysTick_CTRL_TICKINT_Msk      //使能中断
                    | SysTick_CTRL_CLKSOURCE_Msk    //时钟源选择 (core clk) K60只能选择(core clk)
                );
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      获得当前System tick timer的值
//  @return     返回当前System tick timer的值
//  @since      v1.0
//  Sample usage:               uint32 tim = systick_getval();   
//-------------------------------------------------------------------------------------------------------------------
uint32 systick_getval(void)
{
    return SysTick->VAL;
}


