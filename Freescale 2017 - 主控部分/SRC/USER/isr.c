#include "isr.h"

static void UART2_RX_ISR(uint16_t byteReceived); //���ڽ����ж�
static void PIT0_ISR(void);

uint32_t avoidRamWayEffectTime = 0;

uint16_t row = 0;
uint8_t  ImgOK = false;
uint8_t  ImgDealOK = false;

void ISR_Config(void)
{

	//FreeCars���ô����жϵ����� UART2
	UART_CallbackRxInstall(HW_UART2, UART2_RX_ISR);  //����UART2�ж����ã��򿪽����жϣ���װ�жϻص�����
	UART_ITDMAConfig(HW_UART2, kUART_IT_Rx, true);  //�򿪴��ڽ����жϹ��� IT �����жϵ���˼
	
	//5ms��ʱ���ж�
    PIT_CallbackInstall(HW_PIT_CH0, PIT0_ISR);			      //����PIT0�жϣ�ע��ص�����
    PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF,true); 		      //����PIT0��5ms��ʱ���ж�
	
	NVIC_Init(PIT0_IRQn,NVIC_PriorityGroup_2,1,2);			  //�ٶȿ���
	NVIC_Init(UART2_RX_TX_IRQn,NVIC_PriorityGroup_2,2,0);	  //��λ��ͨ��

}


//5ms��ʱ���ж�
static void PIT0_ISR(void)
{	
	PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF,false);				  //�ȹرո��ж�
	
//	Keyboard_Scan();											  //����ɨ���Լ��Ҫ0.2578ms
	//�����ٶ�
	LeftSpeedCtr.SpeedSet = 100;
	RightSpeedCtr.SpeedSet = 100;
	Left_SpeedControl();
	Right_SpeedControl();
	
	PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF,true);	              //�������ж�
}

static void UART2_RX_ISR(uint16_t byteReceived)
{
	int32_t i,b,d1;
    uint32_t d;
	
	SerialPortRx.Data = byteReceived;
	if( SerialPortRx.Stack < UartRxBufferLen )
	{
		SerialPortRx.Buffer[SerialPortRx.Stack++] = SerialPortRx.Data;

		if(   SerialPortRx.Stack >= UartRxDataLen  //UartRxDataLen����Ϊһ֡
				&& SerialPortRx.Buffer[SerialPortRx.Stack - UartRxDataLen]  ==0xff //У����ͷ
				&& SerialPortRx.Buffer[SerialPortRx.Stack - UartRxDataLen+1]==0x55
				&& SerialPortRx.Buffer[SerialPortRx.Stack - UartRxDataLen+2]==0xaa
				&& SerialPortRx.Buffer[SerialPortRx.Stack - UartRxDataLen+3]==0x10 )
		{   //double data 9��ͨ������У��
			SerialPortRx.Check = 0;
			b = SerialPortRx.Stack - UartRxDataLen; //��ʼλ
			for(i=b; i<SerialPortRx.Stack-1; i++)  //��У��λ���λ����У��
			{
				SerialPortRx.Check += SerialPortRx.Buffer[i];//У��
			}

			if( SerialPortRx.Check == SerialPortRx.Buffer[SerialPortRx.Stack-1] )
			{   //У��ɹ����������ݽ���
				for(i = 0; i<9; i++)
				{
					d = SerialPortRx.Buffer[b+i*4+4]*0x1000000L + SerialPortRx.Buffer[b+i*4+5]*0x10000L + SerialPortRx.Buffer[b+i*4+6]*0x100L + SerialPortRx.Buffer[b+i*4+7];
					if(d>0x7FFFFFFF)
					{
						d1 = 0x7FFFFFFF - d;
					}
					else
					{
						d1 = d;
					}
					UartData[i]=d1;
					UartData[i]/=65536.0;
				}
				UartDebug();  //תȥ�������ܵ������ݸ�������
			}
			SerialPortRx.Stack = 0;
		}
		else if(   SerialPortRx.Stack >= UartRxCmdLen //UartRxDataLen����Ϊһ֡
				   && SerialPortRx.Buffer[SerialPortRx.Stack - UartRxCmdLen]  ==0xff
				   && SerialPortRx.Buffer[SerialPortRx.Stack - UartRxCmdLen+1]==0x55
				   && SerialPortRx.Buffer[SerialPortRx.Stack - UartRxCmdLen+2]==0xaa
				   && SerialPortRx.Buffer[SerialPortRx.Stack - UartRxCmdLen+3]==0x77 )//cmd
		{
			SerialPortRx.Check = 0;
			b = SerialPortRx.Stack - UartRxCmdLen; //��ʼλ
			for(i=b; i<SerialPortRx.Stack-1; i++)  //��У��λ���λ����У��
			{
				SerialPortRx.Check += SerialPortRx.Buffer[i];//У��
			}
			if( SerialPortRx.Check == SerialPortRx.Buffer[SerialPortRx.Stack-1] )
			{   //У��ɹ�
				UartCmd(UartCmdNum,UartCmdData);//������յ����������MCU�������
			}
			SerialPortRx.Stack = 0;
		}
	}
	else
	{
		SerialPortRx.Stack = 0;
	}
}
