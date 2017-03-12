#ifndef __BOMASWITCH_H__
#define __BOMASWITCH_H_

#include "gpio.h"
#include "common.h"
#include "KeyEvent2LCD.h"
#include "dirCtrl.h"
#include "speedCtrl.h"
#include "SDStorePara.h"

//���뿪��A
#define BomaASwitch1_PIN  6
#define BomaASwitch2_PIN  7
#define BomaASwitch3_PIN  8
#define BomaASwitch4_PIN  9

//���뿪��B
#define BomaBSwitch1_PIN  10
#define BomaBSwitch2_PIN  11
#define BomaBSwitch3_PIN  12
#define BomaBSwitch4_PIN  26

//���뿪�����ö˿�
#define BomaA_PORT  HW_GPIOE
#define BomaB_PORT  HW_GPIOE

//���뿪��A��������С��״̬
#define StatusSwitch      (((PTE->PDIR)>>6)&0x0F)

#define DEBUG_SELECT_BIT      0x01     //�Ƿ��������
#define START_LINE_BIT        0x02     //�Ƿ����������߼��
#define EMERGRNCYSTOPCAR_BIT  0x04     //�Ƿ�����ͣ
#define RAMWAY_BIT            0x08     //�Ƿ������µ����

//���뿪��B����ѡ��С�����ٶ�
#define SpeedSwitch		 (((PTE->PDIR)>>10)&0x0F)
#define SpeedSwitchB     ((((PTE->PDIR)>>26)&0x01)*8)

#define SPEED0_BIT  0x01  			  
#define SPEED1_BIT  0x02
#define SPEED2_BIT  0x04
#define SPEED3_BIT  0x08
#define SPEED0      (0)
#define SPEED1      (SPEED0_BIT)
#define SPEED2      (SPEED1_BIT)
#define SPEED3      (SPEED0_BIT|SPEED1_BIT)
#define SPEED4      (SPEED2_BIT)
#define SPEED5      (SPEED2_BIT|SPEED0_BIT)
#define SPEED6		(SPEED2_BIT|SPEED1_BIT)
#define SPEED7      (SPEED0_BIT|SPEED1_BIT|SPEED2_BIT)
#define SPEED8      (SPEED3_BIT)

#define LOW_SPEED       0            //����
#define MIDDLE_SPEED    1            //����
#define HIGH_SPEED      2            //����
#define KEY_SPEED       3            //�������ٶ�
#define ROADTYPE_SPEED  4            //�������͸��ٶ�
#define AFUZZY_SPEED    5            //A��ģ������
#define BFUZZY_SPEED    6            //B��ģ������
#define CFUZZY_SPEED    7            //C��ģ������
#define DFUZZY_SPEED    8            //D��ģ������

extern uint8_t AUTO_RUN_EN;   	   	  //�Զ�����
extern uint8_t STARTLINE_DETECT;  	  //�����߼�⣬Ĭ�Ͻ�ֹ
extern uint8_t EMERGRNCYSTOPCAR;  
extern uint8_t RAMWAY_DETECT;
extern uint8_t Sys_Speed_Mode;        //ϵͳ�ٶȣ�Ĭ�ϵ�ȡ����

void BomaSwitch_Init(void);        //���뿪�س�ʼ��
void ModeChoose(void);                //���뿪��ѡģʽ���ٶ�

#endif
