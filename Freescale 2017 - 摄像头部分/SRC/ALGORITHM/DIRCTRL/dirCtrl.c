#include "dirCtrl.h"

void LoadDirValue(void);

//参数定义  
#define ComplememtCtl      1     //在小S使用加权平均,其他地方用当行算法
#define MinUsefulLines     20    //最小有效行数，超过这么多行数左右都没线，车停(用在停车判断那里)
DirPIDType DirPID;
uint8_t lastLeftPreSee = 55, lastRightPreSee = 55; //上一次的前瞻   90
uint8_t PreLine = 'L';	        				   //上一次车轮拐的方向
uint8_t changeSA = 0; 

//二次P参数调整 
double Servo_P = 4.0;
double DirStatus = 300.0;

//军神动态P参数调整
double MinKp = 7.8, MaxKp= 8.8, SysKp=7.5, CutOffset = 0.40;  //0.5    0.9     1      0.85    //4.8  8.8 
double Servo_D = 9.0;
double blockErrorAmpFactor = 2.0;   //障碍error的放大倍数

//平均行算法参数调整
uint8_t MinAvgLinePreSee = 0;	 //平均行求偏差里的起始计算行
uint8_t MaxAvgLinePreSee = 144;	 //平均行求偏差里的终止计算行   60-100

//单行算法参数调整 
int16_t UseLines = 1;     	   //使用的行数，做平均
int16_t singleCloseLine = 80;  //用于求权重，小个值就不用这边的偏差了
int16_t MaxSinglePreSee = 102, MinSinglePreSee = 92, SinglePreSee = 92; //除大S都可跑110,100,100   //95,80//87,72   //108 98
int16_t useDynamicPreSeeMaxZigma = 150;//80;    //除大S都可跑80
int16_t useDynamicPreSeeMinZigma = 120 ;//30;   //除大S都可跑30

//上电屏蔽障碍检测时间
uint32_t BlockMaskTime = 0;

//左侧基准图像
uint8_t ZeroLeft[144]= 
{
181,
181,180,180,179,179,179,178,178,178,177,
177,177,177,176,176,176,175,175,174,174,
174,173,173,173,172,172,171,171,171,170,
170,170,169,169,168,168,168,167,167,167,
166,166,165,165,165,164,164,163,163,163,
162,162,162,161,161,160,160,159,159,159,
158,158,157,157,156,156,155,155,155,154,
154,153,153,152,152,151,151,151,150,150,
149,149,148,148,147,147,146,146,145,145,
144,144,143,143,142,142,141,141,140,140,
139,139,139,138,138,137,137,136,136,135,
135,134,134,133,133,132,132,131,131,130,
130,129,129,128,128,127,127,126,126,125,
125,124,123,123,122,121,121,120,120,119,
119,118,118
};

//右侧基准图像
uint8_t ZeroRight[144]=
{
20,
20,20,21,21,21,22,22,22,23,23,
23,24,24,24,25,25,26,26,26,27,
27,27,27,28,28,29,29,29,30,30,
31,31,31,32,32,33,33,33,34,34,
35,35,35,36,36,37,37,38,38,39,
39,40,40,40,41,41,42,42,43,43,
43,44,44,45,45,45,46,46,47,47,
48,48,49,49,50,50,51,51,51,52,
52,53,53,54,54,55,55,56,56,56,
57,57,58,58,59,59,60,60,61,61,
62,62,63,63,64,64,65,65,66,66,
67,67,68,68,69,69,70,70,71,71,
72,72,73,73,74,74,75,75,76,76,
77,77,78,79,79,80,80,81,81,82,
82,83,84
};

//左侧基准图像
uint8_t BlockZeroLeft[144]= 
{
157,
157,156,156,156,156,156,155,155,155,155,
154,154,154,154,153,153,153,153,152,152,
152,152,151,151,151,151,150,150,150,150,
149,149,149,149,148,148,148,148,147,147,
147,146,146,146,146,145,145,145,144,144,
144,144,143,143,143,142,142,142,142,141,
141,141,140,140,140,139,139,139,138,138,
138,138,137,137,137,136,136,136,135,135,
135,134,134,134,134,133,133,132,132,132,
132,131,131,131,130,130,130,129,129,129,
128,128,128,128,127,127,126,126,126,125,
125,124,124,124,123,123,122,122,122,121,
121,121,120,120,120,119,119,119,118,118,
117,117,117,117,116,116,115,115,115,114,
114,114,113
};

