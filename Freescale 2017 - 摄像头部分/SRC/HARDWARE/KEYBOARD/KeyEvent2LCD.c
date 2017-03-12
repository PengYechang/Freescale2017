#include "KeyEvent2LCD.h"

#define VAROFFSET    		 12

bool PrintfZero_Enable = true;				   //��ӡ����
bool StartRun = false;						   //�������

static void VarSelect(VAR_EVENT ctrl);    	   //�л�����
static void VarValue(VAR_EVENT ctrl);          //�ı������ֵ
static void VarOk(void);                       //ȷ�ϵ�ǰ��ֵ
static void VarCancel(void);                   //ȡ����ǰ��ֵ
void VarDisplay(uint8_t tab);                  //��ʾָ����ֵ,tab = VAR_NUM ʱ��ʾȫ����ֵ��С������ʾ��Ӧ��ֵ
void PageDisplay(void);

static void MenuMain_KeyEvent(void);			   //���˵��İ����¼�
static void MenuOne_KeyEvent(void);				   //�˵�1�İ����¼�
static void MenuTwo_KeyEvent(void);                //�˵�2�İ����¼�

//���´�����Ҫ��ʾ�ı���
//VAR_NUM ָ�����ܵı�����
char VarName[VAR_NUM][VAROFFSET] =
{
    "MinKp:","MaxKp:","CutOffset:","SMinPreSee:","SMaxPreSee:","MinZigma:","MaxZigma:","Servo_D:",     //��̬P�Ĳ�������
    "BlockAmp:","UFF[0]:","UFF[1]:","UFF[2]:","UFF[3]:","UFF[4]:","UFF[5]:","UFF[6]:",
    "BlockCS:","SpeedKi:","MinSpeed:","MaxSpeed:","RampSpeed:","BlockSpeed:","S2CSpeed:","ServoM:",
};

//���б����ĳ�ʼ��״̬
//ע����Сֵ��Ҫ���������ֵ
VAR_INFO VarInfo[VAR_NUM]=
{   
    //VAR    OLDVAR   MINVAR   MAXVAR     SITEX   SITEY        NAME
    {  0,		0,		0,		2000,		0,		0,		VarName[0]},			        //���� var0
    {  0,		0,		0,		2000,		0,		1,		VarName[1]},                    //���� var1,
    {  0,		0,		0,		2000,		0,		2,		VarName[2]},			        //���� var2,
    {  0,		0,		0,		2000,		0,		3,		VarName[3]},			        //���� var3,
    {  0,		0,		0,		2000,		0,		4,		VarName[4]},			        //���� var4,
    {  0,		0,		0,		2000,		0,		5,		VarName[5]},			        //���� var5,
    {  0,		0,		0,		2000,		0,		6,		VarName[6]},			        //���� var6,
    {  0,		0,		0,		2000,		0,		7,		VarName[7]},			        //���� var7,
	
    {  0,		0,		0,		2000,		0,		0,		VarName[8]},			        //���� var8,
    {  0,		0,		0,		2000,		0,		1,		VarName[9]},			        //���� var9,
    {  0,		0,		0,		2000,		0,		2,		VarName[10]},			        //���� var10,
    {  0,		0,		0,		2000,		0,		3,		VarName[11]},			        //���� var11,
    {  0,		0,		0,		2000,		0,		4,		VarName[12]},			        //���� var12,
    {  0,		0,		0,		2000,		0,		5,		VarName[13]},			        //���� var13,
    {  0,		0,		0,		2000,		0,		6,		VarName[14]},			        //���� var14,
    {  0,		0,		0,		2000,		0,		7,		VarName[15]},			        //���� var15,
	
    {  0,		0,		0,		10,		    0,		0,		VarName[16]},			        //���� var16,
	{  0,		0,		0,		2000,		0,		1,		VarName[17]},			        //���� var17,
    {  0,		0,		0,		2000,		0,		2,		VarName[18]},			        //���� var18,
    {  0,		0,		0,		2000,		0,		3,		VarName[19]},			        //���� var19,
    {  0,		0,		0,		2000,		0,		4,		VarName[20]},			        //���� var20,
    {  0,		0,		0,		2000,	    0,		5,		VarName[21]},			        //���� var21,
    {  0,		0,		0,		2000,		0,		6,		VarName[22]},			        //���� var22,
    {  0,		0,		0,		8000,		0,		7,		VarName[23]},			        //���� var23,
};

