#include "servo.h"
#include "camera.h"

#define IMG_CENTER	(Image_Width/2)
/* ¶æ»ú×ÊÁÏ
????: CW pulse ?(1500 ? 900 [us])
CCW pulse ?(1500? 2100 [us])
?????:= 5°
????: 4.0 ~ 6.0 [V]
????: -10 ~ + 50[?]

*/

uint8_t Shift = 3;
uint32_t Servo_Error  = 100;       //¶æ»úÆ«²î
uint32_t Servo_Middle = 430;		  					//¶æ»úÖÐ¼ä
uint32_t Servo_Left   = 530; 	    //¶æ»úÓÒ¹Õµ½µ×
uint32_t Servo_Right  = 330;		//¶æ»ú×ó¹Õµ½µ×

int16_t Car_servo_PWM;
int16_t Car_centerLine;

void calculateTurnAngle(int16_t *centerLine)
{
	int16_t angle = Servo_Middle;
	int16_t startCount = centerLine[Image_Height-1];
	int16_t middleCount = centerLine[(Image_Height-1+centerLine[Image_Height])/2];
	int16_t endCount = centerLine[centerLine[Image_Height]];
	
	int16_t end_middle = endCount - middleCount;
	int16_t middle_start = middleCount - startCount;
	//int16_t end_start = endCount - startCount;
	
	//int16_t end_middle_abs = (end_middle<0) ? (0-end_middle) : end_middle;
	int16_t middle_start_abs = (middle_start<0) ? (0-middle_start) : middle_start;
	//int16_t end_start_abs = (end_start<0) ? (0-end_start) : end_start;
	
	int16_t parameterX0;
	int16_t parameterY0;
	int16_t parameterX1;
	int16_t parameterY1;
	double parameterK;
	double parameterB;
	int16_t pointX;
	//int16_t pointY;
	
	if((end_middle >= 0 && middle_start >= 0) || (end_middle <= 0 && middle_start <= 0) || middle_start_abs <= 3)
	{
		if(centerLine[Image_Height] > 0)
			angle = (centerLine[centerLine[Image_Height]]>=IMG_CENTER)
				  ? ((Servo_Middle-IMG_CENTER)-centerLine[Image_Height]*3)
				  : ((Servo_Middle+IMG_CENTER)+centerLine[Image_Height]*3);
		else
			angle = Servo_Middle-(centerLine[0]-IMG_CENTER);
	}

	
	else if((middle_start > 0 && end_middle < 0) || (middle_start < 0 && end_middle > 0))
	{
		parameterX0 = startCount;
		parameterY0 = Image_Height - 1;
		parameterX1 = middleCount;
		parameterY1 = (Image_Height-1+centerLine[Image_Height])/2;
		parameterK = (parameterY0 - parameterY1)/(parameterX0 - parameterX1);
		parameterB = parameterY0 - parameterK * parameterX0;			
		
		pointX = (int16_t)((0-parameterB/parameterK) + 0.5);
		
		if(pointX < 0)     //Òª×ó¹ÕºÜ´óµÄ½Ç¶È
		{
			//pointY = (int16_t)parameterB;
			//angle = (Servo_Middle+IMG_CENTER)+pointY*3;
			angle = Servo_Middle + 20;              //×¢Òâ£¡£¡£¡£¡£¡£¡20Òª¸Ä°¡£¡£¡£¡£¡
		}
		else if(pointX > Image_Width-1)   //ÒªÓÒ¹ÕºÜ´óµÄ½Ç¶È
		{
			//pointY = (int16_t)((Image_Width-1)*parameterK+parameterB+0.5);
			//angle = (Servo_Middle-IMG_CENTER)-pointY*3;
			angle = Servo_Middle - 20;				//×¢Òâ£¡£¡£¡£¡£¡£¡20Òª¸Ä°¡£¡£¡£¡£¡
		}
		else
			// angle = Servo_Middle-(pointX-IMG_CENTER);
			angle = (int16_t)((Servo_Middle - (pointX-IMG_CENTER)/2) + 0.5);     //Òª¸ù¾ÝÊµ¼ÊÇé¿ö¸Ä
	}

	
	angle -= ((centerLine[Image_Height-1] - IMG_CENTER) * 3); 	  //Ò»¿ªÊ¼µÄ²¹³¥½Ç  £¡£¡£¡¡¢¸Ä
	Car_servo_PWM = calculatePidAngle(angle);
	
	
	if(angle > Servo_Left)
		angle = Servo_Left;
	else if(angle < Servo_Right)
		angle = Servo_Right;	
	if((end_middle >= 0 && middle_start >= 0) || (end_middle <= 0 && middle_start <= 0) || middle_start_abs <= 3)
		Car_centerLine = (int8_t)((Servo_Middle-angle)/10+0.5);
	else
		Car_centerLine = 10;	
	if(Car_centerLine > 10)
		Car_centerLine = 10;
	else if(Car_centerLine < -10)
		Car_centerLine = -10;
	
	FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH3,Car_servo_PWM);	  //ÉèÖÃ¶æ»úµÄÖµ
}