//右侧基准图像
uint8_t BlockZeroRight[144]=
{
51,
51,51,51,52,52,52,52,52,53,53,
53,53,54,54,54,54,54,55,55,55,
55,56,56,56,56,56,57,57,57,57,
58,58,58,58,59,59,59,60,60,60,
60,60,61,61,61,62,62,62,63,63,
63,63,64,64,65,65,65,65,66,66,
66,67,67,67,68,68,68,69,69,69,
70,70,70,71,71,71,71,72,72,72,
73,73,73,73,74,74,74,74,75,75,
75,76,76,76,76,77,77,77,77,78,
78,78,79,79,79,79,80,80,80,81,
81,81,82,82,82,83,83,83,83,84,
84,84,85,85,85,86,86,86,87,87,
87,88,88,89,89,89,90,90,90,90,
91,91,91
};
//方向PID(舵机PID)的初始化
void DirPID_Init(void)
{	
	DirPID.Kp=MinKp;						
	DirPID.Ki=0;				 
	DirPID.Kd=Servo_D;
	DirPID.Kk=0;
	DirPID.Error1 = 0;				 	 //上次偏差
	DirPID.Error2 = 0;					 //上上次偏差
	DirPID.PWM = Servo_Middle;
	DirPID.LowerLimit = Servo_Right;   //摆角限制
	DirPID.UperLimit = Servo_Left;	 //最高速度限制
	DirPID.SetVal = 0;         //Speed set as 0 
}

/*
    形参：*useMiddleLine取SinglePreSee和endLine的最小值，其最小值为60
          *lastPreSee取SinglePreSee和endLine的最小值，其最小值为90，用来限制下一幅的endLine
		  单行算法里所用的行取SinglePreSee和endLine中的较小值
*/
//单行算法，在速度高的时候用的
int16_t getSingleLineError(ErChengClassType *erCheng, uint8_t *line, uint8_t *zero, uint8_t *useMiddleLine, uint8_t *lastPreSee)
{
    int16_t  i,total = 0 , single_err;
    int16_t  endLine;               //用来储存本次处理所使用的行
    endLine = erCheng->endLine;     //最远前瞻
    if(endLine > *lastPreSee)       //前瞻在增长
        endLine = *lastPreSee + 1;  //为了使得前瞻均匀变化，不会太突然了
    if(endLine > SinglePreSee)
    {
        for(i = SinglePreSee - UseLines + 1; i <= SinglePreSee ; i++)
        total += line[i] - zero[i];       //求偏差
        *useMiddleLine = SinglePreSee ;
    }
    else 								  //即使endLine大于了SinglePreSee，所用的行数也不会突然变化
    {        
        for(i = endLine - UseLines ; i < endLine ; i++)
            total += line[i] - zero[i];
        *useMiddleLine = endLine;
    }

	//记录上一次的前瞻
    *lastPreSee = endLine;
    if(*lastPreSee > SinglePreSee)
	{
		*lastPreSee = SinglePreSee;                   //限制前一个前瞻的值为最大前瞻
	}
    else if(*lastPreSee < MinSinglePreSee)
	{
		*lastPreSee = MinSinglePreSee;       		  //避免大幅变化
	}
	
    if(*useMiddleLine<singleCloseLine)         		  //*useMiddleLine(即leftUseLine或rightUseLine)
        *useMiddleLine = singleCloseLine;			  //用于求权重
  
    single_err = total/(UseLines);
    return single_err;
}

//平均行算法
int16_t getAvgLineError(uint8_t minPreSee,uint8_t preSee, uint8_t *line, uint8_t *zero)
{
    int16_t i, eerr;
    double total=0, avg=1, square=0;
    for(i=minPreSee; i < preSee; i++)
    {
        if(line[i]>0 && line[i]<Image_Width)
        {
            square = i*i*i;
            total += (line[i] - zero[i])*square;
            avg += square;
        }
    }
	
    eerr = (int16_t)(total/avg);
    return eerr;
}

