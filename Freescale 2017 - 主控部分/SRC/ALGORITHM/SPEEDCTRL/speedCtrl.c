#include "speedCtrl.h"

#define SpeedToPulse (7000L)//(3500L)//(7000L)//(6678L)

SpeedPIDType SpeedPID;			   //�ٶ�PID
int32_t SysRelativeSpeed = 0;      //���۵�����ֵ
uint32_t ramWaySpeedLimitDelay = 0;//�����µ�����ʱ���ڲ�����
bool StopCar = false;			   //ͣ������
int16_t Motor_Speed = 0;		   //ʵ���ٶȣ�����ֵ��
bool Motor_Run_Enable = false;	   //����Ƿ���������

long MinSpeed = 230;			   //��С�ٶ�
long MaxSpeed = 280;			   //����ٶ�
long RampSpeed = 250;			   //�µ��ٶ�
long BlockSpeed = 250;			   //�ϰ��ٶ�
long StraightToCurveSpeed = 240;   //�������

int32_t roadTypeSpeedCtrl(void);
int32_t buttonSpeedCtrl(void);
int32_t fuzzySpeedCtrl(void);

//����ٶ�PID��ʼ��
void SpeedPID_Init(void)
{	
	SpeedPID.Kp=360;//300;//96;		 //ʹ��PI
	SpeedPID.Ki=6.5;//2.6;//4.8;				 
	SpeedPID.Kd=0; 
	SpeedPID.Error1 = 0;		 //�ϴ�ƫ��
	SpeedPID.Error2 = 0;		 //���ϴ�ƫ��
	SpeedPID.PWM = 0;//500;
	SpeedPID.LowerLimit = -9999; //����ٶ�����
	SpeedPID.UperLimit = 9999;	 //����ٶ�����
	SpeedPID.SetVal = 0;         //��ʼ�ٶ���Ϊ0
}

//�ٶ�PID����
void SpeedPID_Cal(void)	 //�ٶ�PID
{
	static uint16_t motorStartCount=0;
	static int32_t SetVal,PreSetVal=0;

	if(StopCar==false)
	{
		if(SetVal<SysRelativeSpeed && PreSetVal == 0)      //SysRelativeSpeed�Ǹ����㷨(������Ϊ������һ������)
		{												   												//�㷨�ı����SysRelativeSpeed
			motorStartCount++;
			if(motorStartCount>2)
			{
				SetVal+=(int16_t)(0.1*SysRelativeSpeed);	   //�˴�����С��������,�˴�ϵ������̫��,��������ʱ��ѹ�ᱻ�����ܵ�
				motorStartCount = 0; 
			}
		}
		else
		{
		  SetVal = SysRelativeSpeed;
		  PreSetVal = SetVal;
		}
	} 
	else							//����ȡͣ������֮��SetVal������Ϊ0
	{

		SetVal-= 1;
		if(SetVal<0)
		{
			SetVal = 0;
		}
	}	
	//ע�⣺������ֵ����0֮�������¼�����65535�����Գ�������Ҫ���д���
	Encoder_GetData();	   //��ȡ��������ֵ�ͷ���
	
	if(speed > 0x7fff)
	{
		speed -= 65536;
	}
	
	Motor_Speed = (Motor_Speed*3 - speed)/4;		//�˲�
	
	if(MyAbs(Motor_Speed)<180)WDOG_Refresh();        //��ֹ���ַ�ת
	
	push(0,(SetVal*200*100/SpeedToPulse));				//�ٶ�����ֵ
	push(1,(Motor_Speed*200*100/SpeedToPulse)); 		//������ֵ
	SpeedPID.Input = Motor_Speed;
	SpeedPID.Error = SpeedPID.Input - SetVal;
	SpeedPID.Output = SpeedPID.Kp * (SpeedPID.Error - SpeedPID.Error1) + SpeedPID.Ki * SpeedPID.Error + SpeedPID.Kd * (SpeedPID.Error - 2*SpeedPID.Error1 + SpeedPID.Error2);
	SpeedPID.Error2 = SpeedPID.Error1;
	SpeedPID.Error1 = SpeedPID.Error;
	SpeedPID.PWM += SpeedPID.Output;
	
	if(SpeedPID.PWM < SpeedPID.LowerLimit)  		//����һ��
	{
		SpeedPID.PWM = SpeedPID.LowerLimit;
	}
	if(SpeedPID.PWM > SpeedPID.UperLimit)
	{
		SpeedPID.PWM = SpeedPID.UperLimit;
	}
	
	SingleMotor_Control((int16_t)-SpeedPID.PWM);		//���PWM
}

