#ifndef __SERVO_H__
#define __SERVO_H__

#include "ftm.h"

//#define Servo_Middle 4540//4535        
//#define Servo_Error  1400
//#define Servo_Left   (Servo_Middle + Servo_Error)          //舵机右拐到底
//#define Servo_Right  (Servo_Middle - Servo_Error)          //舵机左拐到底
void Servo_Init(void);			   	//舵机初始化

extern uint8_t Shift;
extern uint32_t Servo_Error;
extern uint32_t Servo_Middle;		  					//舵机中间
extern uint32_t Servo_Left; 					//舵机右拐到底
extern uint32_t Servo_Right;  
#define Left   2
#define Middle 3
#define Right  4

#endif

