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
#define LineDataNum     2  //Ӧ��ż��


#define UartRxDataLen   41
#define UartRxCmdLen    7

#define UartCmdNum  SerialPortRx.Buffer[SerialPortRx.Stack-3]//�����
#define UartCmdData SerialPortRx.Buffer[SerialPortRx.Stack-2]//��������

extern SerialPortType SerialPortRx;
extern double UartData[12];

void UartDebug(void);                                        //������λ��������
void UartCmd(uint8_t Num,uint8_t Data); 					 //���ռ��̵�ֵ������Ӧ������
void FreeCars_Init(void);									 //FreeCars��λ���˿ڵĳ�ʼ��
void sendCamImgToCamViewer(void);        					 //����ͼ����λ��
void sendDataToScope(void);									 //�������ݵ���λ��
void sendRoadDataToCamViewer(void);				 //��������ͼ����λ��
void push(uint8_t chanel,uint16_t data); 					 //����װ�أ�ע���ʱ���ݲ�û�з��ͳ�ȥ��ֻ��װ�ص���������

#endif
