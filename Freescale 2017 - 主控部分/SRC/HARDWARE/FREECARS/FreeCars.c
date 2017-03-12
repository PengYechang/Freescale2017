#include "FreeCars.h"
#include "encoder.h"

#define IsSingleLine  0   		  //Ĭ��ʹ��˫��
uint8_t FreeCarsDataNum=UartDataNum*2;
uint8_t uSendBuf[UartDataNum*2]={0};
SerialPortType SerialPortRx;
double UartData[9];

/*
 ʱ�䣺2015/12/2
 �汾��V1.0
 ���ܣ�FreeCars��λ���˿ڵĳ�ʼ��
*/
void FreeCars_Init(void)
{
	UART_QuickInit( UART3_RX_PB10_TX_PB11,115200);  //UART0 PD02 PD03���ٳ�ʼ��
}


//�������ݵ���λ��
void sendDataToScope(void)
{
    uint8_t i,sum=0; 
    //ʹ����ѯ�ķ�ʽ�������ݣ�������δ���ͣ�����ͣ�ڴ˴�ֱ���������
    UART_WriteByte(HW_UART3,251);
    UART_WriteByte(HW_UART3,109);
    UART_WriteByte(HW_UART3,37);
    sum+=(251);      //ȫ�����ݼ���У��
    sum+=(109);
    sum+=(37);
    for(i=0;i<FreeCarsDataNum;i++)
    {
        UART_WriteByte(HW_UART3,uSendBuf[i]);
        sum+=uSendBuf[i];         //ȫ�����ݼ���У��
    }
    UART_WriteByte(HW_UART3,sum);
}


//����װ�أ�ע���ʱ���ݲ�û�з��ͳ�ȥ��ֻ��װ�ص���������
void push(uint8_t chanel,uint16_t data)
{
    uSendBuf[chanel*2]=data/256;
    uSendBuf[chanel*2+1]=data%256;
}

//������λ��������
void UartDebug(void)
{ 
	LeftSpeedCtr.Kp = UartData[0];
	LeftSpeedCtr.Ki = UartData[1];
	LeftSpeedCtr.Kd = UartData[2];
	RightSpeedCtr.Kp = UartData[3];
	RightSpeedCtr.Ki = UartData[4];
	RightSpeedCtr.Kd = UartData[5];
//	SysRelativeSpeed = UartData[3];	
}

//���ռ��̵�ֵ������Ӧ������
void UartCmd(uint8_t Num,uint8_t Data) //���ݼ��̵�ֵ�ı�����еĲ���
{
	if(Num == 1)
	{
		switch(Data)
		{
			case 6:              //F6
				
				break;
			case 7:              //F7
				
				break;
			case 8:              //F8
				
				break;
			case 9:              //F9
				
				break;
			case 10:             //F10
				
				break;
			case 11:             //F11
				
				break;
			case 12:             //F12
				
				break;
			default :
				break;
		}
	}
	else if(Num == 2)
	{
		switch(Data)
		{
			case 100:              //Pause
				
				break;
			case 101:              //Home
	LeftSpeedCtr.SpeedSet = 0;
	RightSpeedCtr.SpeedSet = 0;
				break;
			case 102:              //Paup
				
				break;
			case 103:              //Padn
				
				break;
			case 104:              //end
	LeftSpeedCtr.SpeedSet = 100;
	RightSpeedCtr.SpeedSet = 100;
				break;
			default :
				break;
		}
		
	}
}


