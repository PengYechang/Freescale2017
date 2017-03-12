#include "common.h"
#include "isr.h"
#include "FreeCars.h"
#include "encoder.h"
#include "motor.h"
#include "rtc.h"
#include "wdog.h"
#include "usart.h"

/* 修改主频 请使用 CMSIS标准文件 system_MKxxxx.c 中的 CLOCK_SETUP 宏 */

void SysInitialization(void);			//系统初始化

unsigned char ImageData[128];

int main(void)
{
	uint8_t ImgDealTimes = 0;  			//1s内的图像处理次数
	uint32_t StartTime = 0;             //主程序开始时的时间
	uint32_t EndTime = 0;				//主程序结束时的时间

	DelayInit();
	SysInitialization();   				//系统初始化
	EnableInterrupts();       			//开启全部中断

	while(1)
	{
//		DelayMs(5);
//		Right_SpeedControl();
	}
}

//系统初始化
void SysInitialization(void)
{
	DisableInterrupts();      			//禁止全部中断
	DelayInit();              			//延时函数初始化，使用的是DWT
	Encoder_Init();           			//编码器初始化(包含单电机和双电机的,在Preprocessor Symbols中设置)
	Motor_Init();             			//电机初始化(包含单电机和双电机的,在Preprocessor Symbols中设置)
	FreeCars_Init();          			//FreeCars上位机UART引脚配置
	PIT_QuickInit(HW_PIT_CH0,5000);		//5ms中断，用于电机的加速
	ISR_Config();			  			//所有中断的配置，该函数应放在所有初始化函数之后
}
