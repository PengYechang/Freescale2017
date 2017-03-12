#include "pid.h"
#include "freeCars.h"

PIDLongType   MotorPID;	//����ٶ�PID

int32_t Motor_Speed = 0;

//����ٶ�PID��ʼ��
void Motor_SpeedPID_Init(void)
{	
	MotorPID.Kp=65;				 //��ʹ��PI
	MotorPID.Ki=5;				 
	MotorPID.Kd=0; 
	MotorPID.Error1 = 0;		 //�ϴ�ƫ��
	MotorPID.Error2 = 0;		 //���ϴ�ƫ��
	MotorPID.PWM = 500;
	MotorPID.LowerLimit = 100;   //����ٶ�����
	MotorPID.UperLimit = 900;	 //����ٶ�����
	MotorPID.SetVal = 0;         //Speed set as 0 
//	MotorPID.PWM = 0;			 //�ٶȵ��������PWM
//	MotorPID.LowerLimit = -9999; //����
//	MotorPID.UperLimit = 9999;   //����
}

void Motor_SpeedPID_Cal(void)	 //�ٶ�PID
{
    int32_t speed=0;
    uint8_t dir=0;
	
	//ע�⣺������ֵ����0֮�������¼�����65535�����Գ�������Ҫ���д���
	Encoder_GetData(speed,dir);	   //��ȡ��������ֵ�ͷ���
	
	if(speed > 0x7fff)
	{
		speed -= 65536;
	}
    
//	Motor_Speed = (Motor_Speed*3 - speed)/4;		//��������д��Ŀǰ��֪��ԭ��
//	MotorPID.Input = Motor_Speed;
	
	MotorPID.Input = speed;
	MotorPID.Error = MotorPID.Input - MotorPID.SetVal;
	MotorPID.Output = MotorPID.Kp * (MotorPID.Error - MotorPID.Error1) + MotorPID.Ki * MotorPID.Error + MotorPID.Kd * (MotorPID.Error - 2*MotorPID.Error1 + MotorPID.Error2);
	MotorPID.Error2 = MotorPID.Error1;
	MotorPID.Error1 = MotorPID.Error;
	MotorPID.PWM += MotorPID.Output;
	
	if(MotorPID.PWM < MotorPID.LowerLimit)  		//����һ��
	{
		MotorPID.PWM = MotorPID.LowerLimit;
	}
	if(MotorPID.PWM > MotorPID.UperLimit)
	{
		MotorPID.PWM = MotorPID.UperLimit;
	}
}