//keyecnet_t�Ǻ���ָ��
keyevent_t KeyEvent[PAGE_MAX] =       	   //����3ҳ�˵�
{
    MenuMain_KeyEvent,					   //��ҳ
    MenuOne_KeyEvent,					   //��һҳ
    MenuTwo_KeyEvent,					   //�ڶ�ҳ
//	MenuThree_KeyEvent,					   //����ҳ
};

PAGEEVENT       PageEvent;		           //ҳ�¼�����Ҫ������ҳ�͵�ǰ��ҳ��
int16_t         Loop_tab;				   //��� 0-23

//װ�ر�����ֵ�������24��ֵ����ʹ��
void LoadVarValue(void)
{
	VAR_VALUE(0) = (int32_t)(MinKp*10);					//��̬P
	VAR_VALUE(1) = (int32_t)(MaxKp*10);
	VAR_VALUE(2) = (int32_t)(CutOffset*100);			
	VAR_VALUE(3) = (int32_t)MinSinglePreSee;
	VAR_VALUE(4) = (int32_t)MaxSinglePreSee;
	VAR_VALUE(5) = (int32_t)useDynamicPreSeeMinZigma;
	VAR_VALUE(6) = (int32_t)useDynamicPreSeeMaxZigma;
	VAR_VALUE(7) = (int32_t)(Servo_D*10);
	
	VAR_VALUE(8) = (int32_t)(blockErrorAmpFactor*100);
	VAR_VALUE(9) = (int32_t)(UFF[0]);
	VAR_VALUE(10) = (int32_t)(UFF[1]);
	VAR_VALUE(11) = (int32_t)(UFF[2]);
	VAR_VALUE(12) = (int32_t)(UFF[3]);
	VAR_VALUE(13) = (int32_t)(UFF[4]);
	VAR_VALUE(14) = (int32_t)(UFF[5]);
	VAR_VALUE(15) = (int32_t)(UFF[6]);

//	VAR_VALUE(16) = (int32_t)(SpeedPID.Kp*10);	
	VAR_VALUE(16) = (int32_t)(roadLeftBlock.stateChangeCountThr);	
	VAR_VALUE(17) = (int32_t)(SpeedPID.Ki*10);
	VAR_VALUE(18) = (int32_t)(MinSpeed);
	VAR_VALUE(19) = (int32_t)(MaxSpeed);
	VAR_VALUE(20) = (int32_t)(RampSpeed);
	VAR_VALUE(21) = (int32_t)(BlockSpeed);
	VAR_VALUE(22) = (int32_t)(StraightToCurveSpeed);
	VAR_VALUE(23) = (int32_t)(Servo_Middle);
	

	VAR_OLDVALUE(0) = (int32_t)(MinKp*10);
	VAR_OLDVALUE(1) = (int32_t)(MaxKp*10);
	VAR_OLDVALUE(2) = (int32_t)(CutOffset*100);
	VAR_OLDVALUE(3) = (int32_t)MinSinglePreSee;
	VAR_OLDVALUE(4) = (int32_t)MaxSinglePreSee;
	VAR_OLDVALUE(5) = (int32_t)useDynamicPreSeeMinZigma;
	VAR_OLDVALUE(6) = (int32_t)useDynamicPreSeeMaxZigma;
	VAR_OLDVALUE(7) = (int32_t)(Servo_P*10);
	
	VAR_OLDVALUE(8) = (int32_t)(blockErrorAmpFactor*100);
	VAR_OLDVALUE(9) = (int32_t)(UFF[0]);
	VAR_OLDVALUE(10) = (int32_t)(UFF[1]);
	VAR_OLDVALUE(11) = (int32_t)(UFF[2]);
	VAR_OLDVALUE(12) = (int32_t)(UFF[3]);
	VAR_OLDVALUE(13) = (int32_t)(UFF[4]);
	VAR_OLDVALUE(14) = (int32_t)(UFF[5]);
	VAR_OLDVALUE(15) = (int32_t)(UFF[6]);
	
//	VAR_OLDVALUE(16) = (int32_t)(SpeedPID.Kp*10);
	VAR_OLDVALUE(16) = (int32_t)(roadLeftBlock.stateChangeCountThr);	
	VAR_OLDVALUE(17) = (int32_t)(SpeedPID.Ki*10);
	VAR_OLDVALUE(18) = (int32_t)(MinSpeed);
	VAR_OLDVALUE(19) = (int32_t)(MaxSpeed);
	VAR_OLDVALUE(20) = (int32_t)(RampSpeed);
	VAR_OLDVALUE(21) = (int32_t)(BlockSpeed);
	VAR_OLDVALUE(22) = (int32_t)(StraightToCurveSpeed);
	VAR_OLDVALUE(23) = (int32_t)(Servo_Middle);}

