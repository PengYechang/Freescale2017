#include "LedBuz.h"

BUZEvent BuzEvent;		//�������¼�

void GPIO_LedBuzInit(void)
{
	GPIO_QuickInit(LED1_PORT,LED1_PIN,LED1_MODE); //LED1���ٳ�ʼ��
	GPIO_QuickInit(LED2_PORT,LED2_PIN,LED2_MODE); //LED2���ٳ�ʼ��
	GPIO_QuickInit(BUZ_PORT,BUZ_PIN,BUZ_MODE);    //BUZ���ٳ�ʼ��	
	
	BuzEvent.BuzMode = MODE_IDIE;
	BuzEvent.BuzState = STATE_IDIE;
	BuzEvent.AfterRingState = Stop;
	
	LED1_OFF;
	LED2_OFF;
	BUZ_OFF;
}

//�������¼�ˢ��
//������ˢ�´���:
//BUZ_ON-->��������һ��ʱ��BuzTimeMSH-->BUZ_OFF-->��������һ��ʱ��BuzTimeMSL-->����BUZ_ON
void BUZRefrech(void)
{
	static uint8_t BuzOnTimeNum = 0;
	if(BuzEvent.BuzMode != (BUZMode)MODE_IDIE)					  //�������ǿ���̬
	{ 
		if(checkdelay(BuzEvent.BuzSetTimeMSL) && (BuzEvent.BuzState != (BuzState)STATE_ON)) //�¼�һ�����ͣ��������Ϳ�ʼ��
		{
			BUZ_ON;
			BuzEvent.BuzSetTimeMSH = setdelay(BuzEvent.BuzTimeMSH);  //���������ʱ��
			BuzEvent.BuzState = (BuzState)STATE_ON;               //��������״̬תΪ����״̬ 
			BuzOnTimeNum++;
		}
		if(checkdelay(BuzEvent.BuzSetTimeMSH) && (BuzEvent.BuzState == (BuzState)STATE_ON))   //�����������ˣ��رշ�����
		{
			BUZ_OFF;
			BuzEvent.BuzSetTimeMSL = setdelay(BuzEvent.BuzTimeMSL);  //���������ʱ��
			BuzEvent.BuzState = (BuzState)STATE_OFF;                 //��������״̬תΪ�ر�״̬
		}
		
		if(BuzEvent.AfterRingState == ((AfterRingState)Stop) && (BuzOnTimeNum == BuzEvent.BuzOnTime))  	  //����������һ���ں󲻼�����,�����¼�������־
		{
			if(BuzEvent.BuzState == (BuzState)STATE_OFF)		  //�Ѿ�����һ����
			{
				BuzEvent.BuzState = (BuzState)STATE_IDIE;    	  //������״̬��Ϊ����̬
				BuzEvent.BuzMode = (BUZMode)MODE_IDIE;			  //ģʽҲ��Ϊ����̬
				BuzOnTimeNum = 0;
			}
		}
	}	
}

//���°�������
void BUZ_Key(void)
{
	if(BuzEvent.BuzState == (BuzState)STATE_IDIE)  			//û�������������¼�����
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

//������������
//������֮���״̬��Ҫ���ã���Ϊ�����Continue״̬�������������¼��ͻ�ʧЧ������Continue״̬����˵�ǽ���״̬
void BUZ_CarDanger(void)
{
	if(BuzEvent.BuzState == (BuzState)STATE_IDIE)  			//û�������������¼�����
	{
		BuzEvent.BuzMode = CarDanger;
		BuzEvent.BuzTimeMSH = 10;
		BuzEvent.BuzTimeMSL = 100;
		BuzEvent.BuzSetTimeMSH = 0;
		BuzEvent.BuzSetTimeMSL = 0;
		BuzEvent.AfterRingState = Continue;			    
	}
}

//���յ����ݱ���
void BUZ_DataRecOk(void)
{
	if(BuzEvent.BuzState == (BuzState)STATE_IDIE)  			//û�������������¼�����
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

//�͵�ѹ����
void BUZ_LowVoltage(void)
{
	if(BuzEvent.BuzState == (BuzState)STATE_IDIE)  			//û�������������¼�����
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

//��̬
void BUZ_Normal(void)
{
	if(BuzEvent.BuzState == (BuzState)STATE_IDIE)  			//û�������������¼�����
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
