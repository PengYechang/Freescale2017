#ifndef __STOPCAR_H__
#define __STOPCAR_H__

#include "gpio.h"
#include "speedCtrl.h"

#define STOPCAR_PORT  HW_GPIOC
#define STARTCAR_PORT HW_GPIOC
#define STOPCAR_MODE  kGPIO_Mode_IPU
#define STARTCAR_MODE kGPIO_Mode_IPU
#define STARTCAR_PIN  2
#define STOPCAR_PIN   3
void GPIO_StopCarInit(void);
void StopCarDetect(void);

#endif