//���仯���ֵ���д���
void SaveVarValue(void)
{
	MinKp = (double)(VAR_VALUE(0))/10.0;
	MaxKp = (double)(VAR_VALUE(1))/10.0;
	CutOffset = (double)(VAR_VALUE(2))/100.0;
	MinSinglePreSee = VAR_VALUE(3);
	MaxSinglePreSee = VAR_VALUE(4);
	useDynamicPreSeeMinZigma = VAR_VALUE(5);
	useDynamicPreSeeMaxZigma = VAR_VALUE(6);
	Servo_D = (double)(VAR_VALUE(7)/10.0);
	
	blockErrorAmpFactor = (double)VAR_VALUE(8)/100.0;
	UFF[0] = VAR_VALUE(9);
	UFF[1] = VAR_VALUE(10);
	UFF[2] = VAR_VALUE(11);
	UFF[3] = VAR_VALUE(12);
	UFF[4] = VAR_VALUE(13);
	UFF[5] = VAR_VALUE(14);
	UFF[6] = VAR_VALUE(15);
	
//	SpeedPID.Kp = (double)(VAR_VALUE(16))/10.0;
	roadLeftBlock.stateChangeCountThr = VAR_VALUE(16);
	SpeedPID.Ki = (double)(VAR_VALUE(17))/10.0;
	MinSpeed = VAR_VALUE(18);
	MaxSpeed = VAR_VALUE(19);	
	RampSpeed = VAR_VALUE(20);
	BlockSpeed = VAR_VALUE(21);
	StraightToCurveSpeed = VAR_VALUE(22);
	Servo_Middle = VAR_VALUE(23);
}

//���˵��İ����¼�����
//���˵���Ҫ�г������˵��Ĺ���
static void MenuMain_KeyEvent(void)
{
	KEY_MSG keymsg;
	
	while(Get_KeyMsg(& keymsg))   				//��ȡ��������Ϣ
	{
		if(keymsg.status == KEY_DOWN || keymsg.status == KEY_HOLD)	//����������ʱ
		{
			switch(keymsg.key)					//���ݰ����ĸ�������ȡ��Ӧ�Ķ���
			{
				case KEY_PREV:					//��һҳ,��һҳ��Ϊ���һ���˵���Ϊͼ����ʾҳ
					 MenuInfo.page = (Menufunc)2;
					break;
				case KEY_NEXT:					//��һҳ����һҳΪ״̬ҳ
					 MenuInfo.page = (Menufunc)1; 
					break;
				default:
					break;
			}
		}
	}
}

