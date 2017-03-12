#include "dirCtrl.h"

void LoadDirValue(void);

//��������  
#define ComplememtCtl      1     //��СSʹ�ü�Ȩƽ��,�����ط��õ����㷨
#define MinUsefulLines     20    //��С��Ч������������ô���������Ҷ�û�ߣ���ͣ(����ͣ���ж�����)
DirPIDType DirPID;
uint8_t lastLeftPreSee = 55, lastRightPreSee = 55; //��һ�ε�ǰհ   90
uint8_t PreLine = 'L';	        				   //��һ�γ��ֹյķ���
uint8_t changeSA = 0; 

//����P�������� 
double Servo_P = 4.0;
double DirStatus = 300.0;

//����̬P��������
double MinKp = 7.8, MaxKp= 8.8, SysKp=7.5, CutOffset = 0.40;  //0.5    0.9     1      0.85    //4.8  8.8 
double Servo_D = 9.0;
double blockErrorAmpFactor = 2.0;   //�ϰ�error�ķŴ���

//ƽ�����㷨��������
uint8_t MinAvgLinePreSee = 0;	 //ƽ������ƫ�������ʼ������
uint8_t MaxAvgLinePreSee = 144;	 //ƽ������ƫ�������ֹ������   60-100

//�����㷨�������� 
int16_t UseLines = 1;     	   //ʹ�õ���������ƽ��
int16_t singleCloseLine = 80;  //������Ȩ�أ�С��ֵ�Ͳ�����ߵ�ƫ����
int16_t MaxSinglePreSee = 102, MinSinglePreSee = 92, SinglePreSee = 92; //����S������110,100,100   //95,80//87,72   //108 98
int16_t useDynamicPreSeeMaxZigma = 150;//80;    //����S������80
int16_t useDynamicPreSeeMinZigma = 120 ;//30;   //����S������30

//�ϵ������ϰ����ʱ��
uint32_t BlockMaskTime = 0;

//����׼ͼ��
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

//�Ҳ��׼ͼ��
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

//����׼ͼ��
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

//�Ҳ��׼ͼ��
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
//����PID(���PID)�ĳ�ʼ��
void DirPID_Init(void)
{	
	DirPID.Kp=MinKp;						
	DirPID.Ki=0;				 
	DirPID.Kd=Servo_D;
	DirPID.Kk=0;
	DirPID.Error1 = 0;				 	 //�ϴ�ƫ��
	DirPID.Error2 = 0;					 //���ϴ�ƫ��
	DirPID.PWM = Servo_Middle;
	DirPID.LowerLimit = Servo_Right;   //�ڽ�����
	DirPID.UperLimit = Servo_Left;	 //����ٶ�����
	DirPID.SetVal = 0;         //Speed set as 0 
}

/*
    �βΣ�*useMiddleLineȡSinglePreSee��endLine����Сֵ������СֵΪ60
          *lastPreSeeȡSinglePreSee��endLine����Сֵ������СֵΪ90������������һ����endLine
		  �����㷨�����õ���ȡSinglePreSee��endLine�еĽ�Сֵ
*/
//�����㷨�����ٶȸߵ�ʱ���õ�
int16_t getSingleLineError(ErChengClassType *erCheng, uint8_t *line, uint8_t *zero, uint8_t *useMiddleLine, uint8_t *lastPreSee)
{
    int16_t  i,total = 0 , single_err;
    int16_t  endLine;               //�������汾�δ�����ʹ�õ���
    endLine = erCheng->endLine;     //��Զǰհ
    if(endLine > *lastPreSee)       //ǰհ������
        endLine = *lastPreSee + 1;  //Ϊ��ʹ��ǰհ���ȱ仯������̫ͻȻ��
    if(endLine > SinglePreSee)
    {
        for(i = SinglePreSee - UseLines + 1; i <= SinglePreSee ; i++)
        total += line[i] - zero[i];       //��ƫ��
        *useMiddleLine = SinglePreSee ;
    }
    else 								  //��ʹendLine������SinglePreSee�����õ�����Ҳ����ͻȻ�仯
    {        
        for(i = endLine - UseLines ; i < endLine ; i++)
            total += line[i] - zero[i];
        *useMiddleLine = endLine;
    }

	//��¼��һ�ε�ǰհ
    *lastPreSee = endLine;
    if(*lastPreSee > SinglePreSee)
	{
		*lastPreSee = SinglePreSee;                   //����ǰһ��ǰհ��ֵΪ���ǰհ
	}
    else if(*lastPreSee < MinSinglePreSee)
	{
		*lastPreSee = MinSinglePreSee;       		  //�������仯
	}
	
    if(*useMiddleLine<singleCloseLine)         		  //*useMiddleLine(��leftUseLine��rightUseLine)
        *useMiddleLine = singleCloseLine;			  //������Ȩ��
  
    single_err = total/(UseLines);
    return single_err;
}

