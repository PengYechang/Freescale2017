#include "keyboard.h"

//KEY_DOWN_TIME��KEY_HOLD_TIME���ݵ��������ȷ��
#define KEY_MAX            6							//����������
uint32_t KEY_HOLD_TIME   =   70;//20000;						//�����жϰ������µ�ʱ�䣬���������ʱ������Ϊ�ǳ���
uint32_t KEY_DOWN_TIME   =	 2;//2500;							//�����жϰ������µ�ʱ�䣬���������ʱ������Ϊ�Ƕ̰�
#define KEY_MSG_FIFO_MAX   200           	    		//���洢�ļ�����Ϣ
KEY_MSG KEY_MSG_FIFO[KEY_MSG_FIFO_MAX];   				//�������水������Ϣ
uint32_t KEY_Press_Time[KEY_MAX];
uint8_t key_msg_rear = 0,key_msg_front = 0;             //key_msg_rear����ָʾ��ǰ����İ�����Ϣ
														//key_msg_front����ָʾ��ǰ��ȡ�İ�����Ϣ
volatile KEY_MSG_STATUS key_msg_status = KEY_MSG_EMPTY; //key_msg_status����ָʾ��ǰ��Ϣ���е�״̬

//������ʼ��
void Keyboard_Init(void)
{
	//���̳�ʼ��
	GPIO_QuickInit(KEY_PORT,KEY1_PIN,kGPIO_Mode_IPU);      
	GPIO_QuickInit(KEY_PORT,KEY2_PIN,kGPIO_Mode_IPU);
	GPIO_QuickInit(KEY_PORT,KEY3_PIN,kGPIO_Mode_IPU);
	GPIO_QuickInit(KEY_PORT,KEY4_PIN,kGPIO_Mode_IPU);
	GPIO_QuickInit(KEY_PORT,KEY5_PIN,kGPIO_Mode_IPU);
	GPIO_QuickInit(KEY_PORT,KEY6_PIN,kGPIO_Mode_IPU);
}

//���水����Ϣ
uint8_t Send_KeyMsg(KEY_MSG key_msg)
{
	if(key_msg_status == KEY_MSG_FULL)		//��Ϣ���������Ϣ�Ѿ�ȡ��
	{
		return 0;
	}
	
	//���水����Ϣ
	KEY_MSG_FIFO[key_msg_rear].status = key_msg.status;
	KEY_MSG_FIFO[key_msg_rear].key    = key_msg.key;
	key_msg_rear++;
	
	if(key_msg_rear >= KEY_MSG_FIFO_MAX)   //���������Ϣ���������ͷ�ٴ���
	{
		key_msg_rear = 0;			
	}
	
	if(key_msg_rear == key_msg_front)	   //һ�������key_msg_rear>key_msg_front�ģ����һֱ������Ϣ
	{									   //ʹkey_msg_rear�ӵ���key_msg_front���ʱ�����ʱ���ǲ�����
		key_msg_status = KEY_MSG_FULL;	   //������Ϣ�ģ���Ϊ��Ϣ��û��ȡ��
	}
	else
	{
		key_msg_status = KEY_MSG_NORMAL;
	}
	return 1;
}

