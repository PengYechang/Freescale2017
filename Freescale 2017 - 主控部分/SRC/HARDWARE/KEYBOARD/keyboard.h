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
					     �����ϵİ���ͼ
			��:
�У�			KEY_PREV		KEY_ADD		KEY_OK
			KEY_NEXT		KEY_SUB		KEY_CANCLE
************************************************************/
//���������࣬�������Ƶ�ʱ��Ҫ�޸�
typedef enum 
{    
	KEY_PREV,
	KEY_NEXT, 
    KEY_SUB,
	KEY_ADD,
	KEY_CANCLE,
	KEY_OK,
}KEY_Type;

//������״̬
typedef enum
{
    KEY_IDLE,          			//����̬
    KEY_DEBOUNCE,      			//����̬
    KEY_DOWN,       			//���°���
	KEY_HOLD,					//����
}KEY_Status;

//������
typedef struct
{
	KEY_Type 	key;			//�ĸ�����
	KEY_Status  status;         //������״̬
}KEY_MSG;

//��Ϣ��״̬
typedef enum
{
    KEY_MSG_NORMAL,    //һ����Ϣ
    KEY_MSG_FULL,      //��Ϣ����
    KEY_MSG_EMPTY,     //��Ϣ��
}KEY_MSG_STATUS;

void Keyboard_Init(void);
uint8_t Send_KeyMsg(KEY_MSG key_msg);
uint8_t Get_KeyMsg(KEY_MSG *key_msg);
void Keyboard_Scan(void);

extern uint32_t KEY_HOLD_TIME;						//�����жϰ������µ�ʱ�䣬���������ʱ������Ϊ�ǳ���
extern uint32_t KEY_DOWN_TIME;							

#endif
