#ifndef __SDSTOREPARA_H_
#define __SDSTOREPARA_H_

#include "sd.h"
#include "dirCtrl.h"
#include "stdbool.h"
#include "common.h"
#include "fuzzySpeed.h"

typedef struct
{
	uint8_t   SDCState;			  //SD卡状态
	uint32_t Sectors; 			  //扇区
	uint32_t Capacity;			  //容量
	int32_t  TransferTime;		  //传输时间
}SDCardInfoClass;				  //SD卡信息结构体

/****************内部函数***************/
static uint8_t readSDPara(void);         //读SD卡里的数据到系统
static void pushSDDouble(uint8_t x,uint8_t y,double data);
static void pushSDInt32(uint8_t x,uint8_t y,int32_t data);
static double readSDDouble(uint8_t x,uint8_t y);
static int32_t readSDInt32(uint8_t x,uint8_t y);

/***************高层函数***************/
void SDCard_Init(void);        //SD卡初始化
void setSDParaToSys(void);		  //需要存的参数在这两个函数里修改
void pushDataToSDBuffer(void);
uint8_t saveSDPara(void);         //储存数据到SD卡

extern SDCardInfoClass SDCardInfo;
#endif
