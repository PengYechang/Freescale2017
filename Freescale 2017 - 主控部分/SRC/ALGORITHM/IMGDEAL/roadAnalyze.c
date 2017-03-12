#include "MyMath.h"
#include "roadAnalyze.h"
#include "ImgProcess.h"

//2016.6.6�޸�
uint8_t roadType = RT_Curve;
uint8_t preRoadType = RT_Curve;

#define roadLineInMiddleDelta     60//50 //ֱ����СS����S������ж�ʱԶ���������Ķ�ͼ�����ĵ�ƫ��
#define roadLineCurveTouchDelta   25
void roadTypeAnalyze(int16_t lEndLine, int16_t rEndLine, int16_t lEffLine, int16_t rEffLine, double lK, double rK, int16_t lzigma, int16_t rzigma)
{
    int32_t farMiddleLine = 0, notFarLeftLine = 0, notFarRightLine = 0;//��Զ���������ߵ�λ��
    int32_t i, m, n;
    int32_t zigma = 500, low, uper;

    zigma = Max(lzigma, rzigma);
    low = ImgMiddle - roadLineInMiddleDelta;
    uper = ImgMiddle + roadLineInMiddleDelta;
	preRoadType = roadType;
	
// 	if(lEndLine > 130)lEndLine = 130;
// 	if(rEndLine > 130)rEndLine = 130;
  
    if (lEndLine > 120 && rEndLine > 120)//ǰհԶ��   120  120
    { //ֱ����СS����S�������
        m = MIN(lEndLine, rEndLine);
        n = 0;
        for (i = m - 5; i < m; i++)//ǰհ��Զ������£�ȡ�������ֵ֮ǰ���У�������ȫ���е�����²Ż������жϣ���farMiddleLine
        {
            if (ImgClass.leftLine[i] > 0 && ImgClass.leftLine[i] < Image_Width && ImgClass.RightLine[i] > 0 && ImgClass.RightLine[i] < Image_Width)
            {
                farMiddleLine += ((int32_t)ImgClass.leftLine[i] + (int32_t)ImgClass.RightLine[i]) / 2;
                n++;
            }
        }
        if (n > 0)
            farMiddleLine /= n;		//Զ��������
    
        if (farMiddleLine > low && farMiddleLine < uper || zigma>200)//��Զ�������ģ������㹻��
        {//ֱ����СS����S
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
    else    //ǰհ����
    { //�������������󻡣��ع����
        i = 0;
        m = lEndLine;
        while (i < 3)		//ͳ��lEndLine��ǰ���в���lEndLine����С��4��
        {
            if (ImgClass.leftLine[m] > 0 && ImgClass.leftLine[m] < Image_Width) //��Զ�����ߴ��ڣ�ѭ�����Σ�������Զ�п��������н��д���
            {
                notFarLeftLine += ImgClass.leftLine[m]; //��Ӧ�ڵ�������
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
        notFarLeftLine /= 3;  //�õ�����������
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
        notFarRightLine /= 3;	//�õ�����������
        if (notFarLeftLine > Image_Width || notFarRightLine > Image_Width)	//��������ж�����Ϊ��һ�ߵ���̫����
        {
            roadType = RT_Curve;
            return;
        }
        if (notFarRightLine < roadLineCurveTouchDelta || notFarLeftLine > Image_Width - roadLineCurveTouchDelta) //һ���ߴ����߽�
        {//��ͨ������ߴ�
            if (notFarRightLine < roadLineCurveTouchDelta && notFarLeftLine < 70)
            { //�����
                if (lEndLine > 95)//��������ߴ����߽磬��ô����߲��������ұ���  �޸�   lEndLine > 95 && rEndLine > 95
                {
                    roadType = RT_BigCircle;
                }
                else
                {
                    roadType = RT_Curve;
                }
            }
            else if (notFarRightLine > 106 && notFarLeftLine > Image_Width - roadLineCurveTouchDelta)
            { //��
                if (rEndLine > 95)//��������ߴ����߽磬��ô�ұ��߲������������  �޸�   lEndLine > 95 && rEndLine > 95
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
            farMiddleLine = (notFarRightLine + notFarLeftLine) / 2;  //���ǽ������ߣ���ôӦ����Զ����
            if (zigma < 120)
            {
                roadType = RT_StraightToCurve;
            }
            else
            {
                roadType = RT_ReturnCurve;	//�ػ��ģ���������жϺ����Ǻ�׼ȷ
            }
        }
    }//ǰհ�����ж����
}

