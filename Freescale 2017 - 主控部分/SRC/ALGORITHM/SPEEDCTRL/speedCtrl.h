#ifndef __SPEEDCTRL_H__
#define __SPEEDCTRL_H__

#include "fuzzySpeed.h"
#include "ImgProcess.h"
#include "encoder.h"
#include "motor.h"
#include "Bomaswitch.h"
#include "FreeCars.h"
#include "roadAnalyze.h"
#include "KeyEvent2LCD.h"
#include "wdog.h"

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
}SpeedPIDType;

extern bool StopCar;
extern SpeedPIDType SpeedPID;
extern int32_t SysRelativeSpeed;
extern bool Motor_Run_Enable;
extern long MinSpeed;
extern long MaxSpeed;
extern long RampSpeed;
extern long BlockSpeed;	
extern long StraightToCurveSpeed;
extern uint32_t ramWaySpeedLimitDelay;

void SpeedPID_Init(void);
void SpeedPID_Cal(void);
void SpeedControl(void);
void LoadSpeedValue(void);

#endif
