#ifndef _PID_H_
#define _PID_H_

#include "motor.h"
#include "ImgProcess.h"
#include "encoder.h"

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
}PIDLongType;

typedef struct
{
  double   Ki;
  double   Kp;
  double   Kd;
  double   Error;
  double   Error1;
  double   Error2;
  double   PWM;
  double   Output;
  double   SetVal;
  double   Input;
  double   LowerLimit;
  double   UperLimit;
}PIDDoubleType;
    
extern double MotorOutSpeedNew;
extern PIDLongType   Motor1PID;
extern double Motor_Speed;
    
void DirPID_Init(void);
void DirPIDCalculate(void);
void Motor_SpeedPID_Init(void);
void Motor_SpeedPID_Cal(void);		//ÔöÁ¿Ê½PID
void MotorOut(void);



#endif
