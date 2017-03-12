#include "common.h"
#include "isr.h"
#include "FreeCars.h"
#include "camera.h"
#include "servo.h"
#include "BomaSwitch.h"
#include "rtc.h"
#include "wdog.h"
#include "usart.h"
#include "image_control.h"

/* 修改主频 请使用 CMSIS标准文件 system_MKxxxx.c 中的 CLOCK_SETUP 宏 */

void SysInitialization(void);			//系统初始化

int16_t *center;	

int main(void)
{
	uint8_t ImgDealTimes = 0;  			//1s内的图像处理次数
	uint32_t StartTime = 0;             //主程序开始时的时间
	uint32_t EndTime = 0;				//主程序结束时的时间

	DelayInit();
	SysInitialization();   				//系统初始化
//	ModeChoose();						//拨码开关选择模式，并且导入预置好的参数，最后导入的参数是SD里的
	EnableInterrupts();       			//开启全部中断

	while(1)
	{
		FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH3,Servo_Middle);
		DelayMs(10);
		if(ImgOK == true)
		{
//			sendCamImgToCamViewer();
			Image_Deal = Image_Buff1;
			center = findCenterLine(Image_Deal);
			calculateTurnAngle(center);
			ImgDealOK = true;
		}
	}
}

//系统初始化
void SysInitialization(void)
{
	DisableInterrupts();      			//禁止全部中断
	DelayInit();              			//延时函数初始化，使用的是DWT
	Servo_Init();                       //舵机初始化
	servoPidInit();                     //PID初始化
	FreeCars_Init();          			//FreeCars上位机UART引脚配置
	Camera_Init();			  			//摄像头引脚配置
	ISR_Config();			  			//所有中断的配置，该函数应放在所有初始化函数之后
}
