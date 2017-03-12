#include "isr.h"

static void UART2_RX_ISR(uint16_t byteReceived); //串口接收中断
static void PIT0_ISR(void);

uint32_t avoidRamWayEffectTime = 0;

uint16_t row = 0;
uint8_t  ImgOK = false;
uint8_t  ImgDealOK = false;

void ISR_Config(void)
{

	//FreeCars所用串口中断的配置 UART2
	UART_CallbackRxInstall(HW_UART2, UART2_RX_ISR);  //配置UART2中断配置，打开接收中断，安装中断回调函数
	UART_ITDMAConfig(HW_UART2, kUART_IT_Rx, true);  //打开串口接收中断功能 IT 就是中断的意思
	
	//5ms定时器中断
    PIT_CallbackInstall(HW_PIT_CH0, PIT0_ISR);			      //配置PIT0中断，注册回调函数
    PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF,true); 		      //开启PIT0的5ms定时器中断
	
	NVIC_Init(PIT0_IRQn,NVIC_PriorityGroup_2,1,2);			  //速度控制
	NVIC_Init(UART2_RX_TX_IRQn,NVIC_PriorityGroup_2,2,0);	  //上位机通信

}


//5ms定时器中断
static void PIT0_ISR(void)
{	
	PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF,false);				  //先关闭该中断
	
//	Keyboard_Scan();											  //按键扫描大约需要0.2578ms
	//设置速度
	LeftSpeedCtr.SpeedSet = 100;
	RightSpeedCtr.SpeedSet = 100;
	Left_SpeedControl();
	Right_SpeedControl();
	
	PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF,true);	              //再重启中断
}

static void UART2_RX_ISR(uint16_t byteReceived)
{
	int32_t i,b,d1;
    uint32_t d;
	
	SerialPortRx.Data = byteReceived;
	if( SerialPortRx.Stack < UartRxBufferLen )
	{
		SerialPortRx.Buffer[SerialPortRx.Stack++] = SerialPortRx.Data;

		if(   SerialPortRx.Stack >= UartRxDataLen  //UartRxDataLen个数为一帧
				&& SerialPortRx.Buffer[SerialPortRx.Stack - UartRxDataLen]  ==0xff //校验字头
				&& SerialPortRx.Buffer[SerialPortRx.Stack - UartRxDataLen+1]==0x55
				&& SerialPortRx.Buffer[SerialPortRx.Stack - UartRxDataLen+2]==0xaa
				&& SerialPortRx.Buffer[SerialPortRx.Stack - UartRxDataLen+3]==0x10 )
		{   //double data 9个通道数据校验
			SerialPortRx.Check = 0;
			b = SerialPortRx.Stack - UartRxDataLen; //起始位
			for(i=b; i<SerialPortRx.Stack-1; i++)  //除校验位外的位进行校验
			{
				SerialPortRx.Check += SerialPortRx.Buffer[i];//校验
			}

			if( SerialPortRx.Check == SerialPortRx.Buffer[SerialPortRx.Stack-1] )
			{   //校验成功，进行数据解算
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
				UartDebug();  //转去处理，把受到的数据付给变量
			}
			SerialPortRx.Stack = 0;
		}
		else if(   SerialPortRx.Stack >= UartRxCmdLen //UartRxDataLen个数为一帧
				   && SerialPortRx.Buffer[SerialPortRx.Stack - UartRxCmdLen]  ==0xff
				   && SerialPortRx.Buffer[SerialPortRx.Stack - UartRxCmdLen+1]==0x55
				   && SerialPortRx.Buffer[SerialPortRx.Stack - UartRxCmdLen+2]==0xaa
				   && SerialPortRx.Buffer[SerialPortRx.Stack - UartRxCmdLen+3]==0x77 )//cmd
		{
			SerialPortRx.Check = 0;
			b = SerialPortRx.Stack - UartRxCmdLen; //起始位
			for(i=b; i<SerialPortRx.Stack-1; i++)  //除校验位外的位进行校验
			{
				SerialPortRx.Check += SerialPortRx.Buffer[i];//校验
			}
			if( SerialPortRx.Check == SerialPortRx.Buffer[SerialPortRx.Stack-1] )
			{   //校验成功
				UartCmd(UartCmdNum,UartCmdData);//处理接收到的命令，付给MCU命令变量
			}
			SerialPortRx.Stack = 0;
		}
	}
	else
	{
		SerialPortRx.Stack = 0;
	}
}
