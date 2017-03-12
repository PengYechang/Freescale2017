#include "ImgProcess.h"

#define  directSearchLines  10                   //近处与非近处分界线

float   thrOffset = 0.8;   //0.8;               //二值化补偿
uint8_t ImgMiddle = Image_Width/2-1;          //图像中间
uint8_t value = Image_Height-1;				  //
int32_t avgWidth = 0;
const int32_t FarthestDis = 120 * 120;        //14400求最远距离时避免开方运算，这么远之后如果连续丟线MaxLostJudge次就认为这条边线已经结束，不再搜索
uint8_t PrePreSee = 125;					  //前一幅图像的前瞻
static uint8_t onlyOneDetect = 0;

/*------------------------------------------------
  声明几个数组:
  中线数组，记录赛道宽度的数组
  左线数组，右线数组
 ------------------------------------------------*/
uint8_t middleLine[Image_Height];                   //中线
uint8_t recordRoadWidth[Image_Height]; 				//记录赛道宽度
uint8_t leftLine[Image_Height]; 					//左边线数组
uint8_t rightLine[Image_Height];					//右边线数组
uint8_t leftJumpLine[23][2];						//记录左线跳变点
uint8_t rightJumpLine[23][2];						//记录右线跳变点
/*-----------------------------------------------
 声明几个结构体：
 赛道类型结构体
 图像处理结构体
 二乘法结构体
 起始线结构体
 -----------------------------------------------*/
roadTypeConstClass roadTypeConst;                                           //赛道类型
rampWayClass ramWay;													    //坡道
ImgClassType ImgClass;                                                      //图像信息定义
ErChengClassType leftErCheng, rightErCheng, middleErCheng;                  //最小二乘法
startLinesType startLinesDetect;                                            //起跑线
roadblock roadLeftBlock;													//左路障
roadblock roadRightBlock;													//右路障

//求阈值，返回Thr
 uint8_t CalculateThreshold(uint8_t* src, uint16_t widthBottom, uint16_t heightBottom, uint16_t widthTop, uint16_t heightTop)
{
    uint8_t* ImgBaseAddr = src;     //获取图像储存的地址
    uint8_t* pixel = ImgBaseAddr;   //像素指针
    uint16_t PixelNum[256]={0};     //直方图点数统计表;必须清零
    int16_t Thr = 100;              //阈值初始化
    int16_t line, column;          

    int32_t p;
    int32_t total;                          	//直方图平滑化时的左右几个的综合
    int32_t TotalWeight = 0, ForeWeight = 0;    //质量矩
    int32_t TotalPixels = 0;                    //进行统计的点数和

    int32_t BackPixNum = 0, ForePixNum = 0;     //前景图像和背景图像的点数
    double BackAvgGry, ForeAvgGry;              //平均灰度值
    double fmax = -1;
    double sb;

    for(line = heightBottom; line < heightTop; line++)     //图像直方图
    {
        pixel = ImgBaseAddr + line * Image_Width;          //更新像素指针，该值为每幅图像的左边界的地址
        for(column = widthBottom; column < widthTop; column += 4)
        {
            PixelNum[*(pixel + column)]++;
        }
        line += 3;
    }
  
    for(line = 0; line < 256; line++)
    {
        total = 0;
        for(column = -2; column < 3; column++) //左右共5个做平均
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
        TotalWeight += line * PixelNum[line];             //x*f(x)质量矩，也就是每个灰度的值乘以其点数（归一化后为概率），TotalWeight为其总和
        TotalPixels += PixelNum[line];                    //total为图象总的点数，归一化后就是累积概率
    }
  
    for(line = 0; line < 256; line++)
    {
        ForePixNum += PixelNum[line];           //ForePixNum为在当前阈值遍前景图象的点数
        if (ForePixNum == 0) continue;          //没有分出前景后景
        BackPixNum = TotalPixels - ForePixNum;  //BackPixNum为背景图象的点数
        if (BackPixNum == 0) break;             //BackPixNum为0表示全部都是后景图象，与ForePixNum=0情况类似，之后的遍历不可能使前景点数增加，所以此时可以退出循环
        ForeWeight += line * PixelNum[line];    //前景图像质量矩
        ForeAvgGry = ((double)(ForeWeight)) / ForePixNum;   //前景图像平均灰度
        BackAvgGry = ((double)(TotalWeight - ForeWeight)) / BackPixNum; //背景图像平均灰度
        sb = ForePixNum * BackPixNum * (ForeAvgGry - BackAvgGry) * (ForeAvgGry - BackAvgGry);
        if (sb > fmax)
        {
            fmax = sb;
            Thr = line;
        }
    }
    return (uint8_t)Thr;
}

