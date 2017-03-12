#include "KeyEvent2LCD.h"

#define VAROFFSET    		 12

bool PrintfZero_Enable = true;				   //打印数组
bool StartRun = false;						   //启动电机

static void VarSelect(VAR_EVENT ctrl);    	   //切换变量
static void VarValue(VAR_EVENT ctrl);          //改变变量的值
static void VarOk(void);                       //确认当前的值
static void VarCancel(void);                   //取消当前的值
void VarDisplay(uint8_t tab);                  //显示指定的值,tab = VAR_NUM 时显示全部的值，小于则显示对应的值
void PageDisplay(void);

static void MenuMain_KeyEvent(void);			   //主菜单的按键事件
static void MenuOne_KeyEvent(void);				   //菜单1的按键事件
static void MenuTwo_KeyEvent(void);                //菜单2的按键事件

//以下储存需要显示的变量
//VAR_NUM 指的是总的变量数
char VarName[VAR_NUM][VAROFFSET] =
{
    "MinKp:","MaxKp:","CutOffset:","SMinPreSee:","SMaxPreSee:","MinZigma:","MaxZigma:","Servo_D:",     //动态P的参数调整
    "BlockAmp:","UFF[0]:","UFF[1]:","UFF[2]:","UFF[3]:","UFF[4]:","UFF[5]:","UFF[6]:",
    "BlockCS:","SpeedKi:","MinSpeed:","MaxSpeed:","RampSpeed:","BlockSpeed:","S2CSpeed:","ServoM:",
};

//所有变量的初始化状态
//注意最小值不要大于最最大值
VAR_INFO VarInfo[VAR_NUM]=
{   
    //VAR    OLDVAR   MINVAR   MAXVAR     SITEX   SITEY        NAME
    {  0,		0,		0,		2000,		0,		0,		VarName[0]},			        //变量 var0
    {  0,		0,		0,		2000,		0,		1,		VarName[1]},                    //变量 var1,
    {  0,		0,		0,		2000,		0,		2,		VarName[2]},			        //变量 var2,
    {  0,		0,		0,		2000,		0,		3,		VarName[3]},			        //变量 var3,
    {  0,		0,		0,		2000,		0,		4,		VarName[4]},			        //变量 var4,
    {  0,		0,		0,		2000,		0,		5,		VarName[5]},			        //变量 var5,
    {  0,		0,		0,		2000,		0,		6,		VarName[6]},			        //变量 var6,
    {  0,		0,		0,		2000,		0,		7,		VarName[7]},			        //变量 var7,
	
    {  0,		0,		0,		2000,		0,		0,		VarName[8]},			        //变量 var8,
    {  0,		0,		0,		2000,		0,		1,		VarName[9]},			        //变量 var9,
    {  0,		0,		0,		2000,		0,		2,		VarName[10]},			        //变量 var10,
    {  0,		0,		0,		2000,		0,		3,		VarName[11]},			        //变量 var11,
    {  0,		0,		0,		2000,		0,		4,		VarName[12]},			        //变量 var12,
    {  0,		0,		0,		2000,		0,		5,		VarName[13]},			        //变量 var13,
    {  0,		0,		0,		2000,		0,		6,		VarName[14]},			        //变量 var14,
    {  0,		0,		0,		2000,		0,		7,		VarName[15]},			        //变量 var15,
	
    {  0,		0,		0,		10,		    0,		0,		VarName[16]},			        //变量 var16,
	{  0,		0,		0,		2000,		0,		1,		VarName[17]},			        //变量 var17,
    {  0,		0,		0,		2000,		0,		2,		VarName[18]},			        //变量 var18,
    {  0,		0,		0,		2000,		0,		3,		VarName[19]},			        //变量 var19,
    {  0,		0,		0,		2000,		0,		4,		VarName[20]},			        //变量 var20,
    {  0,		0,		0,		2000,	    0,		5,		VarName[21]},			        //变量 var21,
    {  0,		0,		0,		2000,		0,		6,		VarName[22]},			        //变量 var22,
    {  0,		0,		0,		8000,		0,		7,		VarName[23]},			        //变量 var23,
};

