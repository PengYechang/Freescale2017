#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "gpio.h"
#include "LedBuz.h"

#define KEY_PORT  HW_GPIOD

#define KEY1_PIN  7
#define KEY2_PIN  8
#define KEY3_PIN  9
#define KEY4_PIN  10
#define KEY5_PIN  11
#define KEY6_PIN  13

/************************************************************
					     板子上的按键图
			列:
行：			KEY_PREV		KEY_ADD		KEY_OK
			KEY_NEXT		KEY_SUB		KEY_CANCLE
************************************************************/
//按键的种类，以下名称到时候要修改
typedef enum 
{    
	KEY_PREV,
	KEY_NEXT, 
    KEY_SUB,
	KEY_ADD,
	KEY_CANCLE,
	KEY_OK,
}KEY_Type;

//按键的状态
typedef enum
{
    KEY_IDLE,          			//空闲态
    KEY_DEBOUNCE,      			//消抖态
    KEY_DOWN,       			//按下按键
	KEY_HOLD,					//保持
}KEY_Status;

//按键类
typedef struct
{
	KEY_Type 	key;			//哪个按键
	KEY_Status  status;         //按键的状态
}KEY_MSG;

//消息的状态
typedef enum
{
    KEY_MSG_NORMAL,    //一般消息
    KEY_MSG_FULL,      //消息已满
    KEY_MSG_EMPTY,     //消息空
}KEY_MSG_STATUS;

void Keyboard_Init(void);
uint8_t Send_KeyMsg(KEY_MSG key_msg);
uint8_t Get_KeyMsg(KEY_MSG *key_msg);
void Keyboard_Scan(void);

extern uint32_t KEY_HOLD_TIME;						//用于判断按键按下的时间，当大于这个时间后就认为是长按
extern uint32_t KEY_DOWN_TIME;							

#endif
