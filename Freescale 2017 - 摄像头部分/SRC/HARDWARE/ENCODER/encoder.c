#include "encoder.h"


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
	FTM_QD_QuickInit(FTM1_QD_PHA_PA12_PHB_PA13,kFTM_QD_NormalPolarity,kQD_CountDirectionEncoding);
	LPTMR_PC_QuickInit(LPTMR_ALT2_PC05);  			//使用PTC5来测量脉冲数
	GPIO_QuickInit(HW_GPIOA,17,kGPIO_Mode_IFT);     //使用PTA17来读方向
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

/*
 时间：2015/12/4
 版本：V1.0
 功能: 双电机读取右侧编码器的值
*/

void EncoderRight_GetValue(void)
{
	speedRight = LPTMR_PC_ReadCounter();         //读值  注意带Z与不带Z的芯片的区别(在函数里面有说明)
	LPTMR_ClearCounter();                        //读完之后必须清一下
}

/*
 时间：2015/12/4
 版本：V1.0
 功能: 双电机读取右侧编码器的方向
*/
void EncoderRight_GetDir(void)
{
	dirRight = GPIO_ReadBit(HW_GPIOA,17);       //读方向
}

#endif
