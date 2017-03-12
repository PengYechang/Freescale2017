#include "image_control.h"

#define	IMG_BLACK		0
#define	IMG_WHITE		255
#define FIND_CENTER		0
#define FIND_LEFT		1
#define FIND_RIGHT		2
#define CENTER_POINT	Image_Width/2

int16_t centerLine[Image_Height+1] = {0};			// 最后一个元素用来记录转向点对应的行数
int16_t leftLine[Image_Height] = {0};
int16_t rightLine[Image_Height] = {0};

static uint8_t leftFindFlag;					// 用来标记左黑线是否找到
static uint8_t rightFindFlag;					// 用来标记右黑线是否找到

static int16_t leftCount;
static int16_t rightCount;
static int16_t findLine;

int16_t createPoint(int type, int line);

//获取阈值Thr
 uint8_t CalculateThreshold(uint8_t (* image)[Image_Width], uint16_t widthBottom, uint16_t heightBottom, uint16_t widthTop, uint16_t heightTop)
{ 
    uint8_t (* pixel)[Image_Width] = image;   
    uint16_t PixelNum[256]={0};     
    int16_t Thr = 100;             
    int16_t line, column;          

    int32_t p;
    int32_t total;                          	
    int32_t TotalWeight = 0, ForeWeight = 0;    
    int32_t TotalPixels = 0;                  

    int32_t BackPixNum = 0, ForePixNum = 0;    
    double BackAvgGry, ForeAvgGry;          
    double fmax = -1;
    double sb;

    for(line = heightBottom; line < heightTop; line++)     
    {        
        for(column = widthBottom; column < widthTop; column += 4)
        {
            PixelNum[*(*(pixel + line)+column)]++;
        }
        line += 3;
    }
  
    for(line = 0; line < 256; line++)
    {
        total = 0;
        for(column = -2; column < 3; column++) 
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
        TotalWeight += line * PixelNum[line];            
        TotalPixels += PixelNum[line];                  
    }
  
    for(line = 0; line < 256; line++)
    {
        ForePixNum += PixelNum[line];          
        if (ForePixNum == 0) continue;          
        BackPixNum = TotalPixels - ForePixNum;  
        if (BackPixNum == 0) break;             
        ForeWeight += line * PixelNum[line];  
        ForeAvgGry = ((double)(ForeWeight)) / ForePixNum;  
        BackAvgGry = ((double)(TotalWeight - ForeWeight)) / BackPixNum; 
        sb = ForePixNum * BackPixNum * (ForeAvgGry - BackAvgGry) * (ForeAvgGry - BackAvgGry);
        if (sb > fmax)
        {
            fmax = sb;
            Thr = line;
        }
    }
    return (uint8_t)Thr;
}

//二值化处理
void ImgBinarization(uint8_t (* image)[Image_Width],uint8_t widthBottom, uint8_t heightBottom, uint8_t widthTop, uint8_t heightTop)  
{
    uint8_t threshold = CalculateThreshold(image, widthBottom, heightBottom, widthTop, heightTop); 
    uint16_t i, j;
    uint8_t  value;
    
    for(i = 0; i < Image_Height; i++)
    {
        for(j = 0;j < Image_Width; j++)
        {
            value = Image_Deal[i][j];
            if(value < threshold) Image_Deal[i][j] = 0;
            else Image_Deal[i][j] = 255;
        }
    }
    j=0;
}

