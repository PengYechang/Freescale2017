#include "SDStorePara.h"

#define SDPage 10
#define PageWi 5
#define ParaSector 20000				  //数据扇区
#define USE_SDCARD

int32_t SDPara[SDPage][PageWi];			  //参数缓冲区
uint8_t SDBuffer[512]={0};			      //缓冲区
uint8_t  readSDParaOk = false;			  //读SD卡参数
SDCardInfoClass SDCardInfo;

void pushDataToSDBuffer(void);			  //将数据放到SD卡缓冲区

//初始化SD卡,不论是什么卡,返回0才是初始化正确
void SDCard_Init(void)
{
#ifdef USE_SDCARD							//使用SD卡
	uint8_t SDStatus = 1;
	SDStatus = SD_QuickInit(20000000);		//初始化成功的话,SDStatus应该为0
	SDCardInfo.SDCState = SDStatus;	    
	SDCardInfo.Sectors = SD_GetSizeInMB();  //获取SD卡容量
	if(SDStatus == 0)
	{
		SDCardInfo.SDCState = true;
	}
	else
	{
		SDCardInfo.SDCState = false;
	}
	SDCardInfo.TransferTime  = 0;
#endif
}

//保存数据到SD卡
uint8_t saveSDPara(void)
{
	uint8_t check = 0;
	uint8_t SDBuff[512]={0};
	int16_t i,j,p;
	
	if(SDCardInfo.SDCState == false)return false;	//初始化失败的话直接返回
	
	pushDataToSDBuffer();							//将数据放到SDPara数组
	for(i=0;i<512;i++)								//初始化数组
	{
		SDBuffer[i] = i;
	}
	SDBuffer[0] = 254;							    //起始校验位
	SDBuffer[1] = 119;
	SDBuffer[2] = 3;

	check = (uint8_t)(254 + 119 + 3);
	
	p=3;
	for(i=0;i<SDPage;i++)
	{
		for(j=0;j<PageWi;j++)
		{
		  SDBuffer[p] = (SDPara[i][j]>>24)&0xFF;  check += SDBuffer[p];  p++; 
		  SDBuffer[p] = (SDPara[i][j]>>16)&0xFF;  check += SDBuffer[p];  p++;
		  SDBuffer[p] = (SDPara[i][j]>>8 )&0xFF;  check += SDBuffer[p];  p++;
		  SDBuffer[p] = (SDPara[i][j]>>0 )&0xFF;  check += SDBuffer[p];  p++;
		}
	}
	SDBuffer[p] = check;						   //所有数据之和组成最后的校验位
  
	i=0;
	while(i<10)
	{
		if(!SD_WriteSingleBlock(ParaSector,SDBuffer))//写到SD卡里面去
		{
			DelayMs(2);
			j=0;
			while(j<4)
			{
				if(!SD_ReadSingleBlock(ParaSector,SDBuff))
					break; //读取成功,出去处理
				j++;
			}
			if(j == 4)continue; //如果读取有问题，说明没写成功，再次写

			j=1;
			for(p=0;p<512;p++)
			{
				if(SDBuffer[p] != SDBuff[p])//如果读取成功，校验位也应该正确
				{
					j = 0;
					break;
				}
			}
			if(j == 0)continue;//如果校验有问题，再次写   
			return true;
		}
		i++;
	}
	return false;
}

