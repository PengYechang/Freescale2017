#include "ImgProcess.h"

#define  directSearchLines  10                   //������ǽ����ֽ���

float   thrOffset = 0.8;   //0.8;               //��ֵ������
uint8_t ImgMiddle = Image_Width/2-1;          //ͼ���м�
uint8_t value = Image_Height-1;				  //
int32_t avgWidth = 0;
const int32_t FarthestDis = 120 * 120;        //14400����Զ����ʱ���⿪�����㣬��ôԶ֮����������G��MaxLostJudge�ξ���Ϊ���������Ѿ���������������
uint8_t PrePreSee = 125;					  //ǰһ��ͼ���ǰհ
static uint8_t onlyOneDetect = 0;

/*------------------------------------------------
  ������������:
  �������飬��¼������ȵ�����
  �������飬��������
 ------------------------------------------------*/
uint8_t middleLine[Image_Height];                   //����
uint8_t recordRoadWidth[Image_Height]; 				//��¼�������
uint8_t leftLine[Image_Height]; 					//���������
uint8_t rightLine[Image_Height];					//�ұ�������
uint8_t leftJumpLine[23][2];						//��¼���������
uint8_t rightJumpLine[23][2];						//��¼���������
/*-----------------------------------------------
 ���������ṹ�壺
 �������ͽṹ��
 ͼ����ṹ��
 ���˷��ṹ��
 ��ʼ�߽ṹ��
 -----------------------------------------------*/
roadTypeConstClass roadTypeConst;                                           //��������
rampWayClass ramWay;													    //�µ�
ImgClassType ImgClass;                                                      //ͼ����Ϣ����
ErChengClassType leftErCheng, rightErCheng, middleErCheng;                  //��С���˷�
startLinesType startLinesDetect;                                            //������
roadblock roadLeftBlock;													//��·��
roadblock roadRightBlock;													//��·��

//����ֵ������Thr
 uint8_t CalculateThreshold(uint8_t* src, uint16_t widthBottom, uint16_t heightBottom, uint16_t widthTop, uint16_t heightTop)
{
    uint8_t* ImgBaseAddr = src;     //��ȡͼ�񴢴�ĵ�ַ
    uint8_t* pixel = ImgBaseAddr;   //����ָ��
    uint16_t PixelNum[256]={0};     //ֱ��ͼ����ͳ�Ʊ�;��������
    int16_t Thr = 100;              //��ֵ��ʼ��
    int16_t line, column;          

    int32_t p;
    int32_t total;                          	//ֱ��ͼƽ����ʱ�����Ҽ������ۺ�
    int32_t TotalWeight = 0, ForeWeight = 0;    //������
    int32_t TotalPixels = 0;                    //����ͳ�Ƶĵ�����

    int32_t BackPixNum = 0, ForePixNum = 0;     //ǰ��ͼ��ͱ���ͼ��ĵ���
    double BackAvgGry, ForeAvgGry;              //ƽ���Ҷ�ֵ
    double fmax = -1;
    double sb;

    for(line = heightBottom; line < heightTop; line++)     //ͼ��ֱ��ͼ
    {
        pixel = ImgBaseAddr + line * Image_Width;          //��������ָ�룬��ֵΪÿ��ͼ�����߽�ĵ�ַ
        for(column = widthBottom; column < widthTop; column += 4)
        {
            PixelNum[*(pixel + column)]++;
        }
        line += 3;
    }
  
    for(line = 0; line < 256; line++)
    {
        total = 0;
        for(column = -2; column < 3; column++) //���ҹ�5����ƽ��
        {
            p = line + column;
            if (p < 0) p = 0;
            else
            if (p > 255) p = 255;
            total += PixelNum[p];
        }
//        PixelNum[line] = (uint8_t)((float)total / 5.0 + 0.5);
        PixelNum[line] = (uint8_t)((float)total / 5 + 0.5);
    }
  
    for(line = 0; line < 256; line++)
    {
        TotalWeight += line * PixelNum[line];             //x*f(x)�����أ�Ҳ����ÿ���Ҷȵ�ֵ�������������һ����Ϊ���ʣ���TotalWeightΪ���ܺ�
        TotalPixels += PixelNum[line];                    //totalΪͼ���ܵĵ�������һ��������ۻ�����
    }
  
    for(line = 0; line < 256; line++)
    {
        ForePixNum += PixelNum[line];           //ForePixNumΪ�ڵ�ǰ��ֵ��ǰ��ͼ��ĵ���
        if (ForePixNum == 0) continue;          //û�зֳ�ǰ����
        BackPixNum = TotalPixels - ForePixNum;  //BackPixNumΪ����ͼ��ĵ���
        if (BackPixNum == 0) break;             //BackPixNumΪ0��ʾȫ�����Ǻ�ͼ����ForePixNum=0������ƣ�֮��ı���������ʹǰ���������ӣ����Դ�ʱ�����˳�ѭ��
        ForeWeight += line * PixelNum[line];    //ǰ��ͼ��������
        ForeAvgGry = ((double)(ForeWeight)) / ForePixNum;   //ǰ��ͼ��ƽ���Ҷ�
        BackAvgGry = ((double)(TotalWeight - ForeWeight)) / BackPixNum; //����ͼ��ƽ���Ҷ�
        sb = ForePixNum * BackPixNum * (ForeAvgGry - BackAvgGry) * (ForeAvgGry - BackAvgGry);
        if (sb > fmax)
        {
            fmax = sb;
            Thr = line;
        }
    }
    return (uint8_t)Thr;
}

//��Image_Deal������ͼ����ж�ֵ������������ͼ�����Image_Deal��
void ImgBinarization(uint8_t widthBottom, uint8_t heightBottom, uint8_t widthTop, uint8_t heightTop)    //ͼ���ֵ��
{
    uint8_t threshold = CalculateThreshold(Image_Deal, widthBottom, heightBottom, widthTop, heightTop);  //������ֵ
    uint16_t i, j;
    uint16_t offset;
    uint8_t  value;
    
    for(i = 0; i < Image_Height; i++)
    {
        offset = i*Image_Width;
        for(j = 0;j < Image_Width; j++)
        {
            value = Image_Deal[offset + j];
            if(value < threshold) Image_Deal[offset + j] = 0;
            else Image_Deal[offset + j] = 255;
        }
    }
    j=0;
}

//����ͼ�����һЩ����
void ImgDealInit(void)
{  
    roadTypeConst.StraightRoadJudgeWidth = 28;//26;
    roadTypeConst.StraightRoadJudgeMaxZigMa = 25;     //�ж�Ϊֱ������󷽲�
    roadTypeConst.StraightRoadJudgeMinEffLines = 135; //�ж�Ϊֱ������С��Ч����
    roadTypeConst.RampWayJudgeSeparate = 4;
    roadTypeConst.RampWayConfirmC = 2;                //2�μ�⵽��ĳ���µ����ͣ�����Ϊ��ĳ���µ������ˣ�������һ�����ͼ��
    roadTypeConst.RampWayMaxTime = 150;
	
	//�ϰ��Ĳ�����ʼ��
	roadLeftBlock.line = 0;						 //��������ڵ���
	roadLeftBlock.jumpRow = 0;					 //�����֮ǰ����
	roadLeftBlock.continuousLine = 0;			 //�Ƿ�����������
	roadLeftBlock.avoidStartLineAffectThr = 3;  //�ж���ֵ
	
	roadRightBlock.line = 0;					 //��������ڵ���
	roadRightBlock.jumpRow = 0;					 //�����֮ǰ����
	roadRightBlock.continuousLine = 0;			 //�Ƿ�����������
	roadRightBlock.avoidStartLineAffectThr = 3; //�ж���ֵ
	
	roadLeftBlock.BlockMaxTime = 100;			 //���ϰ�ʱ�������ʱ�����ȫ����0
	roadLeftBlock.stateChangeCount = 0;			 //״̬ת������
	roadLeftBlock.stateChangeCountThr = 3;       //״̬ת����ֵ
	roadLeftBlock.BlockTimeCount = 0;
	
	roadLeftBlock.IsRoadBlock = false;				 //�Ƿ���·��
	roadLeftBlock.BesideBlock = false;
	roadLeftBlock.ThroughBlock = false;
	
	roadRightBlock.IsRoadBlock = false;				 //�Ƿ���·��
	roadRightBlock.BesideBlock = false;
	roadRightBlock.ThroughBlock = false;
	
	ramWay.IsUpRamp = false;
	ramWay.IsOnRamp = false;
	ramWay.IsDownRamp = false;
	ramWay.rampConfirmCount = 0;    //�����μ�⵽���������ƣ�����Ϊ������������µ��ϣ���ʼ�����һ���µ�����
}

////���������߻������ߣ�����Ķ��������Ҳ�ĽǶ�
int16_t kkkMinus(int32_t x1, int32_t x2, int32_t x3, int32_t y1, int32_t y2, int32_t y3)//x��line��y��column
{
    float k1, k2, kk;
    int32_t Theta;

    if (y1 > (Image_Width-1) || y1 == 0 || y2 > (Image_Width-1) || y2 == 0 || y3 > (Image_Width-1) || y3 == 0|| MyAbs(y1 - y2) < 2 && MyAbs(y2 - y3) < 2)
        return 1000;
    k1 = ((float)(y2 - y1)) / (x2 - x1);
    k2 = ((float)(y3 - y2)) / (x3 - x2);
    kk = 1 + k1 * k2;
    if (kk != 0)
    {
        Theta = FastAtan((float)((k2 - k1) / kk)); 
        if (Theta < 0) Theta += 180;
    }
    else
    {
        Theta = 90;
    }
    if (k2 < k1) //0<theta<180 
    {

    }
    else // 180<= theta <360
    {
            Theta += 180;
    }
    return (int16_t)(360 - Theta);
}

