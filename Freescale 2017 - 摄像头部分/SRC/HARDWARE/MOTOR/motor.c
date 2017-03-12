#include "motor.h"

//电机初始化
void Motor_Init(void)
{	
#ifdef DoubleMotor         									  //双电机
	//设置的初始占空比为0%
	FTM_PWM_QuickInit(FTM0_CH4_PD04,kPWM_EdgeAligned,1000);	  //FTM0  通道4  PD04  边沿对齐模式   1000Hz
	FTM_PWM_QuickInit(FTM0_CH5_PD05,kPWM_EdgeAligned,1000);   //FTM0  通道5  PD05  边沿对齐模式
	FTM_PWM_QuickInit(FTM0_CH6_PD06,kPWM_EdgeAligned,1000);   //FTM0  通道6  PD06  边沿对齐模式
	FTM_PWM_QuickInit(FTM0_CH7_PD07,kPWM_EdgeAligned,1000);	  //FTM0  通道7  PD07  边沿对齐模式
#endif
	
#ifdef SingleMotor											  //单电机
	//设置的初始占空比为0%
	FTM_PWM_QuickInit(FTM0_CH4_PA07,kPWM_EdgeAligned,20000);  //FTM0  通道4  PA07  边沿对齐模式    20000Hz
	FTM_PWM_QuickInit(FTM0_CH3_PA06,kPWM_EdgeAligned,20000);  //FTM0  通道3  PA06  边沿对齐模式
	FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH4, 5000);
	FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH3, 5000);
#endif
}

#ifdef DoubleMotor                                             //双电机
//左边电机控制
void MotorLeft_Control(int16_t value)
{
	value = -value;
	if(value < 0)
	{
		if(-value > 10000)
		{
			value = -10000;
		}
		FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH5,-value);
		FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH6, 0);
	}
	else
	{
		if(value > 10000)
		{
			value = 10000;
		}
		FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH5, 0);
		FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH6, value);
	}
}

//右边电机控制
void MotorRight_Control(int16_t value)
{
    if(value < 0)
	{
		if(-value > 10000)
		{
			value = -10000;
		}		
		FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH7, -value);
		FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH4, 0);
	}
	else
	{
		if(value > 10000)
		{
			value = 10000;
		}
		FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH7, 0);
		FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH4, value);
	}
}
#endif

#ifdef SingleMotor														 //单电机
//单电机控制
void SingleMotor_Control(int16_t value)
{
	value = -value;
	if(value < 0)
	{
		if(-value > 10000)
		{
			value = -10000;
		}
		FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH4,-value);
		FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH3, 0);
	}
	else
	{
		if(value > 10000)
		{
			value = 10000;
		}
		FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH4, 0);
		FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH3, value);
	}
}
#endif
