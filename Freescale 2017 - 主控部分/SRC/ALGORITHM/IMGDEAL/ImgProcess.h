/*
 **************************************************
 *��  �ܣ����������ֽṹ�����ͣ��ֱ�Ϊ��
 *        1.ImgClassType       ͼ�������ṹ��
 *        2.ErChengClassType   ���˷����ͽṹ��
 *        3.startLinesType     ��ʼ�����ͽṹ��
 *        4.rampWayClass       �µ����ͽṹ��
 *        5.roadTypeConstClass ��·���͵�һ����
 *        6.closestLineInfo    �G��ǰ�����һ����Ч�е���Ϣ
 *        7.lineInfo           ÿ���ߵ���Ϣ�Ľṹ��
 **************************************************
 */
#ifndef _IMGPROCESS_H_
#define _IMGPROCESS_H_

#include "stdint.h"
#include "MyMath.h"
#include "camera.h"
#include "roadAnalyze.h"
#include "LedBuz.h"
#include "isr.h"

typedef struct
{
    uint8_t ImgThr;         //������ֵ
    int16_t realRoadWidth;  //������ȵ�һ��
	int16_t preRoadWidth;
    int16_t RealMiddle;     //��һ����ʼɨ�����������
    uint8_t leftLine[Image_Height];  //���������
    uint8_t RightLine[Image_Height]; //�ұ�������
	uint8_t middleLine[Image_Height];
}ImgClassType;              //ͼ�������ṹ��

typedef struct
{
    int32_t sumX, sumY;     
    double sumX2, sumXY;    //��ƽ�������ܳ���32λ����
    uint8_t effectLines;    //��Ч��
    uint8_t startLine;      //��ʼ��
    uint8_t endLine;        //��ֹ��
    uint8_t AvgEndLine;     //������˭����Զ����Ч���кŵ�ƽ��
    double  k;
    int16_t b;
    int16_t m;
    int16_t error;          //���
    double  zigma2;         //����
    double  avgLine;        
}ErChengClassType;          //���˷����ͽṹ��

typedef struct 
{
    uint8_t line;   			  //������������
    uint8_t IsStartLine;          //�Ƿ�Ϊ������
    uint8_t roadWidth;            //�������
	int32_t avgMiddle;
}startLinesType;                  //���������ͽṹ��

typedef struct 
{
    uint8_t IsUpRamp;         //����
    uint8_t IsOnRamp;         //���µ���
    uint8_t IsDownRamp;       //����
    int16_t rampConfirmCount; //�����μ�⵽���������£�����Ϊ������������µ��ϣ���ʼ�����һ���µ�����
    uint8_t rampTimeCount;    //ʱ�����
}rampWayClass;                //�µ����ͽṹ��

typedef struct //��������ʶ��ĳ�����
{
    int16_t StraightRoadJudgeWidth;      //�ж�Ϊֱ���Ŀ��
    int16_t StraightRoadJudgeMaxZigMa;   //�ж�Ϊֱ������󷽲�
    int16_t StraightRoadJudgeMinEffLines;//�ж�Ϊֱ������С��Ч����
    int16_t RampWayJudgeSeparate;        //�µ��жϷ���
    uint8_t RampWayConfirmC;             //2�μ�⵽��ĳ���µ����ͣ�����Ϊ��ĳ���µ������ˣ�������һ�����ͼ��
    uint8_t RampWayMaxTime;              //ͨ���µ������ʱ��
}roadTypeConstClass;                     //��·���͵�һ����


typedef struct 
{                        
    uint8_t line;        //�к�
    uint8_t column;      //�кţ�Ҳ��������ߵ�λ��
    uint8_t deltaLine;   //����ʱ�����������������Ĳ�
    uint8_t middle;      //����ʱ������
}closestLineInfo;        //�G��ǰ�����һ����Ч�е���Ϣ

typedef struct
{
    closestLineInfo closestLineBeforeLost;  //��ǰ�е��G��ǰ�����һ�е���Ϣ
}lineInfo;                                  //ÿ���ߵ���Ϣ�Ľṹ��

typedef struct
{
	uint8_t line;
	uint8_t jumpRow;
	uint8_t IsRoadBlock;
	uint8_t BesideBlock;
	uint8_t BlockMaxTime;
	uint8_t ThroughBlock;
	uint8_t BlockTimeCount;
	uint8_t continuousLine;
	uint8_t stateChangeCount;
	uint8_t onlyOneDetectFlag;				//ÿ��ͼ��ֻ�ж�һ�Σ���Ϊ·�϶���ֱ��
	uint8_t stateChangeCountThr;
	uint8_t avoidStartLineAffect;
	uint8_t avoidStartLineAffectThr;
}roadblock;									//·����Ϣ

extern ImgClassType ImgClass;
extern ErChengClassType leftErCheng,rightErCheng,middleErCheng; //��С���˷�

extern float   thrOffset;
extern startLinesType startLinesDetect;
extern roadblock roadLeftBlock;
extern roadblock roadRightBlock;
extern rampWayClass ramWay;

extern uint8_t leftLine[Image_Height];
extern uint8_t rightLine[Image_Height];	
extern uint8_t ImgMiddle;;
extern int32_t avgWidth ;//ƽ��������ȣ������µ����

void ImgDealInit(void);
void ImgBinarization(uint8_t widthBottom, uint8_t heightBottom, uint8_t widthTop, uint8_t heightTop);
void ImgDeal(uint8_t widthBottom, uint8_t heightBottom, uint8_t widthTop, uint8_t heightTop);
uint8_t CalculateThreshold(uint8_t* src, uint16_t widthBottom, uint16_t heightBottom, uint16_t widthTop, uint16_t heightTop);

#endif




