//����Ϊ��С���˷��Ĳ���k��b�ļ���
double ErChengK(uint8_t EffectLines, int32_t sumX, double sumX2, double sumXY, int32_t sumY)
{
    double c, d;
    c = sumX * sumY - EffectLines * sumXY;
    d = sumX * sumX - EffectLines * sumX2;
    c = c / d;
    return c;
}

int16_t ErChengB(uint8_t EffectLines, int32_t sumX, double sumX2, double sumXY, int32_t sumY)
{
    double c, d;
    c = sumX2 * sumY - sumX * sumXY;
    d = EffectLines * sumX2 - sumX * sumX;
    c = c / d;
    return (int16_t)c;
}

//�������
void leftBuLine(uint8_t bottom, uint8_t top)
{
    uint8_t nonEffLineFound;      //û���ҵ���Ӱ�����
    uint8_t startLine, endLine, startV, endV, emptyLlines;
    int16_t i, j, c;
    uint8_t p;
    double k;
    nonEffLineFound = false;
    startLine = 255;
    startV = 255;
    emptyLlines = 1;
    for (i = bottom; i < top; i++)
    {
        p = ImgClass.leftLine[i];  
        if (p > 0 && p < Image_Width)
        {  
            if (nonEffLineFound == false)
            {
                startLine = (uint8_t)i;
                startV = p;
            }
            else//����û�в����ߣ����������ҵ����յ�
            {
                endLine = (uint8_t)i;
                endV = p;

                c = 0;
                j = (int16_t)(startLine - 2);
                while (j > bottom)
                {
                    c++;
                    if (ImgClass.leftLine[j] > 0 && ImgClass.leftLine[j] < Image_Width)
                        startV = (uint8_t)((startV + ImgClass.leftLine[j]) / 2);
                    if (c > 5) break;
                }
        
                c = 0;
                j = (int16_t)(endLine + 2);
                while (j <top)
                {
                    c++;
                    if (ImgClass.leftLine[j] > 0 && ImgClass.leftLine[j] < Image_Width)
                        endV = (uint8_t)((endV + ImgClass.leftLine[j]) / 2);
                    if (c > 5) break;
                }

                k = ((double)(endV - startV)) / (endLine - startLine);
                for (j = 1; j < emptyLlines; j++)
                {
                    ImgClass.leftLine[startLine + j] = (uint8_t)(k * j + startV);
                }
                startLine = endLine;//�������ã���ʼ�µ�һ�β���   
                endV = startV;
                nonEffLineFound = false;
                emptyLlines = 1;
            }
        }
        else
        {
            if (startLine != 255)
            {
                nonEffLineFound = true;
                emptyLlines++;
            }
          
        }
    }
}

//���ұ���
void rightBuLine(uint8_t bottom, uint8_t top)
{
    uint8_t nonEffLineFound;
    uint8_t startLine, endLine, startV, endV, emptyLlines;
    int16_t i, j,c;
    uint8_t p;
    double k;
    nonEffLineFound = false;
    startLine = 255;
    startV = 255;
    emptyLlines = 1;
    for (i = bottom; i < top; i++)
    {
        p = ImgClass.RightLine[i];
        if (p > 0 && p < Image_Width)
        {
            if (nonEffLineFound == false)
            {
                startLine = (uint8_t)i;
                startV = p;
            }
            else//����û�в����ߣ����������ҵ����յ�
            {
                endLine = (uint8_t)i;
                endV = p;
                c = 0;
                j = (int16_t)(startLine - 2);
                while (j > bottom)
                {
                    c++;
                    if (ImgClass.RightLine[j] > 0 && ImgClass.RightLine[j] < Image_Width)
                        startV = (uint8_t)((startV + ImgClass.RightLine[j]) / 2);
                    if (c > 5) break;
                }
                c = 0;
                j = (int16_t)(endLine + 2);
                while (j < top)
                {
                    c++;
                    if (ImgClass.RightLine[j] > 0 && ImgClass.RightLine[j] < Image_Width)
                        endV = (uint8_t)((endV + ImgClass.RightLine[j]) / 2);
                    if (c > 5) break;
                }
                k = ((double)(endV - startV)) / (endLine - startLine);
                for (j = 1; j < emptyLlines; j++)
                {
                    ImgClass.RightLine[startLine + j] = (uint8_t)(k * j + startV);
                }
                startLine = endLine;//�������ã���ʼ�µ�һ�β���
                endV = startV;
                nonEffLineFound = false;
                emptyLlines = 1;
            }
        }
        else
        {
            if (startLine != 255)
            {
                nonEffLineFound = true;
                emptyLlines++;
            }
        }
    }
}


