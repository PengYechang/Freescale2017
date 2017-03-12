#ifndef __ISR_H__
#define __ISR_H__

#include "uart.h"
#include "FreeCars.h"
#include "keyboard.h"
#include "gpio.h"
#include "dma.h"
#include "camera.h"
#include "pit.h"
#include "speedCtrl.h"
#include "rtc.h"
#include "LedBuz.h"
#include "startLine.h"

void ISR_Config(void);   //所有中断的配置函数
extern bool StartLineEnable;
extern uint32_t avoidRamWayEffectTime;
extern uint32_t avoidBlockEffectTime;

extern uint16_t row;
extern uint8_t  ImgOK;
extern uint8_t  ImgDealOK;

#endif
