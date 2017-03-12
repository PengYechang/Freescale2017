#ifndef __SERVO_H__
#define __SERVO_H__

#include "ftm.h"

void Servo_Init(void);			   	//舵机初始化
int16_t calculatePidAngle(int16_t purpostAngle);
void servoPidInit();
void updateServoPid();
void calculateTurnAngle(int16_t *centerLine);

extern uint8_t Shift;
extern uint32_t Servo_Error;
extern uint32_t Servo_Middle;		  					//舵机中间
extern uint32_t Servo_Left; 					//舵机右拐到底
extern uint32_t Servo_Right;  

#define Left   2
#define Middle 3
#define Right  4

#endif

