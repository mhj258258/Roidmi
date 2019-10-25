#include "System.h"
#include "AfeMtp.h"

//#define IO_Error_1  			 P3_1 //led错误io口红灯显示
//#define IO_Error_2  			 P3_5 //led错误io口红灯显示
//#define IO_Error_4  			 P2_6 //led错误io口红灯显示
//#define IO_Error_8  			 P2_7 //led错误io口红灯显示


LED_state_cfg LED1_cfg,LED2_cfg,LED3_cfg,LED4_cfg;
static void LED_ON_OFF(LED_state_cfg LED_cfg_R,LED_select LED_select_r);
static void LED_ON(U8 LED_select_r);
static void LED_OFF(U8 LED_select_r);
U8 SYS_State = LED_STATE_POWERDOWN;
/*******************************************************************************************
* 函数名: SetLEDState
* 参  数: 无
* 返回值: 无
* 作  用: 设置LED亮灭的状态
*********************************************************************************************/
void SetLEDState( LED_DISPLAY_STATE state)					 
{		
	switch(state)
	{
		case LED_STATE_POWERDOWN:
			/***第一个灯的状态***/
			//4个灯是什么样的状态 
			LED1_cfg.ontime  = 0;
			LED1_cfg.offtime = 0;
			LED1_cfg.repeatCount = 0;
			LED1_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED1_cfg,Error1);
			/***第二个灯的状态***/
			LED2_cfg.ontime  = 0;
			LED2_cfg.offtime = 0;
			LED2_cfg.repeatCount = 0;
			LED2_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED2_cfg,Error2);
			/***第三个灯的状态***/
			LED3_cfg.ontime  = 0;
			LED3_cfg.offtime = 0;
			LED3_cfg.repeatCount = 0;
			LED3_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED3_cfg,Error4);
			/***第四个灯的状态***/
			LED4_cfg.ontime  = 0;
			LED4_cfg.offtime = 0;
			LED4_cfg.repeatCount = 0;
			LED4_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED4_cfg,Error8);
			break;
			
		case LED_STATE_IDLE:
			//4个灯是什么样的状态 
			LED1_cfg.ontime  = 0;
			LED1_cfg.offtime = 0;
			LED1_cfg.repeatCount = 0;
			LED1_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED1_cfg,Error1);
			/***第二个灯的状态***/
			LED2_cfg.ontime  = 10;
			LED2_cfg.offtime = 10;
			LED2_cfg.repeatCount = 0xff;
			LED2_cfg.AlawaysOn = 0x01;
			LED_ON_OFF(LED2_cfg,Error2);
			/***第三个灯的状态***/
			LED3_cfg.ontime  = 0;
			LED3_cfg.offtime = 0;
			LED3_cfg.repeatCount = 0;
			LED3_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED3_cfg,Error4);
			/***第四个灯的状态***/
			LED4_cfg.ontime  = 0;
			LED4_cfg.offtime = 0;
			LED4_cfg.repeatCount = 0;
			LED4_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED4_cfg,Error8);
			break;
			
			 //正在充电的状态
		case LED_STATE_CHARGING:
			LED1_cfg.ontime  = 0;
			LED1_cfg.offtime = 0;
			LED1_cfg.repeatCount = 0;
			LED1_cfg.AlawaysOn = 0xff;
			LED_ON_OFF(LED1_cfg,Error1);
			/***第二个灯的状态***/
			LED2_cfg.ontime  = 0;
			LED2_cfg.offtime = 0;
			LED2_cfg.repeatCount = 0;
			LED2_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED2_cfg,Error2);
			/***第三个灯的状态***/
			LED3_cfg.ontime  = 0;
			LED3_cfg.offtime = 0;
			LED3_cfg.repeatCount = 0;
			LED3_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED3_cfg,Error4);
			/***第四个灯的状态***/
			LED4_cfg.ontime  = 0;
			LED4_cfg.offtime = 0;
			LED4_cfg.repeatCount = 0;
			LED4_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED4_cfg,Error8);
			break;
		case LED_STATE_DISCHARGE:
			LED1_cfg.ontime  = 0;
			LED1_cfg.offtime = 0;
			LED1_cfg.repeatCount = 0;
			LED1_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED1_cfg,Error1);
			/***第二个灯的状态***/
			LED2_cfg.ontime  = 0;
			LED2_cfg.offtime = 0;
			LED2_cfg.repeatCount = 0;
			LED2_cfg.AlawaysOn = 0xff;
			LED_ON_OFF(LED2_cfg,Error2);
			/***第三个灯的状态***/
			LED3_cfg.ontime  = 0;
			LED3_cfg.offtime = 0;
			LED3_cfg.repeatCount = 0;
			LED3_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED3_cfg,Error4);
			/***第四个灯的状态***/
			LED4_cfg.ontime  = 0;
			LED4_cfg.offtime = 0;
			LED4_cfg.repeatCount = 0;
			LED4_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED4_cfg,Error8);
			break;
		case LED_STATE_ERROR:
			LED1_cfg.ontime  = 0;
			LED1_cfg.offtime = 0;
			LED1_cfg.repeatCount = 0;
			LED1_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED1_cfg,Error1);
			/***第二个灯的状态***/
			LED2_cfg.ontime  = 0;
			LED2_cfg.offtime = 0;
			LED2_cfg.repeatCount = 0;
			LED2_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED2_cfg,Error2);
			/***第三个灯的状态***/
			LED3_cfg.ontime  = 0;
			LED3_cfg.offtime = 0;
			LED3_cfg.repeatCount = 0;
			LED3_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED3_cfg,Error4);
			/***第四个灯的状态***/
			LED4_cfg.ontime  = 0;
			LED4_cfg.offtime = 0;
			LED4_cfg.repeatCount = 0;
			LED4_cfg.AlawaysOn = 0xff;
			LED_ON_OFF(LED4_cfg,Error8);
			break;	
			
		default:
			LED1_cfg.ontime  = 0;
			LED1_cfg.offtime = 0;
			LED1_cfg.repeatCount = 0;
			LED1_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED1_cfg,Error1);
			/***第二个灯的状态***/
			LED2_cfg.ontime  = 0;
			LED2_cfg.offtime = 0;
			LED2_cfg.repeatCount = 0;
			LED2_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED2_cfg,Error2);
			/***第三个灯的状态***/
			LED3_cfg.ontime  = 0;
			LED3_cfg.offtime = 0;
			LED3_cfg.repeatCount = 0;
			LED3_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED3_cfg,Error4);
			/***第四个灯的状态***/
			LED4_cfg.ontime  = 0;
			LED4_cfg.offtime = 0;
			LED4_cfg.repeatCount = 0;
			LED4_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED4_cfg,Error8);
			break;
	}
	
}
/*******************************************************************************************
* 函数名: LED_ON_OFF
* 参  数: 
* 返回值: 无
* 作  用: 设置对应LED的配置
*********************************************************************************************/
U8 xdata timeCnt = 0;
//LED_ON_OFF(U8 a)
static void LED_ON_OFF(LED_state_cfg LED_cfg_R,LED_select LED_select_r)
{
	U8 xdata ontime = LED_cfg_R.ontime;
	U8 xdata offtime =	LED_cfg_R.offtime;
	U8 xdata repeatCount = LED_cfg_R.repeatCount ;
	U8 xdata AlawaysOn = LED_cfg_R.AlawaysOn;
	if(AlawaysOn == 0xff)//常亮
	{
		LED_ON(LED_select_r);
	}
	else if(AlawaysOn == 0x0)
	{
		LED_OFF(LED_select_r);
	}
	else
	{
		//无线循环闪动
		if(repeatCount == 0xff)
		{
			if(++timeCnt < (ontime + offtime))
			{				
				if(timeCnt < ontime)
				{
					LED_ON(LED_select_r);
				}
				else
				{
					LED_OFF(LED_select_r);
				}
			}
			else 
			{
				timeCnt = 0;
				
			}
			

		}
		else
		{
			if(repeatCount >= 0)
			{
				repeatCount--;
					if(++timeCnt < (ontime + offtime))
				{				
					if(timeCnt < ontime)
					{
						LED_ON(LED_select_r);
					}
					else
					{
						LED_OFF(LED_select_r);
					}
				}
				else 
				{
					timeCnt = 0;
					
				}
			}
		}



	}

}

