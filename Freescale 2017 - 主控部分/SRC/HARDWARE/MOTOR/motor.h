#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "ftm.h"

void Motor_Init(void);						 //�����ʼ��

#ifdef DoubleMotor 							 //˫���
void MotorLeft_Control(int16_t value);	     //��ߵ��
void MotorRight_Control(int16_t value);      //�ұߵ��
#endif

#ifdef SingleMotor							 //�����
void SingleMotor_Control(int16_t value);     
#endif

#endif