int16_t singleUseLeftLine(void)
{
    uint8_t leftUseLine, rightUseLine;//左右使用的第几行
	uint8_t MinPreSee = 0;
    int16_t eerrL, eerrR, eerr, error, serrL, serrR,serr;
    double  rightWeight = 0; 		  //右边线使用的比例

	MinPreSee = MinAvgLinePreSee;
	//求平均行偏差
	if(MinPreSee > leftErCheng.endLine)		 		//实际跑的时候发现在弯道处可能丢图像，导致设定得最小前瞻大于了endline
	{												//这样会导致小车突然没有偏差冲出赛道
		MinPreSee = leftErCheng.endLine;
	}
	else
	{
		MinPreSee = MinAvgLinePreSee;
	}
    eerrL = getAvgLineError(MinPreSee, MaxAvgLinePreSee, ImgClass.leftLine, ZeroLeft); 
	
	MinPreSee = MinAvgLinePreSee;
	if(MinPreSee > rightErCheng.endLine)
	{
		MinPreSee = rightErCheng.endLine;
	}
	else
	{
		MinPreSee = MinAvgLinePreSee;
	}
    eerrR = getAvgLineError(MinPreSee, MaxAvgLinePreSee, ImgClass.RightLine, ZeroRight);
    eerr  = (eerrL+eerrR)/2;
	
	//求单行偏差
    serrL = getSingleLineError(&leftErCheng, ImgClass.leftLine, ZeroLeft, &leftUseLine, &lastLeftPreSee);
    serrR = getSingleLineError(&rightErCheng, ImgClass.RightLine, ZeroRight, &rightUseLine, &lastRightPreSee);
	
    rightWeight  = ((double)(rightUseLine - singleCloseLine))/(SinglePreSee - singleCloseLine); //计算右线的使用比例
    serr = ((serrL + serrR*rightWeight)/(1+rightWeight));	//得出加权之后的单行误差

	error = serr;					//使用单行求偏差
	
	if(roadType == RT_SmallS)			//小S的时候使用平均行
	{
		error = eerr;
	}
	if(roadType == RT_Straight)// && changeSA == 1)
	{
		error = eerr;
	}
    return error;
}

int16_t singleUseRightLine(void)
{
    uint8_t leftUseLine, rightUseLine;
	uint8_t MinPreSee = 0;
    int16_t eerrL, eerrR, eerr, error, serrL, serrR,serr;
    double  leftWeight=0;

	MinPreSee = MinAvgLinePreSee;
	//求平均行偏差								//2016.1.25添加
	if(MinPreSee > leftErCheng.endLine)		 		//实际跑的时候发现在弯道处可能丢图像，导致设定得最小前瞻大于了endline
	{												//这样会导致小车突然没有偏差冲出赛道
		MinPreSee = leftErCheng.endLine;
	}
	else
	{
		MinPreSee = MinAvgLinePreSee;
	}
    eerrL = getAvgLineError(MinPreSee, MaxAvgLinePreSee, ImgClass.leftLine, ZeroLeft);    //144指的是前瞻
	
	MinPreSee = MinAvgLinePreSee;
	if(MinPreSee > rightErCheng.endLine)
	{
		MinPreSee = rightErCheng.endLine;
	}
	else
	{
		MinPreSee = MinAvgLinePreSee;
	}
    eerrR = getAvgLineError(MinPreSee, MaxAvgLinePreSee, ImgClass.RightLine, ZeroRight); 
    eerr  = (eerrL + eerrR)/2;

    serrL = getSingleLineError(&leftErCheng, ImgClass.leftLine, ZeroLeft, &leftUseLine, &lastLeftPreSee);
    serrR = getSingleLineError(&rightErCheng, ImgClass.RightLine, ZeroRight, &rightUseLine,  &lastRightPreSee);

    leftWeight = ((double)(leftUseLine - singleCloseLine))/(SinglePreSee - singleCloseLine);//计算右线的使用比例
    serr = ((serrL*leftWeight + serrR)/(1 + leftWeight));
    
	error = serr;
	if(roadType == RT_SmallS)			//小S的时候使用平均行
	{
		error = eerr;
	}
	if(roadType == RT_Straight && changeSA == 1)
	{
		error = eerr;
	}
    return error;
}

int16_t singleUseDoubleLine(void)
{
  int16_t errorL,errorR,error;
  errorL = ImgClass.leftLine[40] - ZeroLeft[40];
  errorR = ImgClass.RightLine[40] - ZeroRight[40];
  error = (errorL + errorR)/2;
  return error;
}

