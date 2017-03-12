#ifndef __KEYEVENT2LCD_H__
#define __KEYEVENT2LCD_H__

#include "keyboard.h"
#include "oled.h"
#include "LCDEvent.h"
#include "Bomaswitch.h"
#include "VoltageDetect.h"
#include "speedCtrl.h"
#include "dirCtrl.h"
#include "fuzzySpeed.h"

#define PAGE_VAR  	8	      										//一页对应的变量数
#define PAGE_NUM  	3   	   										//页数：变量有3个子页
#define PAGE_MAX	3												//总的页数，有3页 变量页又分为3个子页
#define VAR_NUM  	(PAGE_VAR*PAGE_NUM) 							//总的变量数
#define STATUS_NUM  5												//状态的数量

//count  0-23 对应24个变量
#define	VAR_VALUE(count)	    VarInfo[count].VAL				//count对应的变量的值
#define	VAR_OLDVALUE(count)	    VarInfo[count].OLDVAL			//count对应的变量的最后确认值
#define	VAR_MAXVALUE(count)	    VarInfo[count].MAXVAL   
#define	VAR_MINVALUE(count)	    VarInfo[count].MINVAL
#define	VAR_SITEX(count)	    VarInfo[count].SITEX
#define VAR_SITEY(count)        VarInfo[count].SITEY
#define VAR_NAME(count)         VarInfo[count].NAME

#define	VAR_SELECT_HOLD_OFFSET		((VAR_NUM+4-1)/4)		   //快上快下时，每次切换的偏移量
#define	VAR_VALUE_HOLD_OFFSET	     10						   //快加快减时，每次的加减的偏移量

typedef void (*keyevent_t) (void);							   //定义函数指针

//按键的事件对应到相应的变量事件
typedef enum
{
	VAR_PREV,									//上一个
	VAR_NEXT,									//下一个
	VAR_NEXT_HOLD,		      					//快下，偏移为:VAR_SELECT_HOLD_OFFSET
	VAR_PREV_HOLD,								//快上，偏移为:VAR_SELECT_HOLD_OFFSET
	VAR_ADD,									//加1
	VAR_SUB,									//减1
	VAR_ADD_HOLD,								//快加，偏移为:VAR_VALUE_HOLE_OFFSET
	VAR_SUB_HOLD,								//快减，偏移为:VAR_VALUE_HOLE_OFFSET  
	VAR_OK=KEY_OK,								//确定
	VAR_CANCEL=KEY_CANCLE,						//取消
	VAR_EVENT_MAX,								//最大变量事情
}VAR_EVENT;

//变量的所有信息
typedef struct
{
	int32_t 		VAL;						//目前的值  
	int32_t 		OLDVAL;						//以前的值
	int32_t		MINVAL;							//最小值
	int32_t		MAXVAL;							//最大值
	uint8_t  	SITEX;      					//变量的坐标  X
	uint8_t      SITEY;	    				    //变量的坐标  Y
	const char*   NAME;							//变量名
}VAR_INFO;										//变量信息

//页事件类
typedef struct
{
  int16_t   Page;							//用来指示当前页
  bool 		Change;							//指示页是否改变
}PAGEEVENT;

extern PAGEEVENT       PageEvent;			//页的事件
extern keyevent_t KeyEvent[PAGE_NUM];		//按键事件

extern bool PrintfZero_Enable;
extern bool StartRun;

void VarPageDisplay(void);					//变量显示
void LoadVarValue(void);
#endif
