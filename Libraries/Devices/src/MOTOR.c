#include "MOTOR.h"

void MOTOR_Init(Motor_Typedef* motor_p)
{
  MOTOR_Port_Configuration();
  MOTOR_FTM_Configuration(motor_p->freq,0);
  MOTOR_Duty(LEFT_ACTIVE_CH,motor_p->l_duty);
  MOTOR_Duty(RIGHT_ACTIVE_CH,motor_p->r_duty);
}

void MOTOR_Port_Configuration(void)
{
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK|SIM_SCGC5_PORTE_MASK;
  for(int i=1;i<=4;i++)
  {
    PORTC->PCR[i] &= ~((uint32)(PORT_PCR_MUX_MASK));
    PORTC->PCR[i] |= ALT4;
  }
  PTC->PDDR &= ~(uint32)((1<<4)|(1<<3)|(1<<2)|(1<<1));
  
  PORTE->PCR[1] &= ~((uint32)(PORT_PCR_MUX_MASK)); 
  PORTE->PCR[2] &= ~((uint32)(PORT_PCR_MUX_MASK));
  PORTE->PCR[1] |= ALT1;
  PORTE->PCR[2] |= ALT1;
  PTE->PDDR     |= (uint32)((1<<1)|(1<<2));
  PTE->PDOR     |= (uint32)((1<<1)|(1<<2));
}

void MOTOR_FTM_Configuration(uint32 freq,uint32 duty)
{
  const uint32 clk_hz = (bus_clk_mhz * 1000 * 1000);
  uint16 mod;
  uint16 cv;
  uint8  ps = 0;
  //First to calculate the CLK Prescale and Counter Value
  mod = (clk_hz >> 16 ) / freq ;
  while((mod >> ps) >= 1)
    ps++;
  mod = (clk_hz >> ps) / freq;
  cv = (duty*(mod-0+1))/PWM_PRECISON;
/*      PS      Frequency Divider
        0               1
        1               2
        2               4
        3               8
       ...             ...
        n              2^n      */
  mod = (clk_hz >> ps) / freq;
  Car_Parameter.MOTOR.ftm_mod = mod;
  //FlexTimer Module
  SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;
  FTM0->MOD = mod;              //Modulo Register------The overflow number for FTM counter
  for(int ch=0;ch<=3;ch++)
  {
    FTM0->CONTROLS[ch].CnSC &= ~FTM_CnSC_ELSA_MASK;
    FTM0->CONTROLS[ch].CnSC = FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
    FTM0->CONTROLS[ch].CnV = cv;
  }
  FTM0->SC    = (0                //Prescale Factor Selection
                | FTM_SC_PS(ps)  
                | FTM_SC_CLKS(1));//Select Bus Clock
  FTM0->CNTIN = 0;              //The initial value of FTM Counter
  FTM0->CNT   = 0;
}

void MOTOR_Duty(uint8 ch,uint32 duty)
{
  uint32 cv;
  cv = (duty * (Car_Parameter.MOTOR.ftm_mod - 0 + 1)) / PWM_PRECISON;
  FTM0->CONTROLS[ch].CnV = cv;
}

void MOTOR_Direction(MotorDirType dir)
{
  if(dir == forward)
  {
    MOTOR_Duty(RIGHT_INACTIVE_CH,0);
    MOTOR_Duty(LEFT_INACTIVE_CH,0);
  }
  if(dir == backward)
  {
    MOTOR_Duty(RIGHT_INACTIVE_CH,100);
    MOTOR_Duty(LEFT_INACTIVE_CH,100);
  }
}

inline void MOTOR_CheckParam(void)
{
  if(Car_Parameter.MOTOR.l_duty > Car_Parameter.MOTOR.max_duty)
    Car_Parameter.MOTOR.l_duty = Car_Parameter.MOTOR.max_duty;
  if(Car_Parameter.MOTOR.r_duty > Car_Parameter.MOTOR.max_duty)
    Car_Parameter.MOTOR.r_duty = Car_Parameter.MOTOR.max_duty;
}
