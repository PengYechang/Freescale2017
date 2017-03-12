#include "pid.h"
#include "freeCars.h"

PIDLongType   MotorPID;	//电机速度PID

int32_t Motor_Speed = 0;

//电机速度PID初始化
void Motor_SpeedPID_Init(void)
{	
	MotorPID.Kp=65;				 //先使用PI
	MotorPID.Ki=5;				 
	MotorPID.Kd=0; 
	MotorPID.Error1 = 0;		 //上次偏差
	MotorPID.Error2 = 0;		 //上上次偏差
	MotorPID.PWM = 500;
	MotorPID.LowerLimit = 100;   //最低速度限制
	MotorPID.UperLimit = 900;	 //最高速度限制
	MotorPID.SetVal = 0;         //Speed set as 0 
//	MotorPID.PWM = 0;			 //速度的输出量是PWM
//	MotorPID.LowerLimit = -9999; //加速
//	MotorPID.UperLimit = 9999;   //减速
}

void Motor_SpeedPID_Cal(void)	 //速度PID
{
    int32_t speed=0;
    uint8_t dir=0;
	
	//注意：当计数值减到0之后再往下减会变成65535，所以程序里面要进行处理
	Encoder_GetData(speed,dir);	   //获取编码器的值和方向
	
	if(speed > 0x7fff)
	{
		speed -= 65536;
	}
    
//	Motor_Speed = (Motor_Speed*3 - speed)/4;		//军神这样写，目前不知道原因
//	MotorPID.Input = Motor_Speed;
	
	MotorPID.Input = speed;
	MotorPID.Error = MotorPID.Input - MotorPID.SetVal;
	MotorPID.Output = MotorPID.Kp * (MotorPID.Error - MotorPID.Error1) + MotorPID.Ki * MotorPID.Error + MotorPID.Kd * (MotorPID.Error - 2*MotorPID.Error1 + MotorPID.Error2);
	MotorPID.Error2 = MotorPID.Error1;
	MotorPID.Error1 = MotorPID.Error;
	MotorPID.PWM += MotorPID.Output;
	
	if(MotorPID.PWM < MotorPID.LowerLimit)  		//限制一下
	{
		MotorPID.PWM = MotorPID.LowerLimit;
	}
	if(MotorPID.PWM > MotorPID.UperLimit)
	{
		MotorPID.PWM = MotorPID.UperLimit;
	}
}

