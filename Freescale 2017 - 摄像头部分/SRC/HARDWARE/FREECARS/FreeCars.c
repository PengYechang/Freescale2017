#include "FreeCars.h"

#define IsSingleLine  0   		  //Ĭ��ʹ��˫��
uint8_t FreeCarsDataNum=UartDataNum*2;
uint8_t uSendBuf[UartDataNum*2]={0};
SerialPortType SerialPortRx;
double UartData[12];

/*
 ʱ�䣺2015/12/2
 �汾��V1.0
 ���ܣ�FreeCars��λ���˿ڵĳ�ʼ��
*/
void FreeCars_Init(void)
{
	UART_QuickInit( UART3_RX_PB10_TX_PB11,115200);  //UART0 PD02 PD03���ٳ�ʼ��
}

//����ͼ����λ��
void sendCamImgToCamViewer(void)
{
    uint16_t i,j;
    uint8_t d;
	
    UART_WriteByte(HW_UART3,0xFF);//
//	Image_TxOk = false;			  //���뱣��״̬��ȷ����ͼͼ�������֮�����л�ͼ��	
    for(i = 0;i < Image_Height;i++)
    {
        for(j = 0;j < Image_Width;j++)
        {
			d = Image_Buff1[i][j];      //Image_Deal
            if(d > 0xFD) d = 0xFD;            //�ܿ�У��λ
            UART_WriteByte(HW_UART3,d);
        }
    }
    j=0;
//	Image_TxOk = true;
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



