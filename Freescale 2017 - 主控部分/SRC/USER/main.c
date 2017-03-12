#include "common.h"
#include "isr.h"
#include "FreeCars.h"
#include "encoder.h"
#include "motor.h"
#include "rtc.h"
#include "wdog.h"
#include "usart.h"

/* �޸���Ƶ ��ʹ�� CMSIS��׼�ļ� system_MKxxxx.c �е� CLOCK_SETUP �� */

void SysInitialization(void);			//ϵͳ��ʼ��

unsigned char ImageData[128];

int main(void)
{
	uint8_t ImgDealTimes = 0;  			//1s�ڵ�ͼ�������
	uint32_t StartTime = 0;             //������ʼʱ��ʱ��
	uint32_t EndTime = 0;				//���������ʱ��ʱ��

	DelayInit();
	SysInitialization();   				//ϵͳ��ʼ��
	EnableInterrupts();       			//����ȫ���ж�

	while(1)
	{
//		DelayMs(5);
//		Right_SpeedControl();
	}
}

//ϵͳ��ʼ��
void SysInitialization(void)
{
	DisableInterrupts();      			//��ֹȫ���ж�
	DelayInit();              			//��ʱ������ʼ����ʹ�õ���DWT
	Encoder_Init();           			//��������ʼ��(�����������˫�����,��Preprocessor Symbols������)
	Motor_Init();             			//�����ʼ��(�����������˫�����,��Preprocessor Symbols������)
	FreeCars_Init();          			//FreeCars��λ��UART��������
	PIT_QuickInit(HW_PIT_CH0,5000);		//5ms�жϣ����ڵ���ļ���
	ISR_Config();			  			//�����жϵ����ã��ú���Ӧ�������г�ʼ������֮��
}
