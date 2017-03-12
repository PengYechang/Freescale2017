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

/* �޸���Ƶ ��ʹ�� CMSIS��׼�ļ� system_MKxxxx.c �е� CLOCK_SETUP �� */

void SysInitialization(void);			//ϵͳ��ʼ��

int16_t *center;	

int main(void)
{
	uint8_t ImgDealTimes = 0;  			//1s�ڵ�ͼ�������
	uint32_t StartTime = 0;             //������ʼʱ��ʱ��
	uint32_t EndTime = 0;				//���������ʱ��ʱ��

	DelayInit();
	SysInitialization();   				//ϵͳ��ʼ��
//	ModeChoose();						//���뿪��ѡ��ģʽ�����ҵ���Ԥ�úõĲ����������Ĳ�����SD���
	EnableInterrupts();       			//����ȫ���ж�

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

//ϵͳ��ʼ��
void SysInitialization(void)
{
	DisableInterrupts();      			//��ֹȫ���ж�
	DelayInit();              			//��ʱ������ʼ����ʹ�õ���DWT
	Servo_Init();                       //�����ʼ��
	servoPidInit();                     //PID��ʼ��
	FreeCars_Init();          			//FreeCars��λ��UART��������
	Camera_Init();			  			//����ͷ��������
	ISR_Config();			  			//�����жϵ����ã��ú���Ӧ�������г�ʼ������֮��
}
