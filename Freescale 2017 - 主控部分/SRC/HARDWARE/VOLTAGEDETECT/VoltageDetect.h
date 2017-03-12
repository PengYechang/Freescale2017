#ifndef __VOLTAGEDETECT_H__
#define __VOLTAGEDETECT_H__

#include "LedBuz.h"
#include "adc.h"
#include "oled.h"
#include "speedCtrl.h"

extern uint16_t Voltage;
extern uint16_t SysVolt;
void Voltage_Init(void);    //��ѹ����ʼ��
void VoltageConvert(void);  //��ѹת��

#endif
