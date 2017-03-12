#include "servo.h"

uint8_t Shift = 3;
uint32_t Servo_Error  = 250;       //���ƫ��
uint32_t Servo_Middle = 750;		  					//����м�
uint32_t Servo_Left   = 1000; 	    //����ҹյ���
uint32_t Servo_Right  = 500;		//�����յ���

//�����ʼ��
void Servo_Init(void)
{
	//C�����  50Hz    B�����  300Hz
	FTM_PWM_QuickInit(FTM0_CH3_PC04,kPWM_EdgeAligned,50);    //FTM2  ͨ��0  PB18  ���ض���ģʽ   50Hz
	FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH3,Servo_Middle);	  //���ȶ���ڵ��м�
	
	Servo_Left = Servo_Middle + Servo_Error;				  //����ҹյ���
	Servo_Right = Servo_Middle - Servo_Error;			  	  //�����յ���
}


