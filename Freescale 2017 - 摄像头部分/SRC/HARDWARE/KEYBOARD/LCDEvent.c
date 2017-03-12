#include "LCDEvent.h"

//静态函数，防止外部调用
static void MenuMain(void);         //主菜单
static void MenuOne(void);			//菜单1
static void MenuTwo(void);			//菜单2

void LCDMenuMain_Init(void);
void LCDMenuOne_Init(void);
void LCDMenuTwo_Init(void);
void LCDFlag_Clear(void);

static void LCDMenuMain_Refresh(void);
static void LCDMenuOne_Refresh(void);
static void LCDMenuTwo_Refresh(void);

bool MenuMainRefresh;							//主菜单更新标志
bool MenuOneRefresh;							//菜单1更新标志
bool MenuTwoRefresh;							//菜单2更新标志

//定义函数指针
Menu  MenuSelect[MenuMax]= {MenuMain,MenuOne,MenuTwo};

Menuinfo MenuInfo = {(Menufunc)0};				//菜单的信息

//菜单进行实时更新
//MenuInfo.page不仅选择了相应地菜单，也选择了相应地按键事件
void LCDMenu_Refresh(void) 
{
    MenuSelect[MenuInfo.page]();//选择菜单页
}

//主菜单
static void MenuMain(void)      
{
    if(MenuMainRefresh == true)  //刷新菜单
    {
       LCDMenuMain_Refresh();
    }
    else
    {
       OLED_CLS();
       LCDMenuMain_Init();  	//如果刚回到菜单，应该对菜单进行更新
       LCDFlag_Clear();			//清除标志位
       MenuMainRefresh = true;
    }
}

//菜单1的处理
static void MenuOne(void)    
{
    if(MenuOneRefresh == true)	//刷新菜单
    {
        LCDMenuOne_Refresh();
    }
    else
    { 
        OLED_CLS();
        LCDMenuOne_Init(); 		//如果刚回到菜单，应该对菜单进行更新
        LCDFlag_Clear();		//清除标志位
        MenuOneRefresh = true;
    }   
}

//菜单2的处理
static void MenuTwo(void)
{
    if(MenuTwoRefresh == true)	//刷新菜单
    {
        LCDMenuTwo_Refresh();
    }
    else
    { 
        OLED_CLS();
        LCDMenuTwo_Init();  	//如果刚回到菜单，应该对菜单进行更新
        LCDFlag_Clear();		//清除标志位
        MenuTwoRefresh = true;
    }  
}

//主菜单的刷新
static void LCDMenuMain_Refresh(void)
{
    KeyEvent[MenuInfo.page](); //主菜单对应的按键事件
	
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

//菜单1的刷新
static void LCDMenuOne_Refresh(void)
{
    KeyEvent[MenuInfo.page](); //菜单1对应的按键事件
	Road_Show(&ImgClass.leftLine[0],&ImgClass.RightLine[0]);
}

//菜单2的刷新
static void LCDMenuTwo_Refresh(void)
{
    KeyEvent[MenuInfo.page](); //菜单2对应的按键事件
}

//主菜单的初始界面
void LCDMenuMain_Init()
{
     OLED_Show_Sentence(2,1,"Whut PoFeng Team!");	//顶部显示队名
}

//菜单1的初始界面
void LCDMenuOne_Init()
{
//	StatusPageDisplay();							//进入状态页
	Road_Show(&ImgClass.leftLine[0],&ImgClass.RightLine[0]);
}

//菜单2的初始界面
void LCDMenuTwo_Init(void)
{
	PageEvent.Change = true;						
	VarPageDisplay();								//进入变量页
}

//清除标志位
void LCDFlag_Clear()      		
{
    MenuMainRefresh = false;
    MenuOneRefresh  = false;
    MenuTwoRefresh  = false;
}
