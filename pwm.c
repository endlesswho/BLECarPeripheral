#include <ioCC2540.h>
#include "bcomdef.h"
#include "OSAL.h"
#include "pwm.h"

//pwm pins:
//P0_3:LF  (TX)--ch1
//P0_4:RF (CTS)--ch2
//P0_5:LB 
//P0_6:RB
static uint16 gleftF =1;
static uint16 grightF =1;
static uint16 gleftB =1;
static uint16 grightB =1;

//�ȳ���·PWM������ǰ��

//��ʼ��
void PWM_Init()
{
  //����pwm�˿�Ϊ���
  P0DIR|= BV(3)|BV(4)|BV(5)|BV(6);
  //����pwm�˿�Ϊ����˿ڣ���gpio
  P0SEL|= BV(3)|BV(4)|BV(5)|BV(6);
  //����uart�Ȼ�ռ�����ǵ�ǰʹ�õ�pwm�˿ڣ������Ҫ��uart����ӳ�䵽��Ķ˿�ȥ��
  PERCFG |= 0x03;             // Move USART1&2 to alternate2 location so that T1 is visible

  // Initialize Timer 1
  T1CTL = 0x0C;               // Div = 128, CLR, MODE = Suspended          
  T1CCTL1 = 0x0C;             // IM = 0; CMP = Clear output on compare; Mode = Compare
  T1CCTL2 = 0x0C;             // IM = 0; CMP = Clear output on compare; Mode = Compare
  T1CCTL3 = 0x0C;             // IM = 0, CMP = Clear output on compare; Mode = Compare
  T1CCTL4 = 0x0C;
  T1CNTL = 0;                 // Reset timer to 0;

    //�������ã�����ʱ��������
  T1CCTL0 = 0x4C;            // IM = 1, CMP = Clear output on compare; Mode = Compare

  //�������ڵ�tickΪ375, Ҳ����1.5ms
#if 1
  T1CC0H = 0x01;              // Ticks = 375 (1.5ms initial duty cycle)
  T1CC0L = 0x77;              //             
  T1CC1H = 0x01;              // Ticks = 375 (1.5ms initial duty cycle)
  T1CC1L = 0x77;
  T1CC2H = 0x01;              // Ticks = 375 (1.5ms initial duty cycle)
  T1CC2L = 0x77;
  T1CC3H = 0x01;              // Ticks = 375 (1.5ms initial duty cycle)
  T1CC3L = 0x77;  
  T1CC4H = 0x01;              // Ticks = 375 (1.5ms initial duty cycle)
  T1CC4L = 0x77;  
#else//�������û���1khz
#define VALUE_H     0x00
#define VALUE_L     0x10
  T1CC0H = VALUE_H;    
  T1CC0L = VALUE_L;    
  T1CC1H = VALUE_H;    
  T1CC1L = VALUE_L;
  T1CC2H = VALUE_H;    
  T1CC2L = VALUE_L;
  T1CC3H = VALUE_H;    
  T1CC3L = VALUE_L;  
#endif 

  EA=1;
  IEN1 |= 0x02;               // Enable T1 cpu interrupt
}

//�ı����PWM��ռ�ձ�
void PWM_Pulse(uint16 leftF, uint16 rightF, uint16 leftB, uint16 rightB)
{
  uint16 lf,rf,lb,rb;
  
  lf=leftF;
  rf=rightF;
  lb=leftB;
  rb=rightB;
  
  // Set up the timer registers

  T1CC1L = (uint8)lf;
  T1CC1H = (uint8)(lf >> 8);
  //����Ƚ�ֵΪ0ʱ�����һֱΪ��
  if(lf!=0){
    T1CCTL1 = 0x0C;
  }else{
    T1CCTL1 = 0x00;
  }
  
  T1CC2L = (uint8)rf;
  T1CC2H = (uint8)( rf>> 8);
  if(rf!=0){
    T1CCTL2 = 0x0C;
  }else{
    T1CCTL2 = 0x00;
  }
  
  T1CC3L = (uint8)lb;
  T1CC3H = (uint8)(lb >> 8);
  //����Ƚ�ֵΪ0ʱ�����һֱΪ��
  if(lb!=0){
    T1CCTL3 = 0x0C;
  }else{
    T1CCTL3 = 0x00;
  }  
  
  T1CC4L = (uint8)rb;
  T1CC4H = (uint8)( rb>> 8);
  if(rb!=0){
    T1CCTL2 = 0x0C;
  }else{
    T1CCTL2 = 0x00;
  }
  

  // Reset timer
  T1CNTL = 0;
  

  // Start timer in modulo mode.
  T1CTL |= 0x02;   
}

//
void PWM_LR(uint16 leftF, uint16 rightF, uint16 leftB, uint16 rightB)
{    
  gleftF=leftF;
  grightF=rightF;
  gleftB=leftB;
  grightB=rightB;
}

//PWM�ж�
//#pragma register_bank=2
#pragma vector = T1_VECTOR
__interrupt void pwmISR (void) 
{
    uint8 flags = T1STAT;
    // T1 ch 0
    if (flags & 0x01){          
      
      // Stop Timer 1
      //T1CTL |= 0x02;
      //left ��right  ��ֵ������ 1~375, ����ֵ��Ч
      PWM_Pulse(gleftF,grightF,gleftB,grightB);
     
    }
    T1STAT = ~ flags;
}
