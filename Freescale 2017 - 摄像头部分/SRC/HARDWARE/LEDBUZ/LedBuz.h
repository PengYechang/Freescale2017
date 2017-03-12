#ifndef __LEDBUZ_H__
#define __LEDBUZ_H__

#include "gpio.h"
#include "rtc.h"

#define LED1_PORT  HW_GPIOD
#define LED2_PORT  HW_GPIOD
#define BUZ_PORT   HW_GPIOC

#define LED1_MODE  kGPIO_Mode_OPP
#define LED2_MODE  kGPIO_Mode_OPP
#define BUZ_MODE   kGPIO_Mode_OPP

#define LED1_PIN   14
#define LED2_PIN   15
#define BUZ_PIN    4

#define LED1_ON	   PDout(LED1_PIN) = 1
#define LED1_OFF   PDout(LED1_PIN) = 0
#define LED1_REV   PDout(LED1_PIN) = !PDout(LED1_PIN)     //LED1״̬ȡ��
#define LED2_ON    PDout(LED2_PIN) = 1
#define LED2_OFF   PDout(LED2_PIN) = 0 
#define LED2_REV   PDout(LED2_PIN) = !PDout(LED2_PIN)     //LED2״̬ȡ��
#define BUZ_ON     PCout(BUZ_PIN)  = 1
#define BUZ_OFF    PCout(BUZ_PIN)  = 0
#define BUZ_REV    PCout(BUZ_PIN)  = !PCout(BUZ_PIN)      //BUZ״̬ȡ��

//BUZ���ģʽ
typedef enum
{
	MODE_IDIE,
	Key,
	CarDanger,
	DataRecOk,
	LowVoltage,
	Normal,
}BUZMode;

//��������֮���������״̬
typedef enum
{
	Continue,
	Stop,
}AfterRingState;

//��������״̬
typedef enum
{
	STATE_IDIE,
	STATE_ON,
	STATE_OFF,
}BuzState;

//�������¼��ṹ��
typedef struct
{
	BUZMode          BuzMode;
	uint32_t         BuzTimeMSH;
	uint32_t         BuzTimeMSL;
	uint32_t         BuzSetTimeMSH;
	uint32_t         BuzSetTimeMSL;
	uint8_t          BuzOnTime;
	BuzState    	 BuzState;
	AfterRingState   AfterRingState;
}BUZEvent;

void GPIO_LedBuzInit(void);      //LED�ƺͷ�������ʼ��
void BUZRefrech(void);			 //�������¼�ˢ��				
void BUZ_Key(void);
void BUZ_CarDanger(void);
void BUZ_DataRecOk(void);
void BUZ_LowVoltage(void);
void BUZ_Normal(void);

#endif
