#include "encoder.h"
#include "motor.h"

//电机1KHz，1s计数应为1000转
//CNT要时刻清零，保证以此时位置为0
#ifdef SingleMotor
int32_t speed = 0;
uint8_t dir = 0;
#endif

#ifdef DoubleMotor
int32_t speedLeft = 0;
uint8_t dirLeft = 0;
int32_t speedRight = 0;
uint8_t dirRight = 0;
#endif


//PID控制
SpeedClass LeftSpeedCtr;   
SpeedClass RightSpeedCtr;

/*
 时间：2015/12/2
 版本: V1.0
 功能: 单/双电机的编码初始化
*/
void Encoder_Init(void)
{
#ifdef SingleMotor   		      //单电机
	//使用方向-脉冲型编码器  接口A12 A13
	FTM_QD_QuickInit(FTM1_QD_PHA_PA12_PHB_PA13,kFTM_QD_NormalPolarity,kQD_CountDirectionEncoding);
#endif
	
#ifdef DoubleMotor          	  //双电机
	//两个都使用方向脉冲型编码器，一个使用FTM，一个使用LPTMR，因为双车的电机和舵机一起需要两个FTM
	FTM_QD_QuickInit(FTM1_QD_PHA_PA08_PHB_PA09,kFTM_QD_NormalPolarity,kQD_CountDirectionEncoding);
	FTM_QD_QuickInit(FTM2_QD_PHA_PA10_PHB_PA11,kFTM_QD_NormalPolarity,kQD_CountDirectionEncoding);
#endif
}

#ifdef SingleMotor   		      //单电机

/*
 时间：2015/12/4
 版本：V1.0
 功能: 单电机读取编码器的值与方向
 注意：当计数值减到0之后再往下减会变成65535，所以程序里面要进行处理
*/
void Encoder_GetData(void)
{
	FTM_QD_GetData(HW_FTM1,&speed,&dir);//读值和方向
	FTM_QD_ClearCount(HW_FTM1); 		//读完之后就要清掉
}
#endif

#ifdef DoubleMotor     	  //双电机

/*
 时间：2015/12/4
 版本：V1.0
 功能: 双电机读取左侧编码器的值与方向
 注意：当计数值减到0之后再往下减会变成65535，所以程序里面要进行处理
*/
void EncoderLeft_GetData(void)
{
	FTM_QD_GetData(HW_FTM1,&speedLeft,&dirLeft);//读值和方向						//这样可能会导致读完之后value和dir一直变动
	FTM_QD_ClearCount(HW_FTM1); 				//读完之后就要清掉
}

void EncoderRight_GetData(void)
{
	FTM_QD_GetData(HW_FTM2,&speedRight,&dirRight);//读值和方向						//这样可能会导致读完之后value和dir一直变动
	FTM_QD_ClearCount(HW_FTM2); 				//读完之后就要清掉
}

#endif

/******************************************************************************************
@f_name: void PID_Init(void) 
@brief:	 PID参数初始化
@param:	 None
@return: None
*******************************************************************************************/

void PID_Init(void) 
{
//	//位置式速度PID参数初始化
	LeftSpeedCtr.Kp = 1000;//3750;
	LeftSpeedCtr.Ki = 400;
	LeftSpeedCtr.Kd = 0;
	LeftSpeedCtr.SpeedSet = 0;                     //每秒钟转SpeedSet/100圈
	
	RightSpeedCtr.Kp = 8;
	RightSpeedCtr.Ki = 2;
	RightSpeedCtr.Kd = 0;
	RightSpeedCtr.SpeedSet = 200;                    //每秒钟转SpeedSet/100圈
	
}

/**************************************************************************************
@f_name:  void Left_SpeedControl(void)
@brief:	  左轮速度位置式PID
@param:	 None
@return: None
***************************************************************************************/
float L_fPreDelta;
float L_fDelta = 0;
float L_fP = 0,L_fI = 0,L_fD = 0;
void Left_SpeedControl(void)
{ 
  L_fP = 0,L_fI = 0,L_fD = 0;
	EncoderLeft_GetData();
	LeftSpeedCtr.Cnt=speedLeft;
	LeftSpeedCtr.PreSpeed = LeftSpeedCtr.Speed;
	LeftSpeedCtr.Speed = (float)LeftSpeedCtr.Cnt * MOTOR_SPEED_CONSTANT_LEFT; //每秒转的圈数	
	L_fPreDelta = L_fDelta;            
	L_fDelta  = LeftSpeedCtr.SpeedSet - LeftSpeedCtr.Speed;
	//速度误差计算
	L_fP =  L_fDelta * LeftSpeedCtr.Kp * 10;
	L_fD =  (L_fDelta - L_fPreDelta) * LeftSpeedCtr.Kd * 10;
	L_fI =  L_fDelta * LeftSpeedCtr.Ki * 10;
	LeftSpeedCtr.SpeedCtrIntegral += L_fI;
		
	if( LeftSpeedCtr.SpeedCtrIntegral > SPEED_INTEGRAL_MAX)
    {
        LeftSpeedCtr.SpeedCtrIntegral = SPEED_INTEGRAL_MAX;
    }
    else if( LeftSpeedCtr.SpeedCtrIntegral < -SPEED_INTEGRAL_MAX)
    {
        LeftSpeedCtr.SpeedCtrIntegral = -SPEED_INTEGRAL_MAX;
    }

	LeftSpeedCtr.SpeedCtrOut = 	L_fP + L_fD + LeftSpeedCtr.SpeedCtrIntegral;
	
	MotorLeft_Control(LeftSpeedCtr.SpeedCtrOut);
}

/**************************************************************************************
@f_name:  void Right_SpeedControl(void)
@brief:	  右轮速度位置式PID
@param:	 None
@return: None
***************************************************************************************/

float R_fPreDelta;
float R_fDelta = 0;
float R_fP = 0,R_fI = 0,R_fD = 0;
void Right_SpeedControl(void)
{ 
    R_fP = 0,R_fI = 0,R_fD = 0; 
	EncoderRight_GetData();
	RightSpeedCtr.PreSpeed = RightSpeedCtr.Speed;
	RightSpeedCtr.Speed = (float)RightSpeedCtr.Cnt  * MOTOR_SPEED_CONSTANT_LEFT; //每秒转的圈数		
	R_fPreDelta = R_fDelta;            
    R_fDelta  =  RightSpeedCtr.SpeedSet - RightSpeedCtr.Speed;
	//速度误差计算
    R_fP =  R_fDelta * RightSpeedCtr.Kp * 100;
	R_fD =  (R_fDelta - R_fPreDelta) * RightSpeedCtr.Kd * 100;
    R_fI =  R_fDelta * RightSpeedCtr.Ki * 100;
	RightSpeedCtr.SpeedCtrIntegral += R_fI;
		
	if( RightSpeedCtr.SpeedCtrIntegral > SPEED_INTEGRAL_MAX)
    {
        RightSpeedCtr.SpeedCtrIntegral = SPEED_INTEGRAL_MAX;
    }
    else if( RightSpeedCtr.SpeedCtrIntegral < -SPEED_INTEGRAL_MAX)
    {
        RightSpeedCtr.SpeedCtrIntegral = -SPEED_INTEGRAL_MAX;
    }

	RightSpeedCtr.SpeedCtrOut = R_fP + R_fD + RightSpeedCtr.SpeedCtrIntegral;
	
	MotorRight_Control(RightSpeedCtr.SpeedCtrOut);
}



