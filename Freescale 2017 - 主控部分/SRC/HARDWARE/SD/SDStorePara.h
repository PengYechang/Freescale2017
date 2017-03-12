#ifndef __SDSTOREPARA_H_
#define __SDSTOREPARA_H_

#include "sd.h"
#include "dirCtrl.h"
#include "stdbool.h"
#include "common.h"
#include "fuzzySpeed.h"

typedef struct
{
	uint8_t   SDCState;			  //SD��״̬
	uint32_t Sectors; 			  //����
	uint32_t Capacity;			  //����
	int32_t  TransferTime;		  //����ʱ��
}SDCardInfoClass;				  //SD����Ϣ�ṹ��

/****************�ڲ�����***************/
static uint8_t readSDPara(void);         //��SD��������ݵ�ϵͳ
static void pushSDDouble(uint8_t x,uint8_t y,double data);
static void pushSDInt32(uint8_t x,uint8_t y,int32_t data);
static double readSDDouble(uint8_t x,uint8_t y);
static int32_t readSDInt32(uint8_t x,uint8_t y);

/***************�߲㺯��***************/
void SDCard_Init(void);        //SD����ʼ��
void setSDParaToSys(void);		  //��Ҫ��Ĳ������������������޸�
void pushDataToSDBuffer(void);
uint8_t saveSDPara(void);         //�������ݵ�SD��

extern SDCardInfoClass SDCardInfo;
#endif
