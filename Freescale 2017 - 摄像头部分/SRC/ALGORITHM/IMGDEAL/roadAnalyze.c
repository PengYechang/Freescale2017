#include "MyMath.h"
#include "roadAnalyze.h"
#include "ImgProcess.h"

//2016.6.6修改
uint8_t roadType = RT_Curve;
uint8_t preRoadType = RT_Curve;

#define roadLineInMiddleDelta     60//50 //直道，小S，中S出弯道判断时远处赛道中心对图像中心的偏离
#define roadLineCurveTouchDelta   25
void roadTypeAnalyze(int16_t lEndLine, int16_t rEndLine, int16_t lEffLine, int16_t rEffLine, double lK, double rK, int16_t lzigma, int16_t rzigma)
{
    int32_t farMiddleLine = 0, notFarLeftLine = 0, notFarRightLine = 0;//最远处的中心线的位置
    int32_t i, m, n;
    int32_t zigma = 500, low, uper;

    zigma = Max(lzigma, rzigma);
    low = ImgMiddle - roadLineInMiddleDelta;
    uper = ImgMiddle + roadLineInMiddleDelta;
	preRoadType = roadType;
	
// 	if(lEndLine > 130)lEndLine = 130;
// 	if(rEndLine > 130)rEndLine = 130;
  
    if (lEndLine > 120 && rEndLine > 120)//前瞻远的   120  120
    { //直道，小S，中S，出弯道
        m = MIN(lEndLine, rEndLine);
        n = 0;
        for (i = m - 5; i < m; i++)//前瞻较远的情况下，取最大行数值之前五行，左右线全都有的情况下才会见这个判断，算farMiddleLine
        {
            if (ImgClass.leftLine[i] > 0 && ImgClass.leftLine[i] < Image_Width && ImgClass.RightLine[i] > 0 && ImgClass.RightLine[i] < Image_Width)
            {
                farMiddleLine += ((int32_t)ImgClass.leftLine[i] + (int32_t)ImgClass.RightLine[i]) / 2;
                n++;
            }
        }
        if (n > 0)
            farMiddleLine /= n;		//远处的中线
    
        if (farMiddleLine > low && farMiddleLine < uper || zigma>200)//最远处在中心，车子足够正
        {//直道，小S，中S
            if (zigma < 40)
                roadType = RT_Straight;
            else if (zigma < 150)
                roadType = RT_SmallS;
            else
                roadType = RT_MiddleS;
        }
        else
        {
            roadType = RT_CurveToStraight;
        }
    }
    else    //前瞻近的
    { //弯道，入弯道，大弧，回拐弯道
        i = 0;
        m = lEndLine;
        while (i < 3)		//统计lEndLine的前三行并且lEndLine不能小于4行
        {
            if (ImgClass.leftLine[m] > 0 && ImgClass.leftLine[m] < Image_Width) //最远行左线存在，循环三次，处理最远行靠近的三行进行处理
            {
                notFarLeftLine += ImgClass.leftLine[m]; //对应黑点所在行
                i++;
            }
            if (m < 2)
            {
                notFarLeftLine = 1000;
                break;
            }
            else
			{
                m--;
			}
        }
        notFarLeftLine /= 3;  //得到近处的左线
        i = 0;
        m = rEndLine;
        while (i < 3)
        {
            if(ImgClass.RightLine[m] > 0 && ImgClass.RightLine[m] < Image_Width)
            {
                notFarRightLine += ImgClass.RightLine[m];
                i++;
            }
            if (m < 2)
            {
                notFarRightLine = 1000;
                break;
            }
            else
                m--;
        }
        notFarRightLine /= 3;	//得到近处的右线
        if (notFarLeftLine > Image_Width || notFarRightLine > Image_Width)	//进入这个判断是因为有一边的线太少了
        {
            roadType = RT_Curve;
            return;
        }
        if (notFarRightLine < roadLineCurveTouchDelta || notFarLeftLine > Image_Width - roadLineCurveTouchDelta) //一边线触及边界
        {//普通弯道或者大弧
            if (notFarRightLine < roadLineCurveTouchDelta && notFarLeftLine < 70)
            { //右弯道
                if (lEndLine > 95)//右弯道右线触及边界，那么左边线不会少于右边线  修改   lEndLine > 95 && rEndLine > 95
                {
                    roadType = RT_BigCircle;
                }
                else
                {
                    roadType = RT_Curve;
                }
            }
            else if (notFarRightLine > 106 && notFarLeftLine > Image_Width - roadLineCurveTouchDelta)
            { //左
                if (rEndLine > 95)//左弯道左线触及边界，那么右边线不会少于左边线  修改   lEndLine > 95 && rEndLine > 95
                {
                    roadType = RT_BigCircle;
                }
                else
                {
                    roadType = RT_Curve;
                }
            }
        }
        else
        {
            farMiddleLine = (notFarRightLine + notFarLeftLine) / 2;  //不是近处的线，那么应该是远处的
            if (zigma < 120)
            {
                roadType = RT_StraightToCurve;
            }
            else
            {
                roadType = RT_ReturnCurve;	//回环的，但是这个判断好像不是很准确
            }
        }
    }//前瞻近的判断完毕
}

