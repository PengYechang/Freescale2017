#include "encoder.h"
#include "motor.h"

//���1KHz��1s����ӦΪ1000ת
//CNTҪʱ�����㣬��֤�Դ�ʱλ��Ϊ0
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


//PID����
SpeedClass LeftSpeedCtr;   
SpeedClass RightSpeedCtr;

/*
 ʱ�䣺2015/12/2
 �汾: V1.0
 ����: ��/˫����ı����ʼ��
*/
void Encoder_Init(void)
{
#ifdef SingleMotor   		      //�����
	//ʹ�÷���-�����ͱ�����  �ӿ�A12 A13
	FTM_QD_QuickInit(FTM1_QD_PHA_PA12_PHB_PA13,kFTM_QD_NormalPolarity,kQD_CountDirectionEncoding);
#endif
	
#ifdef DoubleMotor          	  //˫���
	//������ʹ�÷��������ͱ�������һ��ʹ��FTM��һ��ʹ��LPTMR����Ϊ˫���ĵ���Ͷ��һ����Ҫ����FTM
	FTM_QD_QuickInit(FTM1_QD_PHA_PA08_PHB_PA09,kFTM_QD_NormalPolarity,kQD_CountDirectionEncoding);
	FTM_QD_QuickInit(FTM2_QD_PHA_PA10_PHB_PA11,kFTM_QD_NormalPolarity,kQD_CountDirectionEncoding);
#endif
}

#ifdef SingleMotor   		      //�����

/*
 ʱ�䣺2015/12/4
 �汾��V1.0
 ����: �������ȡ��������ֵ�뷽��
 ע�⣺������ֵ����0֮�������¼�����65535�����Գ�������Ҫ���д���
*/
void Encoder_GetData(void)
{
	FTM_QD_GetData(HW_FTM1,&speed,&dir);//��ֵ�ͷ���
	FTM_QD_ClearCount(HW_FTM1); 		//����֮���Ҫ���
}
#endif

#ifdef DoubleMotor     	  //˫���

/*
 ʱ�䣺2015/12/4
 �汾��V1.0
 ����: ˫�����ȡ����������ֵ�뷽��
 ע�⣺������ֵ����0֮�������¼�����65535�����Գ�������Ҫ���д���
*/
void EncoderLeft_GetData(void)
{
	FTM_QD_GetData(HW_FTM1,&speedLeft,&dirLeft);//��ֵ�ͷ���						//�������ܻᵼ�¶���֮��value��dirһֱ�䶯
	FTM_QD_ClearCount(HW_FTM1); 				//����֮���Ҫ���
}

void EncoderRight_GetData(void)
{
	FTM_QD_GetData(HW_FTM2,&speedRight,&dirRight);//��ֵ�ͷ���						//�������ܻᵼ�¶���֮��value��dirһֱ�䶯
	FTM_QD_ClearCount(HW_FTM2); 				//����֮���Ҫ���
}

#endif

/******************************************************************************************
@f_name: void PID_Init(void) 
@brief:	 PID������ʼ��
@param:	 None
@return: None
*******************************************************************************************/

void PID_Init(void) 
{
//	//λ��ʽ�ٶ�PID������ʼ��
	LeftSpeedCtr.Kp = 1000;//3750;
	LeftSpeedCtr.Ki = 400;
	LeftSpeedCtr.Kd = 0;
	LeftSpeedCtr.SpeedSet = 0;                     //ÿ����תSpeedSet/100Ȧ
	
	RightSpeedCtr.Kp = 8;
	RightSpeedCtr.Ki = 2;
	RightSpeedCtr.Kd = 0;
	RightSpeedCtr.SpeedSet = 200;                    //ÿ����תSpeedSet/100Ȧ
	
}

/**************************************************************************************
@f_name:  void Left_SpeedControl(void)
@brief:	  �����ٶ�λ��ʽPID
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
	LeftSpeedCtr.Speed = (float)LeftSpeedCtr.Cnt * MOTOR_SPEED_CONSTANT_LEFT; //ÿ��ת��Ȧ��	
	L_fPreDelta = L_fDelta;            
	L_fDelta  = LeftSpeedCtr.SpeedSet - LeftSpeedCtr.Speed;
	//�ٶ�������
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
@brief:	  �����ٶ�λ��ʽPID
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
	RightSpeedCtr.Speed = (float)RightSpeedCtr.Cnt  * MOTOR_SPEED_CONSTANT_LEFT; //ÿ��ת��Ȧ��		
	R_fPreDelta = R_fDelta;            
    R_fDelta  =  RightSpeedCtr.SpeedSet - RightSpeedCtr.Speed;
	//�ٶ�������
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