//�ٶȿ���
void SpeedControl(void)
{
	int32_t  SpeedOutPut = 0;		   //���߳����������ٶȣ�ʵ��ֵ��  ת��������ֵSetVal
	
	if(StopCar==false)						//����ͣ������ʱ�Ž��п���
	{
		if(Motor_Run_Enable == true)		//���������
		{
			if(Sys_Speed_Mode == KEY_SPEED)      	   //���ݼ��̸��ٶ�
			{
			  SpeedOutPut = buttonSpeedCtrl();
			}
			else if(Sys_Speed_Mode == ROADTYPE_SPEED)//�����������͸�����
			{
			  SpeedOutPut = roadTypeSpeedCtrl();
			}
			else if(Sys_Speed_Mode == AFUZZY_SPEED || Sys_Speed_Mode == BFUZZY_SPEED 
			   || Sys_Speed_Mode == CFUZZY_SPEED || Sys_Speed_Mode == DFUZZY_SPEED)	//ģ������
			{
			  SpeedOutPut = fuzzySpeedCtrl();
			}
			else									   //���ݼ��̵��������С�ٶ�
			{
			  SpeedOutPut = buttonSpeedCtrl();
			}
			
			//���������ٶȴ���
			if(ramWay.IsUpRamp == true || ramWay.IsOnRamp == true || ramWay.IsDownRamp == true || !checkdelay(ramWaySpeedLimitDelay))
			{
				SpeedOutPut = RampSpeed;			  //�µ��ٶ�
			}
			else if(roadLeftBlock.IsRoadBlock == true || roadRightBlock.IsRoadBlock == true)
			{
				SpeedOutPut = BlockSpeed;
			}
			else if(roadType == RT_StraightToCurve &&(Sys_Speed_Mode == LOW_SPEED || Sys_Speed_Mode == MIDDLE_SPEED || Sys_Speed_Mode == HIGH_SPEED))
			{
				SpeedOutPut = StraightToCurveSpeed;	  //��ģ���ٶ�ʱ��Ҫ�������
			}

			SpeedOutPut = (SpeedOutPut*3 + SysRelativeSpeed*20000/SpeedToPulse)/4;	 //�˲�����ֹ̫��ͻ��
		}
		else
		{
			SpeedOutPut = 0;
		}
	}	  
	else											  //ͣ������
	{
		SpeedOutPut  = 0;
	}					
	//SpeedOutPut�ĵ�λ��cm/s     //���ٶ�ת��Ϊ������
	SysRelativeSpeed =  (SpeedOutPut * SpeedToPulse/200/100);  //200����Ϊ�ٶȿ���������5ms,//��1sת����5ms,100��Ϊ�˰��ٶȴ�cmת����m 
}

double SpeedCtlErr = 0.05;
//ʹ�ð������ٶ�
int32_t buttonSpeedCtrl(void)
{
	long deltaSpeed = 0;
	int32_t speedTemp;
	
    deltaSpeed = MaxSpeed - MinSpeed;			//�����С�ٶ�ƫ��
    speedTemp = (int32_t)(MaxSpeed - DirPID.Error * DirPID.Error* deltaSpeed*SpeedCtlErr / 100.0);  //����ƫ����ٶ�
    if(speedTemp>MaxSpeed)						//����һ��
	{
      speedTemp = MaxSpeed;
	}
    else if(speedTemp<MinSpeed)
	{
        speedTemp = MinSpeed;
	}
	return speedTemp;
}

#define MinCurveSpeed     240
#define CurveMinSpeedLine 95
#define CurveMaxSpeedLine 120
					  //ֱ��   СS  ֱ���� ����ֱ  ��   ��S   ��� �ع���
