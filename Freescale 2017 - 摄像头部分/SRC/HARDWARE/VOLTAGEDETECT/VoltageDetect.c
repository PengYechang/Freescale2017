#include "VoltageDetect.h"

uint16_t Voltage = 8000 , SysVolt = 8000;

//��ѹ����ʼ��
void Voltage_Init(void)
{
	ADC_QuickInit(ADC1_SE17_PA17,kADC_SingleDiff10or11);
}

//������ѹ���
void VoltageConvert(void) 
{
	ADC_QuickReadValue(ADC1_SE17_PA17);
	Voltage = ADC_QuickReadValue(ADC1_SE17_PA17)*11350/1024 - 175;//795;//��ѹ ��λmv
    SysVolt = (SysVolt*30 + Voltage)/31;
	push(6,SysVolt);				    //��ѹ���
//    if(SysVolt<7000) 
//	{
//		BUZ_LowVoltage();				//�͵�ѹ����
//	}

//	if(SysVolt < 6900)
//	{
//		StopCar = true;   //��������ͻȻ��ת�����µ�Ƭ����λ֮������ת
//	}
}
