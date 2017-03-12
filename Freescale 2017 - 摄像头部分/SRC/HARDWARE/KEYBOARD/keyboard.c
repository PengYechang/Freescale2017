#include "keyboard.h"

//KEY_DOWN_TIME和KEY_HOLD_TIME根据调试情况来确定
#define KEY_MAX            6							//有六个按键
uint32_t KEY_HOLD_TIME   =   70;//20000;						//用于判断按键按下的时间，当大于这个时间后就认为是长按
uint32_t KEY_DOWN_TIME   =	 2;//2500;							//用于判断按键按下的时间，当大于这个时间后就认为是短按
#define KEY_MSG_FIFO_MAX   200           	    		//最大存储的键盘信息
KEY_MSG KEY_MSG_FIFO[KEY_MSG_FIFO_MAX];   				//用来储存按键的消息
uint32_t KEY_Press_Time[KEY_MAX];
uint8_t key_msg_rear = 0,key_msg_front = 0;             //key_msg_rear用来指示当前储存的按键消息
														//key_msg_front用来指示当前所取的按键消息
volatile KEY_MSG_STATUS key_msg_status = KEY_MSG_EMPTY; //key_msg_status用来指示当前消息队列的状态

//按键初始化
void Keyboard_Init(void)
{
	//键盘初始化
	GPIO_QuickInit(KEY_PORT,KEY1_PIN,kGPIO_Mode_IPU);      
	GPIO_QuickInit(KEY_PORT,KEY2_PIN,kGPIO_Mode_IPU);
	GPIO_QuickInit(KEY_PORT,KEY3_PIN,kGPIO_Mode_IPU);
	GPIO_QuickInit(KEY_PORT,KEY4_PIN,kGPIO_Mode_IPU);
	GPIO_QuickInit(KEY_PORT,KEY5_PIN,kGPIO_Mode_IPU);
	GPIO_QuickInit(KEY_PORT,KEY6_PIN,kGPIO_Mode_IPU);
}

//储存按键消息
uint8_t Send_KeyMsg(KEY_MSG key_msg)
{
	if(key_msg_status == KEY_MSG_FULL)		//消息队列里的消息已经取完
	{
		return 0;
	}
	
	//储存按键消息
	KEY_MSG_FIFO[key_msg_rear].status = key_msg.status;
	KEY_MSG_FIFO[key_msg_rear].key    = key_msg.key;
	key_msg_rear++;
	
	if(key_msg_rear >= KEY_MSG_FIFO_MAX)   //大于最大消息数，清零从头再储存
	{
		key_msg_rear = 0;			
	}
	
	if(key_msg_rear == key_msg_front)	   //一般情况是key_msg_rear>key_msg_front的，如果一直输入消息
	{									   //使key_msg_rear加到和key_msg_front相等时，这个时候是不能再
		key_msg_status = KEY_MSG_FULL;	   //储存消息的，因为消息都没被取完
	}
	else
	{
		key_msg_status = KEY_MSG_NORMAL;
	}
	return 1;
}

