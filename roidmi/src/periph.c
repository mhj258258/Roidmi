#include "System.h"
#include "AfeMtp.h"

//#define IO_Error_1  			 P3_1 //led����io�ں����ʾ
//#define IO_Error_2  			 P3_5 //led����io�ں����ʾ
//#define IO_Error_4  			 P2_6 //led����io�ں����ʾ
//#define IO_Error_8  			 P2_7 //led����io�ں����ʾ


LED_state_cfg LED1_cfg,LED2_cfg,LED3_cfg,LED4_cfg;
static void LED_ON_OFF(LED_state_cfg LED_cfg_R,LED_select LED_select_r);
static void LED_ON(U8 LED_select_r);
static void LED_OFF(U8 LED_select_r);
U8 SYS_State = LED_STATE_POWERDOWN;
/*******************************************************************************************
* ������: SetLEDState
* ��  ��: ��
* ����ֵ: ��
* ��  ��: ����LED�����״̬
*********************************************************************************************/
void SetLEDState( LED_DISPLAY_STATE state)					 
{		
	switch(state)
	{
		case LED_STATE_POWERDOWN:
			/***��һ���Ƶ�״̬***/
			//4������ʲô����״̬ 
			LED1_cfg.ontime  = 0;
			LED1_cfg.offtime = 0;
			LED1_cfg.repeatCount = 0;
			LED1_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED1_cfg,Error1);
			/***�ڶ����Ƶ�״̬***/
			LED2_cfg.ontime  = 0;
			LED2_cfg.offtime = 0;
			LED2_cfg.repeatCount = 0;
			LED2_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED2_cfg,Error2);
			/***�������Ƶ�״̬***/
			LED3_cfg.ontime  = 0;
			LED3_cfg.offtime = 0;
			LED3_cfg.repeatCount = 0;
			LED3_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED3_cfg,Error4);
			/***���ĸ��Ƶ�״̬***/
			LED4_cfg.ontime  = 0;
			LED4_cfg.offtime = 0;
			LED4_cfg.repeatCount = 0;
			LED4_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED4_cfg,Error8);
			break;
			
		case LED_STATE_IDLE:
			//4������ʲô����״̬ 
			LED1_cfg.ontime  = 0;
			LED1_cfg.offtime = 0;
			LED1_cfg.repeatCount = 0;
			LED1_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED1_cfg,Error1);
			/***�ڶ����Ƶ�״̬***/
			LED2_cfg.ontime  = 10;
			LED2_cfg.offtime = 10;
			LED2_cfg.repeatCount = 0xff;
			LED2_cfg.AlawaysOn = 0x01;
			LED_ON_OFF(LED2_cfg,Error2);
			/***�������Ƶ�״̬***/
			LED3_cfg.ontime  = 0;
			LED3_cfg.offtime = 0;
			LED3_cfg.repeatCount = 0;
			LED3_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED3_cfg,Error4);
			/***���ĸ��Ƶ�״̬***/
			LED4_cfg.ontime  = 0;
			LED4_cfg.offtime = 0;
			LED4_cfg.repeatCount = 0;
			LED4_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED4_cfg,Error8);
			break;
			
			 //���ڳ���״̬
		case LED_STATE_CHARGING:
			LED1_cfg.ontime  = 0;
			LED1_cfg.offtime = 0;
			LED1_cfg.repeatCount = 0;
			LED1_cfg.AlawaysOn = 0xff;
			LED_ON_OFF(LED1_cfg,Error1);
			/***�ڶ����Ƶ�״̬***/
			LED2_cfg.ontime  = 0;
			LED2_cfg.offtime = 0;
			LED2_cfg.repeatCount = 0;
			LED2_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED2_cfg,Error2);
			/***�������Ƶ�״̬***/
			LED3_cfg.ontime  = 0;
			LED3_cfg.offtime = 0;
			LED3_cfg.repeatCount = 0;
			LED3_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED3_cfg,Error4);
			/***���ĸ��Ƶ�״̬***/
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
			/***�ڶ����Ƶ�״̬***/
			LED2_cfg.ontime  = 0;
			LED2_cfg.offtime = 0;
			LED2_cfg.repeatCount = 0;
			LED2_cfg.AlawaysOn = 0xff;
			LED_ON_OFF(LED2_cfg,Error2);
			/***�������Ƶ�״̬***/
			LED3_cfg.ontime  = 0;
			LED3_cfg.offtime = 0;
			LED3_cfg.repeatCount = 0;
			LED3_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED3_cfg,Error4);
			/***���ĸ��Ƶ�״̬***/
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
			/***�ڶ����Ƶ�״̬***/
			LED2_cfg.ontime  = 0;
			LED2_cfg.offtime = 0;
			LED2_cfg.repeatCount = 0;
			LED2_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED2_cfg,Error2);
			/***�������Ƶ�״̬***/
			LED3_cfg.ontime  = 0;
			LED3_cfg.offtime = 0;
			LED3_cfg.repeatCount = 0;
			LED3_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED3_cfg,Error4);
			/***���ĸ��Ƶ�״̬***/
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
			/***�ڶ����Ƶ�״̬***/
			LED2_cfg.ontime  = 0;
			LED2_cfg.offtime = 0;
			LED2_cfg.repeatCount = 0;
			LED2_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED2_cfg,Error2);
			/***�������Ƶ�״̬***/
			LED3_cfg.ontime  = 0;
			LED3_cfg.offtime = 0;
			LED3_cfg.repeatCount = 0;
			LED3_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED3_cfg,Error4);
			/***���ĸ��Ƶ�״̬***/
			LED4_cfg.ontime  = 0;
			LED4_cfg.offtime = 0;
			LED4_cfg.repeatCount = 0;
			LED4_cfg.AlawaysOn = 0;
			LED_ON_OFF(LED4_cfg,Error8);
			break;
	}
	
}
/*******************************************************************************************
* ������: LED_ON_OFF
* ��  ��: 
* ����ֵ: ��
* ��  ��: ���ö�ӦLED������
*********************************************************************************************/
U8 xdata timeCnt = 0;
//LED_ON_OFF(U8 a)
static void LED_ON_OFF(LED_state_cfg LED_cfg_R,LED_select LED_select_r)
{
	U8 xdata ontime = LED_cfg_R.ontime;
	U8 xdata offtime =	LED_cfg_R.offtime;
	U8 xdata repeatCount = LED_cfg_R.repeatCount ;
	U8 xdata AlawaysOn = LED_cfg_R.AlawaysOn;
	if(AlawaysOn == 0xff)//����
	{
		LED_ON(LED_select_r);
	}
	else if(AlawaysOn == 0x0)
	{
		LED_OFF(LED_select_r);
	}
	else
	{
		//����ѭ������
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
* ������: LED_Proc
* ��  ��: 
* ����ֵ: ��
* ��  ��: 
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
	//���ڷŵ�
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
		else if(bCHGING == 1)//���ڳ��
		{
			LedDsgCnt = 0;
			LedRCnt = 0;
			if(++LedChgCnt > DelayLedChg)
			{
				LedChgCnt = 0;
				SetLEDState(LED_STATE_CHARGING);
			}
		}
		//���ڳ��Ҳ���ڷŵ�Ļ�����
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
