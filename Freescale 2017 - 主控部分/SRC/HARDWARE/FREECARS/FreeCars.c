#include "FreeCars.h"
#include "encoder.h"

#define IsSingleLine  0   		  //默认使用双线
uint8_t FreeCarsDataNum=UartDataNum*2;
uint8_t uSendBuf[UartDataNum*2]={0};
SerialPortType SerialPortRx;
double UartData[9];

/*
 时间：2015/12/2
 版本：V1.0
 功能：FreeCars上位机端口的初始化
*/
void FreeCars_Init(void)
{
	UART_QuickInit( UART3_RX_PB10_TX_PB11,115200);  //UART0 PD02 PD03快速初始化
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

//接收上位机的数据
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

//接收键盘的值，给相应的命令
void UartCmd(uint8_t Num,uint8_t Data) //根据键盘的值改变程序中的参数
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


