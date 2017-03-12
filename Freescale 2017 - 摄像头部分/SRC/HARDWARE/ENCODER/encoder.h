#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "ftm.h"
#include "lptmr.h"
#include "gpio.h"

void Encoder_Init(void);   	      //��������ʼ��

//�����
#ifdef SingleMotor   		      //�����
extern int32_t speed;
extern uint8_t dir; 
void Encoder_GetData(void);
#endif
	
//˫���	
#ifdef DoubleMotor          	  //˫���
extern int32_t speedLeft;
extern uint8_t dirLeft;
extern int32_t speedRight;
extern uint8_t dirRight;
void EncoderLeft_GetData(void);
void EncoderRight_GetValue(void);
void EncoderRight_GetDir(void);
#endif

#endif
