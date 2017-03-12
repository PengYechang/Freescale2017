#include "Bomaswitch.h"

uint8_t AUTO_RUN_EN = true;   	              //�Զ�����
uint8_t STARTLINE_DETECT = false;  			  //�����߼�⣬Ĭ�Ͻ�ֹ
uint8_t EMERGRNCYSTOPCAR = true;              //�Ƿ�����ͣ��Ĭ������
uint8_t RAMWAY_DETECT = true;				  //�µ���⣬Ĭ������
uint8_t Sys_Speed_Mode = MIDDLE_SPEED;        //ϵͳ�ٶȣ�Ĭ��Ϊ���̵���


//���뿪�س�ʼ��
void BomaSwitch_Init(void)
{
	//���뿪��A
	GPIO_QuickInit(BomaA_PORT,BomaASwitch1_PIN,kGPIO_Mode_IPU);
	GPIO_QuickInit(BomaA_PORT,BomaASwitch2_PIN,kGPIO_Mode_IPU);
	GPIO_QuickInit(BomaA_PORT,BomaASwitch3_PIN,kGPIO_Mode_IPU);
	GPIO_QuickInit(BomaA_PORT,BomaASwitch4_PIN,kGPIO_Mode_IPU);
	
	//���뿪��B
	GPIO_QuickInit(BomaB_PORT,BomaBSwitch1_PIN,kGPIO_Mode_IPU);
	GPIO_QuickInit(BomaB_PORT,BomaBSwitch2_PIN,kGPIO_Mode_IPU);
	GPIO_QuickInit(BomaB_PORT,BomaBSwitch3_PIN,kGPIO_Mode_IPU);
	GPIO_QuickInit(BomaB_PORT,BomaBSwitch4_PIN,kGPIO_Mode_IPU);
}

//void ModeChoose(void)
//{
//	//ѡ��С��״̬
//	AUTO_RUN_EN = (StatusSwitch & DEBUG_SELECT_BIT)?true:false;    //�Զ����ƣ�Ĭ������
//	STARTLINE_DETECT = (StatusSwitch & START_LINE_BIT)?true:false; //�����߼�⣬Ĭ�Ͻ�ֹ
//	EMERGRNCYSTOPCAR = (StatusSwitch & EMERGRNCYSTOPCAR_BIT)?true:false;    //�Ƿ��������ͣ����Ĭ������
//	RAMWAY_DETECT = (StatusSwitch & RAMWAY_BIT)?true:false;	//�Ƿ��������µ���Ĭ������
//	
//	//ѡ��С���ٶ�
//	switch(SpeedSwitch | (SpeedSwitchB))
//	{
//		case SPEED0:
//		{
//			Sys_Speed_Mode = LOW_SPEED;    	   //����
//			break;
//		}
//		case SPEED1:
//		{
//			Sys_Speed_Mode = MIDDLE_SPEED;     //����
//			break;
//		}
//		case SPEED2:
//		{
//			Sys_Speed_Mode = HIGH_SPEED;     //ģ������
//			break;
//		}
//		case SPEED3:
//		{
//			Sys_Speed_Mode = AFUZZY_SPEED;     //���A���ٶ��ܲ���,����
//			break;
//		}
//		case SPEED4:
//		{
//			Sys_Speed_Mode = BFUZZY_SPEED;    //���B��Ҳ�ܲ���,�ٽ���
//			break;
//		}
//		case SPEED5:
//		{
//			Sys_Speed_Mode = CFUZZY_SPEED;    //���C��Ҳ�ܲ��꣬�ٽ���
//			break;
//		}
//		case SPEED6:
//		{
//			Sys_Speed_Mode = DFUZZY_SPEED;     //���C��Ҳ�ܲ��꣬�ٽ���
//			break;
//		}
//		case SPEED7:
//		{
//			Sys_Speed_Mode = ROADTYPE_SPEED;    //�������͸��ٶ�
//			break;
//		}
//		case SPEED8:
//		{
//			Sys_Speed_Mode = KEY_SPEED;     	//�������ٶ�
//			break;
//		}
//	}
//	//�Զ�����,������Ҫ������������
//	if(AUTO_RUN_EN == true)
//	{
//		StartRun = true;
//	}
//	else
//	{
//		StartRun = false;
//	}
//	LoadSpeedValue();					//����Ԥ��õ��ٶȲ���
//	LoadDirValue();						//����Ԥ��õĶ������
//	setSDParaToSys();					//��SD�������ݶ���ϵͳ��
//	LoadVarValue();						//OLED����װ��
//}
