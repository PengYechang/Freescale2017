#ifndef __FREECARS_H__
#define __FREECARS_H__

#include "uart.h"
#include "gpio.h"
#include "camera.h"
#include "speedCtrl.h"

#define UartRxBufferLen 100

typedef struct 
{
  int Stack;
  uint8_t Data;
  uint8_t PreData;
  uint8_t Buffer[UartRxBufferLen];
  uint8_t Enable;
  uint8_t Check;
}SerialPortType;

#define UartDataNum     20
#define LineDataNum     2  //应是偶数


#define UartRxDataLen   41
#define UartRxCmdLen    7

#define UartCmdNum  SerialPortRx.Buffer[SerialPortRx.Stack-3]//命令号
#define UartCmdData SerialPortRx.Buffer[SerialPortRx.Stack-2]//命令数据

extern SerialPortType SerialPortRx;
extern double UartData[12];

void UartDebug(void);                                        //接收上位机的数据
void UartCmd(uint8_t Num,uint8_t Data); 					 //接收键盘的值，给相应的命令
void FreeCars_Init(void);									 //FreeCars上位机端口的初始化
void sendCamImgToCamViewer(void);        					 //发送图像到上位机
void sendDataToScope(void);									 //发送数据到上位机
void sendRoadDataToCamViewer(void);				 //发送赛道图像到上位机
void push(uint8_t chanel,uint16_t data); 					 //数据装载，注意此时数据并没有发送出去，只是装载到了数组里

#endif