int16_t singleUseLeftSingleLine(void)
{
	int16_t  i,total = 0 , single_err;
    int16_t  endLine;               //用来储存本次处理所使用的行
    endLine = leftErCheng.endLine;     //最远前瞻
    if(endLine > lastLeftPreSee)       //前瞻在增长
        endLine = lastLeftPreSee + 1;  //为了使得前瞻均匀变化，不会太突然了
    if(endLine > SinglePreSee)
    {
        for(i = SinglePreSee - UseLines + 1; i <= SinglePreSee ; i++)
        total += ImgClass.leftLine[i] - BlockZeroLeft[i] + 0;       //求偏差
    }
    else 								  //即使endLine大于了SinglePreSee，所用的行数也不会突然变化
    {        
        for(i = endLine - UseLines ; i < endLine ; i++)
            total += ImgClass.leftLine[i] - BlockZeroLeft[i] + 0;
    }

	//记录上一次的前瞻
    lastLeftPreSee = endLine;
    if(lastLeftPreSee> SinglePreSee)
	{
		lastLeftPreSee = SinglePreSee;                   //限制前一个前瞻的值为最大前瞻
	}
    else if(lastLeftPreSee < MinSinglePreSee)
	{
		lastLeftPreSee= MinSinglePreSee;       		  //避免大幅变化
	}
	  
    single_err = total/(UseLines);
    return single_err;
}

int16_t singleUseRightSingleLine(void)
{
	int16_t  i,total = 0 , single_err;
    int16_t  endLine;               //用来储存本次处理所使用的行
    endLine = rightErCheng.endLine;     //最远前瞻
    if(endLine > lastRightPreSee)       //前瞻在增长
        endLine = lastRightPreSee + 1;  //为了使得前瞻均匀变化，不会太突然了
    if(endLine > SinglePreSee)
    {
        for(i = SinglePreSee - UseLines + 1; i <= SinglePreSee ; i++)
        total += ImgClass.RightLine[i] - BlockZeroRight[i] - 0;       //求偏差
    }
    else 								  //即使endLine大于了SinglePreSee，所用的行数也不会突然变化
    {        
        for(i = endLine - UseLines ; i < endLine ; i++)
            total += ImgClass.RightLine[i] - BlockZeroRight[i] - 0;
    }

	//记录上一次的前瞻
    lastRightPreSee = endLine;
    if(lastRightPreSee> SinglePreSee)
	{
		lastRightPreSee = SinglePreSee;                   //限制前一个前瞻的值为最大前瞻
	}
    else if(lastRightPreSee < MinSinglePreSee)
	{
		lastRightPreSee= MinSinglePreSee;       		  //避免大幅变化
	}
	  
    single_err = total/(UseLines);
    return single_err;
}
//动态前瞻
int16_t singleLineGetParameter(void)
{
	static bool preRampWay = false;
	static long rampDelay = 0;
    int16_t error;
    int16_t zigma = 0;

    zigma = Max((int16_t)leftErCheng.zigma2, (int16_t)rightErCheng.zigma2);
	push(8,zigma);
    if(zigma < useDynamicPreSeeMinZigma)     //zigma < 120
        zigma = useDynamicPreSeeMinZigma;
    else if(zigma>useDynamicPreSeeMaxZigma)  //zigma > 150
        zigma = useDynamicPreSeeMaxZigma;

	//一般都是取的最大前瞻，最小前瞻只会在大S、入大弧、中S时出现
	//计算的SinglePreSee在MaxSinglePreSee和MinSinglePreSee之间变化
    SinglePreSee = (int16_t)(MaxSinglePreSee - ((double)(zigma - useDynamicPreSeeMinZigma)/(useDynamicPreSeeMaxZigma - useDynamicPreSeeMinZigma))*(MaxSinglePreSee - MinSinglePreSee)); 
	push(9,SinglePreSee);
	
	//起跑时屏蔽障碍1s
	if(!checkdelay(BlockMaskTime) || BlockMaskTime == 0)
	{
		roadLeftBlock.IsRoadBlock = false;
		roadRightBlock.IsRoadBlock = false;
		roadLeftBlock.BesideBlock = false;
		roadLeftBlock.ThroughBlock = false;
	}
	
	//如果前一幅图像是坡道，而且现在已经离开坡道,则3s内不会在有坡道
	if(preRampWay && ramWay.IsUpRamp == false && ramWay.IsOnRamp == false && ramWay.IsDownRamp == false)
	{
		 rampDelay = setdelay(3000); //3s内不可以通过两个坡道
	}
	
	if(checkdelay(rampDelay) && (ramWay.IsUpRamp || ramWay.IsOnRamp || ramWay.IsDownRamp))  //坡道舵机控制
	{
		error = singleUseDoubleLine();		  //使用两边线
		ramWaySpeedLimitDelay = setdelay(300);//离开坡道后300ms内不加速
	}
	else if(roadLeftBlock.IsRoadBlock == true )   			//左障碍舵机控制 起跑时会屏蔽障碍1s
	{
		PreLine = 'R';
		error = blockErrorAmpFactor*singleUseRightSingleLine();
	}
	else if(roadRightBlock.IsRoadBlock == true)  //右障碍舵机控制
	{
		PreLine = 'L';
		error = blockErrorAmpFactor*singleUseLeftSingleLine();
	}
	else										 //一般情况
	{
		ramWay.IsUpRamp = ramWay.IsOnRamp = ramWay.IsDownRamp = false;
		if(leftErCheng.endLine>rightErCheng.endLine+5)  	//如果左线endline > 右线endline+5 ,求左右线平均(或者采用右线加权)
		{
			PreLine = 'L';
			error = singleUseLeftLine();
		}
		else if(leftErCheng.endLine+5<rightErCheng.endLine) //如果右线endline > 左线endline+5 ,求左右线平均(或者采用左线加权)
		{
			PreLine = 'R';
			error = singleUseRightLine();
		}
		else if(PreLine=='L')
		{
			error = singleUseLeftLine();
		}
		else
		{
			error = singleUseRightLine();
		}
	}
	preRampWay = ramWay.IsUpRamp | ramWay.IsOnRamp | ramWay.IsDownRamp;
    return error;
}

