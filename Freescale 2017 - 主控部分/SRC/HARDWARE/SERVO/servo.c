#include "servo.h"

uint8_t Shift = 3;
uint32_t Servo_Error  = 250;       //舵机偏差
uint32_t Servo_Middle = 750;		  					//舵机中间
uint32_t Servo_Left   = 1000; 	    //舵机右拐到底
uint32_t Servo_Right  = 500;		//舵机左拐到底

//舵机初始化
void Servo_Init(void)
{
	//C车舵机  50Hz    B车舵机  300Hz
	FTM_PWM_QuickInit(FTM0_CH3_PC04,kPWM_EdgeAligned,50);    //FTM2  通道0  PB18  边沿对齐模式   50Hz
	FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH3,Servo_Middle);	  //首先舵机摆到中间
	
	Servo_Left = Servo_Middle + Servo_Error;				  //舵机右拐到底
	Servo_Right = Servo_Middle - Servo_Error;			  	  //舵机左拐到底
}