//ƽ�����㷨
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
    uint8_t leftUseLine, rightUseLine;//����ʹ�õĵڼ���
	uint8_t MinPreSee = 0;
    int16_t eerrL, eerrR, eerr, error, serrL, serrR,serr;
    double  rightWeight = 0; 		  //�ұ���ʹ�õı���

	MinPreSee = MinAvgLinePreSee;
	//��ƽ����ƫ��
	if(MinPreSee > leftErCheng.endLine)		 		//ʵ���ܵ�ʱ��������������ܶ�ͼ�񣬵����趨����Сǰհ������endline
	{												//�����ᵼ��С��ͻȻû��ƫ��������
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
	
	//����ƫ��
    serrL = getSingleLineError(&leftErCheng, ImgClass.leftLine, ZeroLeft, &leftUseLine, &lastLeftPreSee);
    serrR = getSingleLineError(&rightErCheng, ImgClass.RightLine, ZeroRight, &rightUseLine, &lastRightPreSee);
	
    rightWeight  = ((double)(rightUseLine - singleCloseLine))/(SinglePreSee - singleCloseLine); //�������ߵ�ʹ�ñ���
    serr = ((serrL + serrR*rightWeight)/(1+rightWeight));	//�ó���Ȩ֮��ĵ������

	error = serr;					//ʹ�õ�����ƫ��
	
	if(roadType == RT_SmallS)			//СS��ʱ��ʹ��ƽ����
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
	//��ƽ����ƫ��								//2016.1.25���
	if(MinPreSee > leftErCheng.endLine)		 		//ʵ���ܵ�ʱ��������������ܶ�ͼ�񣬵����趨����Сǰհ������endline
	{												//�����ᵼ��С��ͻȻû��ƫ��������
		MinPreSee = leftErCheng.endLine;
	}
	else
	{
		MinPreSee = MinAvgLinePreSee;
	}
    eerrL = getAvgLineError(MinPreSee, MaxAvgLinePreSee, ImgClass.leftLine, ZeroLeft);    //144ָ����ǰհ
	
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

    leftWeight = ((double)(leftUseLine - singleCloseLine))/(SinglePreSee - singleCloseLine);//�������ߵ�ʹ�ñ���
    serr = ((serrL*leftWeight + serrR)/(1 + leftWeight));
    
	error = serr;
	if(roadType == RT_SmallS)			//СS��ʱ��ʹ��ƽ����
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
    int16_t  endLine;               //�������汾�δ�����ʹ�õ���
    endLine = leftErCheng.endLine;     //��Զǰհ
    if(endLine > lastLeftPreSee)       //ǰհ������
        endLine = lastLeftPreSee + 1;  //Ϊ��ʹ��ǰհ���ȱ仯������̫ͻȻ��
    if(endLine > SinglePreSee)
    {
        for(i = SinglePreSee - UseLines + 1; i <= SinglePreSee ; i++)
        total += ImgClass.leftLine[i] - BlockZeroLeft[i] + 0;       //��ƫ��
    }
    else 								  //��ʹendLine������SinglePreSee�����õ�����Ҳ����ͻȻ�仯
    {        
        for(i = endLine - UseLines ; i < endLine ; i++)
            total += ImgClass.leftLine[i] - BlockZeroLeft[i] + 0;
    }

	//��¼��һ�ε�ǰհ
    lastLeftPreSee = endLine;
    if(lastLeftPreSee> SinglePreSee)
	{
		lastLeftPreSee = SinglePreSee;                   //����ǰһ��ǰհ��ֵΪ���ǰհ
	}
    else if(lastLeftPreSee < MinSinglePreSee)
	{
		lastLeftPreSee= MinSinglePreSee;       		  //�������仯
	}
	  
    single_err = total/(UseLines);
    return single_err;
}

