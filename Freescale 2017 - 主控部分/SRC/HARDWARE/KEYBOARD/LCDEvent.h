#ifndef __LCDEVENT_H__
#define __LCDEVENT_H__

#include "KeyEvent2LCD.h"
#include "oled.h"
#include "Bomaswitch.h"
#include "VoltageDetect.h"
#include "ImgProcess.h"
#include "dirCtrl.h"

#define    MenuMax      3 				//�ܹ���3���˵�
typedef    void    (*Menu)(void);  		//����ָ��

//��Ҫ�г�ÿ���˵��Ĺ���
typedef enum
{
    MAIN,								//���˵�
    CARSTATUS,							//С��״̬
	VARVALUE,							//������ֵ
}Menufunc;

//�˵���
typedef struct							
{
	Menufunc page;
}Menuinfo;

extern Menuinfo MenuInfo;
void LCDMenu_Refresh(void);

#endif
