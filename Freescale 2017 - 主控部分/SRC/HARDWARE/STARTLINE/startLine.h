#ifndef _STARTLINE_H_
#define _STARTLINE_H_

#include "gpio.h"
#include "rtc.h"
#include "LedBuz.h"
#include "speedCtrl.h"
#include "roadAnalyze.h"


#define ST188_PIN1	14
#define ST188_PIN2	15
#define LeftST1188OFF 	PCin(ST188_PIN1) = 0
#define RightST188OFF	PCin(ST188_PIN2) = 0


void startLineInit(void);

#endif
