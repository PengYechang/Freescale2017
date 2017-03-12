#include "motor.h"

//�����ʼ��
void Motor_Init(void)
{	
#ifdef DoubleMotor         									  //˫���
	//���õĳ�ʼռ�ձ�Ϊ0%
	FTM_PWM_QuickInit(FTM0_CH4_PD04,kPWM_EdgeAligned,1000);	  //FTM0  ͨ��4  PD04  ���ض���ģʽ   1000Hz
	FTM_PWM_QuickInit(FTM0_CH5_PD05,kPWM_EdgeAligned,1000);   //FTM0  ͨ��5  PD05  ���ض���ģʽ
	FTM_PWM_QuickInit(FTM0_CH6_PD06,kPWM_EdgeAligned,1000);   //FTM0  ͨ��6  PD06  ���ض���ģʽ
	FTM_PWM_QuickInit(FTM0_CH7_PD07,kPWM_EdgeAligned,1000);	  //FTM0  ͨ��7  PD07  ���ض���ģʽ
#endif
	
#ifdef SingleMotor											  //�����
	//���õĳ�ʼռ�ձ�Ϊ0%
	FTM_PWM_QuickInit(FTM0_CH4_PA07,kPWM_EdgeAligned,20000);  //FTM0  ͨ��4  PA07  ���ض���ģʽ    20000Hz
	FTM_PWM_QuickInit(FTM0_CH3_PA06,kPWM_EdgeAligned,20000);  //FTM0  ͨ��3  PA06  ���ض���ģʽ
	FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH4, 5000);
	FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH3, 5000);
#endif
}

#ifdef DoubleMotor                                             //˫���
//��ߵ������
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

//�ұߵ������
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

#ifdef SingleMotor														 //�����
//���������
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
