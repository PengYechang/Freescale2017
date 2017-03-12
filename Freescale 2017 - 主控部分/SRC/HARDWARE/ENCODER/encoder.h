#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "ftm.h"
#include "lptmr.h"
#include "gpio.h"

void Encoder_Init(void);   	      //编码器初始化

//单电机
#ifdef SingleMotor   		      //单电机
extern int32_t speed;
extern uint8_t dir; 
void Encoder_GetData(void);
#endif
	
//双电机	
#ifdef DoubleMotor          	  //双电机
extern int32_t speedLeft;
extern uint8_t dirLeft;
extern int32_t speedRight;
extern uint8_t dirRight;
void EncoderLeft_GetData(void);
void EncoderRight_GetData(void);

#endif

//绝对值，正弦，余弦函数
#define  ABS(x)  ( (x)>0?(x):-(x) ) 
#define  SIN(n)    (sin((float)(n)*3.141592/180) )
#define  COS(n)    (cos((float)(n)*3.141592/180) )
	

//位置式定义常量
#define   SPEED_CONTROL_PERIOD                    5
#define   MOTOR_ENCODE_CONSTANT_RIGHT            256  //        //电机的刻槽数量
#define   MOTOR_ENCODE_CONSTANT_LEFT             256//
#define   MOTOR_SPEED_CONSTANT_RIGHT              1000.0/SPEED_CONTROL_PERIOD/MOTOR_ENCODE_CONSTANT_RIGHT //每秒转动圈数
#define   MOTOR_SPEED_CONSTANT_LEFT               1000.0/SPEED_CONTROL_PERIOD/MOTOR_ENCODE_CONSTANT_LEFT
#define   MOTOR_ANGLE_CONSTANT_LEFT               360.0/MOTOR_ENCODE_CONSTANT_LEFT   //每个脉冲对应的角度
#define   MOTOR_ANGLE_CONSTANT_RIGHT              360.0/MOTOR_ENCODE_CONSTANT_RIGHT
#define   Pai                                     3.1416
#define   SPEED_INTEGRAL_MAX                      17000

#define   MOTOR_OUT_DEAD_VAL                      1000         //死区
#define   MOTOR_OUT_MAX                           17000
#define   MOTOR_OUT_MIN							             -17000
//位置式定义结构体  速度
typedef struct
{
    float     Kp;
    float     Ki;
    float     Kd;   
    float     SpeedCtrOut;
    float     SpeedCtrIntegral;
    float     Speed;
    float     PreSpeed;
    float     SpeedSet;   //设定速度
	uint8_t        SpeedConflag;
    uint8_t        SpeedCtrPeriod;
	int16_t        Cnt;
}SpeedClass;
//位置式定义结构体   角度

extern SpeedClass LeftSpeedCtr;   
extern SpeedClass RightSpeedCtr;

void Left_SpeedControl(void);
void Right_SpeedControl(void);
void PID_Init(void);

#endif