//�˵�1�İ����¼�����
//�˵�1��Ҫ��ʾС���ĸ���״̬
static void MenuOne_KeyEvent(void)
{
	KEY_MSG keymsg;
	
	while(Get_KeyMsg(& keymsg))   				//��ȡ��������Ϣ
	{
		if(keymsg.status == KEY_DOWN || keymsg.status == KEY_HOLD)	//����������ʱ
		{
			switch(keymsg.key)					//���ݰ����ĸ�������ȡ��Ӧ�Ķ���
			{
				case KEY_PREV:					//��һҳ,��һҳΪ��ҳ
					 MenuInfo.page = (Menufunc)0;
					break;
				case KEY_NEXT:					//��һҳ����һҳΪ������ʾҳ
					 MenuInfo.page = (Menufunc)2; 
					break;
				default:
					break;
			}
		}
	}
}

//�˵�2�İ����¼�����
//�˵�2��Ҫ���б������޸�
static void MenuTwo_KeyEvent(void)
{
	KEY_MSG keymsg;
	
	while(Get_KeyMsg(& keymsg))   				//��ȡ��������Ϣ
	{
		if(keymsg.status == KEY_DOWN)			//�������Ƕ̰�ʱ
		{
			switch(keymsg.key)					//���ݰ����ĸ�������ȡ��Ӧ�Ķ���
			{
				case KEY_ADD:
					VarValue(VAR_ADD);			//�������Ǽӿ���ʱ��������1
					break;
				case KEY_SUB:
					VarValue(VAR_SUB);			//�������Ǽ�����ʱ��������1
					break;
				case KEY_PREV:
					VarSelect(VAR_PREV);		//����������һ������ʱ��ѡ����һ������
					break;
				case KEY_NEXT:
					VarSelect(VAR_NEXT);		//����������һ������ʱ��ѡ����һ������
					break;
				case KEY_OK:
					VarOk();					//��������ȷ�Ͽ���ʱ�������ʱ�ı���ֵ
//					Motor_Run_Enable = true;
					StartRun = true;
					PrintfZero_Enable = true;
					break;
				case KEY_CANCLE:				//��������ȡ������ʱ��ȡ����ǰ�ı���ֵ
					VarCancel();
					break;
				default:
					break;
			}
		}
		else										//�������ǳ���ʱ
		{
			switch(keymsg.key)						//���ݰ����ĸ�������ȡ��Ӧ�Ķ���
			{
				case KEY_ADD:
					VarValue(VAR_ADD_HOLD);			//�������Ǽӿ���ʱ
					break;
				case KEY_SUB:
					VarValue(VAR_SUB_HOLD);			//�������Ǽ�����ʱ
					break;
				case KEY_PREV:
					VarSelect(VAR_PREV_HOLD);		//����������һ������ʱ
					break;
				case KEY_NEXT:
					VarSelect(VAR_NEXT_HOLD);		//����������һ������ʱ
					break;
				case KEY_OK:
					MenuInfo.page = (Menufunc)1;	//��������ȷ�Ͽ���ʱ��ת��״̬ҳ
					break;
				case KEY_CANCLE:					
					MenuInfo.page = (Menufunc)0;		//��������ȡ������ʱ������һҳ������ҳ
					break;
				default:
					break;
			}
		}
	}
}