int16_t *findCenterLine(uint8_t (* image)[Image_Width])
{
	int8_t temp, tmp;
	
	//二值化
	ImgBinarization(image,0,0,Image_Width,Image_Height);
	
	// 前十行从中间往两边查找
	for(findLine = Image_Height-1; findLine > Image_Height-11; findLine--)
	{
		leftFindFlag = 0;
		rightFindFlag = 0;
		for(temp = 0; temp < CENTER_POINT; temp++)
		{
			// 寻找左黑线
			if(leftFindFlag == 0
			&& image[findLine][CENTER_POINT-temp-1] == IMG_BLACK
			&& image[findLine][CENTER_POINT-temp] == IMG_WHITE
			&& image[findLine][CENTER_POINT-temp+1] == IMG_WHITE
			&& image[findLine][CENTER_POINT-temp+2] == IMG_WHITE)
			{
				leftLine[findLine] = CENTER_POINT-temp-1;
				leftFindFlag = 1;
			}
			// 寻找右黑线
			if(rightFindFlag == 0
			&& image[findLine][CENTER_POINT+temp] == IMG_BLACK
			&& image[findLine][CENTER_POINT+temp-1] == IMG_WHITE
			&& image[findLine][CENTER_POINT+temp-2] == IMG_WHITE
			&& image[findLine][CENTER_POINT+temp-3] == IMG_WHITE)
			{
				rightLine[findLine] = CENTER_POINT+temp;
				rightFindFlag = 1;
			}
			if(leftFindFlag == 1 && rightFindFlag == 1)
				break;
		}
		// 对未找到的黑线进行补全
		if(leftFindFlag == 0)	leftLine[findLine] = 0;
		if(rightFindFlag == 0)	rightLine[findLine] = Image_Width-1;
		// 对中线进行赋值
		centerLine[findLine] = (leftLine[findLine]+rightLine[findLine])/2;
	}
	
	// 十行后根据前面行位置查找黑线
	for(findLine = Image_Height-11; findLine >= 0; findLine--)
	{
		leftFindFlag = 0;
		rightFindFlag = 0;
		// 预测下一行黑线位置
		leftCount = createPoint(FIND_LEFT, findLine);
		rightCount = createPoint(FIND_RIGHT, findLine);
		//leftCount = (2 * leftLine[findLine+1] - leftLine[findLine+2]);
		//rightCount = (2 * rightLine[findLine+1] - rightLine[findLine+2]);
		
		/* 在预测点的左右 FIND_COUNT 个点查找黑线位置 */
		// 寻找左黑线
		for(temp = 0; temp < FIND_COUNT*2+1; temp++)
		{
			if(leftFindFlag != 0)
				break;
			else if((leftCount-temp+FIND_COUNT)+3 > Image_Width-1)
				continue;
			else if((leftCount-temp+FIND_COUNT) < 0)
				break;
			else if(image[findLine][leftCount-temp+FIND_COUNT] == IMG_BLACK
			&& image[findLine][leftCount-temp+FIND_COUNT+1] == IMG_WHITE
			&& image[findLine][leftCount-temp+FIND_COUNT+2] == IMG_WHITE
			&& image[findLine][leftCount-temp+FIND_COUNT+3] == IMG_WHITE)
			{
				leftLine[findLine] = leftCount-temp+FIND_COUNT;
				leftFindFlag = 1;
			}
		}
		// 寻找右黑线
		for(temp = 0; temp < FIND_COUNT*2+1; temp++)
		{
			if(rightFindFlag != 0)
				break;
			else if((rightCount+temp-FIND_COUNT)-3 < 0)
				continue;
			else if(rightCount+temp-FIND_COUNT > Image_Width-1)
				break;
			else if(image[findLine][rightCount+temp-FIND_COUNT] == IMG_BLACK
			&& image[findLine][rightCount+temp-FIND_COUNT-1] == IMG_WHITE
			&& image[findLine][rightCount+temp-FIND_COUNT-2] == IMG_WHITE
			&& image[findLine][rightCount+temp-FIND_COUNT-3] == IMG_WHITE)
			{
				rightLine[findLine] = rightCount+temp-FIND_COUNT;
				rightFindFlag = 1;
			}
		}
		
		// 补全未找到的左右黑线
		if(leftFindFlag == 0)
			leftLine[findLine] = leftCount;
		if(rightFindFlag == 0)
			rightLine[findLine] = rightCount;
		
		/* 查找中线 */
#if (FIND_TYPE == TYPE_1)				
		
		// 补全未找到的左右黑线
		// if(leftFindFlag == 0)
		// 	leftLine[findLine] = leftCount;
		// if(rightFindFlag == 0)
		// 	rightLine[findLine] = rightCount;
		
		/* 对中线进行赋值 */
		centerLine[findLine] = (leftLine[findLine]+rightLine[findLine])/2;
		if(centerLine[findLine] <= 0)
		{
			centerLine[findLine] = 0;
			break;
		}
		else if(centerLine[findLine] >= Image_Width-1)
		{
			centerLine[findLine] = Image_Width-1;
			break;
		}		
#else		
		// 补全未找到的左右黑线
		// if(leftFindFlag == 0)
		// 	leftLine[findLine] = (leftCount < 0) ? 0 : ((leftCount > Image_Width-1) ? Image_Width-1 : leftCount);
		// if(rightFindFlag == 0)
		// 	rightLine[findLine] = (rightCount < 0) ? 0 : ((rightCount > Image_Width-1) ? Image_Width-1 : rightCount);
		
		/* 对中线进行赋值 */
		// 左右黑线都存在则取左右黑线中值作为黑线值
		if(leftLine[findLine] > 0 && rightLine[findLine] < Image_Width-1)
			centerLine[findLine] = (leftLine[findLine]+rightLine[findLine])/2;
		// 左黑线超出范围
		else if(leftLine[findLine] <= 0 && rightLine[findLine] < Image_Width-1)
		{
			// 根据右黑线的偏移量来确定中线
			temp = centerLine[findLine+1] + (rightLine[findLine] - rightLine[findLine+1]);
			// 根据最小二乘法补全中线
			//temp = createPoint(FIND_CENTER, findLine);
			if(temp <= 0)
			{
				// 中线超出范围则跳出循环，记录该行为转向行
				centerLine[findLine] = 0;
				break;
			}
			else
				centerLine[findLine] = temp;
		}
		// 右黑线超出范围
		else if(leftLine[findLine] > 0 && rightLine[findLine] >= Image_Width-1)
		{
			// 根据左黑线的偏移量来确定中线
			temp = centerLine[findLine+1] + (leftLine[findLine] - leftLine[findLine+1]);
			// 根据最小二乘法补全中线
			//temp = createPoint(FIND_CENTER, findLine);
			if(temp >= Image_Width-1)
			{
				// 中线超出范围则跳出循环，记录该行为转向行
				centerLine[findLine] = Image_Width-1;
				break;
			}
			else
				centerLine[findLine] = temp;
		}
		// 左右黑线都超出范围
		else
		{
			// 根据最小二乘法补全中线
			temp = createPoint(FIND_CENTER, findLine);
			// 根据中线偏移量补全中线
			//temp = centerLine[findLine+1] + (rightLine[findLine] - rightLine[findLine+1]);
			if(temp <= 0)
			{
				// 中线超出范围则跳出循环，记录该行为转向行
				centerLine[findLine] = 0;
				break;
			}
			else if(temp >= Image_Width-1)
			{
				// 中线超出范围则跳出循环，记录该行为转向行
				centerLine[findLine] = Image_Width-1;
				break;
			}
			else
				centerLine[findLine] = temp;
		}		
#endif
		
	}
	if(findLine<0 && centerLine[0]<0)
		centerLine[0] = 0;
	else if(findLine<0 && centerLine[0]>Image_Width-1)
		centerLine[0] = Image_Width-1;
	
	// 最后一个元素用来记录转向行
	centerLine[Image_Height] = (findLine < 0) ? 0 : findLine;
	
/* 检查停车线 */
#ifdef RELEASE	
	if(centerLine[Image_Height] == 0 
	&& leftLine[0] >= 23 
	&& rightLine[0] < Image_Width-23
	&& (centerLine[0]-centerLine[29] <= 3 || centerLine[0]-centerLine[29] >= -3))
	{
		for(temp = Image_Height-1; temp >= centerLine[Image_Height]; temp--)
		{
			if(carParams.stopCarFlag != STOP_LINE
			&& image[temp][(leftLine[temp]+centerLine[temp])/2] == IMG_BLACK
			&& image[temp][(rightLine[temp]+centerLine[temp])/2] == IMG_BLACK
			&& image[temp][(leftLine[temp]+centerLine[temp])/2-1] == IMG_BLACK
			&& image[temp][(leftLine[temp]+centerLine[temp])/2+1] == IMG_BLACK
			&& image[temp][(rightLine[temp]+centerLine[temp])/2-1] == IMG_BLACK
			&& image[temp][(rightLine[temp]+centerLine[temp])/2+1] == IMG_BLACK
			//&& image[temp][leftLine[temp]+1] == IMG_WHITE
			//&& image[temp][rightLine[temp]-1] == IMG_WHITE
			&& carParams.carRunTime > 3000/ENCODE_TIME)		// 小车起跑超过三秒则为停车线
			{
				for(tmp = (leftLine[temp]+centerLine[temp])/2+1; (rightLine[temp]+centerLine[temp])/2-1; tmp++)
				{
					if(image[temp][tmp] == IMG_WHITE && image[temp][tmp+2] == IMG_WHITE)
					{
						carParams.stopCarFlag = STOP_LINE;
						goto STOP_LINE_PASS;
					}
				}
			}
		}
	STOP_LINE_PASS:
		if(carParams.stopCarFlag == STOP_LINE && carParams.passStopLine != PASS_STOP_LINE && temp < 0)
			carParams.passStopLine = PASS_STOP_LINE;		// 小车已越过停车线
	}
#endif
	
	
	return (int16_t *)centerLine;
}

