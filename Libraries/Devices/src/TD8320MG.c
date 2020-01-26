#include "TD8320MG.h"

void TD8320MG_Init(Servo_Typedef* servo_p)
{
  TD8320MG_Port_Configuration();
  TD8320MG_FTM_Configuration(servo_p->freq,servo_p->duty);
}

void TD8320MG_Port_Configuration(void)
{
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
  PORTE->PCR[5] &= ~((uint32)(PORT_PCR_MUX_MASK));
  PORTE->PCR[5] |= ALT6;
  PTE->PDDR &= ~(uint32)(1<<5);
}

void TD8320MG_FTM_Configuration(uint32 freq,uint32 duty)
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
  Car_Parameter.SERVO.ftm_mod = mod;
  //FlexTimer Module
  SIM->SCGC3 |= SIM_SCGC3_FTM3_MASK;
  FTM3->MOD = mod;              //Modulo Register------The overflow number for FTM counter
  for(int ch=0;ch<=0;ch++)
  {
    FTM3->CONTROLS[ch].CnSC &= ~FTM_CnSC_ELSA_MASK;
    FTM3->CONTROLS[ch].CnSC = FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
    FTM3->CONTROLS[ch].CnV = cv;
  }
  FTM3->SC    = (0                //Prescale Factor Selection
                | FTM_SC_PS(ps)  
                | FTM_SC_CLKS(1));//Select Bus Clock
  FTM3->CNTIN = 0;              //The initial value of FTM Counter
  FTM3->CNT   = 0;
}

static inline void TD8320MG_Duty(uint32 duty)
{
  uint32 cv;
  cv = (duty * (Car_Parameter.SERVO.ftm_mod - 0 + 1)) / PWM_PRECISON;
  FTM3->CONTROLS[0].CnV = cv;
}

void SERVO_Angle(int8 angle)
{
  uint32 duty;
  if(   Car_Parameter.SERVO.min_angle > Car_Parameter.SERVO.max_angle
     || Car_Parameter.SERVO.min_duty  > Car_Parameter.SERVO.max_duty
     || angle < Car_Parameter.SERVO.min_angle
     || angle > Car_Parameter.SERVO.max_angle) 
    return;
  duty = (uint32)map_linear(angle,Car_Parameter.SERVO.min_angle
                                 ,Car_Parameter.SERVO.max_angle
                                 ,Car_Parameter.SERVO.min_duty
                                 ,Car_Parameter.SERVO.max_duty);
  Car_Parameter.SERVO.duty = duty;
  TD8320MG_Duty(duty);
}

void SERVO_Duty(uint32 duty)
{
  if(   Car_Parameter.SERVO.min_angle > Car_Parameter.SERVO.max_angle
     || Car_Parameter.SERVO.min_duty  > Car_Parameter.SERVO.max_duty  
     || duty < Car_Parameter.SERVO.min_duty
     || duty > Car_Parameter.SERVO.max_duty)
    return;
  Car_Parameter.SERVO.angle = (int8)map_linear(duty,Car_Parameter.SERVO.min_duty
                                                   ,Car_Parameter.SERVO.max_duty
                                                   ,Car_Parameter.SERVO.min_angle
                                                   ,Car_Parameter.SERVO.max_angle);
  TD8320MG_Duty(duty);
}