static void LED_ON(LED_select LED_select_r)
{
		switch(LED_select_r)
		{
			//LED1
			case Error1:
				IO_Error_1 = 1;			
				break;
			//LED2
			case Error2:
				IO_Error_2 = 1;
				break;
			//LED3
			case Error4:
				IO_Error_4 = 1;
				break;
			//LED4
			case Error8:
				IO_Error_8 = 1;
				break;
			default:
				break;
		}
}

static void LED_OFF(LED_select LED_select_r)
{
			switch(LED_select_r)
		{
			//LED1
			case Error1:
				IO_Error_1 = 0;			
				break;
			//LED2
			case Error2:
				IO_Error_2 = 0;
				break;
			//LED3
			case Error4:
				IO_Error_4 = 0;
				break;
			//LED4
			case Error8:
				IO_Error_8 = 0;
				break;
			default:
				break;
		}

}

/*******************************************************************************************
* 函数名: LED_Proc
* 参  数: 
* 返回值: 无
* 作  用: 
*********************************************************************************************/
U8 xdata LedDsgCnt = 0;
U8 xdata LedChgCnt = 0;
U8 xdata LedRCnt = 0;
#define DelayLedChg 40
void LED_Proc(void)
{
	if(bAFE_OV|| bCTO||bOTD|| bUTD|| bUV||bOCD||bAFE_SC||(bFD&&bDSGEnd)||bLoadConectFlg||bOTC||bUTC\
		||bHV||bOCC||(bFC&&bCHGEnd)||OIV||UIV||FOPEN||Odiff||MCU_Short_falg)
	{
		SetLEDState(LED_STATE_ERROR);
	}
	else
	{
	//正在放电
		if(bDSGING == 1)
		{
			LedChgCnt = 0;
			LedRCnt = 0;
			if(++LedDsgCnt > DelayLedChg)
			{
				LedDsgCnt = 0;
				SetLEDState(LED_STATE_DISCHARGE);
			}
		}
		else if(bCHGING == 1)//正在充电
		{
			LedDsgCnt = 0;
			LedRCnt = 0;
			if(++LedChgCnt > DelayLedChg)
			{
				LedChgCnt = 0;
				SetLEDState(LED_STATE_CHARGING);
			}
		}
		//不在充电也不在放电的话，就
		else
		{
			LedChgCnt = 0;
			LedDsgCnt = 0;
			if(++LedRCnt > DelayLedChg)
			{
				LedRCnt = 0;
				SetLEDState(LED_STATE_IDLE);
			}
			
		}
	}

}
