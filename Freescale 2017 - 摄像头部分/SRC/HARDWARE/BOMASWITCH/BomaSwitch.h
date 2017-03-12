#ifndef __BOMASWITCH_H__
#define __BOMASWITCH_H_

#include "gpio.h"
#include "common.h"
#include "KeyEvent2LCD.h"
#include "dirCtrl.h"
#include "speedCtrl.h"
#include "SDStorePara.h"

//拨码开关A
#define BomaASwitch1_PIN  6
#define BomaASwitch2_PIN  7
#define BomaASwitch3_PIN  8
#define BomaASwitch4_PIN  9

//拨码开关B
#define BomaBSwitch1_PIN  10
#define BomaBSwitch2_PIN  11
#define BomaBSwitch3_PIN  12
#define BomaBSwitch4_PIN  26

//拨码开关所用端口
#define BomaA_PORT  HW_GPIOE
#define BomaB_PORT  HW_GPIOE

//拨码开关A用来控制小车状态
#define StatusSwitch      (((PTE->PDIR)>>6)&0x0F)

#define DEBUG_SELECT_BIT      0x01     //是否允许调试
#define START_LINE_BIT        0x02     //是否允许起跑线检测
#define EMERGRNCYSTOPCAR_BIT  0x04     //是否允许急停
#define RAMWAY_BIT            0x08     //是否允许坡道检测

//拨码开关B用于选择小车的速度
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

#define LOW_SPEED       0            //低速
#define MIDDLE_SPEED    1            //中速
#define HIGH_SPEED      2            //高速
#define KEY_SPEED       3            //按键给速度
#define ROADTYPE_SPEED  4            //赛道类型给速度
#define AFUZZY_SPEED    5            //A类模糊控制
#define BFUZZY_SPEED    6            //B类模糊控制
#define CFUZZY_SPEED    7            //C类模糊控制
#define DFUZZY_SPEED    8            //D类模糊控制

extern uint8_t AUTO_RUN_EN;   	   	  //自动控制
extern uint8_t STARTLINE_DETECT;  	  //起跑线检测，默认禁止
extern uint8_t EMERGRNCYSTOPCAR;  
extern uint8_t RAMWAY_DETECT;
extern uint8_t Sys_Speed_Mode;        //系统速度，默认调取中速

void BomaSwitch_Init(void);        //拨码开关初始化
void ModeChoose(void);                //拨码开关选模式和速度

#endif
