#ifndef _PWM_H_
#define _PWM_H_

void PWM_Init();
void PWM_LR(uint16 leftF, uint16 rightF, uint16 leftB, uint16 rightB);
void PWM_Pulse(uint16 leftF, uint16 rightF, uint16 leftB, uint16 rightB);
#endif