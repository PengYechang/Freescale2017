#ifndef __LCDEVENT_H__
#define __LCDEVENT_H__

#include "KeyEvent2LCD.h"
#include "oled.h"
#include "Bomaswitch.h"
#include "VoltageDetect.h"
#include "ImgProcess.h"
#include "dirCtrl.h"

#define    MenuMax      3 				//总共有3个菜单
typedef    void    (*Menu)(void);  		//函数指针

//主要列出每个菜单的功能
typedef enum
{
    MAIN,								//主菜单
    CARSTATUS,							//小车状态
	VARVALUE,							//变量的值
}Menufunc;

//菜单类
typedef struct							
{
	Menufunc page;
}Menuinfo;

extern Menuinfo MenuInfo;
void LCDMenu_Refresh(void);

#endif
