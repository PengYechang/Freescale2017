#include "speedCtrl.h"

#define SpeedToPulse (7000L)//(3500L)//(7000L)//(6678L)

SpeedPIDType SpeedPID;			   //速度PID
int32_t SysRelativeSpeed = 0;      //理论的脉冲值
uint32_t ramWaySpeedLimitDelay = 0;//上下坡道若干时间内不加速
bool StopCar = false;			   //停车命令
int16_t Motor_Speed = 0;		   //实际速度（脉冲值）
bool Motor_Run_Enable = false;	   //电机是否被允许运行

long MinSpeed = 230;			   //最小速度
long MaxSpeed = 280;			   //最大速度
long RampSpeed = 250;			   //坡道速度
long BlockSpeed = 250;			   //障碍速度
long StraightToCurveSpeed = 240;   //入弯减速

int32_t roadTypeSpeedCtrl(void);
int32_t buttonSpeedCtrl(void);
int32_t fuzzySpeedCtrl(void);

//电机速度PID初始化
void SpeedPID_Init(void)
{	
	SpeedPID.Kp=360;//300;//96;		 //使用PI
	SpeedPID.Ki=6.5;//2.6;//4.8;				 
	SpeedPID.Kd=0; 
	SpeedPID.Error1 = 0;		 //上次偏差
	SpeedPID.Error2 = 0;		 //上上次偏差
	SpeedPID.PWM = 0;//500;
	SpeedPID.LowerLimit = -9999; //最低速度限制
	SpeedPID.UperLimit = 9999;	 //最高速度限制
	SpeedPID.SetVal = 0;         //初始速度设为0
}

//速度PID计算
void SpeedPID_Cal(void)	 //速度PID
{
	static uint16_t motorStartCount=0;
	static int32_t SetVal,PreSetVal=0;

	if(StopCar==false)
	{
		if(SetVal<SysRelativeSpeed && PreSetVal == 0)      //SysRelativeSpeed是根据算法(或者人为给定的一个参数)
		{												   												//算法改变的是SysRelativeSpeed
			motorStartCount++;
			if(motorStartCount>2)
			{
				SetVal+=(int16_t)(0.1*SysRelativeSpeed);	   //此处用于小车的启动,此处系数不可太大,否则启动时电压会被拉到很低
				motorStartCount = 0; 
			}
		}
		else
		{
		  SetVal = SysRelativeSpeed;
		  PreSetVal = SetVal;
		}
	} 
	else							//当获取停车命令之后，SetVal慢慢减为0
	{

		SetVal-= 1;
		if(SetVal<0)
		{
			SetVal = 0;
		}
	}	
	//注意：当计数值减到0之后再往下减会变成65535，所以程序里面要进行处理
	Encoder_GetData();	   //获取编码器的值和方向
	
	if(speed > 0x7fff)
	{
		speed -= 65536;
	}
	
	Motor_Speed = (Motor_Speed*3 - speed)/4;		//滤波
	
	if(MyAbs(Motor_Speed)<180)WDOG_Refresh();        //防止后轮飞转
	
	push(0,(SetVal*200*100/SpeedToPulse));				//速度期望值
	push(1,(Motor_Speed*200*100/SpeedToPulse)); 		//电机输出值
	SpeedPID.Input = Motor_Speed;
	SpeedPID.Error = SpeedPID.Input - SetVal;
	SpeedPID.Output = SpeedPID.Kp * (SpeedPID.Error - SpeedPID.Error1) + SpeedPID.Ki * SpeedPID.Error + SpeedPID.Kd * (SpeedPID.Error - 2*SpeedPID.Error1 + SpeedPID.Error2);
	SpeedPID.Error2 = SpeedPID.Error1;
	SpeedPID.Error1 = SpeedPID.Error;
	SpeedPID.PWM += SpeedPID.Output;
	
	if(SpeedPID.PWM < SpeedPID.LowerLimit)  		//限制一下
	{
		SpeedPID.PWM = SpeedPID.LowerLimit;
	}
	if(SpeedPID.PWM > SpeedPID.UperLimit)
	{
		SpeedPID.PWM = SpeedPID.UperLimit;
	}
	
	SingleMotor_Control((int16_t)-SpeedPID.PWM);		//输出PWM
}

