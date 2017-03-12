#ifndef _roadAnalyze_h_
#define _roadAnalyze_h_
#include "common.h"

#define RT_Straight         1     //ֱ��
#define RT_SmallS           2     //СS
#define RT_MiddleS          3     //�е�S������һ�ߴ���ȥ
#define RT_CurveToStraight  4     //�����
#define RT_StraightToCurve  5     //ֱ�������
#define RT_BigCircle        6     //��
#define RT_Curve            7     //���
#define RT_ReturnCurve      8     //�ع����

void roadTypeAnalyze(int16_t lEndLine, int16_t rEndLine, int16_t lEffLine, int16_t rEffLine, double lK, double rK, int16_t lzigma, int16_t rzigma);

extern uint8_t roadType;
extern uint8_t preRoadType;

#endif