/* 利用最小二乘法生成需要补全的点 */
int16_t createPoint(int type, int line)
{
	int16_t *linePointer;
	int8_t tmp = 0;
	double sumX = 0;
	double sumY = 0;
	double averageX = 0;
	double averageY = 0;
	double sumUp = 0;
	double sumDown = 0;
	double parameterA;
	double parameterB;
	
	if(type == FIND_LEFT)
		linePointer = &leftLine[line];
	else if(type == FIND_RIGHT)
		linePointer = &rightLine[line];
	else
		linePointer = &centerLine[line];
	
	// 取邻近的 POINT_COUNT 个点进行拟合
	while(++tmp <= POINT_COUNT)
	{
		sumX += (line+tmp);
		sumY += linePointer[tmp];
	}
	--tmp;
	averageX = sumX/tmp;
	averageY = sumY/tmp;
	do
	{
		sumUp += (linePointer[tmp]-averageY) * (line+tmp-averageX);
		sumDown += (line+tmp-averageX) * (line+tmp-averageX);
	} while(--tmp > 0);
	
	if(sumDown == 0)
		parameterB = 0;
	else
		parameterB = sumUp/sumDown;
	parameterA = averageY-parameterB*averageX;
	return (int16_t)(parameterA+parameterB*line+0.5);
}