/*
 **************************************************
 *功  能：定义了七种结构体类型，分别为：
 *        1.ImgClassType       图像处理常量结构体
 *        2.ErChengClassType   二乘法类型结构体
 *        3.startLinesType     起始线类型结构体
 *        4.rampWayClass       坡道类型结构体
 *        5.roadTypeConstClass 道路类型的一个类
 *        6.closestLineInfo    丟线前最近的一个有效行的信息
 *        7.lineInfo           每条线的信息的结构体
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
    uint8_t ImgThr;         //近景阈值
    int16_t realRoadWidth;  //赛道宽度的一半
	int16_t preRoadWidth;
    int16_t RealMiddle;     //下一行起始扫描的赛道中心
    uint8_t leftLine[Image_Height];  //左边线数组
    uint8_t RightLine[Image_Height]; //右边线数组
	uint8_t middleLine[Image_Height];
}ImgClassType;              //图像处理常量结构体

typedef struct
{
    int32_t sumX, sumY;     
    double sumX2, sumXY;    //有平方，可能超过32位长度
    uint8_t effectLines;    //有效线
    uint8_t startLine;      //起始线
    uint8_t endLine;        //终止线
    uint8_t AvgEndLine;     //左右线谁看的远，有效行行号的平均
    double  k;
    int16_t b;
    int16_t m;
    int16_t error;          //误差
    double  zigma2;         //方差
    double  avgLine;        
}ErChengClassType;          //二乘法类型结构体

typedef struct 
{
    uint8_t line;   			  //起跑线所在行
    uint8_t IsStartLine;          //是否为起跑线
    uint8_t roadWidth;            //赛道宽度
	int32_t avgMiddle;
}startLinesType;                  //起跑线类型结构体

typedef struct 
{
    uint8_t IsUpRamp;         //上坡
    uint8_t IsOnRamp;         //在坡道上
    uint8_t IsDownRamp;       //下坡
    int16_t rampConfirmCount; //如果多次检测到是上中下坡，才认为是在这个类型坡道上，开始检测下一个坡道类型
    uint8_t rampTimeCount;    //时间计数
}rampWayClass;                //坡道类型结构体

typedef struct //定义赛道识别的常量类
{
    int16_t StraightRoadJudgeWidth;      //判断为直道的宽度
    int16_t StraightRoadJudgeMaxZigMa;   //判断为直道的最大方差
    int16_t StraightRoadJudgeMinEffLines;//判断为直道的最小有效行数
    int16_t RampWayJudgeSeparate;        //坡道判断分离
    uint8_t RampWayConfirmC;             //2次检测到是某种坡道类型，就认为是某种坡道类型了，进行下一个类型检测
    uint8_t RampWayMaxTime;              //通过坡道的最大时间
}roadTypeConstClass;                     //道路类型的一个类


typedef struct 
{                        
    uint8_t line;        //行号
    uint8_t column;      //列号，也就是这个线的位置
    uint8_t deltaLine;   //丢线时的行数到现在行数的差
    uint8_t middle;      //丢线时的中线
}closestLineInfo;        //丟线前最近的一个有效行的信息

typedef struct
{
    closestLineInfo closestLineBeforeLost;  //当前行到丟线前最近的一行的信息
}lineInfo;                                  //每条线的信息的结构体

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
	uint8_t onlyOneDetectFlag;				//每幅图像只判断一次，因为路障都在直道
	uint8_t stateChangeCountThr;
	uint8_t avoidStartLineAffect;
	uint8_t avoidStartLineAffectThr;
}roadblock;									//路障信息

extern ImgClassType ImgClass;
extern ErChengClassType leftErCheng,rightErCheng,middleErCheng; //最小二乘法

extern float   thrOffset;
extern startLinesType startLinesDetect;
extern roadblock roadLeftBlock;
extern roadblock roadRightBlock;
extern rampWayClass ramWay;

extern uint8_t leftLine[Image_Height];
extern uint8_t rightLine[Image_Height];	
extern uint8_t ImgMiddle;;
extern int32_t avgWidth ;//平均赛道宽度，用在坡道检测

void ImgDealInit(void);
void ImgBinarization(uint8_t widthBottom, uint8_t heightBottom, uint8_t widthTop, uint8_t heightTop);
void ImgDeal(uint8_t widthBottom, uint8_t heightBottom, uint8_t widthTop, uint8_t heightTop);
uint8_t CalculateThreshold(uint8_t* src, uint16_t widthBottom, uint16_t heightBottom, uint16_t widthTop, uint16_t heightTop);

#endif




















