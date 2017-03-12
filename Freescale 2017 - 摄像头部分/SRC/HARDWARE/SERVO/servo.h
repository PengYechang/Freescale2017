#ifndef __SERVO_H__
#define __SERVO_H__

#include "ftm.h"

void Servo_Init(void);			   	//�����ʼ��
int16_t calculatePidAngle(int16_t purpostAngle);
void servoPidInit();
void updateServoPid();
void calculateTurnAngle(int16_t *centerLine);

extern uint8_t Shift;
extern uint32_t Servo_Error;
extern uint32_t Servo_Middle;		  					//����м�
extern uint32_t Servo_Left; 					//����ҹյ���
extern uint32_t Servo_Right;  

#define Left   2
#define Middle 3
#define Right  4

#endif

