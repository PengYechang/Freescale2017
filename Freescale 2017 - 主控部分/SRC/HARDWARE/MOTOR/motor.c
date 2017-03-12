#include "motor.h"
#include "encoder.h"

//电机初始化
void Motor_Init(void)
{	
#ifdef DoubleMotor         									  //双电机
	//设置的初始占空比为0%
	FTM_PWM_QuickInit(FTM0_CH0_PC01,kPWM_EdgeAligned,MOTOR_OUT_MAX);	  //FTM0  通道4  PD04  边沿对齐模式   1000Hz
	FTM_PWM_QuickInit(FTM0_CH1_PC02,kPWM_EdgeAligned,MOTOR_OUT_MAX);   //FTM0  通道5  PD05  边沿对齐模式
	FTM_PWM_QuickInit(FTM0_CH2_PC03,kPWM_EdgeAligned,MOTOR_OUT_MAX);   //FTM0  通道6  PD06  边沿对齐模式
	FTM_PWM_QuickInit(FTM0_CH3_PC04,kPWM_EdgeAligned,MOTOR_OUT_MAX);	  //FTM0  通道7  PD07  边沿对齐模式
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
		if(-value > MOTOR_OUT_MAX)
		{
			value = MOTOR_OUT_MIN;
		}
		FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH0,-value);
		FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH1, 0);
	}
	else
	{
		if(value > MOTOR_OUT_MAX)
		{
			value = MOTOR_OUT_MAX;
		}
		FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH0, 0);
		FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH1, value);
	}
}

//右边电机控制
void MotorRight_Control(int16_t value)
{
    if(value < 0)
	{
		if(-value > MOTOR_OUT_MAX)
		{
			value = MOTOR_OUT_MIN;
		}		
		FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH2, -value);
		FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH3, 0);
	}
	else
	{
		if(value > MOTOR_OUT_MAX)
		{
			value = MOTOR_OUT_MAX;
		}
		FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH2, 0);
		FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH3, value);
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