//动态P值
//2016.5.7尝试：由于主要是Kp参数的整定，所以改大了CutOffset
//2016.5.8尝试：修改了之前添加的对于p的确定，添加了Zigma>130
//因为发现一般的弯道，使用之前的p会导致提前打脚，这样会导致入弯之后
//轨迹不会很好，但是去掉这一句会导致大S外切打到路肩，所以才添加了
//zigma>130（因为大S比较复杂，Zigma比较大）
void Caculate_PD(void)
{
	double p = 0;
	int16_t zigma = 0;

    zigma = Max((int16_t)leftErCheng.zigma2, (int16_t)rightErCheng.zigma2);
	
	DirPID.Kp = MinKp;
	if(PreLine == 'R')
	{
		if(rightErCheng.endLine < SinglePreSee)         //用最远处，因为前瞻变小，修正一下
		{
			p = (SinglePreSee - rightErCheng.endLine)/20.0;	//小弯道的Endline比大弯道的EndLine要大很多
		}
		else if(zigma > 120)
		{
			p = (rightErCheng.endLine -SinglePreSee)/20.0; 
		}	
	}
	else if(PreLine == 'L') 
	{
		if(leftErCheng.endLine < SinglePreSee)
		{ 
			p = (SinglePreSee - leftErCheng.endLine)/20.0;
		}
		else if(zigma > 120)
		{
			p = (leftErCheng.endLine -SinglePreSee)/20.0;
		}	
	}
	
	if(p>1.0) p = 1.0;
	if(p<0) p = 0;
	p*=CutOffset;
	DirPID.Kp *= (1.0 + p);//放大一下
//由于得到的Error是根据参考前瞻和Endline算出来的，决策条件是Zigma,Zigma小的时候说明赛道不复杂，用的是远前瞻
//这样的决策方式使得在一般的弯道，大弧，180度弯的时候，算出来的Error相差不大，这个时候起决定性作用的是Kp
//所以路径的优化主要在Kp上，而使用二次P会导致在不同的弯道，其Kp值相差不大。
//	DirPID.Kp = Servo_P +((DirPID.Error * DirPID.Error)/(DirStatus*10));			//二次P	
	if(roadRightBlock.IsRoadBlock == true || roadLeftBlock.IsRoadBlock == true)
	{
		DirPID.Kp = MaxKp;
	}
	
  	if(DirPID.Kp>MaxKp)     
		DirPID.Kp = MaxKp;
	DirPID.Kd = Servo_D;
	
	if(roadType == RT_Straight)// && fabs(DirPID.Error) >5 && changeSA == 0)
	{
		DirPID.Kp = 6.0;
		DirPID.Kd = 9.0;
	}
	
	if(roadType == RT_Straight && fabs(DirPID.Error) <5)
	{
		changeSA = 1;
	}
	else if(roadType != RT_Straight)
	{
		changeSA = 0;
	}
	push(4,(DirPID.Kp*10));
    DirPID.PWM = Servo_Middle;    //首先舵机回正
    DirPID.PWM -= DirPID.Kp*DirPID.Error + DirPID.Kd*(DirPID.Error - DirPID.Error1);
	DirPID.Error2 = DirPID.Error1;
    DirPID.Error1 = DirPID.Error;
}

