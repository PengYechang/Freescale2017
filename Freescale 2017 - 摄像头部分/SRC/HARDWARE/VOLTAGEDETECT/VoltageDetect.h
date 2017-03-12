#ifndef __VOLTAGEDETECT_H__
#define __VOLTAGEDETECT_H__

#include "LedBuz.h"
#include "adc.h"
#include "oled.h"
#include "speedCtrl.h"

extern uint16_t Voltage;
extern uint16_t SysVolt;
void Voltage_Init(void);    //电压检测初始化
void VoltageConvert(void);  //电压转换

#endif
