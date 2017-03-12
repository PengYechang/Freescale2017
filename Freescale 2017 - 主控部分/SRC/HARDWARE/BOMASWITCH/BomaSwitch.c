#include "Bomaswitch.h"

uint8_t AUTO_RUN_EN = true;   	              //自动控制
uint8_t STARTLINE_DETECT = false;  			  //起跑线检测，默认禁止
uint8_t EMERGRNCYSTOPCAR = true;              //是否允许急停，默认允许
uint8_t RAMWAY_DETECT = true;				  //坡道检测，默认允许
uint8_t Sys_Speed_Mode = MIDDLE_SPEED;        //系统速度，默认为键盘调速


//拨码开关初始化
void BomaSwitch_Init(void)
{
	//拨码开关A
	GPIO_QuickInit(BomaA_PORT,BomaASwitch1_PIN,kGPIO_Mode_IPU);
	GPIO_QuickInit(BomaA_PORT,BomaASwitch2_PIN,kGPIO_Mode_IPU);
	GPIO_QuickInit(BomaA_PORT,BomaASwitch3_PIN,kGPIO_Mode_IPU);
	GPIO_QuickInit(BomaA_PORT,BomaASwitch4_PIN,kGPIO_Mode_IPU);
	
	//拨码开关B
	GPIO_QuickInit(BomaB_PORT,BomaBSwitch1_PIN,kGPIO_Mode_IPU);
	GPIO_QuickInit(BomaB_PORT,BomaBSwitch2_PIN,kGPIO_Mode_IPU);
	GPIO_QuickInit(BomaB_PORT,BomaBSwitch3_PIN,kGPIO_Mode_IPU);
	GPIO_QuickInit(BomaB_PORT,BomaBSwitch4_PIN,kGPIO_Mode_IPU);
}

//void ModeChoose(void)
//{
//	//选择小车状态
//	AUTO_RUN_EN = (StatusSwitch & DEBUG_SELECT_BIT)?true:false;    //自动控制，默认允许
//	STARTLINE_DETECT = (StatusSwitch & START_LINE_BIT)?true:false; //起跑线检测，默认禁止
//	EMERGRNCYSTOPCAR = (StatusSwitch & EMERGRNCYSTOPCAR_BIT)?true:false;    //是否允许紧急停车，默认允许
//	RAMWAY_DETECT = (StatusSwitch & RAMWAY_BIT)?true:false;	//是否允许检测坡道，默认允许
//	
//	//选择小车速度
//	switch(SpeedSwitch | (SpeedSwitchB))
//	{
//		case SPEED0:
//		{
//			Sys_Speed_Mode = LOW_SPEED;    	   //低速
//			break;
//		}
//		case SPEED1:
//		{
//			Sys_Speed_Mode = MIDDLE_SPEED;     //中速
//			break;
//		}
//		case SPEED2:
//		{
//			Sys_Speed_Mode = HIGH_SPEED;     //模糊控制
//			break;
//		}
//		case SPEED3:
//		{
//			Sys_Speed_Mode = AFUZZY_SPEED;     //如果A类速度跑不完,降速
//			break;
//		}
//		case SPEED4:
//		{
//			Sys_Speed_Mode = BFUZZY_SPEED;    //如果B类也跑不完,再降速
//			break;
//		}
//		case SPEED5:
//		{
//			Sys_Speed_Mode = CFUZZY_SPEED;    //如果C类也跑不完，再降速
//			break;
//		}
//		case SPEED6:
//		{
//			Sys_Speed_Mode = DFUZZY_SPEED;     //如果C类也跑不完，再降速
//			break;
//		}
//		case SPEED7:
//		{
//			Sys_Speed_Mode = ROADTYPE_SPEED;    //赛道类型给速度
//			break;
//		}
//		case SPEED8:
//		{
//			Sys_Speed_Mode = KEY_SPEED;     	//按键给速度
//			break;
//		}
//	}
//	//自动运行,即不需要经过按键调试
//	if(AUTO_RUN_EN == true)
//	{
//		StartRun = true;
//	}
//	else
//	{
//		StartRun = false;
//	}
//	LoadSpeedValue();					//导入预存好的速度参数
//	LoadDirValue();						//导入预存好的舵机参数
//	setSDParaToSys();					//将SD卡的数据读到系统里
//	LoadVarValue();						//OLED参数装载
//}