void DirControl(void)
{
	static uint8_t emergency = 0;			//紧急停车
	
	if(leftErCheng.effectLines <= MinUsefulLines && rightErCheng.effectLines <= MinUsefulLines)
	{
		emergency++;
		if(emergency>25)			//紧急停车
		{
			if(EMERGRNCYSTOPCAR == true)	//拨码开关选择是否允许
			{
				StopCar = true;
			}
			emergency = 0;
		}
		return;
	}
	else 
	{
		emergency = 0;
	}
	push(3,StopCar);

    DirPID.Error = singleLineGetParameter();
	
	push(7,(DirPID.Error));
	Caculate_PD();

	
	if(DirPID.PWM > Servo_Left)
	{
			DirPID.PWM = Servo_Left;
	}
	else if(DirPID.PWM < Servo_Right)
	{
			DirPID.PWM = Servo_Right;
	}
	FTM_PWM_ChangeDuty(HW_FTM2,HW_FTM_CH0, DirPID.PWM);
}

//根据速度模式导入参数
void LoadDirValue(void)
{
	if(Sys_Speed_Mode == LOW_SPEED)
	{
		MinKp = 5.5;//5.5;
		MaxKp = 7.4;//8.0;
		CutOffset = 0.42;//0.20;
		Servo_D = 18.0;
		MinSinglePreSee = 85;//88;
		MaxSinglePreSee = 95;//102;
		useDynamicPreSeeMinZigma = 120;
		useDynamicPreSeeMaxZigma = 150; 
	}  
	else if(Sys_Speed_Mode == MIDDLE_SPEED)
 	{
		MinKp = 6.0;//6.3; 
		MaxKp = 7.7;//8.3;
		CutOffset = 0.25;//0.18;
		Servo_D = 19.0; 
		MinSinglePreSee = 85;//85;
		MaxSinglePreSee = 95;//95;
		useDynamicPreSeeMinZigma = 120;
		useDynamicPreSeeMaxZigma = 150;		
	}
	else if(Sys_Speed_Mode == HIGH_SPEED)
	{
		MinKp = 6.0;
		MaxKp = 8.0;
		CutOffset = 0.34;
		Servo_D = 22.0;
		MinSinglePreSee = 85;  
		MaxSinglePreSee = 95;
		useDynamicPreSeeMinZigma = 120;
		useDynamicPreSeeMaxZigma = 150;	
	}
	else if(Sys_Speed_Mode == AFUZZY_SPEED)
	{
		MinKp = 6.0;//6.4;
		MaxKp = 8.0;//8.3;
		CutOffset = 0.36;
		Servo_D = 23.0;//25.0;
		MinSinglePreSee = 86;//86;
		MaxSinglePreSee = 96;//100;
		useDynamicPreSeeMinZigma = 120;
		useDynamicPreSeeMaxZigma = 150;				
	}
	else if(Sys_Speed_Mode == BFUZZY_SPEED)
	{
		MinKp = 6.0;//7.0;
		MaxKp = 8.0;//9.0;
		CutOffset = 0.36;//0.28;
		Servo_D = 23.5;//23.0;
		MinSinglePreSee = 86;
		MaxSinglePreSee = 96;
		useDynamicPreSeeMinZigma = 120;
		useDynamicPreSeeMaxZigma = 150;				
	}
	else if(Sys_Speed_Mode == CFUZZY_SPEED)
	{
		MinKp = 6.2;
		MaxKp = 8.0;
		CutOffset = 0.36;
		Servo_D = 24.0;
		MinSinglePreSee = 86;
		MaxSinglePreSee = 96;
		useDynamicPreSeeMinZigma = 120;
		useDynamicPreSeeMaxZigma = 150;				
	}
	else if(Sys_Speed_Mode == DFUZZY_SPEED)
	{
		MinKp = 6.2;
		MaxKp = 8.3;
		CutOffset = 0.36;
		Servo_D = 25.0;
		MinSinglePreSee = 86;
		MaxSinglePreSee = 96;
		useDynamicPreSeeMinZigma = 120;
		useDynamicPreSeeMaxZigma = 150;				
	}
}