//从SD卡里读出上一次设定的参数
static uint8_t readSDPara(void)
{
	uint8_t check = 0;
	int16_t i,j,p;
	if(SDCardInfo.SDCState ==0) return false;		  //SD卡初始化有错直接返回
	i=0;
	while(i<10)
	{
		if(!SD_ReadSingleBlock(ParaSector,SDBuffer))  //读数据到SDBuffer
		break;
		i++;
	}
	if(i==10)return false;							  //读了10次还是没读成功

	p = 3;
	check = (uint8_t)(254 + 119 + 3);
	if(SDBuffer[0]==254 && SDBuffer[1]==119 && SDBuffer[2]==3)   //检查起始校验位
	{
		//读入数据到数据缓冲区
		for(i=0;i<SDPage;i++)
		{
			for(j=0;j<PageWi;j++)
			{
				SDPara[i][j] = ((int32_t)SDBuffer[p]<<24)+((int32_t)SDBuffer[p+1]<<16)+((int32_t)SDBuffer[p+2]<<8)+SDBuffer[p+3];
				check += SDBuffer[p];
				check += SDBuffer[p+1];
				check += SDBuffer[p+2];
				check += SDBuffer[p+3];
				p+=4;
			}
		}

		//检查校验位
		if(SDBuffer[p]==check)	    
		{  
			return true;		 //校验位检查成功
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

//将小数数据放在缓冲区中
static void pushSDDouble(uint8_t x,uint8_t y,double data)
{
	int32_t integer,decimal;				//分别存整数和小数
	integer = (int32_t)data;	            //获取整数部分
	data -= integer;				        //去掉整数部分
	data*=100;				                //得到小数部分
	decimal = (int32_t)data;                //存起来
	SDPara[x][y] = (integer<<8) + decimal;	 
}

//将整数数据放在缓存区中
static void pushSDInt32(uint8_t x,uint8_t y,int32_t data)
{
	SDPara[x][y] = data; 
}

//读小数数据
static double readSDDouble(uint8_t x,uint8_t y)
{
	return (SDPara[x][y]>>8) + ((double)(SDPara[x][y]&0xFF))/100;
}

//读整数数据
static int32_t readSDInt32(uint8_t x,uint8_t y)
{
	return SDPara[x][y];
}

//将数据从SD卡读到系统
void setSDParaToSys(void)
{
	readSDParaOk = readSDPara();
	if(readSDParaOk)			//读取成功
	{
		if(Sys_Speed_Mode != readSDInt32(4,1))  //改变速度模式的第一次，不写参数，因为参数是上一个速度保留的
		{
		}
		else
		{
			//动态P参数
			MinKp 				           = readSDDouble(1,0);
			MaxKp 						   = readSDDouble(1,1);
			CutOffset   				   = readSDDouble(1,2);
			MinSinglePreSee     		   = readSDInt32(1,3);
			MaxSinglePreSee     		   = readSDInt32(1,4);

			useDynamicPreSeeMinZigma	   = readSDInt32(2,0);
			useDynamicPreSeeMaxZigma	   = readSDInt32(2,1);
			UseLines 					   = readSDInt32(2,2);
			Servo_P 					   = readSDDouble(2,4);   		//二次P参数
			
			DirStatus                      = readSDInt32(3,0);
			Servo_D						   = readSDDouble(3,1);         //舵机D参数
			SpeedPID.Kp 				   = readSDDouble(3,2);			//电机PI参数
			SpeedPID.Ki 				   = readSDDouble(3,3);
			MinSpeed 					   = readSDInt32(3,4);		    //二次函数控速参数
			
			MaxSpeed   					   = readSDInt32(4,0);
			
			UFF[0] 						   = readSDInt32(4,2);
			UFF[1] 						   = readSDInt32(4,3);
			UFF[2] 						   = readSDInt32(4,4);
			UFF[3] 						   = readSDInt32(5,0);
			UFF[4] 						   = readSDInt32(5,1);
			UFF[5] 						   = readSDInt32(5,2);
			UFF[6] 						   = readSDInt32(5,3);
			RampSpeed                      = readSDInt32(5,4);
			
			BlockSpeed					   = readSDInt32(6,0);
			StraightToCurveSpeed           = readSDInt32(6,1);
		}
			blockErrorAmpFactor            = readSDDouble(2,3);
			Servo_Middle				   = readSDInt32(6,2);		//改变舵机中值，如果改变速度中值不会归为以前的值
			roadLeftBlock.stateChangeCountThr = readSDInt32(6,3);
		
			Servo_Left  = Servo_Middle + Servo_Error;
			Servo_Right = Servo_Middle - Servo_Error;
		
			roadLeftBlock.avoidStartLineAffectThr = roadLeftBlock.stateChangeCountThr;
			roadRightBlock.avoidStartLineAffectThr = roadLeftBlock.stateChangeCountThr;
	}
	else
	{
 
	}
}

//将数据存到SD卡
void pushDataToSDBuffer(void)
{
	pushSDDouble(1,0,MinKp);
	pushSDDouble(1,1,MaxKp);
	pushSDDouble(1,2,CutOffset);
	pushSDInt32(1,3,MinSinglePreSee);
	pushSDInt32(1,4,MaxSinglePreSee);
	
	pushSDInt32(2,0,useDynamicPreSeeMinZigma);
	pushSDInt32(2,1,useDynamicPreSeeMaxZigma);
	pushSDInt32(2,2,UseLines);
	pushSDDouble(2,3,blockErrorAmpFactor);
	pushSDDouble(2,4,Servo_P);
	
	pushSDInt32(3,0,DirStatus);
	pushSDDouble(3,1,Servo_D);
	pushSDDouble(3,2,SpeedPID.Kp);
	pushSDDouble(3,3,SpeedPID.Ki);
	pushSDInt32(3,4,MinSpeed);
	
	pushSDInt32(4,0,MaxSpeed);
	pushSDInt32(4,1,Sys_Speed_Mode);
	pushSDInt32(4,2,UFF[0]);
	pushSDInt32(4,3,UFF[1]);
	pushSDInt32(4,4,UFF[2]);
	
	pushSDInt32(5,0,UFF[3]);
	pushSDInt32(5,1,UFF[4]);
	pushSDInt32(5,2,UFF[5]);
	pushSDInt32(5,3,UFF[6]);
	pushSDInt32(5,4,RampSpeed);

	pushSDInt32(6,0,BlockSpeed);
	pushSDInt32(6,1,StraightToCurveSpeed);
	pushSDInt32(6,2,Servo_Middle);
	pushSDInt32(6,3,roadLeftBlock.stateChangeCountThr);
}
