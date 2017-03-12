#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "ftm.h"

void Motor_Init(void);						 //电机初始化

#ifdef DoubleMotor 							 //双电机
void MotorLeft_Control(int16_t value);	     //左边电机
void MotorRight_Control(int16_t value);      //右边电机
#endif

#ifdef SingleMotor							 //单电机
void SingleMotor_Control(int16_t value);     
#endif

#endif
