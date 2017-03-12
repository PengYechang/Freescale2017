#include "motor.h"
#include "encoder.h"

//�����ʼ��
void Motor_Init(void)
{	
#ifdef DoubleMotor         									  //˫���
	//���õĳ�ʼռ�ձ�Ϊ0%
	FTM_PWM_QuickInit(FTM0_CH0_PC01,kPWM_EdgeAligned,MOTOR_OUT_MAX);	  //FTM0  ͨ��4  PD04  ���ض���ģʽ   1000Hz
	FTM_PWM_QuickInit(FTM0_CH1_PC02,kPWM_EdgeAligned,MOTOR_OUT_MAX);   //FTM0  ͨ��5  PD05  ���ض���ģʽ
	FTM_PWM_QuickInit(FTM0_CH2_PC03,kPWM_EdgeAligned,MOTOR_OUT_MAX);   //FTM0  ͨ��6  PD06  ���ض���ģʽ
	FTM_PWM_QuickInit(FTM0_CH3_PC04,kPWM_EdgeAligned,MOTOR_OUT_MAX);	  //FTM0  ͨ��7  PD07  ���ض���ģʽ
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

//�ұߵ������
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
