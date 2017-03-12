#include "encoder.h"


//���1KHz��1s����ӦΪ1000ת
//CNTҪʱ�����㣬��֤�Դ�ʱλ��Ϊ0
#ifdef SingleMotor
int32_t speed = 0;
uint8_t dir = 0;
#endif

#ifdef DoubleMotor
int32_t speedLeft = 0;
uint8_t dirLeft = 0;
int32_t speedRight = 0;
uint8_t dirRight = 0;
#endif

/*
 ʱ�䣺2015/12/2
 �汾: V1.0
 ����: ��/˫����ı����ʼ��
*/
void Encoder_Init(void)
{
#ifdef SingleMotor   		      //�����
	//ʹ�÷���-�����ͱ�����  �ӿ�A12 A13
	FTM_QD_QuickInit(FTM1_QD_PHA_PA12_PHB_PA13,kFTM_QD_NormalPolarity,kQD_CountDirectionEncoding);
#endif
	
#ifdef DoubleMotor          	  //˫���
	//������ʹ�÷��������ͱ�������һ��ʹ��FTM��һ��ʹ��LPTMR����Ϊ˫���ĵ���Ͷ��һ����Ҫ����FTM
	FTM_QD_QuickInit(FTM1_QD_PHA_PA12_PHB_PA13,kFTM_QD_NormalPolarity,kQD_CountDirectionEncoding);
	LPTMR_PC_QuickInit(LPTMR_ALT2_PC05);  			//ʹ��PTC5������������
	GPIO_QuickInit(HW_GPIOA,17,kGPIO_Mode_IFT);     //ʹ��PTA17��������
#endif
}

#ifdef SingleMotor   		      //�����

/*
 ʱ�䣺2015/12/4
 �汾��V1.0
 ����: �������ȡ��������ֵ�뷽��
 ע�⣺������ֵ����0֮�������¼�����65535�����Գ�������Ҫ���д���
*/
void Encoder_GetData(void)
{
	FTM_QD_GetData(HW_FTM1,&speed,&dir);//��ֵ�ͷ���
	FTM_QD_ClearCount(HW_FTM1); 		//����֮���Ҫ���
}
#endif

#ifdef DoubleMotor     	  //˫���

/*
 ʱ�䣺2015/12/4
 �汾��V1.0
 ����: ˫�����ȡ����������ֵ�뷽��
 ע�⣺������ֵ����0֮�������¼�����65535�����Գ�������Ҫ���д���
*/
void EncoderLeft_GetData(void)
{
	FTM_QD_GetData(HW_FTM1,&speedLeft,&dirLeft);//��ֵ�ͷ���						//�������ܻᵼ�¶���֮��value��dirһֱ�䶯
	FTM_QD_ClearCount(HW_FTM1); 				//����֮���Ҫ���
}

/*
 ʱ�䣺2015/12/4
 �汾��V1.0
 ����: ˫�����ȡ�Ҳ��������ֵ
*/

void EncoderRight_GetValue(void)
{
	speedRight = LPTMR_PC_ReadCounter();         //��ֵ  ע���Z�벻��Z��оƬ������(�ں���������˵��)
	LPTMR_ClearCounter();                        //����֮�������һ��
}

/*
 ʱ�䣺2015/12/4
 �汾��V1.0
 ����: ˫�����ȡ�Ҳ�������ķ���
*/
void EncoderRight_GetDir(void)
{
	dirRight = GPIO_ReadBit(HW_GPIOA,17);       //������
}

#endif