//�л�����
static void VarSelect(VAR_EVENT  ctrl)
{
	uint8_t Old_tab = Loop_tab;						//���ݵ�ǰ�ı��
	
	//��������ѡ���Ӧ�Ŀ���
	switch(ctrl)
	{	
		case VAR_NEXT:			 					//��һ��	
			Loop_tab++;    							//����Լ�
			if(Loop_tab >= VAR_NUM)					//�жϱ���Ƿ�Խ��
			{
				Loop_tab = 0;
			}
			if((Loop_tab%8) == 0)         			//���л������ӵ�һҳ����������ʱ(7),��ҳ
			{
				PageEvent.Page++; 					//��¼ҳ��
				PageEvent.Change = true;			//ҳ�ı�
				if(PageEvent.Page >= PAGE_NUM) 		//�ж�ҳ���Ƿ�Խ��,�����3ҳ
				{
					PageEvent.Page = 0;
				}
			}
			break;	
		case VAR_PREV:								//��һ��
			Loop_tab--;								//����Լ�
			if(Loop_tab < 0)						//�жϱ���Ƿ�Խ��
			{
				Loop_tab = VAR_NUM - 1;		       
			}
			if((Loop_tab%8) == 7 )    				//���л���������һҳ����С������ʱ(0),��ҳ
			{
				PageEvent.Page--; 					//��¼ҳ��
				PageEvent.Change = true;			//ҳ�ı�
				if(PageEvent.Page < 0) 			    //�ж�ҳ���Ƿ�Խ��
				{
					PageEvent.Page = PAGE_NUM - 1;
				}
			}
			break;	
		case VAR_NEXT_HOLD:							//����,��������һҳ
			PageEvent.Page++; 
			PageEvent.Change = true;
			if(PageEvent.Page >= PAGE_NUM) 			//�ж�ҳ���Ƿ�Խ��
			{
				PageEvent.Page= 0;
			}
			Loop_tab = PageEvent.Page * 8;
			break;
			
		case VAR_PREV_HOLD:					   		//���ϣ���������һҳ
			PageEvent.Page--; 
			PageEvent.Change = true;
			if(PageEvent.Page < 0) 					//�ж�ҳ���Ƿ�Խ��
			{
				PageEvent.Page = PAGE_NUM - 1;
			}
			Loop_tab = PageEvent.Page * 8;
			break;			
		default:									//��Чѡ�������л�
			return;
	}
    if(PageEvent.Change == false)
    {
	    VarDisplay(Old_tab);		    			//���ҳû�иı�ʹ�����һҳ�ı���
    }
    VarDisplay(Loop_tab);							//����ǰ�ı���
    VarPageDisplay();								//ҳ��ʾ
}

//�ı������ֵ
static void VarValue(VAR_EVENT ctrl)
{
	//��������ѡ���Ӧ�Ŀ���
	switch(ctrl)
	{
		case VAR_ADD:											//��1
			if(VAR_VALUE(Loop_tab ) < VAR_MAXVALUE(Loop_tab))	//�жϱ���ֵ�Ƿ�Խ��
			{
				VAR_VALUE(Loop_tab)++;
			}
			else												//�������ﵽ���ֵʱ���������л�Ϊ��Сֵ
			{
				VAR_VALUE(Loop_tab) = VAR_MINVALUE(Loop_tab);
			}
			break;	
		case VAR_SUB:											//��1
			if(VAR_VALUE(Loop_tab) > VAR_MINVALUE(Loop_tab))	//�жϱ���ֵ�Ƿ�Խ��
			{
				VAR_VALUE(Loop_tab)--;
			}
			else
			{
				VAR_VALUE(Loop_tab) = VAR_MAXVALUE(Loop_tab) ;	//�������ﵽ��Сֵʱ���������л�Ϊ���ֵ
			}
			break;
		case VAR_ADD_HOLD:										//���
			if((VAR_MAXVALUE(Loop_tab)-VAR_VALUE(Loop_tab)) >  VAR_VALUE_HOLD_OFFSET )
			{
				VAR_VALUE(Loop_tab)+=VAR_VALUE_HOLD_OFFSET;
			}
			else												//�������ﵽ���ֵʱ���������л�Ϊ��Сֵ
			{
				VAR_VALUE(Loop_tab) = VAR_MINVALUE(Loop_tab);
			}
			break;
			
		case VAR_SUB_HOLD:										//���
			if( ( VAR_VALUE(Loop_tab) - VAR_MINVALUE(Loop_tab)) > VAR_VALUE_HOLD_OFFSET  )
			{
				VAR_VALUE(Loop_tab)-=VAR_VALUE_HOLD_OFFSET;
			}
			else
			{
				VAR_VALUE(Loop_tab) = VAR_MAXVALUE(Loop_tab) ;  //�������ﵽ��Сֵʱ���������л�Ϊ���ֵ
			}
			break;		
		default:												//��Чѡ�񣬱�������仯
			break;
	}
	VarDisplay(Loop_tab);										//����ǰ�ı���
}