void ImgDeal(uint8_t widthBottom, uint8_t heightBottom, uint8_t widthTop, uint8_t heightTop)
{
	//���ڱ��ߵ�����
    uint8_t* ImgBaseAddr = Image_Deal;                //ͼ���������ڵĻ���ַ
    uint8_t* pixel = ImgBaseAddr;                     //��̬����ָ��
    uint8_t* pixelBegin;							  //һ�е�������ʼλ��ָ��
    uint8_t* pixelEnd;                                //һ�е����ؽ���λ��ָ��
	uint8_t i = 0;
	
	//���ڼ�������ĽǶ�  3����Ƕ�
	uint8_t ThetaDeltaX = 7;                          //���ڼ�������ĽǶ�   
    uint8_t ThetaDelta2X = (uint8_t)(ThetaDeltaX * 2);//���ڼ�������ĽǶ�
	int16_t ThetaL,ThetaR;                            //�����߽Ƕ�

	//���ڼ��Զ���µ���
	uint8_t minIsland = 8;                            
    uint8_t minBlank = 8;
    int16_t Eff = 0, InEff = 0, Bottom = 50;           
	
	//�����жϼ�⵽�ı����Ƿ���Ч
    uint8_t leftLineEffeft, rightLineEffect, preLeftLineEffect, preRightLineEffect; //�ж��������Ƿ���Ч�������ε���lL��rL�ظ��ж�  
    uint8_t lL, rL, lL1, rL1;                         //���ߣ����ߣ�ǰһ�����ߣ�ǰһ������

	//����ʮ���ж�	
    uint8_t crossCountToAvoidDoubleLlinesAreEffect = 0;//�����ж�������߶��Ǻ�ɫ�Ͳ���Ϊ��ʮ�֡�������ʱ��������������жϡ�����������ж���ʮ�־���
	uint8_t crossFound = false;                       //ʮ�ֱ�־
	int16_t maxTheta = 230;                           //ʮ�ִ��ĽǶ�
	
	//����ͼ����
	uint8_t Thr;  	                                    //�ֲ���ֵ
    int16_t roadWidth, realRoadWidth;                   //roadWidth��������ȵ�һ�룬realRoadWidth����һ��ͼ���������ȵ�һ��
    int16_t middle, NextRealMiddle, ThisRealMiddle;     //�ҵ���ǰһ�е���������
    int16_t line, column, cCount;                       //ͼ�����б���,cCount������������ȱȽ�
    int16_t longTimeNoLinesCount = 0;                   //��־��ʱ��û���ҵ���
    
	//������С���˷�
    double  erM, erDelta;
    uint8_t ErEffLines = 1;   
    uint8_t EreffLines = 1;
	
    lineInfo leftInfo;                                //���������Ϣ
    lineInfo rightInfo;	                              //����������Ϣ
    ImgClass.ImgThr = (uint8_t)(thrOffset * CalculateThreshold(Image_Deal, widthBottom, heightBottom, widthTop, heightTop)); //ͼ����ֵ
	//��ʼ�����������飬�������鼰�����������
    for (line = heightBottom; line < heightTop; line++)   //���ߣ����ߣ�����ȫ����ʼ��Ϊ����ɫ����·���Ϊ0
    {
		leftLine[line] = 255;
		rightLine[line] = 255;
		middleLine[line] = 255;
		recordRoadWidth[line] = 0;      			//��¼�������
    }
	
//	//6.27���
//	for(i=0;i<24;i++)
//	{
//		leftJumpLine[i][0]=255;
//		leftJumpLine[i][1]=255;
//		rightJumpLine[i][0]=255;
//		rightJumpLine[i][1]=255;
//	}
	
	//��ʼ�������ߵĽṹ��
    leftInfo.closestLineBeforeLost.column = widthTop;    //�G��ʱ���к�
    leftInfo.closestLineBeforeLost.deltaLine = 0;        //��ǰ�е��G��ʱ���кŲ�
    leftInfo.closestLineBeforeLost.middle = widthTop/2;  //����ʱ������
    leftInfo.closestLineBeforeLost.line = 0;             //����ʱ���к�
	
    rightInfo.closestLineBeforeLost.column = 0;           //�G��ʱ���к�
    rightInfo.closestLineBeforeLost.deltaLine = 0;        //��ǰ�е��G��ʱ���кŲ�
    rightInfo.closestLineBeforeLost.middle = widthTop/2;  //�G��ʱ������	
    rightInfo.closestLineBeforeLost.line = 0;             //�G��ʱ���к�
	
	roadLeftBlock.onlyOneDetectFlag = 0;		 //ÿ��ͼ��ֻ���һ�εı�־
	roadLeftBlock.avoidStartLineAffect = 0; 	 //���������ߵ�Ӱ��		 
	roadRightBlock.onlyOneDetectFlag = 0;		 //ÿ��ͼ��ֻ���һ�εı�־
	roadRightBlock.avoidStartLineAffect = 0; 	 //���������ߵ�Ӱ��	
	
	//��ʼ�����˷��Ĳ���
    leftErCheng.sumX = 0;
    leftErCheng.sumX2 = 0;
    leftErCheng.sumY = 0;
    leftErCheng.sumXY = 0;
    leftErCheng.effectLines = 0;
    leftErCheng.startLine = heightTop; 
    leftErCheng.endLine = 0;
    leftErCheng.k = 99999;  //k������ô���
    leftErCheng.b = 9999;   //b������ô���
    rightErCheng.sumX = 0;
    rightErCheng.sumX2 = 0;
    rightErCheng.sumY = 0;
    rightErCheng.sumXY = 0;
    rightErCheng.effectLines = 0;
    rightErCheng.startLine = heightTop;
    rightErCheng.endLine = 0;
    rightErCheng.k = 99999;
    rightErCheng.b = 9999;
    middleErCheng.sumX = 0;
    middleErCheng.sumX2 = 0;
    middleErCheng.sumY = 0;
    middleErCheng.sumXY = 0;
    middleErCheng.effectLines = 0;
    middleErCheng.startLine = heightTop;
    middleErCheng.endLine = 0;
    middleErCheng.k = 99999;
    middleErCheng.b = 9999;
	
	startLinesDetect.IsStartLine = false;
	avgWidth = 0;
  
	//���ڸ���ͼ�������е�������Ⱥ���������
    middle = widthTop/2-1;         					//�������ߣ�ʹ�þֲ�������
    NextRealMiddle = widthTop/2; 					//��һ����������
    roadWidth = 120;     							//ǰ���д�һЩ����Χ����
    realRoadWidth = 120; 
	
	for(line = heightBottom; line < heightTop; line++) //��ÿһ�кڵ�������������leftLine[]��RightLine[]����
	{	
		Thr = ImgClass.ImgThr;                          //��ȡ��ֵ
				
	//************�����������*******************/
		cCount = 0;                              			//�������ͳ�ƣ�����roadWidth����Ϊû����
		column = middle;                         			//�м�������,middle��ʵʱ����
		pixelBegin = ImgBaseAddr + line * Image_Width;      //�������Ʒ�Χ��ÿһ�е�ͼ����߽�
		pixel = pixelBegin + middle;             			//�м������ߣ�ÿһ�е�ͼ���м�
		pixelEnd = pixelBegin + Image_Width;            	//�������Ʒ�Χ��ÿһ�е�ͼ���ұ߽�

		while (true)                             			
		{
			if (*pixel < Thr && *(pixel + 1) < Thr)     	//һ������������ͬΪ��ɫ�����ҵ������ڵ���
			{
				leftLine[line] = (uint8_t)column;
				break;
			}
			pixel++;
			column++;
			cCount++;
			if (pixel == pixelEnd)              //������м����������������ߣ���ô���еĺڵ㼴Ϊͼ����߽�
			{
				leftLine[line] = Image_Width;
				break;
			}
			else if(cCount == roadWidth) 		//û������������,�Ѿ�����������ȵķ�Χ,��ʼ��roadWidth���õıȽϴ󣬼���Χ����
			{
				break;  
			}
		}
		//������������
		
		//��ʼ�����ұ���
		cCount = 0;                                   //��������
		column = middle;
		pixel = pixelBegin + middle;

		while (true)
		{
			if (*pixel < Thr && *(pixel - 1) < Thr) 
			{                
				rightLine[line] = (uint8_t)column;
				break;
			}
			pixel--;
			column--;
			cCount++;
			if (pixel == pixelBegin)
			{
				rightLine[line] = 0;          //������м����������������ߣ���ô���еĺڵ㼴Ϊͼ���ұ߽�
				break;
			}
			else if (cCount == roadWidth)
			{
				break;
			}
		}
	    //�ұ����������
		
		//����������������
//		if(line>1 && line<25)
//		{
//			//������������
//			Judge = 0;
//			cCount = 0;                              			//�������ͳ�ƣ�����roadWidth����Ϊû����
//			column = middle;                         			//�м�������
//			pixelBegin = ImgBaseAddr + line * Image_Width;      //�������Ʒ�Χ��ÿһ�е�ͼ����߽�
//			pixel = pixelBegin + middle;             			//�м������ߣ�ÿһ�е�ͼ���м�
//			pixelEnd = pixelBegin + Image_Width;            	//�������Ʒ�Χ��ÿһ�е�ͼ���ұ߽�

//			while (true)                             			
//			{
//				if((*(pixel-1) > Thr && *pixel < Thr && *(pixel + 1) < Thr) && (Judge == 1))
//				{
//					leftJumpLine[line-2][1] = (uint8_t)column;					//�ѵ��ڶ��������
//					Judge = 0;
//					break;
//				}
//				
//				if ((*pixel < Thr && *(pixel + 1) < Thr) && (Judge == 0))     	//һ������������ͬΪ��ɫ�����ҵ������ڵ���
//				{
// 					leftJumpLine[line-2][0] = (uint8_t)column;
//					pixel+=5;
//					column+=5;
//					cCount+=5;
//					if(pixel == pixelEnd)break;
//					Judge = 1;													//�ѵ�һ�������
//				}

//				pixel++;
//				column++;
//				cCount++;
//				
//				if (pixel == pixelEnd)              //������м����������������ߣ���ô���еĺڵ㼴Ϊͼ����߽�
//				{
//					if(Judge == 1)					//ֻ�ѵ�һ�������,Judge����0Ҫô�Ƕ��ѵ�Ҫô�Ƕ�û�ѵ�
//					{
//						leftJumpLine[line-2][0] = 255;
//						leftJumpLine[line-2][0] = 255;
//					}
//					break;
//				}
//				else if(cCount == 95) 				//��������ȴ����80
//				{
//					if(Judge == 1)					//ֻ�ѵ�һ�������,Judge����0Ҫô�Ƕ��ѵ�Ҫô�Ƕ�û�ѵ�
//					{
//						leftJumpLine[line-2][0] = 255;
//						leftJumpLine[line-2][0] = 255;
//					}
//					break;  
//				}
//			}
//			
//			//���������ұ������
//			cCount = 0;                                   //��������
//			Judge = 0;
//			column = middle;
//			pixel = pixelBegin + middle;

//			while (true)
//			{
//				if((*(pixel + 1) > Thr && *pixel < Thr && *(pixel - 1) < Thr) && (Judge == 1))
//				{
//					rightJumpLine[line-2][1] = (uint8_t)column;
//					Judge = 0;
//					break;
//				}
//				if ((*pixel < Thr && *(pixel - 1) < Thr) && (Judge == 0))
//				{                
//					rightJumpLine[line-2][0] = (uint8_t)column;
//					pixel-=5;
//					column-=5;
//					cCount+=5;
//					if(pixel == pixelBegin)break;
//					Judge = 1;
//				}
//				pixel--;
//				column--;
//				cCount++;
//				if (pixel == pixelBegin)
//				{
//					if(Judge == 1)					//ֻ�ѵ�һ�������,Judge����0Ҫô�Ƕ��ѵ�Ҫô�Ƕ�û�ѵ�
//					{
//						rightJumpLine[line-2][0] = 255;
//						rightJumpLine[line-2][0] = 255;
//					}
//					break;
//				}
//				else if (cCount == 95)
//				{
//					if(Judge == 1)					//ֻ�ѵ�һ�������,Judge����0Ҫô�Ƕ��ѵ�Ҫô�Ƕ�û�ѵ�
//					{
//						rightJumpLine[line-2][0] = 255;
//						rightJumpLine[line-2][0] = 255;
//					}
//					break;
//				}
//			}
//			if(rightJumpLine[line-2][0] != 255 && rightJumpLine[line-2][1] !=255 
//				&& (rightJumpLine[line-2][0] - rightJumpLine[line-2][1]) > 30)
//			{
//				rightConfirm = 1;
//			}
//			if(leftJumpLine[line-2][0] != 255 && leftJumpLine[line-2][1] !=255 
//				&& (leftJumpLine[line-2][1] - leftJumpLine[line-2][0]) > 30)
//			{
//				leftConfirm = 1;
//			}
//			if(leftConfirm == 1 && rightConfirm == 1) 
//			{
//				BUZ_Normal();
//			}
//		}
		
		//ʹ�þֲ���������������ٶ�   
        lL = leftLine[line]; 		//�����
        rL = rightLine[line];		//�ұ���
        leftLineEffeft = (lL > 0 && lL < (Image_Width-1)) ? true : false;	//true:����,false:ͼ����ػ���û���ҵ���
        rightLineEffect = (rL > 0 && rL < (Image_Width-1)) ? true : false;	//���ж���һ���Ƿ����ұ���
        if (line < 45)                     								    //���ڲ�ͬ��������Χ�������ж�ʮ�ִ��ĵĽǶȲ�ͬ
		{
            maxTheta = 250;
		}
        else if (line < 90)
		{
            maxTheta = 260;
		}
        else
		{
            maxTheta = 270;
		}
		
		ThisRealMiddle = NextRealMiddle;   	//������һ��ɨ������ߣ�����֮��Ĵ���

		//////////////////////////////////////////////////////////////////////////////////////////////
		//�Ƚϸ���������ǰһ�����ݣ��ɴ����жϸ��������Ƿ���ã������޷��˵������Ĳ�������
		//////////////////////////////////////////////////////////////////////////////////////////////

        if (line > heightBottom) 			//����heightBottom���������ܽ��������ж�
        {
            lL1 = leftLine[line - 1];       //ǰһ�е�����������λ��
            rL1 = rightLine[line - 1];
            preLeftLineEffect = (lL1 > 0 && lL1 < (Image_Width-1)) ? true : false;      //pre�������ڴ����ǰһ���ǲ����������
            preRightLineEffect = (rL1 > 0 && rL1 < (Image_Width-1)) ? true : false;     //pre�������ڴ����ǰһ���ǲ������ұ���
        
            if (preLeftLineEffect && leftLineEffeft)    //���ڵ����ж���
            {
                if (line > 100 && MyAbs(lL - lL1) > 7)  //������ڴ������һ����������100����һ�кڵ�����Ӧ��������ǰһ�кڵ�����Ӧ������֮�����7
                {
                    lL = 255;               			//����������������ô��һ������Ϊû����
                    leftLine[line] = 255;  				//��Ϊ��ʼ������Ϊ255������������Ҳ����Ϊ255
                    leftLineEffeft = false; 			//�������
                }
            }
        
            if (preRightLineEffect && rightLineEffect)   //���ڵ����ж���
            {
                if (line > 100 && MyAbs(rL - rL1) > 7 )  //������ڴ������һ����������100����һ�кڵ�����Ӧ��������ǰһ�кڵ�����Ӧ������֮�����7
                {
                    rL = 255;                            //��������������1����ô��һ������Ϊû����
                    rightLine[line] = 255;               //��Ϊ��ʼ������Ϊ255������������Ҳ����Ϊ255
                    rightLineEffect = false;             //���ұ���
                }
            }
			
			////////////////////////////////////////////////////////////////////////////////
			//�ϰ����   100�����ڽ��м��
		    //���⣺��֪���жϵ���ʱ�䣬�жϵ���ʱ��᲻��̫���ˣ����¶����������Ӧ
			//������Ҫ���������Ļ������ıȽϳ�
			///////////////////////////////////////////////////////////////////////////////
			if(roadRightBlock.IsRoadBlock == false && roadLeftBlock.IsRoadBlock == false )
			{
				if(preLeftLineEffect && leftLineEffeft && rightLineEffect && preRightLineEffect)
				{
					if(roadRightBlock.onlyOneDetectFlag == 0)		           //ÿ��ͼ��ֻ���һ�α�־λ
					{
						if(((lL1-lL)<2 || (lL-lL1)<2)  && MyAbs(lL1-lL)>=0 &&(rL - rL1)>30 &&  (line - leftErCheng.effectLines)<6)  //����û�д������  ����������  ���ҳ���ֱ����Ҫ��
						{ 
							roadRightBlock.onlyOneDetectFlag = 1;
							roadRightBlock.line = line;						   //�����������
							roadRightBlock.continuousLine = line - 1;		   //Ӧ��������������
							roadRightBlock.jumpRow = rL1;		               //�������һ�����ɼ��ĵ����ڵ���
						}
					}
					if(roadRightBlock.onlyOneDetectFlag == 1)				   //��⵽�����֮���һ���жϱ���������Ӱ��
					{
						if(((rL - roadRightBlock.jumpRow)>25)&&((line - roadRightBlock.continuousLine) == 1)) //����֮������߻����������м�
						{
							roadRightBlock.avoidStartLineAffect++;            
						}
						else
						{
							roadRightBlock.onlyOneDetectFlag = 0;
							roadRightBlock.avoidStartLineAffect = 0;
						}
						if(roadRightBlock.avoidStartLineAffect > roadRightBlock.avoidStartLineAffectThr) //�����ߴ�û����ô������������
						{
							roadRightBlock.IsRoadBlock = true;				 //�������ϰ�
						}
						roadRightBlock.continuousLine++;   					 //�����������������
					}
					///////////////////////
					//���ϰ�������
					///////////////////////
					
					//////////////////////
					//���ϰ���ʼ���
					//////////////////////
					if(roadLeftBlock.onlyOneDetectFlag == 0)                //ÿ��ͼ��ֻ����һ��
					{
						if(((rL-rL1)<2 || (rL1-rL)<2) && MyAbs(rL-rL1)>=0 &&(lL1 - lL)>30 && (line - rightErCheng.effectLines)<6) //����û�д�����䣬����д�����
						{
							roadLeftBlock.onlyOneDetectFlag = 1;
							roadLeftBlock.line = line;						//�����������
							roadLeftBlock.continuousLine = line - 1; 		//Ӧ����������������
							roadLeftBlock.jumpRow = lL1;					//�������һ�����ɼ��ĵ����ڵ���
						}
					}
					if(roadLeftBlock.onlyOneDetectFlag == 1)				//��⵽�����֮���һ���жϱ���������Ӱ��
					{
						if(((roadLeftBlock.jumpRow - lL)>25)&&(line - roadLeftBlock.continuousLine) == 1) //����֮������������м俿
						{
							roadLeftBlock.avoidStartLineAffect++;
						}
						else
						{
							roadLeftBlock.onlyOneDetectFlag = 0;
							roadLeftBlock.avoidStartLineAffect = 0;
						}

						if(roadLeftBlock.avoidStartLineAffect > roadLeftBlock.avoidStartLineAffectThr)  //�����ߴ�û����ô������������
						{
							roadLeftBlock.IsRoadBlock = true;		//�������ϰ�
						}
						roadLeftBlock.continuousLine++;
					}
					///////////////////////
					//���ϰ�������
					///////////////////////				
				}
			}
			else			  //�Ѿ�ȷ�����ϰ���
			{
				if(roadLeftBlock.BesideBlock == false)   //�ж��Ƿ����ϰ���
				{
					if(recordRoadWidth[line-1] < 60 && line < 12)
					{
						roadLeftBlock.stateChangeCount++;
						if(roadLeftBlock.stateChangeCount>roadLeftBlock.avoidStartLineAffectThr)
						{
							roadLeftBlock.BesideBlock = true;
							roadLeftBlock.stateChangeCount = 0;
						}
					}
				}
				else if(roadLeftBlock.ThroughBlock == false) //��ȷ�����ϰ���֮���ж��Ƿ�ͨ���ϰ���
				{
					if(recordRoadWidth[line-1] > 60 && line < 12)
					{
						roadLeftBlock.stateChangeCount++;
						if(roadLeftBlock.stateChangeCount>roadLeftBlock.avoidStartLineAffectThr)
						{
							roadRightBlock.IsRoadBlock = false;
							roadLeftBlock.IsRoadBlock = false;
							roadLeftBlock.BesideBlock = false;
							roadLeftBlock.ThroughBlock = false;
							roadLeftBlock.BlockTimeCount = 0;
							roadLeftBlock.stateChangeCount = 0;
						}
					}
				}
			}
			/////////////////
			//�ϰ�������
			////////////////

			//////////////////////////////////////
			///////��¼������Ϣ
			/////////////////////////////////
			
            if ((leftLineEffeft && rightLineEffect) || (line < directSearchLines))  //�������������߶����ڣ���¼�����ߵ���Ϣ
            {
                longTimeNoLinesCount -= 3;                                          //longTimeNoLinesCount��ʼֵΪ0�������Լ�3
                if (longTimeNoLinesCount < 0) longTimeNoLinesCount = 0;             //���С��0����Ϊ0
                rightInfo.closestLineBeforeLost.column = rL;                        //��¼�ұ���
                rightInfo.closestLineBeforeLost.deltaLine = 0;                      //deltaline = 0��������ʱ����������һ�е�����Ϊ0
                rightInfo.closestLineBeforeLost.line = (uint8_t)line;               //line = ��һ������
//                rightInfo.closestLineBeforeLost.middle = (uint8_t)NextRealMiddle;   //middle = ��һ���м�����

                leftInfo.closestLineBeforeLost.column = lL;                         
                leftInfo.closestLineBeforeLost.deltaLine = 0;
                leftInfo.closestLineBeforeLost.line = (uint8_t)line;
//                leftInfo.closestLineBeforeLost.middle = (uint8_t)NextRealMiddle;
                crossFound = false;													//10�����ڲ�����ʮ��
            }
            else if (rightLineEffect)                               //ֻ�����ߣ��ж��Ƿ���Ҫ��¼������һ����Ϣ
            {
                if (longTimeNoLinesCount > 0) longTimeNoLinesCount--;       
                if (preLeftLineEffect && leftLineEffeft == false)   //������������һ���кڵ㣬һ��û��ʱ���������ߵ�����£�����ͻȻû����
                {
                    leftInfo.closestLineBeforeLost.column = lL1;                        //��һ��ͻȻû�����ˣ�����������ȡ��һ�е����ߣ����ľͲ��ᷢ��ͻ��
                    leftInfo.closestLineBeforeLost.deltaLine = 0;                       //�����ж������������߶��ˣ���ô����ʱ����������һ�е�����Ϊ0
                    leftInfo.closestLineBeforeLost.line = (uint8_t)line;                //��¼�������޵��ٽ�����
//                    leftInfo.closestLineBeforeLost.middle = (uint8_t)NextRealMiddle;    //��¼�����м�����
                }
				
                leftInfo.closestLineBeforeLost.deltaLine = (uint8_t)(line - leftInfo.closestLineBeforeLost.line);   //������¼���߶��˶�����
            }
            else if (leftLineEffeft)    				//ֻ�����ߣ��ж��Ƿ���Ҫ��¼������һ�е���Ϣ
            {
                if (longTimeNoLinesCount > 0) longTimeNoLinesCount--;
                if (preRightLineEffect && rightLineEffect == false)  //������������һ���кڵ㣬һ��û��ʱ���������ߵ�����£�����ͻȻû����
                {
                    rightInfo.closestLineBeforeLost.column = rL1;
                    rightInfo.closestLineBeforeLost.deltaLine = 0;
                    rightInfo.closestLineBeforeLost.line = (uint8_t)line;
//                    rightInfo.closestLineBeforeLost.middle = (uint8_t)NextRealMiddle;
                }
				
                rightInfo.closestLineBeforeLost.deltaLine = (uint8_t)(line - rightInfo.closestLineBeforeLost.line);  //������¼���߶��˶�����
            }
            else    //�����߶�û�У�����������10
            {
                if (longTimeNoLinesCount < 25) longTimeNoLinesCount++;  //�Լ�1
                if (preLeftLineEffect)  //�����һ�������кڵ�
                {
                    leftInfo.closestLineBeforeLost.column = lL1;            //���߶���ǰ����
                    leftInfo.closestLineBeforeLost.deltaLine = 0;       
                    leftInfo.closestLineBeforeLost.line = (uint8_t)line;    //���߶���ǰ����
//                    leftInfo.closestLineBeforeLost.middle = (uint8_t)NextRealMiddle;
                }
                if (preRightLineEffect)  //�����һ�������кڵ�
                {
                    rightInfo.closestLineBeforeLost.column = rL1;
                    rightInfo.closestLineBeforeLost.deltaLine = 0;
                    rightInfo.closestLineBeforeLost.line = (uint8_t)line;
//                    rightInfo.closestLineBeforeLost.middle = (uint8_t)NextRealMiddle;
                }
                leftInfo.closestLineBeforeLost.deltaLine = (uint8_t)(line - leftInfo.closestLineBeforeLost.line);   //��¼���߶��˶�����
                rightInfo.closestLineBeforeLost.deltaLine = (uint8_t)(line - rightInfo.closestLineBeforeLost.line); //��¼���߶��˶�����
            }
			
		//////////////////////////////
		///////������¼���        
		/////////////////////////////
        }
      
        if (leftLineEffeft)  //������ߴ��ڣ���Ϊ��Ч��
        {
            leftErCheng.effectLines++;
        }
        if (rightLineEffect) //������ߴ��ڣ���Ϊ��Ч��
        { 
            rightErCheng.effectLines++;
        }
		
        //���³����ɵ�ǰ�����ı��߼����´��õ����ĺ͵�ǰ������ȣ������´�ɨ�跶Χ
        if (line > directSearchLines) 	//�ǽ�������Ҫ��ʼ����ʮ��
        {
			/*---------------ʮ��ʶ�����-------------*/
            if (line < (Image_Height-1) && line > ThetaDelta2X) //14��144��ִ��ʮ��ʶ�����127��ִ���������(15~142)
            {
				/////////////////////////////////////
				//���ݸ���֮ǰ��14�У����������Ƕ�
				////////////////////////////////////
				
                ThetaL = 0;
                ThetaR = 0;
                //��������ǰ��14�У����Ƿ�������ڼ���Ƕȣ����14������һ�������þͲ��ܼ���
                i = line - ThetaDelta2X;    
                for (; i <= line; i++) if (leftLine[i] == 0 || leftLine[i] > (Image_Width-1)) { ThetaL = 1000; break; } //14���м��ж��ߣ������нǶȼ���
                //��������ǰ��14�У����Ƿ�������ڼ���Ƕȣ����14������һ�������þͲ��ܼ���
                i = line - ThetaDelta2X;
                for (; i <= line; i++) if (rightLine[i] == 0 || rightLine[i] > (Image_Width-1)) { ThetaR = 1000; break; } //14�м��ж��ߣ������нǶȼ���
				
				//�����Ƕȿ��Լ���
                if (ThetaL < 1)
                {
                    //ʮ�ֿհ��ڲ����ͨ���Ƚϳ�
                    if (longTimeNoLinesCount == 0)//�����߻�δ�������Ѿ�ͨ��ʮ��
                    {
                        ThetaL = kkkMinus(line, line - ThetaDeltaX, line - ThetaDelta2X, leftLine[line], leftLine[line - ThetaDeltaX], leftLine[line - ThetaDelta2X]);
                    }
                    else   
                    {
                        ThetaL = kkkMinus(line - ThetaDelta2X, line - ThetaDeltaX, line, leftLine[line - ThetaDelta2X], leftLine[line - ThetaDeltaX], leftLine[line]);
                    }
                }
          
                if (ThetaR < 1)//��14���У����߶�û������ô��ThetaR
                {
                    if (longTimeNoLinesCount == 0)
                    {
                        ThetaR = kkkMinus(line, line - ThetaDeltaX, line - ThetaDelta2X, rightLine[line], rightLine[line - ThetaDeltaX], rightLine[line - ThetaDelta2X]);
                    }
                    else
                    {
                        ThetaR = kkkMinus(line - ThetaDelta2X, line - ThetaDeltaX, line, rightLine[line - ThetaDelta2X], rightLine[line - ThetaDeltaX], rightLine[line]);
                    }
                }
                if (ThetaR < 360) ThetaR = (int16_t)(360 - ThetaR);     //��������
				
				/////////////////////////////////
				//�����Ƕȼ������
				////////////////////////////////				
				
				////////////////////////////
				////ʮ��������ʶ��
				////////////////////////////
                //���ThetaL��230��360֮�䣬����ThetaR��230��360֮�䣬����ʮ�ֳ��֣�һ�߳�������ʮ���������
                if ((ThetaL > maxTheta && ThetaL < 360 )||( ThetaR > maxTheta && ThetaR < 360)) 
                {
                    if (crossCountToAvoidDoubleLlinesAreEffect < 10)    //ʮ���ۼ������Լ�1
						crossCountToAvoidDoubleLlinesAreEffect++;
                }
                else
                {
                    crossCountToAvoidDoubleLlinesAreEffect = 0;         //û��ʮ�ֻ���ʮ���ѹ�
                }
				
                //�����ʮ�ִ����У�����߳����Һ��߲����ڣ����Һ��߳�������߲�����
                if(((ThetaL > maxTheta && ThetaL < 360 )&& (rightInfo.closestLineBeforeLost.deltaLine > 2 || line < 50 || crossCountToAvoidDoubleLlinesAreEffect > 4))
                    ||(( ThetaR > maxTheta && ThetaR < 360 )&& (leftInfo.closestLineBeforeLost.deltaLine > 2 || line < 50 || crossCountToAvoidDoubleLlinesAreEffect > 4)))
                {
					//����ͼ����䣬��������ʮ��ʱ����ɼ���һ�������Һ��ߣ����´������ȥ�������
                    if (longTimeNoLinesCount > 0)
                    {
                        if (ThetaL > maxTheta && ThetaL < 360)  //����к��ߣ����ǰ5���ð�
                        {
                            for (i = 5; i < 10; i++) leftLine[line - i] = 255;//ʮ�ֹյ�֮ǰ��5����,Ϊ��ɫ
                        }
                        else                                   //�ұ��к��ߣ��ұ�5���ð�
                        {
                            for (i = 5; i < 10; i++) rightLine[line - i] = 255;
                        }
                    }
                    else      
                    {
                        if (ThetaL > maxTheta && ThetaL < 360)   //����������
                        {
                            for (i = 0; i < 5; i++) leftLine[line - i] = 255;//��ǰ��֮ǰ��5���㣬��������Ϊ�ǹյ�֮���5����
                        }
                        else
                        {
                            for (i = 0; i < 5; i++) rightLine[line - i] = 255;
                        }
                    }
            
                    if(ThisRealMiddle < 30 || ThisRealMiddle > 170)    //��ʮ�����㹻�����߽磬������������Ϊ����һ���������ʮ�֡��ý�����Զ��������
                    {
                        break;  	//ֱ����������������������һ������
                    }
					
                    if(longTimeNoLinesCount < 2)   //�ս���ʮ��
                    {
						///////////////////////////////
						/////���˷�Ԥ����һ�е��е�
						//////////////////////////////
                        if(line > 30)
                        {
                            middleErCheng.effectLines = 0;
                            middleErCheng.sumX = 0;
                            middleErCheng.sumX2 = 0;
                            middleErCheng.sumY = 0;         //middle������һ�е�ֵ��Ӧ�����������еģ�����NextRealMiddle
                            middleErCheng.sumXY = 0;
                            i = line - ThetaDelta2X - 20;   
                            if (i < heightBottom) i = heightBottom;
                            for (; i < line - ThetaDeltaX; i++)//ȡ���ڴ������һ��ǰ34�е�ǰ14�е�20�У����ж��˷�Ԥ��
                            {
                                if (middleLine[i] > 0 && middleLine[i] < Image_Width)   //��һ�д���ʱ��MiddleLine[i]=255,���������жϣ�������ڴ������һ�����е�
                                {
                                    middleErCheng.effectLines++;
                                    middleErCheng.sumX += i;
                                    middleErCheng.sumX2 += i * i;
                                    middleErCheng.sumY += middleLine[i];         //middle������һ�е�ֵ��Ӧ�����������еģ�����NextRealMiddle
                                    middleErCheng.sumXY += i * middleLine[i];
                                }
                            }
                            if (middleErCheng.effectLines > 10)//20����������10����Ч����ô�Ϳ��Ը�����һ�ε�����Ԥ����һ�е��е�
                            {
                                middleErCheng.k = ErChengK(middleErCheng.effectLines, middleErCheng.sumX, middleErCheng.sumX2, middleErCheng.sumXY, middleErCheng.sumY);
                                middleErCheng.b = ErChengB(middleErCheng.effectLines, middleErCheng.sumX, middleErCheng.sumX2, middleErCheng.sumXY, middleErCheng.sumY);//ImgClass.RealMiddle;

                                NextRealMiddle = (int16_t)(middleErCheng.k * (line) + middleErCheng.b);
                            }
                            else    //20��������10����Ч����ô�����Ը�����һ�ε�����Ԥ����һ�е��е㣬ֻ������ǰ������Ԥ����һ�е��е�
                            {
                                NextRealMiddle = (int16_t)((middleLine[line - 3] + middleLine[line - 4]) / 2);
                            }
                        }
                        else//�������С��30����ֻ������ǰ������Ԥ����һ�е��е�
                        {
                            NextRealMiddle = (int16_t)((middleLine[line - 3] + middleLine[line - 4]) / 2);
                        }
              
						//��ʱ��Ϊ��Ԥ����������ߣ�����������ȱ�����һ�ε�ֵ
                        realRoadWidth = ImgClass.realRoadWidth;
                        roadWidth = (int16_t)(ImgClass.realRoadWidth + 10);	 //���������������ȵĻ���������10,��������ʱȴ�޷��ѵ�
                        ImgClass.RealMiddle = NextRealMiddle;       //�Ʋ������
						
						
                        middle = NextRealMiddle;                //middle��ʵʱ����
                        i = line;
						
						/////////////////////////
						////���˷�����������
						/////////////////////////
						
                        crossFound = true;          //����ʮ��	
                        if (line < 50)      //����ԽС�������Ŀհ״�����Խ�࣬����һ��������������
                        {
                            line += 30;
                            longTimeNoLinesCount = 25;//Ӧ����Ϊ��ʮ�ֿհ�ǰ��ֱ��
                        }
                        else if (line < 100)
                        {
                            line += 10;
                            longTimeNoLinesCount = 25;
                        }
                        else
                        {
                            line += 5;
                            longTimeNoLinesCount = 25;
                        }
              
                        for (; i <= line; i++) if (i < heightTop) middleLine[i] = (uint8_t)NextRealMiddle;      //ʮ�ִ�������
                    }
                    continue;  	 //��ֻҪ���ֺ��߾Ͳ��Ͻ��б���������ֱ�����жϵ�ʮ��
                }
            }
			////////////////////////
			//ʮ��ʶ��������
			///////////////////////

			////////////////////////////////////
			//����������Ⱥ������м���
			///////////////////////////////////
            //����������10�������
            if (longTimeNoLinesCount == 0 && (leftLineEffeft || rightLineEffect))//�����ҵ���������������һ�����ڣ��Ͳ���ֱ��ʹ�����ĵ�k��b
            {
                middleErCheng.k = 99999;
            }
			
			ImgClass.preRoadWidth = realRoadWidth;			//��һ�е��������
			
            //˫�߶����ڻ���ֻ��һ���ߴ���ʱ
            if (longTimeNoLinesCount == 0 && leftLineEffeft && rightLineEffect)  //��������rL != 255 && lL != 255
            {
				NextRealMiddle = (int16_t)((rL + lL) / 2);    		   //��һ�е����߿�ʼλ��
				realRoadWidth = (int16_t)((lL - rL) / 2);			   //������ȵ�һ��
                ImgClass.realRoadWidth = realRoadWidth;
                ImgClass.RealMiddle = NextRealMiddle;   
            }
            else if (longTimeNoLinesCount == 0 && rightLineEffect) 		   //�������ұ�����
            {

                //NextRealMiddle = (int16_t)((rightInfo.closestLineBeforeLost.middle + (rL - rightInfo.closestLineBeforeLost.column) / 1));//�G��ʱ�����ļ������ƫ��
                NextRealMiddle = (int16_t)((leftInfo.closestLineBeforeLost.column + rL) / 2); //�G��ʱ�����ļ������ƫ��
                realRoadWidth = (int16_t)((NextRealMiddle - rL));

                ImgClass.realRoadWidth = realRoadWidth;//��Ȼ�ܵõ�һ����ȷ��������Ⱥ��������ģ����Լ�¼����
                ImgClass.RealMiddle = NextRealMiddle;
            }
            else if (longTimeNoLinesCount == 0 && leftLineEffeft)   //�������������
            {
                //NextRealMiddle = (int16_t)(leftInfo.closestLineBeforeLost.middle + (lL - leftInfo.closestLineBeforeLost.column) / 1);// �G��ʱ�����ļ������ƫ����
                NextRealMiddle = (int16_t)((lL + rightInfo.closestLineBeforeLost.column) / 2);// �G��ʱ�����ļ������ƫ����
                realRoadWidth = (int16_t)(lL - NextRealMiddle);                               //���ļ�ȥ����,��������ȵ�һ��
                ImgClass.realRoadWidth = realRoadWidth;                                       //��Ȼ�ܵõ�һ����ȷ��������Ⱥ��������ģ����Լ�¼����
                ImgClass.RealMiddle = NextRealMiddle;

            }
            else if (crossFound == false && longTimeNoLinesCount < 25)  //��ʮ�֣�����û���ߵ�����С��25
            {
                NextRealMiddle = (int16_t)((leftInfo.closestLineBeforeLost.column + rightInfo.closestLineBeforeLost.column) / 2);
                realRoadWidth = (int16_t)((leftInfo.closestLineBeforeLost.column - rightInfo.closestLineBeforeLost.column) / 2); 
                ImgClass.realRoadWidth = realRoadWidth;//��Ȼ�ܵõ�һ����ȷ��������Ⱥ��������ģ����Լ�¼����
                ImgClass.RealMiddle = NextRealMiddle;
            }
            else		//���߶�û�ѵ��ߣ�������С���˷�Ԥ��
            {
                if (middleErCheng.k > 99998)
                {//�G���ˣ�ֻ����һ�Σ�ֱ���ٴ�����
                    middleErCheng.effectLines = 0;
                    middleErCheng.sumX = 0;
                    middleErCheng.sumX2 = 0;
                    middleErCheng.sumY = 0;         
                    middleErCheng.sumXY = 0;
                    if (line < 20)                              //���Line<20,��ôi=line-10��
					{
						i = (int16_t)(line - directSearchLines);
					}
                    else                                        //���line>20,��ôi=line-20��
					{
						i = (int16_t)(line - 20);
					}
                    if (i < heightBottom)                       //��֤i��С��heightBottom+2��
					{
						i = heightBottom + 2;
					}
                    for (; i < line; i++)   //line��ǰn��
                    {
                        if (middleLine[i] > 0 && middleLine[i] < Image_Width)   //����������������ʱ
                        {
                            middleErCheng.effectLines++;
                            middleErCheng.sumX += i;
                            middleErCheng.sumX2 += i * i;
                            middleErCheng.sumY += middleLine[i];         //middle������һ�е�ֵ��Ӧ�����������еģ�����NextRealMiddle
                            middleErCheng.sumXY += i * middleLine[i];
                        }
                    }
                    middleErCheng.k = ErChengK(middleErCheng.effectLines, middleErCheng.sumX, middleErCheng.sumX2, middleErCheng.sumXY, middleErCheng.sumY);
                    middleErCheng.b = ErChengB(middleErCheng.effectLines, middleErCheng.sumX, middleErCheng.sumX2, middleErCheng.sumXY, middleErCheng.sumY);
                }
                
                if (line > 30 && (leftErCheng.effectLines > 30 || rightErCheng.effectLines > 30) && middleErCheng.k < 99998)    //������������30ʱ
                {
                    NextRealMiddle = (int16_t)(middleErCheng.k * (line) + middleErCheng.b);
                }
                else
                {
                    NextRealMiddle = Image_Width/2;
                }
                
                realRoadWidth = ImgClass.realRoadWidth;//��Ȼ�ܵõ�һ����ȷ��������Ⱥ��������ģ����Լ�¼����
                ImgClass.RealMiddle = NextRealMiddle;
            }
            //���ն�ֵ����������������������ֵ
			middleLine[line] = (uint8_t)NextRealMiddle;//��������������Ļ��Ǽ�������Ķ���Ϊ����ȷ��
            recordRoadWidth[line] = (uint8_t)realRoadWidth;
			
			if(line < 50)
			{
				startLinesDetect.roadWidth = 35;
			}
            else if (line < 100)
			{
				startLinesDetect.roadWidth = 25;
			}
            else
			{
			    startLinesDetect.roadWidth = 20;
			}

			//�����ߴ���	 
			if (
				(roadType == RT_Straight || roadType == RT_CurveToStraight || roadType == RT_StraightToCurve) //ֻ����������ֱ�������ֱ��
			    &&  PrePreSee > 100
				&& startLinesDetect.IsStartLine == false && line < 120           //��θ÷�ͼ��û�д���������߶���������խ��
                && roadLeftBlock.IsRoadBlock == false && roadRightBlock.IsRoadBlock == false  //�����ϰ�
				&& 
				(realRoadWidth < startLinesDetect.roadWidth  //�㹻С������
				 || line<80 && ((MyAbs(lL - ThisRealMiddle) < 15 || MyAbs(rL - ThisRealMiddle) < 15)))//���������ߺܽ�
				)
			{
				int16_t leftCloseEff = 0, rightCloseEff = 0,count = 0;
				i = line - 15;
				if (i < heightBottom) i = heightBottom;
				for (; i < line - 4; i++)
				{
					if (leftLine[i] > 0 && leftLine[i] < Image_Width) leftCloseEff++;   //��������ֱ�����ӽ������ߵĵط��϶��ܶ���Ϣ
				    if (rightLine[i] > 0 && rightLine[i] < Image_Width) rightCloseEff++;//
					if (middleLine[i] > 0 && middleLine[i] < Image_Width)//�Խ�������ͳ��
					{
						startLinesDetect.avgMiddle += middleLine[i];
						count++;
					}
				}
				if(count != 0)
				{
					startLinesDetect.avgMiddle /=count;
				}
			
				if (((leftCloseEff > 6 && rightCloseEff > 6) || (line < 50)))//�������Ĳ��Ǻ�ƫ��
				{
					if (line < 120) 
					{
						startLinesDetect.IsStartLine = true;
						startLinesDetect.line = (uint8_t)line; //�������ͣ����ʱʱ��
					}
					middleLine[line] = (uint8_t)(((int16_t)middleLine[line - 7] + (int16_t)middleLine[line - 8]) / 2);
					recordRoadWidth[line] = (uint8_t)(((int16_t)recordRoadWidth[line - 7] + (int16_t)recordRoadWidth[line - 8]) / 2);

					NextRealMiddle = middleLine[line];
					realRoadWidth = recordRoadWidth[line];
					ImgClass.realRoadWidth = realRoadWidth;
					ImgClass.RealMiddle = NextRealMiddle;
              
					i = line - 4;
					if (line > 100)
					{
						line += 5;//����
					}
					else if (line > 60)
					{
						line += 10;//����
					}
					else
					{
						line += 15;//����
					}
					if (line > heightTop) line = heightTop;
					for (; i < line; i++)
					{
						leftLine[i] = 255;
						rightLine[i] = 255;
						middleLine[i] = (uint8_t)NextRealMiddle;
						recordRoadWidth[i] = (uint8_t)realRoadWidth;
					}
					lL = 110;
					rL = 40;
					ThisRealMiddle = NextRealMiddle;
				}
			}
			////////////////////
		    //�����ߴ������
			////////////////////
		  
            //����������Χ
            if(NextRealMiddle < 1) NextRealMiddle = 1;
            else if(NextRealMiddle > (Image_Width-2)) NextRealMiddle = (Image_Width-2);
          
            if (realRoadWidth < 0)
                realRoadWidth = 0;
            else if (realRoadWidth > 120) realRoadWidth = 120;

            roadWidth = (int16_t)(realRoadWidth + 10 );  //�ڴ˴����£����ڱ��ߵ�����
            middle = NextRealMiddle;
        }
        else      										//line <= directSearchLines,����������
        {
            NextRealMiddle = (uint8_t)((rL + lL) / 2);            //������εı���ֵԤ����һ�е���������
            realRoadWidth = (int16_t)((lL - rL) / 2);
            middleLine[line] = (uint8_t)NextRealMiddle;           //��������������Ļ��Ǽ�������Ķ���Ϊ����ȷ��
            recordRoadWidth[line] = (uint8_t)realRoadWidth;
            ImgClass.realRoadWidth = realRoadWidth;               //�����������ı��ǲż�¼
            ImgClass.RealMiddle = NextRealMiddle;
            roadWidth = 120;   //ǰ���д�һЩ����Χ����
            if (lL - rL < 45)  //���������ر����ʱ������ȥ
            {
                for (i = 0; i < 30; i++)
                {
                    leftLine[i] = Image_Width;
                    rightLine[i] = 0;
                    middleLine[i] = Image_Width/2;//��������������Ļ��Ǽ�������Ķ���Ϊ����ȷ��
                    recordRoadWidth[i] = 100;
                }
                NextRealMiddle = Image_Width/2;
                roadWidth = 100;
                line += 15;
            }
        }
        if (realRoadWidth < 5                 //ʵ�ʵĵ�·���С��һ��ֵ
          || ((heightTop - line < 5) && (!leftLineEffeft && !rightLineEffect))
            || (ThisRealMiddle < 4 || ThisRealMiddle > (Image_Width-4))  //̫����������
              || (leftLineEffeft && MyAbs(lL - ThisRealMiddle) < 5)  	 //������߽�
                || (rightLineEffect && MyAbs(rL - ThisRealMiddle) < 5) 	 //�����ұ߽�
                  /*|| lL == rL && lL != 255 && dis > FarthestDis*/) //Զ����lL��rL��ȣ�������255�ų���
        {
            break;//ʵ��·�Ŀ��С��һ��ֵ���������߽磬���ұ��غ�
        }
    }
  /**********************************************������ȡ���*****************************************************/
  /**********************************************������ȡ���*****************************************************/
  
  
  /*******************************������ȡ��ɣ���������С���˷����Լ�������***************************************/
  /*******************************������ȡ��ɣ���������С���˷����Լ�������***************************************/
  /*******************************������ȡ��ɣ���������С���˷����Լ�������***************************************/
  
  //******************ȥ��Զ���µ���*************/
  //******************ȥ��Զ���µ���*************/
  //******************ȥ��Զ���µ���*************/
  //��Զ�����������
    for (i = heightTop - 1; i > Bottom; i--)//50��֮��
    {//���ҹµ���
        value--; 
        lL = leftLine[i];
        leftLineEffeft = (lL > 0 && lL < (Image_Width-1)) ? true : false;
        if (leftLineEffeft)//����������ǰ��һ���հ�
        {
            Eff++;
            InEff = 0;
            lL1 = leftLine[i - 1];
            if (Eff == 1)
			{
				if (MyAbs(lL - lL1) > 4 || (lL1 == 0 || lL1 > (Image_Width-1)))
				{
					leftLine[i-1] = 255;//ɾ�����һ����������
				}
			}
        }
        else
        {
            InEff++;
        }
		
        if (Eff >= minIsland) break; 		//�Ѿ�û�б�Ҫ�������ˣ�minIslandΪ8
        if (i < 100) minBlank = 10; else if (i < 120) minBlank = 7; else minBlank = 4;
        if (Eff > 0 && Eff < minIsland && InEff > minBlank)
        {//������������ǰ�Ŀհ׿飬˵���ǵ��������ǲ�������
            for (line = (int16_t)i; line < heightTop; line++)
            {
                leftLine[line] = 255;
            }
            break;//ֻ����һ��
        }
    }
    Eff = 0;
    InEff = 0;
    for (i = heightTop - 1; i > Bottom; i--)
    {//���ҹµ���
        rL = rightLine[i];
        rightLineEffect = (rL > 0 && rL < (Image_Width-1)) ? true : false;
        if (rightLineEffect)//����������ǰ��һ���հ�
        {
            Eff++;
            InEff = 0;
            rL1 = rightLine[i - 1];
            if (Eff == 1)
                if (MyAbs(rL - rL1) > 4 || (rL1 == 0 || rL1 > (Image_Width-1))) rightLine[i] = 255;//ɾ�����һ����������
        }
        else
        {
            InEff++;
        }
        if (Eff >= minIsland) break; //�Ѿ�û�б�Ҫ��������
        if (i < 100) minBlank = 10; else if (minBlank < 120) minBlank = 7; else minBlank = 4;
        if (Eff > 0 && Eff < minIsland && InEff > minBlank)
        {//������������ǰ�Ŀհ׿飬˵���ǵ��������ǲ�������
            for (line = (int16_t)i; line < heightTop; line++)
            {
                rightLine[line] = 255;
            }
            break;//ֻ����һ��
        }
    }
	
	/////////////
    //�����ӵ�
	////////////
	for(i = 3; i<130; i++)
	{
		if((rightLine[i-1] == 0||rightLine[i-1] == 255) && (rightLine[i+1] == 0 || rightLine[i + 1] == 255))
		{
			rightLine[i] = rightLine[i-1];
		}
		if((leftLine[i-1] == Image_Width||leftLine[i-1] == 255) && (leftLine[i+1] == Image_Width || leftLine[i + 1] == 255))
		{
			leftLine[i] = leftLine[i-1];
		}
	}
  //*********************���½�������ͼ�����С���˷�***************************/
  //*********************���½�������ͼ�����С���˷�***************************/

    leftErCheng.effectLines = 0;
    rightErCheng.effectLines = 0;
    for (line = heightBottom; line < heightTop; line++)
    {
        lL = leftLine[line];
        rL = rightLine[line];
        if (lL > 0 && lL < Image_Width)
        {
            if (line < leftErCheng.startLine) leftErCheng.startLine = (uint8_t)line;
            leftErCheng.endLine = (uint8_t)line;
            leftErCheng.effectLines++;
            leftErCheng.sumX += line;
            leftErCheng.sumX2 += line * line;
            leftErCheng.sumY += lL;
            leftErCheng.sumXY += line * lL;
        }
    
        if (rL > 0 && rL < Image_Width)
        {
            if (line < rightErCheng.startLine) rightErCheng.startLine = (uint8_t)line;
            rightErCheng.endLine = (uint8_t)line;
            rightErCheng.effectLines++;
            rightErCheng.sumX += line;
            rightErCheng.sumX2 += line * line;
            rightErCheng.sumY += rL;
            rightErCheng.sumXY += line * rL;
        }
    }//��ͽ���
  
    leftErCheng.AvgEndLine = (leftErCheng.effectLines > 0 ? (uint8_t)(leftErCheng.sumX / leftErCheng.effectLines) : (uint8_t)0);   //������ж�������ǰհ�Ĵ�С��˭����˭
    rightErCheng.AvgEndLine = (rightErCheng.effectLines > 0 ? (uint8_t)(rightErCheng.sumX / rightErCheng.effectLines) : (uint8_t)0);   //������ж�������ǰհ�Ĵ�С��˭����˭

    if (leftErCheng.effectLines > 4)
    {
        leftErCheng.k = ErChengK(leftErCheng.effectLines, leftErCheng.sumX, leftErCheng.sumX2, leftErCheng.sumXY, leftErCheng.sumY);
        leftErCheng.b = ErChengB(leftErCheng.effectLines, leftErCheng.sumX, leftErCheng.sumX2, leftErCheng.sumXY, leftErCheng.sumY);
        //********************���㷽���ƽ��ֵ*******************/
		leftErCheng.avgLine = 0;//����Ч��
		leftErCheng.zigma2 = 0; //����ƫ��
		
		for (line = heightBottom; line < heightTop; line++)
		{
			lL = leftLine[line];
			if (lL > 0 && lL < Image_Width)
			{
				ErEffLines++;
				erM = (leftErCheng.k * line + leftErCheng.b);  //���˷���ϵ�ֵ
				erDelta = lL - erM;                            //ʵ��ֵ�����ֵ��ƫ��
				leftErCheng.avgLine += (int16_t)erDelta;       //ƫ���ܺ�
				leftErCheng.zigma2 += erDelta * erDelta;       //ƫ���ƽ��
			}
		}
		leftErCheng.zigma2 /= ErEffLines;
		leftErCheng.avgLine *= 100;
		leftErCheng.avgLine /= ErEffLines;
    }
    else //���õ�����̫��
    {
        leftErCheng.avgLine = 10000;
        leftErCheng.zigma2 = 0;
    }
  
    if (rightErCheng.effectLines > 4)
    {
        rightErCheng.k = ErChengK(rightErCheng.effectLines, rightErCheng.sumX, rightErCheng.sumX2, rightErCheng.sumXY, rightErCheng.sumY);
        rightErCheng.b = ErChengB(rightErCheng.effectLines, rightErCheng.sumX, rightErCheng.sumX2, rightErCheng.sumXY, rightErCheng.sumY);

        rightErCheng.avgLine = 0;
        rightErCheng.zigma2 = 0;
        for (line = heightBottom; line < heightTop; line++)
        {
            rL = rightLine[line];
            if (rL > 0 && rL < Image_Width)
            {
                EreffLines++;
                erM = (rightErCheng.k * line + rightErCheng.b);
                erDelta = rL - erM;
                rightErCheng.avgLine += (int16_t)erDelta;
                rightErCheng.zigma2 += erDelta * erDelta;
            }
        }
        rightErCheng.zigma2 /= EreffLines;
        rightErCheng.avgLine *= 100;
        rightErCheng.avgLine /= EreffLines;
    }
    else
    {
        rightErCheng.avgLine = 10000;
        rightErCheng.zigma2 = 0;
    }

	///////////////////////////////
	//�µ����
	//��ʱ���ܼ�⵽����Ҫ����
	///////////////////////////
	int16_t effLines = 1;
	for (line = 88; line < heightTop; line++)			//��Զ����ƽ���������
	{
		lL = leftLine[line];
		rL = rightLine[line];
		if (lL > 0 && lL < Image_Width && rL > 0 && rL < Image_Width)
		{
			avgWidth += recordRoadWidth[line];
			effLines++;
		}
	}
	avgWidth /= effLines;  //�õ�ƽ����� 
	
	//���µ�ʱ��Զ����������Ⱥ�ֱ����������ͬ
	//״̬ת�������µ�-->���µ�-->���µ�
	//�ھ����µ�ʱ��ʹ��ƽ��ƫ�������������
	//�ٶ�Ҳ�ή
	//����������ʱ�򿼽��߽磬�ᵼ�¼�ⲻ��
	if(RAMWAY_DETECT == true)
	{
		if (ramWay.IsUpRamp == false) //û�м�⵽�Ѿ����µ�
		{
			if (((leftErCheng.effectLines > roadTypeConst.StraightRoadJudgeMinEffLines && rightErCheng.effectLines > roadTypeConst.StraightRoadJudgeMinEffLines)//��Ч�����㹻
				||((leftErCheng.effectLines > roadTypeConst.StraightRoadJudgeMinEffLines) && (rightErCheng.effectLines > roadTypeConst.StraightRoadJudgeMinEffLines -30))
				||((leftErCheng.effectLines > roadTypeConst.StraightRoadJudgeMinEffLines - 30) && (rightErCheng.effectLines > roadTypeConst.StraightRoadJudgeMinEffLines)))
				&& leftErCheng.zigma2 < roadTypeConst.StraightRoadJudgeMaxZigMa && rightErCheng.zigma2 < roadTypeConst.StraightRoadJudgeMaxZigMa//��������С��zigmaС
				&& avgWidth > roadTypeConst.StraightRoadJudgeWidth + roadTypeConst.RampWayJudgeSeparate//��������㹻��
				)
			{
				ramWay.rampConfirmCount++;
			} 
			else
			{
				ramWay.rampConfirmCount = 0;
			}
			if (ramWay.rampConfirmCount > roadTypeConst.RampWayConfirmC)
			{ //��ȷ�����µ���
				ramWay.IsUpRamp = true; //ת��������µ���
	//			BUZ_Normal();
				ramWay.rampConfirmCount = 0;
			}
			ramWay.rampTimeCount = 0;//�����������ʱͨ���µ���ʱ��
		}
		else//�Ѿ����µ�����
		{
			if (ramWay.IsOnRamp == false)//�Ƿ����µ���
			{ //IsUpRamp=true.������û��ȷ�����µ��ϣ��ͽ������µ��ϵ�ȷ��
				if (avgWidth < roadTypeConst.StraightRoadJudgeWidth - roadTypeConst.RampWayJudgeSeparate)
				{//�����������Ѿ���ú�խ��˵��ǰհ��Զ��������ú�խ�������µ�������
					ramWay.rampConfirmCount++;
				}
				else
				{
					ramWay.rampConfirmCount = 0;
				}
				if (ramWay.rampConfirmCount > roadTypeConst.RampWayConfirmC)
				{//��ȷ���µ������� 
					ramWay.IsOnRamp = true;//ȷ�����µ�����
	//				BUZ_Normal();
					ramWay.rampConfirmCount = 0;
				}
			}
			else if (ramWay.IsDownRamp == false)
			{ //���µ���ȴ��û�����£��ͼ�����µĵ���,�ͼ���Ƿ����µ���һ����.�������ڿ��ܻ���ֳ�������һ�����������Բ��ж���Ч����
				if (leftErCheng.zigma2 < roadTypeConst.StraightRoadJudgeMaxZigMa && rightErCheng.zigma2 < roadTypeConst.StraightRoadJudgeMaxZigMa//��������С��zigmaС
					&& avgWidth > roadTypeConst.StraightRoadJudgeWidth + roadTypeConst.RampWayJudgeSeparate//��������㹻��
					  )
				{
					ramWay.rampConfirmCount++;
				}
				else
				{
					ramWay.rampConfirmCount = 0;
				}
				if (ramWay.rampConfirmCount > roadTypeConst.RampWayConfirmC)
				{ //��ȷ�����µ���
					ramWay.IsDownRamp = true; //ת��������µ���
	//				BUZ_Normal();
					ramWay.rampConfirmCount = 0;
				}
			}
			else
			{ //�Ѿ����µ��ˣ����ǻ�û���뿪��Ҫȷ���뿪���µ�
				if (avgWidth < roadTypeConst.StraightRoadJudgeWidth + roadTypeConst.RampWayJudgeSeparate
					|| leftErCheng.effectLines < roadTypeConst.StraightRoadJudgeMinEffLines
					  || rightErCheng.effectLines < roadTypeConst.StraightRoadJudgeMinEffLines
	//				    || leftErCheng.zigma2 > roadTypeConst.StraightRoadJudgeMaxZigMa
	//					   || rightErCheng.zigma2 > roadTypeConst.StraightRoadJudgeMaxZigMa
						)
				{//��������Ѿ��б�խ�ˣ��Ѿ��뿪�µ� 
				  ramWay.IsUpRamp = false;
				  ramWay.IsOnRamp = false;
				  ramWay.IsDownRamp = false;
				  ramWay.rampConfirmCount = 0;
				}
			}
		}
		if (ramWay.IsUpRamp || ramWay.IsOnRamp || ramWay.IsDownRamp)//�����µ�
			ramWay.rampTimeCount++;//ͳ��ʱ��
		else//�����µ���һֱ����
			ramWay.rampTimeCount = 0;
		if (ramWay.rampTimeCount > roadTypeConst.RampWayMaxTime)
		{//���µ��ϵ�ʱ�䳬ʱ��
			ramWay.IsUpRamp = false;
			ramWay.IsOnRamp = false;
			ramWay.IsDownRamp = false;
			ramWay.rampConfirmCount = 0;
			ramWay.rampTimeCount = 0;
	//		BUZ_Normal();
		}
	}
    /////////////////
	//�µ�������
	////////////////
	
	///////////////////
	//�ϰ�ʱ�䳬ʱ��0
	///////////////////
	if(roadRightBlock.IsRoadBlock == true || roadLeftBlock.IsRoadBlock == true
	   || roadLeftBlock.BesideBlock == true)
	{
		roadLeftBlock.BlockTimeCount++;
	}
	else
	{
		roadLeftBlock.BlockTimeCount = 0;
	}
	if(roadLeftBlock.BlockTimeCount >= roadLeftBlock.BlockMaxTime)
	{
		roadRightBlock.IsRoadBlock = false;
		roadLeftBlock.IsRoadBlock = false;
		roadLeftBlock.BesideBlock = false;
		roadLeftBlock.ThroughBlock = false;
		roadLeftBlock.BlockTimeCount = 0;
		roadLeftBlock.stateChangeCount = 0;
	}
	/////////////////
	//�������
	////////////////
	
	for (line = 0; line < Image_Height; line++)
    {
        ImgClass.leftLine[line] = leftLine[line];
        ImgClass.RightLine[line] = rightLine[line];
		ImgClass.middleLine[line]= middleLine[line];
    }

    leftBuLine(heightBottom, leftErCheng.endLine);  //���ߣ�endLine�������һ����Ч��
    rightBuLine(heightBottom, rightErCheng.endLine);
	
    roadTypeAnalyze(leftErCheng.endLine, rightErCheng.endLine, leftErCheng.effectLines, rightErCheng.effectLines, leftErCheng.k, rightErCheng.k, (int16_t)leftErCheng.zigma2, (int16_t)rightErCheng.zigma2);
	push(10,roadType);
    
	//�����µ������ʮ��ʱ�����ߵ�����
	if (leftErCheng.zigma2 > 40 || rightErCheng.zigma2 > 40 || (roadType != RT_Straight))
	{
		startLinesDetect.IsStartLine = false;  
	}
	
//	if( ramWay.IsUpRamp == true || ramWay.IsOnRamp == true || ramWay.IsDownRamp == true)
//	{
//		roadLeftBlock.IsRoadBlock = false;
//		roadRightBlock.IsRoadBlock = false;
//	}
	
	//�жϵ��µ�֮�����������߼��2��
	if(ramWay.IsUpRamp == true && onlyOneDetect == 0)
	{
		onlyOneDetect = 1;
		avoidRamWayEffectTime = setdelay(2000);
	}
	else if(checkdelay(avoidRamWayEffectTime))
	{
		onlyOneDetect = 0;
		avoidRamWayEffectTime= 0;
	}
	
	//�����ϰ��������ߵ�����
	if(roadLeftBlock.IsRoadBlock == true || roadRightBlock.IsRoadBlock == true)
	{
		LED2_ON;
		startLinesDetect.IsStartLine = false;
//		avoidBlockEffectTime = setdelay(1000);
	} 
	else //if(checkdelay(avoidBlockEffectTime))
	{
		LED2_OFF;
	}

    PrePreSee = (uint8_t)MAX(leftErCheng.endLine, rightErCheng.endLine);
}