typedef struct
{
	int16_t preOneDeviation;		
	int16_t preTwoDeviation;		
	int16_t pidAngle;			
} anglePID;

typedef struct
{
	float proportion;			
	float integral;			
	float derviative;			
} anglePidParams;

static anglePID anglePid;
static anglePidParams angleParams;

void servoPidInit()
{
	anglePid.preOneDeviation = 0;
	anglePid.preTwoDeviation = 0;
	anglePid.pidAngle = 0;
	
	angleParams.proportion = 0;
	angleParams.integral = 0;
	angleParams.derviative = 0;
}

void updateServoPid()
{
	angleParams.proportion = 0;
	angleParams.integral = 0;
	angleParams.derviative = 0;
}

int16_t calculatePidAngle(int16_t purpostAngle)
{
	int16_t currentDeviation = purpostAngle - anglePid.pidAngle;
	
	//double increase = (carParams.encode>encodeTable[15]) ? (carParams.encode/encodeTable[15])
	//	: ((carParams.encode<-encodeTable[15]) ? -(carParams.encode/encodeTable[15]) : 1);
	
	double angleIncremental = angleParams.proportion * currentDeviation// * increase
		+ angleParams.integral * (currentDeviation - anglePid.preOneDeviation)
		+ angleParams.derviative * ((currentDeviation - anglePid.preOneDeviation) - (anglePid.preOneDeviation - anglePid.preTwoDeviation));
	
	anglePid.preOneDeviation = currentDeviation;
	anglePid.preTwoDeviation = anglePid.preOneDeviation;
	
	anglePid.pidAngle += (int16_t)(angleIncremental+0.5);
	
	//anglePid.pidAngle *= increase;
	
	if(anglePid.pidAngle > Servo_Left)
		anglePid.pidAngle = Servo_Left;
	else if(anglePid.pidAngle < Servo_Right)
		anglePid.pidAngle = Servo_Right;
	
	return anglePid.pidAngle;
}


//¶æ»ú³õÊ¼»¯
void Servo_Init(void)
{
	//C³µ¶æ»ú  50Hz    B³µ¶æ»ú  300Hz
	FTM_PWM_QuickInit(FTM0_CH3_PC04,kPWM_EdgeAligned,300);    //FTM2  Í¨µÀ0  PB18  ±ßÑØ¶ÔÆëÄ£Ê½   50Hz
	FTM_PWM_ChangeDuty(HW_FTM0,HW_FTM_CH3,Servo_Middle);	  //Ê×ÏÈ¶æ»ú°Úµ½ÖÐ¼ä
	
	Servo_Left = Servo_Middle + Servo_Error;				  //¶æ»úÓÒ¹Õµ½µ×
	Servo_Right = Servo_Middle - Servo_Error;			  	  //¶æ»ú×ó¹Õµ½µ×
}