int16_t singleUseRightSingleLine(void)
{
	int16_t  i,total = 0 , single_err;
    int16_t  endLine;               //�������汾�δ�����ʹ�õ���
    endLine = rightErCheng.endLine;     //��Զǰհ
    if(endLine > lastRightPreSee)       //ǰհ������
        endLine = lastRightPreSee + 1;  //Ϊ��ʹ��ǰհ���ȱ仯������̫ͻȻ��
    if(endLine > SinglePreSee)
    {
        for(i = SinglePreSee - UseLines + 1; i <= SinglePreSee ; i++)
        total += ImgClass.RightLine[i] - BlockZeroRight[i] - 0;       //��ƫ��
    }
    else 								  //��ʹendLine������SinglePreSee�����õ�����Ҳ����ͻȻ�仯
    {        
        for(i = endLine - UseLines ; i < endLine ; i++)
            total += ImgClass.RightLine[i] - BlockZeroRight[i] - 0;
    }

	//��¼��һ�ε�ǰհ
    lastRightPreSee = endLine;
    if(lastRightPreSee> SinglePreSee)
	{
		lastRightPreSee = SinglePreSee;                   //����ǰһ��ǰհ��ֵΪ���ǰհ
	}
    else if(lastRightPreSee < MinSinglePreSee)
	{
		lastRightPreSee= MinSinglePreSee;       		  //�������仯
	}
	  
    single_err = total/(UseLines);
    return single_err;
}
//��̬ǰհ
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

	//һ�㶼��ȡ�����ǰհ����Сǰհֻ���ڴ�S����󻡡���Sʱ����
	//�����SinglePreSee��MaxSinglePreSee��MinSinglePreSee֮��仯
    SinglePreSee = (int16_t)(MaxSinglePreSee - ((double)(zigma - useDynamicPreSeeMinZigma)/(useDynamicPreSeeMaxZigma - useDynamicPreSeeMinZigma))*(MaxSinglePreSee - MinSinglePreSee)); 
	push(9,SinglePreSee);
	
	//����ʱ�����ϰ�1s
	if(!checkdelay(BlockMaskTime) || BlockMaskTime == 0)
	{
		roadLeftBlock.IsRoadBlock = false;
		roadRightBlock.IsRoadBlock = false;
		roadLeftBlock.BesideBlock = false;
		roadLeftBlock.ThroughBlock = false;
	}
	
	//���ǰһ��ͼ�����µ������������Ѿ��뿪�µ�,��3s�ڲ��������µ�
	if(preRampWay && ramWay.IsUpRamp == false && ramWay.IsOnRamp == false && ramWay.IsDownRamp == false)
	{
		 rampDelay = setdelay(3000); //3s�ڲ�����ͨ�������µ�
	}
	
	if(checkdelay(rampDelay) && (ramWay.IsUpRamp || ramWay.IsOnRamp || ramWay.IsDownRamp))  //�µ��������
	{
		error = singleUseDoubleLine();		  //ʹ��������
		ramWaySpeedLimitDelay = setdelay(300);//�뿪�µ���300ms�ڲ�����
	}
	else if(roadLeftBlock.IsRoadBlock == true )   			//���ϰ�������� ����ʱ�������ϰ�1s
	{
		PreLine = 'R';
		error = blockErrorAmpFactor*singleUseRightSingleLine();
	}
	else if(roadRightBlock.IsRoadBlock == true)  //���ϰ��������
	{
		PreLine = 'L';
		error = blockErrorAmpFactor*singleUseLeftSingleLine();
	}
	else										 //һ�����
	{
		ramWay.IsUpRamp = ramWay.IsOnRamp = ramWay.IsDownRamp = false;
		if(leftErCheng.endLine>rightErCheng.endLine+5)  	//�������endline > ����endline+5 ,��������ƽ��(���߲������߼�Ȩ)
		{
			PreLine = 'L';
			error = singleUseLeftLine();
		}
		else if(leftErCheng.endLine+5<rightErCheng.endLine) //�������endline > ����endline+5 ,��������ƽ��(���߲������߼�Ȩ)
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

//��̬Pֵ
//2016.5.7���ԣ�������Ҫ��Kp���������������ԸĴ���CutOffset
//2016.5.8���ԣ��޸���֮ǰ��ӵĶ���p��ȷ���������Zigma>130
//��Ϊ����һ��������ʹ��֮ǰ��p�ᵼ����ǰ��ţ������ᵼ������֮��
//�켣����ܺã�����ȥ����һ��ᵼ�´�S���д�·�磬���Բ������
//zigma>130����Ϊ��S�Ƚϸ��ӣ�Zigma�Ƚϴ�
void Caculate_PD(void)
{
	double p = 0;
	int16_t zigma = 0;

    zigma = Max((int16_t)leftErCheng.zigma2, (int16_t)rightErCheng.zigma2);
	
	DirPID.Kp = MinKp;
	if(PreLine == 'R')
	{
		if(rightErCheng.endLine < SinglePreSee)         //����Զ������Ϊǰհ��С������һ��
		{
			p = (SinglePreSee - rightErCheng.endLine)/20.0;	//С�����Endline�ȴ������EndLineҪ��ܶ�
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
	DirPID.Kp *= (1.0 + p);//�Ŵ�һ��
//���ڵõ���Error�Ǹ��ݲο�ǰհ��Endline������ģ�����������Zigma,ZigmaС��ʱ��˵�����������ӣ��õ���Զǰհ
//�����ľ��߷�ʽʹ����һ���������󻡣�180�����ʱ���������Error�������ʱ������������õ���Kp
//����·�����Ż���Ҫ��Kp�ϣ���ʹ�ö���P�ᵼ���ڲ�ͬ���������Kpֵ����
//	DirPID.Kp = Servo_P +((DirPID.Error * DirPID.Error)/(DirStatus*10));			//����P	
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
    DirPID.PWM = Servo_Middle;    //���ȶ������
    DirPID.PWM -= DirPID.Kp*DirPID.Error + DirPID.Kd*(DirPID.Error - DirPID.Error1);
	DirPID.Error2 = DirPID.Error1;
    DirPID.Error1 = DirPID.Error;
}

void DirControl(void)
{
	static uint8_t emergency = 0;			//����ͣ��
	
	if(leftErCheng.effectLines <= MinUsefulLines && rightErCheng.effectLines <= MinUsefulLines)
	{
		emergency++;
		if(emergency>25)			//����ͣ��
		{
			if(EMERGRNCYSTOPCAR == true)	//���뿪��ѡ���Ƿ�����
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

//�����ٶ�ģʽ�������
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

