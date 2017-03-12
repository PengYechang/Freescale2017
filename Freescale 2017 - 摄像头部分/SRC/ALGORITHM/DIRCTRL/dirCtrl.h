#ifndef __DIRCTRL_H__
#define __DIRCTRL_H__

#include "ftm.h"
#include "speedCtrl.h"
#include "ImgProcess.h"
#include "FreeCars.h"
#include "servo.h"
#include "Bomaswitch.h"
#include "KeyEvent2LCD.h"

typedef struct
{
    long   SetVal;
    long   Input;
    double Ki;
    double Kk;
    double Kp;
    double Kd;
    double D;
    long   i;
    long   Error;
    long   Error1;
    long   Error2;
    double PWM;
    double Output;
    long   LowerLimit;
    long   UperLimit;
}DirPIDType;

extern DirPIDType DirPID;

//二次函数P参数
extern double DirStatus;
extern double Servo_P;
extern double Servo_D;

//平均行算法参数
extern uint8_t MinAvgLinePreSee;
extern uint8_t MaxAvgLinePreSee;

//当行算法参数
extern  int16_t MinSinglePreSee;
extern  int16_t MaxSinglePreSee;
extern  int16_t useDynamicPreSeeMinZigma;
extern  int16_t useDynamicPreSeeMaxZigma;
extern  double MinKp;
extern 	double MaxKp;
extern  double CutOffset;
extern int16_t UseLines;
extern int16_t singleCloseLine;
extern double blockErrorAmpFactor;

extern uint32_t BlockMaskTime;

void DirPID_Init(void);
void DirControl(void);
void LoadDirValue(void);

#endif