//keyecnet_t是函数指针
keyevent_t KeyEvent[PAGE_MAX] =       	   //控制3页菜单
{
    MenuMain_KeyEvent,					   //首页
    MenuOne_KeyEvent,					   //第一页
    MenuTwo_KeyEvent,					   //第二页
//	MenuThree_KeyEvent,					   //第三页
};

PAGEEVENT       PageEvent;		           //页事件，主要包括翻页和当前的页数
int16_t         Loop_tab;				   //标号 0-23

//装载变量的值，最大有24个值可以使用
void LoadVarValue(void)
{
	VAR_VALUE(0) = (int32_t)(MinKp*10);					//动态P
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

//将变化后的值进行储存
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

//主菜单的按键事件处理
//主菜单主要列出其他菜单的功能
static void MenuMain_KeyEvent(void)
{
	KEY_MSG keymsg;
	
	while(Get_KeyMsg(& keymsg))   				//获取按键的信息
	{
		if(keymsg.status == KEY_DOWN || keymsg.status == KEY_HOLD)	//当按键按下时
		{
			switch(keymsg.key)					//根据按下哪个按键采取相应的动作
			{
				case KEY_PREV:					//上一页,上一页即为最后一个菜单，为图像显示页
					 MenuInfo.page = (Menufunc)2;
					break;
				case KEY_NEXT:					//下一页，下一页为状态页
					 MenuInfo.page = (Menufunc)1; 
					break;
				default:
					break;
			}
		}
	}
}

//菜单1的按键事件处理
//菜单1主要显示小车的各种状态
static void MenuOne_KeyEvent(void)
{
	KEY_MSG keymsg;
	
	while(Get_KeyMsg(& keymsg))   				//获取按键的信息
	{
		if(keymsg.status == KEY_DOWN || keymsg.status == KEY_HOLD)	//当按键按下时
		{
			switch(keymsg.key)					//根据按下哪个按键采取相应的动作
			{
				case KEY_PREV:					//上一页,上一页为主页
					 MenuInfo.page = (Menufunc)0;
					break;
				case KEY_NEXT:					//下一页，下一页为变量显示页
					 MenuInfo.page = (Menufunc)2; 
					break;
				default:
					break;
			}
		}
	}
}

//菜单2的按键事件处理
//菜单2主要进行变量的修改
static void MenuTwo_KeyEvent(void)
{
	KEY_MSG keymsg;
	
	while(Get_KeyMsg(& keymsg))   				//获取按键的信息
	{
		if(keymsg.status == KEY_DOWN)			//当按键是短按时
		{
			switch(keymsg.key)					//根据按下哪个按键采取相应的动作
			{
				case KEY_ADD:
					VarValue(VAR_ADD);			//当按键是加控制时，变量加1
					break;
				case KEY_SUB:
					VarValue(VAR_SUB);			//当按键是减控制时，变量减1
					break;
				case KEY_PREV:
					VarSelect(VAR_PREV);		//当按键是上一个控制时，选择上一个变量
					break;
				case KEY_NEXT:
					VarSelect(VAR_NEXT);		//当按键是下一个控制时，选择下一个变量
					break;
				case KEY_OK:
					VarOk();					//当按键是确认控制时，保存此时的变量值
//					Motor_Run_Enable = true;
					StartRun = true;
					PrintfZero_Enable = true;
					break;
				case KEY_CANCLE:				//当按键是取消控制时，取消当前的变量值
					VarCancel();
					break;
				default:
					break;
			}
		}
		else										//当按键是长按时
		{
			switch(keymsg.key)						//根据按下哪个按键采取相应的动作
			{
				case KEY_ADD:
					VarValue(VAR_ADD_HOLD);			//当按键是加控制时
					break;
				case KEY_SUB:
					VarValue(VAR_SUB_HOLD);			//当按键是减控制时
					break;
				case KEY_PREV:
					VarSelect(VAR_PREV_HOLD);		//当按键是上一个控制时
					break;
				case KEY_NEXT:
					VarSelect(VAR_NEXT_HOLD);		//当按键是下一个控制时
					break;
				case KEY_OK:
					MenuInfo.page = (Menufunc)1;	//当按键是确认控制时，转回状态页
					break;
				case KEY_CANCLE:					
					MenuInfo.page = (Menufunc)0;		//当按键是取消控制时，到下一页，即首页
					break;
				default:
					break;
			}
		}
	}
}

//切换变量
static void VarSelect(VAR_EVENT  ctrl)
{
	uint8_t Old_tab = Loop_tab;						//备份当前的标号
	
	//根据命令选择对应的控制
	switch(ctrl)
	{	
		case VAR_NEXT:			 					//下一个	
			Loop_tab++;    							//标号自加
			if(Loop_tab >= VAR_NUM)					//判断标号是否越界
			{
				Loop_tab = 0;
			}
			if((Loop_tab%8) == 0)         			//当切换变量加到一页的最大的行数时(7),换页
			{
				PageEvent.Page++; 					//记录页数
				PageEvent.Change = true;			//页改变
				if(PageEvent.Page >= PAGE_NUM) 		//判断页数是否越界,最多有3页
				{
					PageEvent.Page = 0;
				}
			}
			break;	
		case VAR_PREV:								//上一个
			Loop_tab--;								//标号自减
			if(Loop_tab < 0)						//判断标号是否越界
			{
				Loop_tab = VAR_NUM - 1;		       
			}
			if((Loop_tab%8) == 7 )    				//当切换变量减到一页的最小的行数时(0),换页
			{
				PageEvent.Page--; 					//记录页数
				PageEvent.Change = true;			//页改变
				if(PageEvent.Page < 0) 			    //判断页数是否越界
				{
					PageEvent.Page = PAGE_NUM - 1;
				}
			}
			break;	
		case VAR_NEXT_HOLD:							//快下,即翻到下一页
			PageEvent.Page++; 
			PageEvent.Change = true;
			if(PageEvent.Page >= PAGE_NUM) 			//判断页数是否越界
			{
				PageEvent.Page= 0;
			}
			Loop_tab = PageEvent.Page * 8;
			break;
			
		case VAR_PREV_HOLD:					   		//快上，即翻到上一页
			PageEvent.Page--; 
			PageEvent.Change = true;
			if(PageEvent.Page < 0) 					//判断页数是否越界
			{
				PageEvent.Page = PAGE_NUM - 1;
			}
			Loop_tab = PageEvent.Page * 8;
			break;			
		default:									//无效选择，无需切换
			return;
	}
    if(PageEvent.Change == false)
    {
	    VarDisplay(Old_tab);		    			//如果页没有改变就处理上一页的变量
    }
    VarDisplay(Loop_tab);							//处理当前的变量
    VarPageDisplay();								//页显示
}

//改变变量的值
static void VarValue(VAR_EVENT ctrl)
{
	//根据命令选择对应的控制
	switch(ctrl)
	{
		case VAR_ADD:											//加1
			if(VAR_VALUE(Loop_tab ) < VAR_MAXVALUE(Loop_tab))	//判断变量值是否越界
			{
				VAR_VALUE(Loop_tab)++;
			}
			else												//当变量达到最大值时，将变量切换为最小值
			{
				VAR_VALUE(Loop_tab) = VAR_MINVALUE(Loop_tab);
			}
			break;	
		case VAR_SUB:											//减1
			if(VAR_VALUE(Loop_tab) > VAR_MINVALUE(Loop_tab))	//判断变量值是否越界
			{
				VAR_VALUE(Loop_tab)--;
			}
			else
			{
				VAR_VALUE(Loop_tab) = VAR_MAXVALUE(Loop_tab) ;	//当变量达到最小值时，将变量切换为最大值
			}
			break;
		case VAR_ADD_HOLD:										//快加
			if((VAR_MAXVALUE(Loop_tab)-VAR_VALUE(Loop_tab)) >  VAR_VALUE_HOLD_OFFSET )
			{
				VAR_VALUE(Loop_tab)+=VAR_VALUE_HOLD_OFFSET;
			}
			else												//当变量达到最大值时，将变量切换为最小值
			{
				VAR_VALUE(Loop_tab) = VAR_MINVALUE(Loop_tab);
			}
			break;
			
		case VAR_SUB_HOLD:										//快减
			if( ( VAR_VALUE(Loop_tab) - VAR_MINVALUE(Loop_tab)) > VAR_VALUE_HOLD_OFFSET  )
			{
				VAR_VALUE(Loop_tab)-=VAR_VALUE_HOLD_OFFSET;
			}
			else
			{
				VAR_VALUE(Loop_tab) = VAR_MAXVALUE(Loop_tab) ;  //当变量达到最小值时，将变量切换为最大值
			}
			break;		
		default:												//无效选择，变量无需变化
			break;
	}
	VarDisplay(Loop_tab);										//处理当前的变量
}

//确认当前的值
static void VarOk(void)
{
	uint8_t i=0;
    for(i=0;i<VAR_NUM;i++)
    {
        VAR_OLDVALUE(i) = VAR_VALUE(i);							//更新旧的值
    }
    SaveVarValue();
}

//取消当前选择的值
static void VarCancel(void)
{
	VAR_VALUE(Loop_tab) = VAR_OLDVALUE(Loop_tab);				//还原当前值
	VarDisplay(Loop_tab);										//处理当前的变量
}

//显示指定的值,tab = VAR_NUM 时显示全部的值，小于则显示对应的值
//实参是需要更新显示的标号，但是在程序里面会判断是否已经进行翻页了
//和是否已经达到最大标号了，这样处理是为了在防止翻页或者标号达到最大
//标号之后，参数更新错误
void VarDisplay(uint8_t tab)
{
	uint8_t  i = 0;
	uint8_t Tab;
	if(PageEvent.Change == true)								//如果有翻页，则跳到该页的第一行
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
			//显示当前的值:判断值是否改变
			if(VAR_VALUE(Tab) == VAR_OLDVALUE(Tab))				//值没改变，不需要处理
			{		
                OLED_Show_Sentence(20,VAR_SITEY(Tab),"N");
			}
			else
			{
				OLED_Show_Sentence(20,VAR_SITEY(Tab),"Y");
			}
            OLED_Cursor(21,VAR_SITEY(Tab));   				//光标显示
		}
		else											
		{
			//显示当前的值:判断值是否改变
			if(VAR_VALUE(Tab) == VAR_OLDVALUE(Tab))				//值没改变，不需要处理
			{
				OLED_Show_Sentence(20,VAR_SITEY(Tab),"N");	
			}
			else
			{
				OLED_Show_Sentence(20,VAR_SITEY(Tab),"Y");
			}
            OLED_ClearCursor(21,VAR_SITEY(Tab));   				//清除光标
		}
		OLED_PARTCLS(VAROFFSET,VAR_SITEY(Tab),6);				//清除上次可能残留的数字
        OLED_Show_Number(VAROFFSET,VAR_SITEY(Tab),VAR_VALUE(Tab));
        Tab++;
	}while(i--);												//tab != VAR_NUM的时候，执行一次就跳出
}

void VarPageDisplay(void)
{
	uint8_t Tab = 0,i =0;
    if(PageEvent.Change == true)								//如果已经换页
    {
        OLED_CLS();
        Tab = PageEvent.Page * 8;								//标号切换到该页第一行      
        for(i=Tab;i<Tab+8;i++)  								//8行变量名进行显示
        {
            OLED_Show_Sentence(VAR_SITEX(i),VAR_SITEY(i),(uint8_t *)VAR_NAME(i));       
        }
        VarDisplay(0);
        PageEvent.Change = false;
    }
}
