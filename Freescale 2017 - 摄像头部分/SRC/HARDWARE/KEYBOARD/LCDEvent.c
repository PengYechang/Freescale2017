#include "LCDEvent.h"

//��̬��������ֹ�ⲿ����
static void MenuMain(void);         //���˵�
static void MenuOne(void);			//�˵�1
static void MenuTwo(void);			//�˵�2

void LCDMenuMain_Init(void);
void LCDMenuOne_Init(void);
void LCDMenuTwo_Init(void);
void LCDFlag_Clear(void);

static void LCDMenuMain_Refresh(void);
static void LCDMenuOne_Refresh(void);
static void LCDMenuTwo_Refresh(void);

bool MenuMainRefresh;							//���˵����±�־
bool MenuOneRefresh;							//�˵�1���±�־
bool MenuTwoRefresh;							//�˵�2���±�־

//���庯��ָ��
Menu  MenuSelect[MenuMax]= {MenuMain,MenuOne,MenuTwo};

Menuinfo MenuInfo = {(Menufunc)0};				//�˵�����Ϣ

//�˵�����ʵʱ����
//MenuInfo.page����ѡ������Ӧ�ز˵���Ҳѡ������Ӧ�ذ����¼�
void LCDMenu_Refresh(void) 
{
    MenuSelect[MenuInfo.page]();//ѡ��˵�ҳ
}

//���˵�
static void MenuMain(void)      
{
    if(MenuMainRefresh == true)  //ˢ�²˵�
    {
       LCDMenuMain_Refresh();
    }
    else
    {
       OLED_CLS();
       LCDMenuMain_Init();  	//����ջص��˵���Ӧ�öԲ˵����и���
       LCDFlag_Clear();			//�����־λ
       MenuMainRefresh = true;
    }
}

//�˵�1�Ĵ���
static void MenuOne(void)    
{
    if(MenuOneRefresh == true)	//ˢ�²˵�
    {
        LCDMenuOne_Refresh();
    }
    else
    { 
        OLED_CLS();
        LCDMenuOne_Init(); 		//����ջص��˵���Ӧ�öԲ˵����и���
        LCDFlag_Clear();		//�����־λ
        MenuOneRefresh = true;
    }   
}

//�˵�2�Ĵ���
static void MenuTwo(void)
{
    if(MenuTwoRefresh == true)	//ˢ�²˵�
    {
        LCDMenuTwo_Refresh();
    }
    else
    { 
        OLED_CLS();
        LCDMenuTwo_Init();  	//����ջص��˵���Ӧ�öԲ˵����и���
        LCDFlag_Clear();		//�����־λ
        MenuTwoRefresh = true;
    }  
}

//���˵���ˢ��
static void LCDMenuMain_Refresh(void)
{
    KeyEvent[MenuInfo.page](); //���˵���Ӧ�İ����¼�
	
	if(AUTO_RUN_EN == true)
	{
		OLED_Show_Sentence(0,2,"1.Mode:AutoRun");
	}
	else
	{
		OLED_Show_Sentence(0,2,"1.Mode:Debug");
	}
//	OLED_Show_StrNum(0,2,"1.AutoRun:",AUTO_RUN_EN);
	OLED_Show_StrNum(0,3,"2.StartLine:",STARTLINE_DETECT);
//	OLED_Show_StrNum(0,4,"3.StopCar:",StopCar);
	OLED_Show_StrNum(0,4,"3.RamwayD:",RAMWAY_DETECT);
//	OLED_Show_StrNum(0,3,"2.Voltage:",SysVolt);
//	OLED_Show_StrNum(0,4,"3.Lendline:",leftErCheng.endLine);
//	OLED_Show_StrNum(0,5,"4.Rendline:",rightErCheng.endLine);
//	OLED_Show_StrNum(0,3,"2.LZigma",leftErCheng.zigma2);
//	OLED_Show_StrNum(0,4,"3.RZigma",rightErCheng.zigma2);

	if(Sys_Speed_Mode == LOW_SPEED)
	{
		OLED_Show_Sentence(0,5,"4.SpeedMode:LSpeed");
	}
	else if(Sys_Speed_Mode == MIDDLE_SPEED)
	{
		OLED_Show_Sentence(0,5,"4.SpeedMode:MSpeed");
	}
	else if(Sys_Speed_Mode == HIGH_SPEED)
	{
		OLED_Show_Sentence(0,5,"4.SpeedMode:HSpeed");
	}
	else if(Sys_Speed_Mode == KEY_SPEED)
	{
		OLED_Show_Sentence(0,5,"4.SpeedMode:KeySpeed");
	}
	else if(Sys_Speed_Mode == ROADTYPE_SPEED)
	{
		OLED_Show_Sentence(0,5,"4.SpeedMode:RSPeed");
	}
	else if(Sys_Speed_Mode == AFUZZY_SPEED)
	{
		OLED_Show_Sentence(0,5,"4.SpeedMode:AFPeed");
	}
	else if(Sys_Speed_Mode == BFUZZY_SPEED)
	{
		OLED_Show_Sentence(0,5,"4.SpeedMode:BFPeed");
	}
	else if(Sys_Speed_Mode == CFUZZY_SPEED)
	{
		OLED_Show_Sentence(0,5,"4.SpeedMode:CFPeed");
	}
	else if(Sys_Speed_Mode == DFUZZY_SPEED)
	{
		OLED_Show_Sentence(0,5,"4.SpeedMode:DFPeed");
	}
	OLED_Show_StrNum(0,6,"5.Voltage:",SysVolt);
	OLED_Show_StrNum(0,7,"6.error",DirPID.Error);
}

//�˵�1��ˢ��
static void LCDMenuOne_Refresh(void)
{
    KeyEvent[MenuInfo.page](); //�˵�1��Ӧ�İ����¼�
	Road_Show(&ImgClass.leftLine[0],&ImgClass.RightLine[0]);
}

//�˵�2��ˢ��
static void LCDMenuTwo_Refresh(void)
{
    KeyEvent[MenuInfo.page](); //�˵�2��Ӧ�İ����¼�
}

//���˵��ĳ�ʼ����
void LCDMenuMain_Init()
{
     OLED_Show_Sentence(2,1,"Whut PoFeng Team!");	//������ʾ����
}

//�˵�1�ĳ�ʼ����
void LCDMenuOne_Init()
{
//	StatusPageDisplay();							//����״̬ҳ
	Road_Show(&ImgClass.leftLine[0],&ImgClass.RightLine[0]);
}

//�˵�2�ĳ�ʼ����
void LCDMenuTwo_Init(void)
{
	PageEvent.Change = true;						
	VarPageDisplay();								//�������ҳ
}

//�����־λ
void LCDFlag_Clear()      		
{
    MenuMainRefresh = false;
    MenuOneRefresh  = false;
    MenuTwoRefresh  = false;
}
