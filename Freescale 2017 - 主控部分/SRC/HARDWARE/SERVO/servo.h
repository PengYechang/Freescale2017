#ifndef __SERVO_H__
#define __SERVO_H__

#include "ftm.h"

//#define Servo_Middle 4540//4535        
//#define Servo_Error  1400
//#define Servo_Left   (Servo_Middle + Servo_Error)          //����ҹյ���
//#define Servo_Right  (Servo_Middle - Servo_Error)          //�����յ���
void Servo_Init(void);			   	//�����ʼ��

extern uint8_t Shift;
extern uint32_t Servo_Error;
extern uint32_t Servo_Middle;		  					//����м�
extern uint32_t Servo_Left; 					//����ҹյ���
extern uint32_t Servo_Right;  
#define Left   2
#define Middle 3
#define Right  4

#endif

