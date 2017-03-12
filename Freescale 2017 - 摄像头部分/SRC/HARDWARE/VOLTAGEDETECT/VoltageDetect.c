#include "VoltageDetect.h"

uint16_t Voltage = 8000 , SysVolt = 8000;

//电压检测初始化
void Voltage_Init(void)
{
	ADC_QuickInit(ADC1_SE17_PA17,kADC_SingleDiff10or11);
}

//启动电压检测
void VoltageConvert(void) 
{
	ADC_QuickReadValue(ADC1_SE17_PA17);
	Voltage = ADC_QuickReadValue(ADC1_SE17_PA17)*11350/1024 - 175;//795;//电压 单位mv
    SysVolt = (SysVolt*30 + Voltage)/31;
	push(6,SysVolt);				    //电压检测
//    if(SysVolt<7000) 
//	{
//		BUZ_LowVoltage();				//低电压报警
//	}

//	if(SysVolt < 6900)
//	{
//		StopCar = true;   //避免因电机突然堵转，导致单片机复位之后电机疯转
//	}
}
