#include "FreeCars.h"

#define IsSingleLine  0   		  //默认使用双线
uint8_t FreeCarsDataNum=UartDataNum*2;
uint8_t uSendBuf[UartDataNum*2]={0};
SerialPortType SerialPortRx;
double UartData[12];

/*
 时间：2015/12/2
 版本：V1.0
 功能：FreeCars上位机端口的初始化
*/
void FreeCars_Init(void)
{
	UART_QuickInit( UART3_RX_PB10_TX_PB11,115200);  //UART0 PD02 PD03快速初始化
}

//发送图像到上位机
void sendCamImgToCamViewer(void)
{
    uint16_t i,j;
    uint8_t d;
	
    UART_WriteByte(HW_UART3,0xFF);//
//	Image_TxOk = false;			  //进入保护状态，确保这图图像传输完成之后再切换图像	
    for(i = 0;i < Image_Height;i++)
    {
        for(j = 0;j < Image_Width;j++)
        {
			d = Image_Buff1[i][j];      //Image_Deal
            if(d > 0xFD) d = 0xFD;            //避开校验位
            UART_WriteByte(HW_UART3,d);
        }
    }
    j=0;
//	Image_TxOk = true;
}

//发送数据到上位机
void sendDataToScope(void)
{
    uint8_t i,sum=0; 
    //使用轮询的方式发送数据，当数据未发送，程序停在此处直到发送完成
    UART_WriteByte(HW_UART3,251);
    UART_WriteByte(HW_UART3,109);
    UART_WriteByte(HW_UART3,37);
    sum+=(251);      //全部数据加入校验
    sum+=(109);
    sum+=(37);
    for(i=0;i<FreeCarsDataNum;i++)
    {
        UART_WriteByte(HW_UART3,uSendBuf[i]);
        sum+=uSendBuf[i];         //全部数据加入校验
    }
    UART_WriteByte(HW_UART3,sum);
}


//数据装载，注意此时数据并没有发送出去，只是装载到了数组里
void push(uint8_t chanel,uint16_t data)
{
    uSendBuf[chanel*2]=data/256;
    uSendBuf[chanel*2+1]=data%256;
}