//速度控制
void SpeedControl(void)
{
	int32_t  SpeedOutPut = 0;		   //决策出来的理论速度（实际值）  转换到脉冲值SetVal
	
	if(StopCar==false)						//当无停车命令时才进行控制
	{
		if(Motor_Run_Enable == true)		//电机被允许
		{
			if(Sys_Speed_Mode == KEY_SPEED)      	   //根据键盘给速度
			{
			  SpeedOutPut = buttonSpeedCtrl();
			}
			else if(Sys_Speed_Mode == ROADTYPE_SPEED)//根据赛道类型给参数
			{
			  SpeedOutPut = roadTypeSpeedCtrl();
			}
			else if(Sys_Speed_Mode == AFUZZY_SPEED || Sys_Speed_Mode == BFUZZY_SPEED 
			   || Sys_Speed_Mode == CFUZZY_SPEED || Sys_Speed_Mode == DFUZZY_SPEED)	//模糊控制
			{
			  SpeedOutPut = fuzzySpeedCtrl();
			}
			else									   //根据键盘调节最大最小速度
			{
			  SpeedOutPut = buttonSpeedCtrl();
			}
			
			//特殊赛道速度处理
			if(ramWay.IsUpRamp == true || ramWay.IsOnRamp == true || ramWay.IsDownRamp == true || !checkdelay(ramWaySpeedLimitDelay))
			{
				SpeedOutPut = RampSpeed;			  //坡道速度
			}
			else if(roadLeftBlock.IsRoadBlock == true || roadRightBlock.IsRoadBlock == true)
			{
				SpeedOutPut = BlockSpeed;
			}
			else if(roadType == RT_StraightToCurve &&(Sys_Speed_Mode == LOW_SPEED || Sys_Speed_Mode == MIDDLE_SPEED || Sys_Speed_Mode == HIGH_SPEED))
			{
				SpeedOutPut = StraightToCurveSpeed;	  //非模糊速度时需要入弯减速
			}

			SpeedOutPut = (SpeedOutPut*3 + SysRelativeSpeed*20000/SpeedToPulse)/4;	 //滤波，防止太大突变
		}
		else
		{
			SpeedOutPut = 0;
		}
	}	  
	else											  //停车命令
	{
		SpeedOutPut  = 0;
	}					
	//SpeedOutPut的单位是cm/s     //将速度转换为脉冲数
	SysRelativeSpeed =  (SpeedOutPut * SpeedToPulse/200/100);  //200是因为速度控制周期是5ms,//从1s转换到5ms,100是为了把速度从cm转换到m 
}

double SpeedCtlErr = 0.05;
//使用按键给速度
int32_t buttonSpeedCtrl(void)
{
	long deltaSpeed = 0;
	int32_t speedTemp;
	
    deltaSpeed = MaxSpeed - MinSpeed;			//最大最小速度偏差
    speedTemp = (int32_t)(MaxSpeed - DirPID.Error * DirPID.Error* deltaSpeed*SpeedCtlErr / 100.0);  //根据偏差给速度
    if(speedTemp>MaxSpeed)						//限制一下
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
					  //直道   小S  直入弯 弯入直  大弧   中S   弯道 回拐弯
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
		case RT_Straight:			//直道
			{
				SpeedOutPut = rTSpeed[0];
				break;
			}
	   case RT_SmallS:             //小S
		   {
			   SpeedOutPut = rTSpeed[1];
			   break;
		   }
       case RT_StraightToCurve:    //直入弯
		   {
			   SpeedOutPut = rTSpeed[2];
			   break;
		   }
	  case RT_CurveToStraight:     //弯入直
		  {
			  SpeedOutPut = rTSpeed[3];
			  break;
		  }
	  case RT_BigCircle:           //大弧
		  {
			  SpeedOutPut = rTSpeed[4];
			  break;
		  }
	  case RT_MiddleS:            //中S
		  {
			  SpeedOutPut = rTSpeed[5];
			  break;
		  }
	  case RT_Curve:             //弯道
		  {
			  if(MinCurveSpeed>rTSpeed[6])
				  SpeedOutPut = MinCurveSpeed + (MinCurveSpeed-rTSpeed[6])*(preSee - CurveMinSpeedLine)/(CurveMaxSpeedLine - CurveMinSpeedLine);
			  else
				  SpeedOutPut = rTSpeed[6];
			  break;
		  }
	  case RT_ReturnCurve:      //回环
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

//模糊控制
int32_t fuzzySpeedCtrl(void)
{
	int32_t sysFuzzySpeed = 0;
	sysFuzzySpeed = Fuzzy((int16_t)MAXf(leftErCheng.zigma2, rightErCheng.zigma2), (int16_t)MAX(leftErCheng.endLine, rightErCheng.endLine));
	return sysFuzzySpeed;
}

//导入预存好的参数
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