//��ȡ��Ϣ������İ�����Ϣ
uint8_t Get_KeyMsg(KEY_MSG *key_msg)
{
	if(key_msg_status == KEY_MSG_EMPTY)		//��Ϣ���������Ϣ�Ѿ�ȡ��
	{
		return 0;
	}
	
	//ȡ��Ϣ
	key_msg->status = KEY_MSG_FIFO[key_msg_front].status;
	key_msg->key    = KEY_MSG_FIFO[key_msg_front].key;
	key_msg_front++;
	
	if(key_msg_front >= KEY_MSG_FIFO_MAX)  //���������Ϣ���������ͷ�ٴ���
	{
		key_msg_front = 0;			
	}
	
	if(key_msg_front == key_msg_rear)	  //���������ʱ��˵����Ϣ�������Ѿ�û����Ϣ��
	{
		key_msg_status = KEY_MSG_EMPTY;
	}
	return 1;
}
//����ɨ�� �������õ��Ƕ�������������״̬��������ɨ��
//�ó�������һ����Ҫ��Լ0.0809ms
/************************************************************
					     �����ϵİ���ͼ
			��:
�У�			KEY_PREV		KEY_ADD		KEY_OK
			KEY_NEXT		KEY_SUB		KEY_CANCLE
************************************************************/
void Keyboard_Scan(void)
{
	uint8_t count = 0;							//���ڼ���
	static KEY_Status status[9];				//�������水����״̬,9�������Ÿ�״̬
	static uint8_t OnlyOne = 0;					//���ڳ�ʼ��״̬����״̬
	KEY_MSG keyMsgTem;							//����������ʱ������������Ϣ���͵�һ��������������̴���
	
	if(OnlyOne == 0)							//ֻ������һ�Σ����ڳ�ʼ��״̬����״̬
	{
		OnlyOne = 1;
		for(count=0;count<KEY_MAX;count++)
		{
			status[count] = KEY_IDLE;
		}
	}

	for(count=0;count<KEY_MAX-1;count++)				//��ʼ����ɨ�裬5������ɨ��5��
	{
		switch(status[count])
		{
			case KEY_IDLE:
				if(GPIO_ReadBit(KEY_PORT,count+7) == 0)
				{
					status[count] = KEY_DEBOUNCE;		//�������������,��������̬
				}
				break;
			case KEY_DEBOUNCE: 							//����̬
				if(GPIO_ReadBit(KEY_PORT,count+7) == 1)
				{
					status[count] = KEY_IDLE;		   	//�������δ����,�˻ؿ���״̬������հ��µ�ʱ��
					KEY_Press_Time[count] = 0;
				}
				else
				{
					KEY_Press_Time[count]++;
					if(KEY_Press_Time[count] >= KEY_DOWN_TIME)
					{
						status[count] = KEY_DOWN;	    //�������������,����ȷ��̬
//						BUZ_Key();						//��������ʾ
						KEY_Press_Time[count] = 0;
					}
				}
				break;
			case KEY_DOWN: 								//ȷ��̬
				if(GPIO_ReadBit(KEY_PORT,count+7) == 1) 
				{
					status[count] = KEY_IDLE;			//�������δ����,�˻ؿ���̬�������水����״̬����Ϣ
					keyMsgTem.key = (KEY_Type)count;
					keyMsgTem.status = KEY_DOWN;
					Send_KeyMsg(keyMsgTem);				//������Ϣ�����水����Ϣ
					KEY_Press_Time[count] = 0;			//��հ��µ�ʱ��
				}
				else		
				{
					KEY_Press_Time[count]++;
					if(KEY_Press_Time[count] >= KEY_HOLD_TIME+1)  //�������൱����ʱ������Ϊ�ǳ���
					{
						status[count] = KEY_HOLD;				  //�������������,���뱣��̬
//						BUZ_Key();								  //��������ʾ
						KEY_Press_Time[count] = 0;
					}
				}
				break;
			case KEY_HOLD:								//����̬
				if(GPIO_ReadBit(KEY_PORT,count+7) == 1) 
				{
					status[count] = KEY_IDLE;			//�����ɿ�,���»ع����̬
					keyMsgTem.key = (KEY_Type)count;
					keyMsgTem.status = KEY_HOLD;
					Send_KeyMsg(keyMsgTem);				//������Ϣ�����水����Ϣ
				}
				break;
			default:
				break;
		}
	}
	
	switch(status[5])
	{
		case KEY_IDLE:
			if(GPIO_ReadBit(KEY_PORT,13) == 0)
			{
				status[5] = KEY_DEBOUNCE;		//�������������,��������̬
			}
			break;
		case KEY_DEBOUNCE: 							//����̬
			if(GPIO_ReadBit(KEY_PORT,13) == 1)
			{
				status[5] = KEY_IDLE;		   	//�������δ����,�˻ؿ���״̬������հ��µ�ʱ��
				KEY_Press_Time[5] = 0;
			}
			else
			{
				KEY_Press_Time[5]++;
				if(KEY_Press_Time[5] >= KEY_DOWN_TIME)
				{
					status[5] = KEY_DOWN;	    //�������������,����ȷ��̬
//					BUZ_Key();					//��������ʾ
					KEY_Press_Time[5] = 0;
				}
			}
			break;
		case KEY_DOWN: 								//ȷ��̬
			if(GPIO_ReadBit(KEY_PORT,13) == 1) 
			{
				status[5] = KEY_IDLE;			//�������δ����,�˻ؿ���̬�������水����״̬����Ϣ
				keyMsgTem.key = (KEY_Type)5;
				keyMsgTem.status = KEY_DOWN;
				Send_KeyMsg(keyMsgTem);				//������Ϣ�����水����Ϣ
				KEY_Press_Time[5] = 0;			//��հ��µ�ʱ��
			}
			else		
			{
				KEY_Press_Time[5]++;
				if(KEY_Press_Time[5] >= KEY_HOLD_TIME+1)  //�������൱����ʱ������Ϊ�ǳ���
				{
					status[5] = KEY_HOLD;			  //�������������,���뱣��̬
//					BUZ_Key();						  //��������ʾ
					KEY_Press_Time[5] = 0;
				}
			}
			break;
		case KEY_HOLD:								//����̬
			if(GPIO_ReadBit(KEY_PORT,13) == 1) 
			{
				status[5] = KEY_IDLE;				//�����ɿ�,���»ع����̬
				keyMsgTem.key = (KEY_Type)5;
				keyMsgTem.status = KEY_HOLD;
				Send_KeyMsg(keyMsgTem);				//������Ϣ�����水����Ϣ
			}
			break;
		default:
			break;
	}
}