int16_t rTSpeed[8] = {  260,    250,   250,   245,    240,   240,   240,   240};
int32_t roadTypeSpeedCtrl(void)
{
	int32_t SpeedOutPut = rTSpeed[7];
	int16_t preSee;
	preSee = Max(leftErCheng.endLine,rightErCheng.endLine);
	if(preSee<CurveMinSpeedLine)
	{
		preSee = CurveMinSpeedLine;
	}
	else if(preSee>CurveMaxSpeedLine)
	{
		preSee = CurveMaxSpeedLine;
	}
	switch(roadType)
    {
		case RT_Straight:			//ֱ��
			{
				SpeedOutPut = rTSpeed[0];
				break;
			}
	   case RT_SmallS:             //СS
		   {
			   SpeedOutPut = rTSpeed[1];
			   break;
		   }
       case RT_StraightToCurve:    //ֱ����
		   {
			   SpeedOutPut = rTSpeed[2];
			   break;
		   }
	  case RT_CurveToStraight:     //����ֱ
		  {
			  SpeedOutPut = rTSpeed[3];
			  break;
		  }
	  case RT_BigCircle:           //��
		  {
			  SpeedOutPut = rTSpeed[4];
			  break;
		  }
	  case RT_MiddleS:            //��S
		  {
			  SpeedOutPut = rTSpeed[5];
			  break;
		  }
	  case RT_Curve:             //���
		  {
			  if(MinCurveSpeed>rTSpeed[6])
				  SpeedOutPut = MinCurveSpeed + (MinCurveSpeed-rTSpeed[6])*(preSee - CurveMinSpeedLine)/(CurveMaxSpeedLine - CurveMinSpeedLine);
			  else
				  SpeedOutPut = rTSpeed[6];
			  break;
		  }
	  case RT_ReturnCurve:      //�ػ�
		  {
			  SpeedOutPut = rTSpeed[7];
			  break;
		  }
	  default:
		  {
			  SpeedOutPut = rTSpeed[7];
			  break;
		  }
	}
	return SpeedOutPut;
}

//ģ������
int32_t fuzzySpeedCtrl(void)
{
	int32_t sysFuzzySpeed = 0;
	sysFuzzySpeed = Fuzzy((int16_t)MAXf(leftErCheng.zigma2, rightErCheng.zigma2), (int16_t)MAX(leftErCheng.endLine, rightErCheng.endLine));
	return sysFuzzySpeed;
}

//����Ԥ��õĲ���
void LoadSpeedValue(void)
{
	if(Sys_Speed_Mode == LOW_SPEED)
	{
		MinSpeed = 210;
		MaxSpeed = 240;
		RampSpeed = 230;			  
		BlockSpeed = 230;			  
		StraightToCurveSpeed = 220;
	}
	else if(Sys_Speed_Mode == MIDDLE_SPEED)
	{
		MinSpeed = 235;
		MaxSpeed = 280;
		RampSpeed = 240;			  
		BlockSpeed = 260;			  
		StraightToCurveSpeed = 240;
	}
	else if(Sys_Speed_Mode == HIGH_SPEED)
	{
		MinSpeed = 250;
		MaxSpeed = 320;
		RampSpeed = 250;			  
		BlockSpeed = 260;			  
		StraightToCurveSpeed = 250;
	}
	else if(Sys_Speed_Mode == AFUZZY_SPEED)
	{
		UFF = UFFA;
		RampSpeed = 250;			  
		BlockSpeed = 260;			  
		StraightToCurveSpeed = 260;
	}
	else if(Sys_Speed_Mode == BFUZZY_SPEED)
	{
		UFF = UFFB;
		RampSpeed = 250;			  
		BlockSpeed = 260;			  
		StraightToCurveSpeed = 265;
	}
	else if(Sys_Speed_Mode == CFUZZY_SPEED)
	{
		UFF = UFFC;
		RampSpeed = 250;			  
		BlockSpeed = 260;			  
		StraightToCurveSpeed = 265;
	}
	else if(Sys_Speed_Mode == DFUZZY_SPEED)
	{
		UFF = UFFD;
		RampSpeed = 250;			  
		BlockSpeed = 260;			  
		StraightToCurveSpeed = 265;

	}
}
