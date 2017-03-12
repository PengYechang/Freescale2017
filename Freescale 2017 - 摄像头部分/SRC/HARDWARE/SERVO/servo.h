#ifndef __SERVO_H__
#define __SERVO_H__

#include "ftm.h"

//#define Servo_Middle 4540//4535        
//#define Servo_Error  1400
//#define Servo_Left   (Servo_Middle + Servo_Error)          //����ҹյ���
//#define Servo_Right  (Servo_Middle - Servo_Error)          //�����յ���

void Servo_Init(void);			   	//�����ʼ��
int16_t calculatePidAngle(int16_t purpostAngle);
void servoPidInit(float _proportion, float _integral, float _derviative);
void updateServoPid(float _proportion, float _integral, float _derviative);
int16_t calculateTurnAngle(int16_t *centerLine);

extern uint8_t Shift;
extern uint32_t Servo_Error;
extern uint32_t Servo_Middle;		  					//����м�
extern uint32_t Servo_Left; 					//����ҹյ���
extern uint32_t Servo_Right;  

#define Left   2
#define Middle 3
#define Right  4

#endif

