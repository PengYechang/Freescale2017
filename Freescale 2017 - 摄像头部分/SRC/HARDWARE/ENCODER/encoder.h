#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "ftm.h"
#include "lptmr.h"
#include "gpio.h"

void Encoder_Init(void);   	      //编码器初始化

//单电机
#ifdef SingleMotor   		      //单电机
extern int32_t speed;
extern uint8_t dir; 
void Encoder_GetData(void);
#endif
	
//双电机	
#ifdef DoubleMotor          	  //双电机
extern int32_t speedLeft;
extern uint8_t dirLeft;
extern int32_t speedRight;
extern uint8_t dirRight;
void EncoderLeft_GetData(void);
void EncoderRight_GetValue(void);
void EncoderRight_GetDir(void);
#endif

#endif