//ȷ�ϵ�ǰ��ֵ
static void VarOk(void)
{
	uint8_t i=0;
    for(i=0;i<VAR_NUM;i++)
    {
        VAR_OLDVALUE(i) = VAR_VALUE(i);							//���¾ɵ�ֵ
    }
    SaveVarValue();
}

//ȡ����ǰѡ���ֵ
static void VarCancel(void)
{
	VAR_VALUE(Loop_tab) = VAR_OLDVALUE(Loop_tab);				//��ԭ��ǰֵ
	VarDisplay(Loop_tab);										//����ǰ�ı���
}

//��ʾָ����ֵ,tab = VAR_NUM ʱ��ʾȫ����ֵ��С������ʾ��Ӧ��ֵ
//ʵ������Ҫ������ʾ�ı�ţ������ڳ���������ж��Ƿ��Ѿ����з�ҳ��
//���Ƿ��Ѿ��ﵽ������ˣ�����������Ϊ���ڷ�ֹ��ҳ���߱�Ŵﵽ���
//���֮�󣬲������´���
void VarDisplay(uint8_t tab)
{
	uint8_t  i = 0;
	uint8_t Tab;
	if(PageEvent.Change == true)								//����з�ҳ����������ҳ�ĵ�һ��
	{
		tab = PageEvent.Page * 8;
        i = 7;
	}
    if(tab == VAR_NUM) 										
    {
        tab = 0;
        i = 7;
    }
    Tab = tab;
	do
	{
		if(Tab == Loop_tab)
		{
			//��ʾ��ǰ��ֵ:�ж�ֵ�Ƿ�ı�
			if(VAR_VALUE(Tab) == VAR_OLDVALUE(Tab))				//ֵû�ı䣬����Ҫ����
			{		
                OLED_Show_Sentence(20,VAR_SITEY(Tab),"N");
			}
			else
			{
				OLED_Show_Sentence(20,VAR_SITEY(Tab),"Y");
			}
            OLED_Cursor(21,VAR_SITEY(Tab));   				//�����ʾ
		}
		else											
		{
			//��ʾ��ǰ��ֵ:�ж�ֵ�Ƿ�ı�
			if(VAR_VALUE(Tab) == VAR_OLDVALUE(Tab))				//ֵû�ı䣬����Ҫ����
			{
				OLED_Show_Sentence(20,VAR_SITEY(Tab),"N");	
			}
			else
			{
				OLED_Show_Sentence(20,VAR_SITEY(Tab),"Y");
			}
            OLED_ClearCursor(21,VAR_SITEY(Tab));   				//������
		}
		OLED_PARTCLS(VAROFFSET,VAR_SITEY(Tab),6);				//����ϴο��ܲ���������
        OLED_Show_Number(VAROFFSET,VAR_SITEY(Tab),VAR_VALUE(Tab));
        Tab++;
	}while(i--);												//tab != VAR_NUM��ʱ��ִ��һ�ξ�����
}

void VarPageDisplay(void)
{
	uint8_t Tab = 0,i =0;
    if(PageEvent.Change == true)								//����Ѿ���ҳ
    {
        OLED_CLS();
        Tab = PageEvent.Page * 8;								//����л�����ҳ��һ��      
        for(i=Tab;i<Tab+8;i++)  								//8�б�����������ʾ
        {
            OLED_Show_Sentence(VAR_SITEX(i),VAR_SITEY(i),(uint8_t *)VAR_NAME(i));       
        }
        VarDisplay(0);
        PageEvent.Change = false;
    }
}
