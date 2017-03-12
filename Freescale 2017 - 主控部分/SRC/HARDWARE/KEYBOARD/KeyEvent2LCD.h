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

#define PAGE_VAR  	8	      										//һҳ��Ӧ�ı�����
#define PAGE_NUM  	3   	   										//ҳ����������3����ҳ
#define PAGE_MAX	3												//�ܵ�ҳ������3ҳ ����ҳ�ַ�Ϊ3����ҳ
#define VAR_NUM  	(PAGE_VAR*PAGE_NUM) 							//�ܵı�����
#define STATUS_NUM  5												//״̬������

//count  0-23 ��Ӧ24������
#define	VAR_VALUE(count)	    VarInfo[count].VAL				//count��Ӧ�ı�����ֵ
#define	VAR_OLDVALUE(count)	    VarInfo[count].OLDVAL			//count��Ӧ�ı��������ȷ��ֵ
#define	VAR_MAXVALUE(count)	    VarInfo[count].MAXVAL   
#define	VAR_MINVALUE(count)	    VarInfo[count].MINVAL
#define	VAR_SITEX(count)	    VarInfo[count].SITEX
#define VAR_SITEY(count)        VarInfo[count].SITEY
#define VAR_NAME(count)         VarInfo[count].NAME

#define	VAR_SELECT_HOLD_OFFSET		((VAR_NUM+4-1)/4)		   //���Ͽ���ʱ��ÿ���л���ƫ����
#define	VAR_VALUE_HOLD_OFFSET	     10						   //��ӿ��ʱ��ÿ�εļӼ���ƫ����

typedef void (*keyevent_t) (void);							   //���庯��ָ��

//�������¼���Ӧ����Ӧ�ı����¼�
typedef enum
{
	VAR_PREV,									//��һ��
	VAR_NEXT,									//��һ��
	VAR_NEXT_HOLD,		      					//���£�ƫ��Ϊ:VAR_SELECT_HOLD_OFFSET
	VAR_PREV_HOLD,								//���ϣ�ƫ��Ϊ:VAR_SELECT_HOLD_OFFSET
	VAR_ADD,									//��1
	VAR_SUB,									//��1
	VAR_ADD_HOLD,								//��ӣ�ƫ��Ϊ:VAR_VALUE_HOLE_OFFSET
	VAR_SUB_HOLD,								//�����ƫ��Ϊ:VAR_VALUE_HOLE_OFFSET  
	VAR_OK=KEY_OK,								//ȷ��
	VAR_CANCEL=KEY_CANCLE,						//ȡ��
	VAR_EVENT_MAX,								//����������
}VAR_EVENT;

//������������Ϣ
typedef struct
{
	int32_t 		VAL;						//Ŀǰ��ֵ  
	int32_t 		OLDVAL;						//��ǰ��ֵ
	int32_t		MINVAL;							//��Сֵ
	int32_t		MAXVAL;							//���ֵ
	uint8_t  	SITEX;      					//����������  X
	uint8_t      SITEY;	    				    //����������  Y
	const char*   NAME;							//������
}VAR_INFO;										//������Ϣ

//ҳ�¼���
typedef struct
{
  int16_t   Page;							//����ָʾ��ǰҳ
  bool 		Change;							//ָʾҳ�Ƿ�ı�
}PAGEEVENT;

extern PAGEEVENT       PageEvent;			//ҳ���¼�
extern keyevent_t KeyEvent[PAGE_NUM];		//�����¼�

extern bool PrintfZero_Enable;
extern bool StartRun;

void VarPageDisplay(void);					//������ʾ
void LoadVarValue(void);
#endif