//将Image_Deal里面存的图像进行二值化处理，处理后的图像存入Image_Deal中
void ImgBinarization(uint8_t widthBottom, uint8_t heightBottom, uint8_t widthTop, uint8_t heightTop)    //图像二值化
{
    uint8_t threshold = CalculateThreshold(Image_Deal, widthBottom, heightBottom, widthTop, heightTop);  //计算阈值
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

//设置图像处理的一些常量
void ImgDealInit(void)
{  
    roadTypeConst.StraightRoadJudgeWidth = 28;//26;
    roadTypeConst.StraightRoadJudgeMaxZigMa = 25;     //判断为直道的最大方差
    roadTypeConst.StraightRoadJudgeMinEffLines = 135; //判断为直道的最小有效行数
    roadTypeConst.RampWayJudgeSeparate = 4;
    roadTypeConst.RampWayConfirmC = 2;                //2次检测到是某种坡道类型，就认为是某种坡道类型了，进行下一个类型检测
    roadTypeConst.RampWayMaxTime = 150;
	
	//障碍的参数初始化
	roadLeftBlock.line = 0;						 //跳变点所在的行
	roadLeftBlock.jumpRow = 0;					 //跳变点之前的列
	roadLeftBlock.continuousLine = 0;			 //是否是连续的线
	roadLeftBlock.avoidStartLineAffectThr = 3;  //判断阈值
	
	roadRightBlock.line = 0;					 //跳变点所在的行
	roadRightBlock.jumpRow = 0;					 //跳变点之前的列
	roadRightBlock.continuousLine = 0;			 //是否是连续的线
	roadRightBlock.avoidStartLineAffectThr = 3; //判断阈值
	
	roadLeftBlock.BlockMaxTime = 100;			 //过障碍时超过这个时间参数全部置0
	roadLeftBlock.stateChangeCount = 0;			 //状态转换计数
	roadLeftBlock.stateChangeCountThr = 3;       //状态转换阈值
	roadLeftBlock.BlockTimeCount = 0;
	
	roadLeftBlock.IsRoadBlock = false;				 //是否是路障
	roadLeftBlock.BesideBlock = false;
	roadLeftBlock.ThroughBlock = false;
	
	roadRightBlock.IsRoadBlock = false;				 //是否是路障
	roadRightBlock.BesideBlock = false;
	roadRightBlock.ThroughBlock = false;
	
	ramWay.IsUpRamp = false;
	ramWay.IsOnRamp = false;
	ramWay.IsDownRamp = false;
	ramWay.rampConfirmCount = 0;    //如果多次检测到是上中下破，才认为是在这个类型坡道上，开始检测下一个坡道类型
}

////无论是左线还是右线，计算的都是赛道右侧的角度
int16_t kkkMinus(int32_t x1, int32_t x2, int32_t x3, int32_t y1, int32_t y2, int32_t y3)//x是line，y是column
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

//以下为最小二乘法的参数k，b的计算
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

//补左边线
void leftBuLine(uint8_t bottom, uint8_t top)
{
    uint8_t nonEffLineFound;      //没有找到有影响的线
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
            else//存在没有补的线，而且现在找到了终点
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
                startLine = endLine;//重新设置，开始新的一次补线   
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

//补右边线
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
            else//存在没有补的线，而且现在找到了终点
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
                startLine = endLine;//重新设置，开始新的一次补线
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
	//用于边线的搜索
    uint8_t* ImgBaseAddr = Image_Deal;                //图像处理所基于的基地址
    uint8_t* pixel = ImgBaseAddr;                     //动态像素指针
    uint8_t* pixelBegin;							  //一行的像素起始位置指针
    uint8_t* pixelEnd;                                //一行的像素结束位置指针
	uint8_t i = 0;
	
	//用于计算弯道的角度  3点求角度
	uint8_t ThetaDeltaX = 7;                          //用于计算弯道的角度   
    uint8_t ThetaDelta2X = (uint8_t)(ThetaDeltaX * 2);//用于计算弯道的角度
	int16_t ThetaL,ThetaR;                            //左右线角度

	//用于检测远处孤岛行
	uint8_t minIsland = 8;                            
    uint8_t minBlank = 8;
    int16_t Eff = 0, InEff = 0, Bottom = 50;           
	
	//用于判断检测到的边线是否有效
    uint8_t leftLineEffeft, rightLineEffect, preLeftLineEffect, preRightLineEffect; //判断左右线是否有效，避免多次调用lL和rL重复判读  
    uint8_t lL, rL, lL1, rL1;                         //左线，右线，前一行左线，前一行右线

	//用于十字判断	
    uint8_t crossCountToAvoidDoubleLlinesAreEffect = 0;//程序判断如果两边都是黑色就不认为是十字。但是有时车出弯道，会误判断。因此如果多次判断是十字就跳
	uint8_t crossFound = false;                       //十字标志
	int16_t maxTheta = 230;                           //十字处的角度
	
	//用于图像处理
	uint8_t Thr;  	                                    //局部阈值
    int16_t roadWidth, realRoadWidth;                   //roadWidth是赛道宽度的一半，realRoadWidth是这一幅图像的赛道宽度的一半
    int16_t middle, NextRealMiddle, ThisRealMiddle;     //找到的前一行的赛道中心
    int16_t line, column, cCount;                       //图像行列遍历,cCount用于与赛道宽度比较
    int16_t longTimeNoLinesCount = 0;                   //标志长时间没有找到线
    
	//用于最小二乘法
    double  erM, erDelta;
    uint8_t ErEffLines = 1;   
    uint8_t EreffLines = 1;
	
    lineInfo leftInfo;                                //左边赛道信息
    lineInfo rightInfo;	                              //右线赛道信息
    ImgClass.ImgThr = (uint8_t)(thrOffset * CalculateThreshold(Image_Deal, widthBottom, heightBottom, widthTop, heightTop)); //图像阈值
	//初始化左右线数组，中线数组及赛道宽度数组
    for (line = heightBottom; line < heightTop; line++)   //左线，右线，中线全部初始化为纯白色，道路宽度为0
    {
		leftLine[line] = 255;
		rightLine[line] = 255;
		middleLine[line] = 255;
		recordRoadWidth[line] = 0;      			//记录赛道宽度
    }
	
//	//6.27添加
//	for(i=0;i<24;i++)
//	{
//		leftJumpLine[i][0]=255;
//		leftJumpLine[i][1]=255;
//		rightJumpLine[i][0]=255;
//		rightJumpLine[i][1]=255;
//	}
	
	//初始化检测的线的结构体
    leftInfo.closestLineBeforeLost.column = widthTop;    //丟线时的列号
    leftInfo.closestLineBeforeLost.deltaLine = 0;        //当前行到丟线时的行号差
    leftInfo.closestLineBeforeLost.middle = widthTop/2;  //丢线时的中线
    leftInfo.closestLineBeforeLost.line = 0;             //丢线时的行号
	
    rightInfo.closestLineBeforeLost.column = 0;           //丟线时的列号
    rightInfo.closestLineBeforeLost.deltaLine = 0;        //当前行到丟线时的行号差
    rightInfo.closestLineBeforeLost.middle = widthTop/2;  //丟线时的中线	
    rightInfo.closestLineBeforeLost.line = 0;             //丟线时的行号
	
	roadLeftBlock.onlyOneDetectFlag = 0;		 //每幅图像只检测一次的标志
	roadLeftBlock.avoidStartLineAffect = 0; 	 //避免起跑线的影响		 
	roadRightBlock.onlyOneDetectFlag = 0;		 //每幅图像只检测一次的标志
	roadRightBlock.avoidStartLineAffect = 0; 	 //避免起跑线的影响	
	
	//初始化二乘法的参数
    leftErCheng.sumX = 0;
    leftErCheng.sumX2 = 0;
    leftErCheng.sumY = 0;
    leftErCheng.sumXY = 0;
    leftErCheng.effectLines = 0;
    leftErCheng.startLine = heightTop; 
    leftErCheng.endLine = 0;
    leftErCheng.k = 99999;  //k不会这么大的
    leftErCheng.b = 9999;   //b不会这么大的
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
  
	//用于更新图像处理类中的赛道宽度和赛道中线
    middle = widthTop/2-1;         					//赛道中线（使用局部变量）
    NextRealMiddle = widthTop/2; 					//下一行赛道中线
    roadWidth = 120;     							//前几行大一些，大范围搜索
    realRoadWidth = 120; 
	
	for(line = heightBottom; line < heightTop; line++) //把每一行黑点所在列数存在leftLine[]和RightLine[]里面
	{	
		Thr = ImgClass.ImgThr;                          //获取阈值
				
	//************先搜索左边线*******************/
		cCount = 0;                              			//赛道宽度统计，到达roadWidth就认为没有线
		column = middle;                         			//中间向两边,middle在实时更新
		pixelBegin = ImgBaseAddr + line * Image_Width;      //最终限制范围，每一行的图像左边界
		pixel = pixelBegin + middle;             			//中间向两边，每一行的图像中间
		pixelEnd = pixelBegin + Image_Width;            	//最终限制范围，每一行的图像右边界

		while (true)                             			
		{
			if (*pixel < Thr && *(pixel + 1) < Thr)     	//一行中相邻两点同为黑色，则找到线所在的列
			{
				leftLine[line] = (uint8_t)column;
				break;
			}
			pixel++;
			column++;
			cCount++;
			if (pixel == pixelEnd)              //如果从中间向左搜索不到黑线，那么这行的黑点即为图像左边界
			{
				leftLine[line] = Image_Width;
				break;
			}
			else if(cCount == roadWidth) 		//没有搜索到边线,已经到了赛道宽度的范围,开始的roadWidth设置的比较大，即大范围搜索
			{
				break;  
			}
		}
		//左边线搜索完毕
		
		//开始搜索右边线
		cCount = 0;                                   //参数重置
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
				rightLine[line] = 0;          //如果从中间向右搜索不到黑线，那么这行的黑点即为图像右边界
				break;
			}
			else if (cCount == roadWidth)
			{
				break;
			}
		}
	    //右边线搜索完毕
		
		//近处搜索左边跳变点
//		if(line>1 && line<25)
//		{
//			//搜索左边跳变点
//			Judge = 0;
//			cCount = 0;                              			//赛道宽度统计，到达roadWidth就认为没有线
//			column = middle;                         			//中间向两边
//			pixelBegin = ImgBaseAddr + line * Image_Width;      //最终限制范围，每一行的图像左边界
//			pixel = pixelBegin + middle;             			//中间向两边，每一行的图像中间
//			pixelEnd = pixelBegin + Image_Width;            	//最终限制范围，每一行的图像右边界

//			while (true)                             			
//			{
//				if((*(pixel-1) > Thr && *pixel < Thr && *(pixel + 1) < Thr) && (Judge == 1))
//				{
//					leftJumpLine[line-2][1] = (uint8_t)column;					//搜到第二个跳变点
//					Judge = 0;
//					break;
//				}
//				
//				if ((*pixel < Thr && *(pixel + 1) < Thr) && (Judge == 0))     	//一行中相邻两点同为黑色，则找到线所在的列
//				{
// 					leftJumpLine[line-2][0] = (uint8_t)column;
//					pixel+=5;
//					column+=5;
//					cCount+=5;
//					if(pixel == pixelEnd)break;
//					Judge = 1;													//搜到一个跳变点
//				}

//				pixel++;
//				column++;
//				cCount++;
//				
//				if (pixel == pixelEnd)              //如果从中间向左搜索不到黑线，那么这行的黑点即为图像左边界
//				{
//					if(Judge == 1)					//只搜到一个跳变点,Judge等于0要么是都搜到要么是都没搜到
//					{
//						leftJumpLine[line-2][0] = 255;
//						leftJumpLine[line-2][0] = 255;
//					}
//					break;
//				}
//				else if(cCount == 95) 				//近处最大宽度大概是80
//				{
//					if(Judge == 1)					//只搜到一个跳变点,Judge等于0要么是都搜到要么是都没搜到
//					{
//						leftJumpLine[line-2][0] = 255;
//						leftJumpLine[line-2][0] = 255;
//					}
//					break;  
//				}
//			}
//			
//			//近处搜索右边跳变点
//			cCount = 0;                                   //参数重置
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
//					if(Judge == 1)					//只搜到一个跳变点,Judge等于0要么是都搜到要么是都没搜到
//					{
//						rightJumpLine[line-2][0] = 255;
//						rightJumpLine[line-2][0] = 255;
//					}
//					break;
//				}
//				else if (cCount == 95)
//				{
//					if(Judge == 1)					//只搜到一个跳变点,Judge等于0要么是都搜到要么是都没搜到
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
		
		//使用局部变量，提高运算速度   
        lL = leftLine[line]; 		//左边线
        rL = rightLine[line];		//右边线
        leftLineEffeft = (lL > 0 && lL < (Image_Width-1)) ? true : false;	//true:有线,false:图像边沿或者没有找到线
        rightLineEffect = (rL > 0 && rL < (Image_Width-1)) ? true : false;	//即判断这一行是否有右边线
        if (line < 45)                     								    //对于不同的行数范围，对于判断十字处的的角度不同
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
		
		ThisRealMiddle = NextRealMiddle;   	//更新这一行扫描的中线，便于之后的处理

		//////////////////////////////////////////////////////////////////////////////////////////////
		//比较该行数据与前一行数据，由此来判断该行数据是否可用，但是无法滤掉连续的不正常点
		//////////////////////////////////////////////////////////////////////////////////////////////

        if (line > heightBottom) 			//大于heightBottom，这样才能进行滤线判断
        {
            lL1 = leftLine[line - 1];       //前一行的左右线所在位置
            rL1 = rightLine[line - 1];
            preLeftLineEffect = (lL1 > 0 && lL1 < (Image_Width-1)) ? true : false;      //pre代表现在处理的前一行是不是有左边线
            preRightLineEffect = (rL1 > 0 && rL1 < (Image_Width-1)) ? true : false;     //pre代表现在处理的前一行是不是有右边线
        
            if (preLeftLineEffect && leftLineEffeft)    //相邻的两行都有
            {
                if (line > 100 && MyAbs(lL - lL1) > 7)  //如果正在处理的这一行行数大于100且这一行黑点所对应的列数和前一行黑点所对应的列数之差大于7
                {
                    lL = 255;               			//满足上述条件后，那么这一行设置为没有线
                    leftLine[line] = 255;  				//因为初始化设置为255，所以在这里也设置为255
                    leftLineEffeft = false; 			//无左边线
                }
            }
        
            if (preRightLineEffect && rightLineEffect)   //相邻的两行都有
            {
                if (line > 100 && MyAbs(rL - rL1) > 7 )  //如果正在处理的这一行行数大于100且这一行黑点所对应的列数和前一行黑点所对应的列数之差大于7
                {
                    rL = 255;                            //满足上述条件后1，那么这一行设置为没有线
                    rightLine[line] = 255;               //因为初始化设置为255，所以在这里也设置为255
                    rightLineEffect = false;             //无右边线
                }
            }
			
			////////////////////////////////////////////////////////////////////////////////
			//障碍检测   100行以内进行检测
		    //问题：不知道判断到的时间，判断到的时间会不会太迟了，导致舵机来不及反应
			//车出弯要正，不正的话，检测的比较迟
			///////////////////////////////////////////////////////////////////////////////
			if(roadRightBlock.IsRoadBlock == false && roadLeftBlock.IsRoadBlock == false )
			{
				if(preLeftLineEffect && leftLineEffeft && rightLineEffect && preRightLineEffect)
				{
					if(roadRightBlock.onlyOneDetectFlag == 0)		           //每幅图像只检测一次标志位
					{
						if(((lL1-lL)<2 || (lL-lL1)<2)  && MyAbs(lL1-lL)>=0 &&(rL - rL1)>30 &&  (line - leftErCheng.effectLines)<6)  //左线没有大的跳变  右线有跳变  并且车在直道上要正
						{ 
							roadRightBlock.onlyOneDetectFlag = 1;
							roadRightBlock.line = line;						   //跳变点所在列
							roadRightBlock.continuousLine = line - 1;		   //应该是连续的跳变
							roadRightBlock.jumpRow = rL1;		               //跳变的那一行所采集的点所在的列
						}
					}
					if(roadRightBlock.onlyOneDetectFlag == 1)				   //检测到跳变点之后进一步判断避免起跑线影响
					{
						if(((rL - roadRightBlock.jumpRow)>25)&&((line - roadRightBlock.continuousLine) == 1)) //跳变之后的右线会连续靠向中间
						{
							roadRightBlock.avoidStartLineAffect++;            
						}
						else
						{
							roadRightBlock.onlyOneDetectFlag = 0;
							roadRightBlock.avoidStartLineAffect = 0;
						}
						if(roadRightBlock.avoidStartLineAffect > roadRightBlock.avoidStartLineAffectThr) //起跑线处没有这么多的连续跳变点
						{
							roadRightBlock.IsRoadBlock = true;				 //发现右障碍
						}
						roadRightBlock.continuousLine++;   					 //连续的行有跳变才行
					}
					///////////////////////
					//右障碍检测完毕
					///////////////////////
					
					//////////////////////
					//左障碍开始检测
					//////////////////////
					if(roadLeftBlock.onlyOneDetectFlag == 0)                //每幅图像只会检测一次
					{
						if(((rL-rL1)<2 || (rL1-rL)<2) && MyAbs(rL-rL1)>=0 &&(lL1 - lL)>30 && (line - rightErCheng.effectLines)<6) //右线没有大的跳变，左边有大跳变
						{
							roadLeftBlock.onlyOneDetectFlag = 1;
							roadLeftBlock.line = line;						//跳变点所在列
							roadLeftBlock.continuousLine = line - 1; 		//应该是里连续的跳变
							roadLeftBlock.jumpRow = lL1;					//跳变的那一行所采集的点所在的列
						}
					}
					if(roadLeftBlock.onlyOneDetectFlag == 1)				//检测到跳变点之后进一步判断避免起跑线影响
					{
						if(((roadLeftBlock.jumpRow - lL)>25)&&(line - roadLeftBlock.continuousLine) == 1) //跳变之后的列数会向中间靠
						{
							roadLeftBlock.avoidStartLineAffect++;
						}
						else
						{
							roadLeftBlock.onlyOneDetectFlag = 0;
							roadLeftBlock.avoidStartLineAffect = 0;
						}

						if(roadLeftBlock.avoidStartLineAffect > roadLeftBlock.avoidStartLineAffectThr)  //起跑线处没有这么多的连续跳变点
						{
							roadLeftBlock.IsRoadBlock = true;		//发现左障碍
						}
						roadLeftBlock.continuousLine++;
					}
					///////////////////////
					//左障碍检测完毕
					///////////////////////				
				}
			}
			else			  //已经确认有障碍了
			{
				if(roadLeftBlock.BesideBlock == false)   //判断是否在障碍旁
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
				else if(roadLeftBlock.ThroughBlock == false) //在确认在障碍旁之后判断是否通过障碍了
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
			//障碍检测结束
			////////////////

			//////////////////////////////////////
			///////记录赛道信息
			/////////////////////////////////
			
            if ((leftLineEffeft && rightLineEffect) || (line < directSearchLines))  //近处或者左右线都存在，记录左右线的信息
            {
                longTimeNoLinesCount -= 3;                                          //longTimeNoLinesCount初始值为0，这里自减3
                if (longTimeNoLinesCount < 0) longTimeNoLinesCount = 0;             //如果小于0，则为0
                rightInfo.closestLineBeforeLost.column = rL;                        //记录右边线
                rightInfo.closestLineBeforeLost.deltaLine = 0;                      //deltaline = 0，即丢线时的行数到这一行的行数为0
                rightInfo.closestLineBeforeLost.line = (uint8_t)line;               //line = 这一行行数
//                rightInfo.closestLineBeforeLost.middle = (uint8_t)NextRealMiddle;   //middle = 这一行中间列数

                leftInfo.closestLineBeforeLost.column = lL;                         
                leftInfo.closestLineBeforeLost.deltaLine = 0;
                leftInfo.closestLineBeforeLost.line = (uint8_t)line;
//                leftInfo.closestLineBeforeLost.middle = (uint8_t)NextRealMiddle;
                crossFound = false;													//10行以内不搜索十字
            }
            else if (rightLineEffect)                               //只有右线，判断是否需要记录左线上一行信息
            {
                if (longTimeNoLinesCount > 0) longTimeNoLinesCount--;       
                if (preLeftLineEffect && leftLineEffeft == false)   //左线相邻两行一个有黑点，一个没有时，即有右线的情况下，左线突然没有了
                {
                    leftInfo.closestLineBeforeLost.column = lL1;                        //这一行突然没左线了，但左线列数取上一行的左线，中心就不会发生突变
                    leftInfo.closestLineBeforeLost.deltaLine = 0;                       //根据判断条件，这条线丢了，那么丢线时的行数到这一行的行数为0
                    leftInfo.closestLineBeforeLost.line = (uint8_t)line;                //记录左线有无的临界行数
//                    leftInfo.closestLineBeforeLost.middle = (uint8_t)NextRealMiddle;    //记录本行中间列数
                }
				
                leftInfo.closestLineBeforeLost.deltaLine = (uint8_t)(line - leftInfo.closestLineBeforeLost.line);   //用来记录左线丢了多少线
            }
            else if (leftLineEffeft)    				//只有左线，判断是否需要记录右线上一行的信息
            {
                if (longTimeNoLinesCount > 0) longTimeNoLinesCount--;
                if (preRightLineEffect && rightLineEffect == false)  //右线相邻两行一个有黑点，一个没有时，即有左线的情况下，右线突然没有了
                {
                    rightInfo.closestLineBeforeLost.column = rL1;
                    rightInfo.closestLineBeforeLost.deltaLine = 0;
                    rightInfo.closestLineBeforeLost.line = (uint8_t)line;
//                    rightInfo.closestLineBeforeLost.middle = (uint8_t)NextRealMiddle;
                }
				
                rightInfo.closestLineBeforeLost.deltaLine = (uint8_t)(line - rightInfo.closestLineBeforeLost.line);  //用来记录右线丢了多少线
            }
            else    //左右线都没有，且行数大于10
            {
                if (longTimeNoLinesCount < 25) longTimeNoLinesCount++;  //自加1
                if (preLeftLineEffect)  //如果上一行左线有黑点
                {
                    leftInfo.closestLineBeforeLost.column = lL1;            //左线丢线前列数
                    leftInfo.closestLineBeforeLost.deltaLine = 0;       
                    leftInfo.closestLineBeforeLost.line = (uint8_t)line;    //左线丢线前行数
//                    leftInfo.closestLineBeforeLost.middle = (uint8_t)NextRealMiddle;
                }
                if (preRightLineEffect)  //如果上一行右线有黑点
                {
                    rightInfo.closestLineBeforeLost.column = rL1;
                    rightInfo.closestLineBeforeLost.deltaLine = 0;
                    rightInfo.closestLineBeforeLost.line = (uint8_t)line;
//                    rightInfo.closestLineBeforeLost.middle = (uint8_t)NextRealMiddle;
                }
                leftInfo.closestLineBeforeLost.deltaLine = (uint8_t)(line - leftInfo.closestLineBeforeLost.line);   //记录左线丢了多少线
                rightInfo.closestLineBeforeLost.deltaLine = (uint8_t)(line - rightInfo.closestLineBeforeLost.line); //记录右线丢了多少线
            }
			
		//////////////////////////////
		///////赛道记录完毕        
		/////////////////////////////
        }
      
        if (leftLineEffeft)  //如果左线存在，记为有效行
        {
            leftErCheng.effectLines++;
        }
        if (rightLineEffect) //如果右线存在，记为有效行
        { 
            rightErCheng.effectLines++;
        }
		
        //以下程序由当前赛道的边线计算下次用的中心和当前赛道宽度，推算下次扫描范围
        if (line > directSearchLines) 	//非近处，就要开始考虑十字
        {
			/*---------------十字识别程序-------------*/
            if (line < (Image_Height-1) && line > ThetaDelta2X) //14到144行执行十字识别程序，127行执行这个程序(15~142)
            {
				/////////////////////////////////////
				//根据该行之前的14行，计算赛道角度
				////////////////////////////////////
				
                ThetaL = 0;
                ThetaR = 0;
                //遍历该行前的14行，看是否可以用于计算角度，如果14行里有一个不可用就不能计算
                i = line - ThetaDelta2X;    
                for (; i <= line; i++) if (leftLine[i] == 0 || leftLine[i] > (Image_Width-1)) { ThetaL = 1000; break; } //14行中间有丢线，不进行角度计算
                //遍历该行前的14行，看是否可以用于计算角度，如果14行里有一个不可用就不能计算
                i = line - ThetaDelta2X;
                for (; i <= line; i++) if (rightLine[i] == 0 || rightLine[i] > (Image_Width-1)) { ThetaR = 1000; break; } //14中间有丢线，不进行角度计算
				
				//赛道角度可以计算
                if (ThetaL < 1)
                {
                    //十字空白内侧的线通常比较长
                    if (longTimeNoLinesCount == 0)//两边线还未丢或者已经通过十字
                    {
                        ThetaL = kkkMinus(line, line - ThetaDeltaX, line - ThetaDelta2X, leftLine[line], leftLine[line - ThetaDeltaX], leftLine[line - ThetaDelta2X]);
                    }
                    else   
                    {
                        ThetaL = kkkMinus(line - ThetaDelta2X, line - ThetaDeltaX, line, leftLine[line - ThetaDelta2X], leftLine[line - ThetaDeltaX], leftLine[line]);
                    }
                }
          
                if (ThetaR < 1)//这14行中，右线都没丢，那么算ThetaR
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
                if (ThetaR < 360) ThetaR = (int16_t)(360 - ThetaR);     //进行修正
				
				/////////////////////////////////
				//赛道角度计算完毕
				////////////////////////////////				
				
				////////////////////////////
				////十字赛道的识别
				////////////////////////////
                //如果ThetaL在230到360之间，或者ThetaR在230到360之间，即有十字出现，一边出现这种十字情况就行
                if ((ThetaL > maxTheta && ThetaL < 360 )||( ThetaR > maxTheta && ThetaR < 360)) 
                {
                    if (crossCountToAvoidDoubleLlinesAreEffect < 10)    //十字累加行数自加1
						crossCountToAvoidDoubleLlinesAreEffect++;
                }
                else
                {
                    crossCountToAvoidDoubleLlinesAreEffect = 0;         //没有十字或者十字已过
                }
				
                //如果在十字处理中，左横线出现右横线不存在，或右横线出现左横线不存在
                if(((ThetaL > maxTheta && ThetaL < 360 )&& (rightInfo.closestLineBeforeLost.deltaLine > 2 || line < 50 || crossCountToAvoidDoubleLlinesAreEffect > 4))
                    ||(( ThetaR > maxTheta && ThetaR < 360 )&& (leftInfo.closestLineBeforeLost.deltaLine > 2 || line < 50 || crossCountToAvoidDoubleLlinesAreEffect > 4)))
                {
					//由于图像畸变，所以在入十字时，会采集到一点点左和右横线，以下代码就是去除这个点
                    if (longTimeNoLinesCount > 0)
                    {
                        if (ThetaL > maxTheta && ThetaL < 360)  //左边有横线，左边前5点置白
                        {
                            for (i = 5; i < 10; i++) leftLine[line - i] = 255;//十字拐点之前的5个点,为白色
                        }
                        else                                   //右边有横线，右边5点置白
                        {
                            for (i = 5; i < 10; i++) rightLine[line - i] = 255;
                        }
                    }
                    else      
                    {
                        if (ThetaL > maxTheta && ThetaL < 360)   //如果有左横线
                        {
                            for (i = 0; i < 5; i++) leftLine[line - i] = 255;//当前点之前的5个点，理论上认为是拐点之后的5个点
                        }
                        else
                        {
                            for (i = 0; i < 5; i++) rightLine[line - i] = 255;
                        }
                    }
            
                    if(ThisRealMiddle < 30 || ThisRealMiddle > 170)    //是十字且足够触及边界，不再搜索，因为这里一般是弯道入十字。用近处，远处看不见
                    {
                        break;  	//直接跳出本次搜索，进行下一行搜索
                    }
					
                    if(longTimeNoLinesCount < 2)   //刚进入十字
                    {
						///////////////////////////////
						/////二乘法预测下一行的中点
						//////////////////////////////
                        if(line > 30)
                        {
                            middleErCheng.effectLines = 0;
                            middleErCheng.sumX = 0;
                            middleErCheng.sumX2 = 0;
                            middleErCheng.sumY = 0;         //middle还是上一行的值，应该用现在这行的，就是NextRealMiddle
                            middleErCheng.sumXY = 0;
                            i = line - ThetaDelta2X - 20;   
                            if (i < heightBottom) i = heightBottom;
                            for (; i < line - ThetaDeltaX; i++)//取正在处理的这一行前34行到前14行的20行，进行二乘法预测
                            {
                                if (middleLine[i] > 0 && middleLine[i] < Image_Width)   //第一行处理时，MiddleLine[i]=255,不会进这个判断，如果正在处理的这一行有中点
                                {
                                    middleErCheng.effectLines++;
                                    middleErCheng.sumX += i;
                                    middleErCheng.sumX2 += i * i;
                                    middleErCheng.sumY += middleLine[i];         //middle还是上一行的值，应该用现在这行的，就是NextRealMiddle
                                    middleErCheng.sumXY += i * middleLine[i];
                                }
                            }
                            if (middleErCheng.effectLines > 10)//20行中至少有10行有效，那么就可以根据这一次的数据预测下一行的中点
                            {
                                middleErCheng.k = ErChengK(middleErCheng.effectLines, middleErCheng.sumX, middleErCheng.sumX2, middleErCheng.sumXY, middleErCheng.sumY);
                                middleErCheng.b = ErChengB(middleErCheng.effectLines, middleErCheng.sumX, middleErCheng.sumX2, middleErCheng.sumXY, middleErCheng.sumY);//ImgClass.RealMiddle;

                                NextRealMiddle = (int16_t)(middleErCheng.k * (line) + middleErCheng.b);
                            }
                            else    //20行中少于10行有效，那么不可以根据这一次的数据预测下一行的中点，只能用以前的数据预测下一行的中点
                            {
                                NextRealMiddle = (int16_t)((middleLine[line - 3] + middleLine[line - 4]) / 2);
                            }
                        }
                        else//如果行数小于30，，只能用以前的数据预测下一行的中点
                        {
                            NextRealMiddle = (int16_t)((middleLine[line - 3] + middleLine[line - 4]) / 2);
                        }
              
						//此时因为是预测出来的中线，所以赛道宽度保持上一次的值
                        realRoadWidth = ImgClass.realRoadWidth;
                        roadWidth = (int16_t)(ImgClass.realRoadWidth + 10);	 //在算出来的赛道宽度的基础上扩宽10,以免有线时却无法搜到
                        ImgClass.RealMiddle = NextRealMiddle;       //推测出来的
						
						
                        middle = NextRealMiddle;                //middle在实时更新
                        i = line;
						
						/////////////////////////
						////二乘法曲线拟合完毕
						/////////////////////////
						
                        crossFound = true;          //发现十字	
                        if (line < 50)      //行数越小，看到的空白处行数越多，跳过一定行数继续搜索
                        {
                            line += 30;
                            longTimeNoLinesCount = 25;//应该认为是十字空白前的直角
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
              
                        for (; i <= line; i++) if (i < heightTop) middleLine[i] = (uint8_t)NextRealMiddle;      //十字处补中线
                    }
                    continue;  	 //即只要出现横线就不断进行边线搜索，直到能判断到十字
                }
            }
			////////////////////////
			//十字识别程序结束
			///////////////////////

			////////////////////////////////////
			//计算赛道宽度和赛道中间线
			///////////////////////////////////
            //在行数大于10的情况下
            if (longTimeNoLinesCount == 0 && (leftLineEffeft || rightLineEffect))//真正找到线且左右至少有一条线在，就不能直接使用中心的k和b
            {
                middleErCheng.k = 99999;
            }
			
			ImgClass.preRoadWidth = realRoadWidth;			//上一行的赛道宽度
			
            //双线都存在或者只有一边线存在时
            if (longTimeNoLinesCount == 0 && leftLineEffeft && rightLineEffect)  //两边有线rL != 255 && lL != 255
            {
				NextRealMiddle = (int16_t)((rL + lL) / 2);    		   //下一行的搜线开始位置
				realRoadWidth = (int16_t)((lL - rL) / 2);			   //赛道宽度的一半
                ImgClass.realRoadWidth = realRoadWidth;
                ImgClass.RealMiddle = NextRealMiddle;   
            }
            else if (longTimeNoLinesCount == 0 && rightLineEffect) 		   //仅仅是右边有线
            {

                //NextRealMiddle = (int16_t)((rightInfo.closestLineBeforeLost.middle + (rL - rightInfo.closestLineBeforeLost.column) / 1));//丟线时的中心加上相对偏移
                NextRealMiddle = (int16_t)((leftInfo.closestLineBeforeLost.column + rL) / 2); //丟线时的中心加上相对偏移
                realRoadWidth = (int16_t)((NextRealMiddle - rL));

                ImgClass.realRoadWidth = realRoadWidth;//必然能得到一个正确的赛道宽度和赛道中心，所以记录它们
                ImgClass.RealMiddle = NextRealMiddle;
            }
            else if (longTimeNoLinesCount == 0 && leftLineEffeft)   //仅仅是左边有线
            {
                //NextRealMiddle = (int16_t)(leftInfo.closestLineBeforeLost.middle + (lL - leftInfo.closestLineBeforeLost.column) / 1);// 丟线时的中心加上相对偏移量
                NextRealMiddle = (int16_t)((lL + rightInfo.closestLineBeforeLost.column) / 2);// 丟线时的中心加上相对偏移量
                realRoadWidth = (int16_t)(lL - NextRealMiddle);                               //中心减去边线,是赛道宽度的一半
                ImgClass.realRoadWidth = realRoadWidth;                                       //必然能得到一个正确的赛道宽度和赛道中心，所以记录它们
                ImgClass.RealMiddle = NextRealMiddle;

            }
            else if (crossFound == false && longTimeNoLinesCount < 25)  //无十字，并且没有线的行数小于25
            {
                NextRealMiddle = (int16_t)((leftInfo.closestLineBeforeLost.column + rightInfo.closestLineBeforeLost.column) / 2);
                realRoadWidth = (int16_t)((leftInfo.closestLineBeforeLost.column - rightInfo.closestLineBeforeLost.column) / 2); 
                ImgClass.realRoadWidth = realRoadWidth;//必然能得到一个正确的赛道宽度和赛道中心，所以记录它们
                ImgClass.RealMiddle = NextRealMiddle;
            }
            else		//两边都没搜到线，就用最小二乘法预测
            {
                if (middleErCheng.k > 99998)
                {//丟线了，只计算一次，直到再次有线
                    middleErCheng.effectLines = 0;
                    middleErCheng.sumX = 0;
                    middleErCheng.sumX2 = 0;
                    middleErCheng.sumY = 0;         
                    middleErCheng.sumXY = 0;
                    if (line < 20)                              //如果Line<20,那么i=line-10；
					{
						i = (int16_t)(line - directSearchLines);
					}
                    else                                        //如果line>20,那么i=line-20；
					{
						i = (int16_t)(line - 20);
					}
                    if (i < heightBottom)                       //保证i最小是heightBottom+2；
					{
						i = heightBottom + 2;
					}
                    for (; i < line; i++)   //line的前n行
                    {
                        if (middleLine[i] > 0 && middleLine[i] < Image_Width)   //待处理行中线正常时
                        {
                            middleErCheng.effectLines++;
                            middleErCheng.sumX += i;
                            middleErCheng.sumX2 += i * i;
                            middleErCheng.sumY += middleLine[i];         //middle还是上一行的值，应该用现在这行的，就是NextRealMiddle
                            middleErCheng.sumXY += i * middleLine[i];
                        }
                    }
                    middleErCheng.k = ErChengK(middleErCheng.effectLines, middleErCheng.sumX, middleErCheng.sumX2, middleErCheng.sumXY, middleErCheng.sumY);
                    middleErCheng.b = ErChengB(middleErCheng.effectLines, middleErCheng.sumX, middleErCheng.sumX2, middleErCheng.sumXY, middleErCheng.sumY);
                }
                
                if (line > 30 && (leftErCheng.effectLines > 30 || rightErCheng.effectLines > 30) && middleErCheng.k < 99998)    //正常行数大于30时
                {
                    NextRealMiddle = (int16_t)(middleErCheng.k * (line) + middleErCheng.b);
                }
                else
                {
                    NextRealMiddle = Image_Width/2;
                }
                
                realRoadWidth = ImgClass.realRoadWidth;//必然能得到一个正确的赛道宽度和赛道中心，所以记录它们
                ImgClass.RealMiddle = NextRealMiddle;
            }
            //最终定值，整个函数都在求这两个值
			middleLine[line] = (uint8_t)NextRealMiddle;//不管是推算出来的还是计算出来的都认为是正确的
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

			//起跑线处理	 
			if (
				(roadType == RT_Straight || roadType == RT_CurveToStraight || roadType == RT_StraightToCurve) //只有在赛道是直道或出入直道
			    &&  PrePreSee > 100
				&& startLinesDetect.IsStartLine == false && line < 120           //如何该幅图像没有处理过起跑线而且赛道变窄了
                && roadLeftBlock.IsRoadBlock == false && roadRightBlock.IsRoadBlock == false  //不是障碍
				&& 
				(realRoadWidth < startLinesDetect.roadWidth  //足够小的赛道
				 || line<80 && ((MyAbs(lL - ThisRealMiddle) < 15 || MyAbs(rL - ThisRealMiddle) < 15)))//近处中心线很近
				)
			{
				int16_t leftCloseEff = 0, rightCloseEff = 0,count = 0;
				i = line - 15;
				if (i < heightBottom) i = heightBottom;
				for (; i < line - 4; i++)
				{
					if (leftLine[i] > 0 && leftLine[i] < Image_Width) leftCloseEff++;   //起跑线在直道，接近起跑线的地方肯定很多信息
				    if (rightLine[i] > 0 && rightLine[i] < Image_Width) rightCloseEff++;//
					if (middleLine[i] > 0 && middleLine[i] < Image_Width)//对近处中心统计
					{
						startLinesDetect.avgMiddle += middleLine[i];
						count++;
					}
				}
				if(count != 0)
				{
					startLinesDetect.avgMiddle /=count;
				}
			
				if (((leftCloseEff > 6 && rightCloseEff > 6) || (line < 50)))//赛道中心不是很偏离
				{
					if (line < 120) 
					{
						startLinesDetect.IsStartLine = true;
						startLinesDetect.line = (uint8_t)line; //方便计算停车延时时间
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
						line += 5;//跳过
					}
					else if (line > 60)
					{
						line += 10;//跳过
					}
					else
					{
						line += 15;//跳过
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
		    //起跑线处理结束
			////////////////////
		  
            //限制搜索范围
            if(NextRealMiddle < 1) NextRealMiddle = 1;
            else if(NextRealMiddle > (Image_Width-2)) NextRealMiddle = (Image_Width-2);
          
            if (realRoadWidth < 0)
                realRoadWidth = 0;
            else if (realRoadWidth > 120) realRoadWidth = 120;

            roadWidth = (int16_t)(realRoadWidth + 10 );  //在此处更新，用于边线的搜索
            middle = NextRealMiddle;
        }
        else      										//line <= directSearchLines,近处若干行
        {
            NextRealMiddle = (uint8_t)((rL + lL) / 2);            //根据这次的边线值预测下一行的赛道中心
            realRoadWidth = (int16_t)((lL - rL) / 2);
            middleLine[line] = (uint8_t)NextRealMiddle;           //不管是推算出来的还是计算出来的都认为是正确的
            recordRoadWidth[line] = (uint8_t)realRoadWidth;
            ImgClass.realRoadWidth = realRoadWidth;               //仅当被正常改变是才记录
            ImgClass.RealMiddle = NextRealMiddle;
            roadWidth = 120;   //前几行大一些，大范围搜索
            if (lL - rL < 45)  //起跑线在特别近的时候跳过去
            {
                for (i = 0; i < 30; i++)
                {
                    leftLine[i] = Image_Width;
                    rightLine[i] = 0;
                    middleLine[i] = Image_Width/2;//不管是推算出来的还是计算出来的都认为是正确的
                    recordRoadWidth[i] = 100;
                }
                NextRealMiddle = Image_Width/2;
                roadWidth = 100;
                line += 15;
            }
        }
        if (realRoadWidth < 5                 //实际的道路宽度小于一定值
          || ((heightTop - line < 5) && (!leftLineEffeft && !rightLineEffect))
            || (ThisRealMiddle < 4 || ThisRealMiddle > (Image_Width-4))  //太靠近边线了
              || (leftLineEffeft && MyAbs(lL - ThisRealMiddle) < 5)  	 //到了左边界
                || (rightLineEffect && MyAbs(rL - ThisRealMiddle) < 5) 	 //到了右边界
                  /*|| lL == rL && lL != 255 && dis > FarthestDis*/) //远处，lL和rL相等，但不是255才出来
        {
            break;//实际路的宽度小于一定值，搜索到边界，左右边重合
        }
    }
  /**********************************************黑线提取完成*****************************************************/
  /**********************************************黑线提取完成*****************************************************/
  
  
  /*******************************边线提取完成，下面是最小二乘法，以及误差计算***************************************/
  /*******************************边线提取完成，下面是最小二乘法，以及误差计算***************************************/
  /*******************************边线提取完成，下面是最小二乘法，以及误差计算***************************************/
  
  //******************去除远处孤岛行*************/
  //******************去除远处孤岛行*************/
  //******************去除远处孤岛行*************/
  //从远处向近处搜索
    for (i = heightTop - 1; i > Bottom; i--)//50行之后
    {//查找孤岛行
        value--; 
        lL = leftLine[i];
        leftLineEffeft = (lL > 0 && lL < (Image_Width-1)) ? true : false;
        if (leftLineEffeft)//查找少量线前的一大块空白
        {
            Eff++;
            InEff = 0;
            lL1 = leftLine[i - 1];
            if (Eff == 1)
			{
				if (MyAbs(lL - lL1) > 4 || (lL1 == 0 || lL1 > (Image_Width-1)))
				{
					leftLine[i-1] = 255;//删除最后一个不正常点
				}
			}
        }
        else
        {
            InEff++;
        }
		
        if (Eff >= minIsland) break; 		//已经没有必要再搜索了，minIsland为8
        if (i < 100) minBlank = 10; else if (i < 120) minBlank = 7; else minBlank = 4;
        if (Eff > 0 && Eff < minIsland && InEff > minBlank)
        {//若发现少量线前的空白块，说明那点少量线是不正常的
            for (line = (int16_t)i; line < heightTop; line++)
            {
                leftLine[line] = 255;
            }
            break;//只查找一次
        }
    }
    Eff = 0;
    InEff = 0;
    for (i = heightTop - 1; i > Bottom; i--)
    {//查找孤岛行
        rL = rightLine[i];
        rightLineEffect = (rL > 0 && rL < (Image_Width-1)) ? true : false;
        if (rightLineEffect)//查找少量线前的一大块空白
        {
            Eff++;
            InEff = 0;
            rL1 = rightLine[i - 1];
            if (Eff == 1)
                if (MyAbs(rL - rL1) > 4 || (rL1 == 0 || rL1 > (Image_Width-1))) rightLine[i] = 255;//删除最后一个不正常点
        }
        else
        {
            InEff++;
        }
        if (Eff >= minIsland) break; //已经没有必要再搜索了
        if (i < 100) minBlank = 10; else if (minBlank < 120) minBlank = 7; else minBlank = 4;
        if (Eff > 0 && Eff < minIsland && InEff > minBlank)
        {//若发现少量线前的空白块，说明那点少量线是不正常的
            for (line = (int16_t)i; line < heightTop; line++)
            {
                rightLine[line] = 255;
            }
            break;//只查找一次
        }
    }
	
	/////////////
    //清理杂点
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
  //*********************以下进行整幅图像的最小二乘法***************************/
  //*********************以下进行整幅图像的最小二乘法***************************/

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
    }//求和结束
  
    leftErCheng.AvgEndLine = (leftErCheng.effectLines > 0 ? (uint8_t)(leftErCheng.sumX / leftErCheng.effectLines) : (uint8_t)0);   //用这个判断左右线前瞻的大小，谁大用谁
    rightErCheng.AvgEndLine = (rightErCheng.effectLines > 0 ? (uint8_t)(rightErCheng.sumX / rightErCheng.effectLines) : (uint8_t)0);   //用这个判断左右线前瞻的大小，谁大用谁

    if (leftErCheng.effectLines > 4)
    {
        leftErCheng.k = ErChengK(leftErCheng.effectLines, leftErCheng.sumX, leftErCheng.sumX2, leftErCheng.sumXY, leftErCheng.sumY);
        leftErCheng.b = ErChengB(leftErCheng.effectLines, leftErCheng.sumX, leftErCheng.sumX2, leftErCheng.sumXY, leftErCheng.sumY);
        //********************计算方差和平均值*******************/
		leftErCheng.avgLine = 0;//最有效行
		leftErCheng.zigma2 = 0; //中心偏差
		
		for (line = heightBottom; line < heightTop; line++)
		{
			lL = leftLine[line];
			if (lL > 0 && lL < Image_Width)
			{
				ErEffLines++;
				erM = (leftErCheng.k * line + leftErCheng.b);  //二乘法拟合的值
				erDelta = lL - erM;                            //实际值与拟合值的偏差
				leftErCheng.avgLine += (int16_t)erDelta;       //偏差总和
				leftErCheng.zigma2 += erDelta * erDelta;       //偏差的平方
			}
		}
		leftErCheng.zigma2 /= ErEffLines;
		leftErCheng.avgLine *= 100;
		leftErCheng.avgLine /= ErEffLines;
    }
    else //可用的行数太少
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
	//坡道检测
	//有时候不能检测到，需要完善
	///////////////////////////
	int16_t effLines = 1;
	for (line = 88; line < heightTop; line++)			//求远处的平均赛道宽度
	{
		lL = leftLine[line];
		rL = rightLine[line];
		if (lL > 0 && lL < Image_Width && rL > 0 && rL < Image_Width)
		{
			avgWidth += recordRoadWidth[line];
			effLines++;
		}
	}
	avgWidth /= effLines;  //得到平均宽度 
	
	//有坡道时，远处的赛道宽度和直道上有所不同
	//状态转换：上坡道-->在坡道-->下坡道
	//在经过坡道时，使用平均偏差来给舵机控制
	//速度也会降
	//车如果出弯的时候考进边界，会导致检测不到
	if(RAMWAY_DETECT == true)
	{
		if (ramWay.IsUpRamp == false) //没有检测到已经上坡道
		{
			if (((leftErCheng.effectLines > roadTypeConst.StraightRoadJudgeMinEffLines && rightErCheng.effectLines > roadTypeConst.StraightRoadJudgeMinEffLines)//有效行数足够
				||((leftErCheng.effectLines > roadTypeConst.StraightRoadJudgeMinEffLines) && (rightErCheng.effectLines > roadTypeConst.StraightRoadJudgeMinEffLines -30))
				||((leftErCheng.effectLines > roadTypeConst.StraightRoadJudgeMinEffLines - 30) && (rightErCheng.effectLines > roadTypeConst.StraightRoadJudgeMinEffLines)))
				&& leftErCheng.zigma2 < roadTypeConst.StraightRoadJudgeMaxZigMa && rightErCheng.zigma2 < roadTypeConst.StraightRoadJudgeMaxZigMa//赛道畸变小，zigma小
				&& avgWidth > roadTypeConst.StraightRoadJudgeWidth + roadTypeConst.RampWayJudgeSeparate//赛道变得足够宽
				)
			{
				ramWay.rampConfirmCount++;
			} 
			else
			{
				ramWay.rampConfirmCount = 0;
			}
			if (ramWay.rampConfirmCount > roadTypeConst.RampWayConfirmC)
			{ //的确是上坡道了
				ramWay.IsUpRamp = true; //转到检测在坡道上
	//			BUZ_Normal();
				ramWay.rampConfirmCount = 0;
			}
			ramWay.rampTimeCount = 0;//清零计数，定时通过坡道的时间
		}
		else//已经在坡道上了
		{
			if (ramWay.IsOnRamp == false)//是否在坡道上
			{ //IsUpRamp=true.而且又没有确认在坡道上，就进行在坡道上的确认
				if (avgWidth < roadTypeConst.StraightRoadJudgeWidth - roadTypeConst.RampWayJudgeSeparate)
				{//多次赛道宽度已经变得很窄，说明前瞻很远，赛道变得很窄，是在坡道上面了
					ramWay.rampConfirmCount++;
				}
				else
				{
					ramWay.rampConfirmCount = 0;
				}
				if (ramWay.rampConfirmCount > roadTypeConst.RampWayConfirmC)
				{//的确在坡道上面了 
					ramWay.IsOnRamp = true;//确认在坡道上面
	//				BUZ_Normal();
					ramWay.rampConfirmCount = 0;
				}
			}
			else if (ramWay.IsDownRamp == false)
			{ //在坡道上却又没有下坡，就检测下坡的到来,和检测是否上坡道是一样的.但是由于可能会出现车身歪了一点的情况，所以不判断有效行数
				if (leftErCheng.zigma2 < roadTypeConst.StraightRoadJudgeMaxZigMa && rightErCheng.zigma2 < roadTypeConst.StraightRoadJudgeMaxZigMa//赛道畸变小，zigma小
					&& avgWidth > roadTypeConst.StraightRoadJudgeWidth + roadTypeConst.RampWayJudgeSeparate//赛道变得足够宽
					  )
				{
					ramWay.rampConfirmCount++;
				}
				else
				{
					ramWay.rampConfirmCount = 0;
				}
				if (ramWay.rampConfirmCount > roadTypeConst.RampWayConfirmC)
				{ //的确是下坡道了
					ramWay.IsDownRamp = true; //转到检测在坡道上
	//				BUZ_Normal();
					ramWay.rampConfirmCount = 0;
				}
			}
			else
			{ //已经下坡道了，但是还没有离开。要确认离开了坡道
				if (avgWidth < roadTypeConst.StraightRoadJudgeWidth + roadTypeConst.RampWayJudgeSeparate
					|| leftErCheng.effectLines < roadTypeConst.StraightRoadJudgeMinEffLines
					  || rightErCheng.effectLines < roadTypeConst.StraightRoadJudgeMinEffLines
	//				    || leftErCheng.zigma2 > roadTypeConst.StraightRoadJudgeMaxZigMa
	//					   || rightErCheng.zigma2 > roadTypeConst.StraightRoadJudgeMaxZigMa
						)
				{//赛道宽度已经有变窄了，已经离开坡道 
				  ramWay.IsUpRamp = false;
				  ramWay.IsOnRamp = false;
				  ramWay.IsDownRamp = false;
				  ramWay.rampConfirmCount = 0;
				}
			}
		}
		if (ramWay.IsUpRamp || ramWay.IsOnRamp || ramWay.IsDownRamp)//进入坡道
			ramWay.rampTimeCount++;//统计时间
		else//不在坡道上一直清零
			ramWay.rampTimeCount = 0;
		if (ramWay.rampTimeCount > roadTypeConst.RampWayMaxTime)
		{//在坡道上的时间超时了
			ramWay.IsUpRamp = false;
			ramWay.IsOnRamp = false;
			ramWay.IsDownRamp = false;
			ramWay.rampConfirmCount = 0;
			ramWay.rampTimeCount = 0;
	//		BUZ_Normal();
		}
	}
    /////////////////
	//坡道检测结束
	////////////////
	
	///////////////////
	//障碍时间超时置0
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
	//处理结束
	////////////////
	
	for (line = 0; line < Image_Height; line++)
    {
        ImgClass.leftLine[line] = leftLine[line];
        ImgClass.RightLine[line] = rightLine[line];
		ImgClass.middleLine[line]= middleLine[line];
    }

    leftBuLine(heightBottom, leftErCheng.endLine);  //补线，endLine就是最后一个有效行
    rightBuLine(heightBottom, rightErCheng.endLine);
	
    roadTypeAnalyze(leftErCheng.endLine, rightErCheng.endLine, leftErCheng.effectLines, rightErCheng.effectLines, leftErCheng.k, rightErCheng.k, (int16_t)leftErCheng.zigma2, (int16_t)rightErCheng.zigma2);
	push(10,roadType);
    
	//避免坡道和入出十字时起跑线的误判
	if (leftErCheng.zigma2 > 40 || rightErCheng.zigma2 > 40 || (roadType != RT_Straight))
	{
		startLinesDetect.IsStartLine = false;  
	}
	
//	if( ramWay.IsUpRamp == true || ramWay.IsOnRamp == true || ramWay.IsDownRamp == true)
//	{
//		roadLeftBlock.IsRoadBlock = false;
//		roadRightBlock.IsRoadBlock = false;
//	}
	
	//判断到坡道之后，屏蔽起跑线检测2秒
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
	
	//避免障碍处起跑线的误判
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
