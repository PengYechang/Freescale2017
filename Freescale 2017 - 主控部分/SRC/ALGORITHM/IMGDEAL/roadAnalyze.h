#ifndef _roadAnalyze_h_
#define _roadAnalyze_h_
#include "common.h"

#define RT_Straight         1     //直道
#define RT_SmallS           2     //小S
#define RT_MiddleS          3     //中等S，不能一线闯过去
#define RT_CurveToStraight  4     //出弯道
#define RT_StraightToCurve  5     //直道入弯道
#define RT_BigCircle        6     //大弧
#define RT_Curve            7     //弯道
#define RT_ReturnCurve      8     //回拐弯道

void roadTypeAnalyze(int16_t lEndLine, int16_t rEndLine, int16_t lEffLine, int16_t rEffLine, double lK, double rK, int16_t lzigma, int16_t rzigma);

extern uint8_t roadType;
extern uint8_t preRoadType;

#endif
