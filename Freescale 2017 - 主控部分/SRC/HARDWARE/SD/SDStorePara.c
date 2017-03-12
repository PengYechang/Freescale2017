#include "SDStorePara.h"

#define SDPage 10
#define PageWi 5
#define ParaSector 20000				  //��������
#define USE_SDCARD

int32_t SDPara[SDPage][PageWi];			  //����������
uint8_t SDBuffer[512]={0};			      //������
uint8_t  readSDParaOk = false;			  //��SD������
SDCardInfoClass SDCardInfo;

void pushDataToSDBuffer(void);			  //�����ݷŵ�SD��������

//��ʼ��SD��,������ʲô��,����0���ǳ�ʼ����ȷ
void SDCard_Init(void)
{
#ifdef USE_SDCARD							//ʹ��SD��
	uint8_t SDStatus = 1;
	SDStatus = SD_QuickInit(20000000);		//��ʼ���ɹ��Ļ�,SDStatusӦ��Ϊ0
	SDCardInfo.SDCState = SDStatus;	    
	SDCardInfo.Sectors = SD_GetSizeInMB();  //��ȡSD������
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

//�������ݵ�SD��
uint8_t saveSDPara(void)
{
	uint8_t check = 0;
	uint8_t SDBuff[512]={0};
	int16_t i,j,p;
	
	if(SDCardInfo.SDCState == false)return false;	//��ʼ��ʧ�ܵĻ�ֱ�ӷ���
	
	pushDataToSDBuffer();							//�����ݷŵ�SDPara����
	for(i=0;i<512;i++)								//��ʼ������
	{
		SDBuffer[i] = i;
	}
	SDBuffer[0] = 254;							    //��ʼУ��λ
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
	SDBuffer[p] = check;						   //��������֮���������У��λ
  
	i=0;
	while(i<10)
	{
		if(!SD_WriteSingleBlock(ParaSector,SDBuffer))//д��SD������ȥ
		{
			DelayMs(2);
			j=0;
			while(j<4)
			{
				if(!SD_ReadSingleBlock(ParaSector,SDBuff))
					break; //��ȡ�ɹ�,��ȥ����
				j++;
			}
			if(j == 4)continue; //�����ȡ�����⣬˵��ûд�ɹ����ٴ�д

			j=1;
			for(p=0;p<512;p++)
			{
				if(SDBuffer[p] != SDBuff[p])//�����ȡ�ɹ���У��λҲӦ����ȷ
				{
					j = 0;
					break;
				}
			}
			if(j == 0)continue;//���У�������⣬�ٴ�д   
			return true;
		}
		i++;
	}
	return false;
}

//��SD���������һ���趨�Ĳ���
static uint8_t readSDPara(void)
{
	uint8_t check = 0;
	int16_t i,j,p;
	if(SDCardInfo.SDCState ==0) return false;		  //SD����ʼ���д�ֱ�ӷ���
	i=0;
	while(i<10)
	{
		if(!SD_ReadSingleBlock(ParaSector,SDBuffer))  //�����ݵ�SDBuffer
		break;
		i++;
	}
	if(i==10)return false;							  //����10�λ���û���ɹ�

	p = 3;
	check = (uint8_t)(254 + 119 + 3);
	if(SDBuffer[0]==254 && SDBuffer[1]==119 && SDBuffer[2]==3)   //�����ʼУ��λ
	{
		//�������ݵ����ݻ�����
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

		//���У��λ
		if(SDBuffer[p]==check)	    
		{  
			return true;		 //У��λ���ɹ�
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

//��С�����ݷ��ڻ�������
static void pushSDDouble(uint8_t x,uint8_t y,double data)
{
	int32_t integer,decimal;				//�ֱ��������С��
	integer = (int32_t)data;	            //��ȡ��������
	data -= integer;				        //ȥ����������
	data*=100;				                //�õ�С������
	decimal = (int32_t)data;                //������
	SDPara[x][y] = (integer<<8) + decimal;	 
}

//���������ݷ��ڻ�������
static void pushSDInt32(uint8_t x,uint8_t y,int32_t data)
{
	SDPara[x][y] = data; 
}

//��С������
static double readSDDouble(uint8_t x,uint8_t y)
{
	return (SDPara[x][y]>>8) + ((double)(SDPara[x][y]&0xFF))/100;
}

//����������
static int32_t readSDInt32(uint8_t x,uint8_t y)
{
	return SDPara[x][y];
}

//�����ݴ�SD������ϵͳ
void setSDParaToSys(void)
{
	readSDParaOk = readSDPara();
	if(readSDParaOk)			//��ȡ�ɹ�
	{
		if(Sys_Speed_Mode != readSDInt32(4,1))  //�ı��ٶ�ģʽ�ĵ�һ�Σ���д��������Ϊ��������һ���ٶȱ�����
		{
		}
		else
		{
			//��̬P����
			MinKp 				           = readSDDouble(1,0);
			MaxKp 						   = readSDDouble(1,1);
			CutOffset   				   = readSDDouble(1,2);
			MinSinglePreSee     		   = readSDInt32(1,3);
			MaxSinglePreSee     		   = readSDInt32(1,4);

			useDynamicPreSeeMinZigma	   = readSDInt32(2,0);
			useDynamicPreSeeMaxZigma	   = readSDInt32(2,1);
			UseLines 					   = readSDInt32(2,2);
			Servo_P 					   = readSDDouble(2,4);   		//����P����
			
			DirStatus                      = readSDInt32(3,0);
			Servo_D						   = readSDDouble(3,1);         //���D����
			SpeedPID.Kp 				   = readSDDouble(3,2);			//���PI����
			SpeedPID.Ki 				   = readSDDouble(3,3);
			MinSpeed 					   = readSDInt32(3,4);		    //���κ������ٲ���
			
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
			Servo_Middle				   = readSDInt32(6,2);		//�ı�����ֵ������ı��ٶ���ֵ�����Ϊ��ǰ��ֵ
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

//�����ݴ浽SD��
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
