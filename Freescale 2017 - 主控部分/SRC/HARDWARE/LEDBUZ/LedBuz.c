#include "LedBuz.h"

BUZEvent BuzEvent;		//蜂鸣器事件

void GPIO_LedBuzInit(void)
{
	GPIO_QuickInit(LED1_PORT,LED1_PIN,LED1_MODE); //LED1快速初始化
	GPIO_QuickInit(LED2_PORT,LED2_PIN,LED2_MODE); //LED2快速初始化
	GPIO_QuickInit(BUZ_PORT,BUZ_PIN,BUZ_MODE);    //BUZ快速初始化	
	
	BuzEvent.BuzMode = MODE_IDIE;
	BuzEvent.BuzState = STATE_IDIE;
	BuzEvent.AfterRingState = Stop;
	
	LED1_OFF;
	LED2_OFF;
	BUZ_OFF;
}

//蜂鸣器事件刷新
//蜂鸣器刷新次序:
//BUZ_ON-->蜂鸣器响一段时间BuzTimeMSH-->BUZ_OFF-->蜂鸣器灭一段时间BuzTimeMSL-->返回BUZ_ON
void BUZRefrech(void)
{
	static uint8_t BuzOnTimeNum = 0;
	if(BuzEvent.BuzMode != (BUZMode)MODE_IDIE)					  //蜂鸣器非空闲态
	{ 
		if(checkdelay(BuzEvent.BuzSetTimeMSL) && (BuzEvent.BuzState != (BuzState)STATE_ON)) //事件一旦发送，蜂鸣器就开始响
		{
			BUZ_ON;
			BuzEvent.BuzSetTimeMSH = setdelay(BuzEvent.BuzTimeMSH);  //蜂鸣器响的时间
			BuzEvent.BuzState = (BuzState)STATE_ON;               //蜂鸣器的状态转为开启状态 
			BuzOnTimeNum++;
		}
		if(checkdelay(BuzEvent.BuzSetTimeMSH) && (BuzEvent.BuzState == (BuzState)STATE_ON))   //蜂鸣器响完了，关闭蜂鸣器
		{
			BUZ_OFF;
			BuzEvent.BuzSetTimeMSL = setdelay(BuzEvent.BuzTimeMSL);  //蜂鸣器灭的时间
			BuzEvent.BuzState = (BuzState)STATE_OFF;                 //蜂鸣器的状态转为关闭状态
		}
		
		if(BuzEvent.AfterRingState == ((AfterRingState)Stop) && (BuzOnTimeNum == BuzEvent.BuzOnTime))  	  //蜂鸣器响完一周期后不继续响,就清事件发生标志
		{
			if(BuzEvent.BuzState == (BuzState)STATE_OFF)		  //已经响完一周期
			{
				BuzEvent.BuzState = (BuzState)STATE_IDIE;    	  //蜂鸣器状态置为空闲态
				BuzEvent.BuzMode = (BUZMode)MODE_IDIE;			  //模式也置为空闲态
				BuzOnTimeNum = 0;
			}
		}
	}	
}

//按下按键报警
void BUZ_Key(void)
{
	if(BuzEvent.BuzState == (BuzState)STATE_IDIE)  			//没有其他蜂鸣器事件发生
	{
		BuzEvent.BuzMode = Key;
		BuzEvent.BuzTimeMSH = 50;
		BuzEvent.BuzTimeMSL = 100;
		BuzEvent.BuzSetTimeMSH = 0;
		BuzEvent.BuzSetTimeMSL = 0;
		BuzEvent.BuzOnTime = 1;
		BuzEvent.AfterRingState = Stop;
	}
}

//车出赛道报警
//蜂鸣器之后的状态需要斟酌，因为如果是Continue状态，其他蜂鸣器事件就会失效，所以Continue状态可以说是紧急状态
void BUZ_CarDanger(void)
{
	if(BuzEvent.BuzState == (BuzState)STATE_IDIE)  			//没有其他蜂鸣器事件发生
	{
		BuzEvent.BuzMode = CarDanger;
		BuzEvent.BuzTimeMSH = 10;
		BuzEvent.BuzTimeMSL = 100;
		BuzEvent.BuzSetTimeMSH = 0;
		BuzEvent.BuzSetTimeMSL = 0;
		BuzEvent.AfterRingState = Continue;			    
	}
}

//接收到数据报警
void BUZ_DataRecOk(void)
{
	if(BuzEvent.BuzState == (BuzState)STATE_IDIE)  			//没有其他蜂鸣器事件发生
	{
		BuzEvent.BuzMode = DataRecOk;
		BuzEvent.BuzTimeMSH = 50;
		BuzEvent.BuzTimeMSL = 100;
		BuzEvent.BuzSetTimeMSH = 0;
		BuzEvent.BuzSetTimeMSL = 0;
		BuzEvent.BuzOnTime = 5;
		BuzEvent.AfterRingState = Stop;
	}
}

//低电压报警
void BUZ_LowVoltage(void)
{
	if(BuzEvent.BuzState == (BuzState)STATE_IDIE)  			//没有其他蜂鸣器事件发生
	{
		BuzEvent.BuzMode = LowVoltage;
		BuzEvent.BuzTimeMSH = 30;
		BuzEvent.BuzTimeMSL = 10;
		BuzEvent.BuzSetTimeMSH = 0;
		BuzEvent.BuzSetTimeMSL = 0;
		BuzEvent.BuzOnTime = 10;
		BuzEvent.AfterRingState = Stop;
	}
}

//常态
void BUZ_Normal(void)
{
	if(BuzEvent.BuzState == (BuzState)STATE_IDIE)  			//没有其他蜂鸣器事件发生
	{
		BuzEvent.BuzMode = Normal;
		BuzEvent.BuzTimeMSH = 400;
		BuzEvent.BuzTimeMSL = 50;
		BuzEvent.BuzSetTimeMSH = 0;
		BuzEvent.BuzSetTimeMSL = 0;
		BuzEvent.BuzOnTime = 1;
		BuzEvent.AfterRingState = Stop;
	}
}