//获取消息队列里的按键消息
uint8_t Get_KeyMsg(KEY_MSG *key_msg)
{
	if(key_msg_status == KEY_MSG_EMPTY)		//消息队列里的消息已经取完
	{
		return 0;
	}
	
	//取消息
	key_msg->status = KEY_MSG_FIFO[key_msg_front].status;
	key_msg->key    = KEY_MSG_FIFO[key_msg_front].key;
	key_msg_front++;
	
	if(key_msg_front >= KEY_MSG_FIFO_MAX)  //大于最大消息数，清零从头再储存
	{
		key_msg_front = 0;			
	}
	
	if(key_msg_front == key_msg_rear)	  //当两者相等时，说明消息队列里已经没有消息了
	{
		key_msg_status = KEY_MSG_EMPTY;
	}
	return 1;
}
//按键扫描 板子上用的是独立按键，采用状态机来进行扫描
//该程序运行一次需要大约0.0809ms
/************************************************************
					     板子上的按键图
			列:
行：			KEY_PREV		KEY_ADD		KEY_OK
			KEY_NEXT		KEY_SUB		KEY_CANCLE
************************************************************/
void Keyboard_Scan(void)
{
	uint8_t count = 0;							//用于计数
	static KEY_Status status[9];				//用来储存按键的状态,9个按键九个状态
	static uint8_t OnlyOne = 0;					//用于初始化状态机的状态
	KEY_MSG keyMsgTem;							//当按键按下时，将按键的信息发送到一个储存器里面进程储存
	
	if(OnlyOne == 0)							//只会运行一次，用于初始化状态机的状态
	{
		OnlyOne = 1;
		for(count=0;count<KEY_MAX;count++)
		{
			status[count] = KEY_IDLE;
		}
	}

	for(count=0;count<KEY_MAX-1;count++)				//开始进行扫描，5个按键扫描5次
	{
		switch(status[count])
		{
			case KEY_IDLE:
				if(GPIO_ReadBit(KEY_PORT,count+7) == 0)
				{
					status[count] = KEY_DEBOUNCE;		//如果按键被按下,进入消抖态
				}
				break;
			case KEY_DEBOUNCE: 							//消抖态
				if(GPIO_ReadBit(KEY_PORT,count+7) == 1)
				{
					status[count] = KEY_IDLE;		   	//如果按键未按下,退回空闲状态，并清空按下的时间
					KEY_Press_Time[count] = 0;
				}
				else
				{
					KEY_Press_Time[count]++;
					if(KEY_Press_Time[count] >= KEY_DOWN_TIME)
					{
						status[count] = KEY_DOWN;	    //如果按键被按下,进入确认态
//						BUZ_Key();						//蜂鸣器提示
						KEY_Press_Time[count] = 0;
					}
				}
				break;
			case KEY_DOWN: 								//确认态
				if(GPIO_ReadBit(KEY_PORT,count+7) == 1) 
				{
					status[count] = KEY_IDLE;			//如果按键未按下,退回空闲态，并储存按键的状态和消息
					keyMsgTem.key = (KEY_Type)count;
					keyMsgTem.status = KEY_DOWN;
					Send_KeyMsg(keyMsgTem);				//发送消息，储存按键消息
					KEY_Press_Time[count] = 0;			//清空按下的时间
				}
				else		
				{
					KEY_Press_Time[count]++;
					if(KEY_Press_Time[count] >= KEY_HOLD_TIME+1)  //当按下相当长的时间后才认为是长按
					{
						status[count] = KEY_HOLD;				  //如果按键被按下,进入保持态
//						BUZ_Key();								  //蜂鸣器提示
						KEY_Press_Time[count] = 0;
					}
				}
				break;
			case KEY_HOLD:								//保持态
				if(GPIO_ReadBit(KEY_PORT,count+7) == 1) 
				{
					status[count] = KEY_IDLE;			//按键松开,重新回归空闲态
					keyMsgTem.key = (KEY_Type)count;
					keyMsgTem.status = KEY_HOLD;
					Send_KeyMsg(keyMsgTem);				//发送消息，储存按键消息
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
				status[5] = KEY_DEBOUNCE;		//如果按键被按下,进入消抖态
			}
			break;
		case KEY_DEBOUNCE: 							//消抖态
			if(GPIO_ReadBit(KEY_PORT,13) == 1)
			{
				status[5] = KEY_IDLE;		   	//如果按键未按下,退回空闲状态，并清空按下的时间
				KEY_Press_Time[5] = 0;
			}
			else
			{
				KEY_Press_Time[5]++;
				if(KEY_Press_Time[5] >= KEY_DOWN_TIME)
				{
					status[5] = KEY_DOWN;	    //如果按键被按下,进入确认态
//					BUZ_Key();					//蜂鸣器提示
					KEY_Press_Time[5] = 0;
				}
			}
			break;
		case KEY_DOWN: 								//确认态
			if(GPIO_ReadBit(KEY_PORT,13) == 1) 
			{
				status[5] = KEY_IDLE;			//如果按键未按下,退回空闲态，并储存按键的状态和消息
				keyMsgTem.key = (KEY_Type)5;
				keyMsgTem.status = KEY_DOWN;
				Send_KeyMsg(keyMsgTem);				//发送消息，储存按键消息
				KEY_Press_Time[5] = 0;			//清空按下的时间
			}
			else		
			{
				KEY_Press_Time[5]++;
				if(KEY_Press_Time[5] >= KEY_HOLD_TIME+1)  //当按下相当长的时间后才认为是长按
				{
					status[5] = KEY_HOLD;			  //如果按键被按下,进入保持态
//					BUZ_Key();						  //蜂鸣器提示
					KEY_Press_Time[5] = 0;
				}
			}
			break;
		case KEY_HOLD:								//保持态
			if(GPIO_ReadBit(KEY_PORT,13) == 1) 
			{
				status[5] = KEY_IDLE;				//按键松开,重新回归空闲态
				keyMsgTem.key = (KEY_Type)5;
				keyMsgTem.status = KEY_HOLD;
				Send_KeyMsg(keyMsgTem);				//发送消息，储存按键消息
			}
			break;
		default:
			break;
	}
}
