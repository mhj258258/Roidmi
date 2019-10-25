/********************************************************************************
Copyright (C), Sinowealth Electronic. Ltd.
Author: 	andyliu
Version: 	V0.0
Date: 		2014/05/30
History:
	V0.0		2014/05/30		 Preliminary
********************************************************************************/
#include "system.h"
#include "AfeMtp.h"
#include "Calibrate.h"
void AFE_MODE2Idle(void);

extern void InitClk(void);

/*******************************************************************************
Function: WakeUpProcess(void)
Description:  
Input:	 	
Output: 
Others:
*******************************************************************************/
void WakeUpProcess(void)
{
	bWakeupFlg = 0;
	if(bIdleFlg || bPDFlg)
	{
		InitClk();

		bPDFlg = 0;
		bIdleFlg = 0;
		ucIdleTimerCnt = 0;
		ucPDTimerCnt = 0;		 
		ucTimer3Cnt = 0;

		InitIRQ();

		REG.AFEINTEN = 0x6C;					//Enable SC（硬件短路）, OV（硬件过充保护）, CD（充放电状态中断） 中断报警使能
		TWIWriteAFE(AFE_INT_EN, &REG.AFEINTEN);
	
		REG.AFESCONF1 |= 0x10;					//Enable WDT
		TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);
#if debug
		printf("\n -----WakeUp from idle or pd-----\n");    
#endif	
	}

		ucIdleTimerCnt = 0;
		ucPDTimerCnt = 0;
		bPCSleepFlg = 0;
		bIdleFlg = 0; 
		ucUartTimeoutCnt = 0;		
}


/*******************************************************************************
Function: SystemIntoPD(void)
Description:  
Input:	 	
Output: 
Others:
*******************************************************************************/
void SystemIntoPD(void) 
{	
	
#if 1 //进入PD的条件：1,不放电不充电状态下UV，2.串口命令 3.CTO断线
#if debug
			printf("-------------AFE IntoPowerDown-----------\n");
#endif	

	IntoPowerDown();		//只有插充电器和uart命令能唤醒-->这个是针对AFE的pd的
	
#if 0
/-------------------------------------------------------------
	if(!( OIV|| UIV || FOPEN || Odiff ||Short_flag))
	{
		EA = 0;
		IEN0 = 0x10;							//串口，定时器
		IEN1 = 0x00;	
		EA = 1;
#if debug
			printf("-------------MCU into idle PD函数------------\n");
#endif
		IntoIdle(); 						//MCU进入idle
	}
/--------------------------------------------------------------	
#else //由于AFE都关闭了，MCU开在那也没有什么意义
		//所以这边就关闭所有电
		
		IO_POWER_ON = 0;
#endif
	
	
	
#else
	//PWM2CON &= ~0x01;
	//DSG1PWM = 0;
	
	//bBLEOPEN = 0;
	//IO_BLEPW = 1;
	
  IEN0 = 0x00;							//disable Interrupt
	IEN1 = 0x00;						
	//LEDOff;								    //shut down LED

	IntoPowerDown();	
#endif
}


/*******************************************************************************
Function: SystemIntoIdle(void)
Description:  
Input:	 	
Output: 
Others:
*******************************************************************************/
void SystemIntoIdle(void)    
{	
	REG.AFESCONF1 &= ~0x10;					//关闭WDT
	TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);

	REG.AFEINTEN = 0x70;					//开启SC,OV,CD的Alarm功能
	TWIWriteAFE(AFE_INT_EN, &REG.AFEINTEN);
#if debug
			printf("-------------进入了低功耗模式2------------\n");
#endif
//	LEDOff;								//shut down LED
//	bBLEOPEN = 0;
//	IO_BLEPW = 1;
//	PWM2CON &= ~0x01;
//	if(bDSGMOS)
//	{
//		DSG1PWM = 1;
//	}
//	else
//	{
//		DSG1PWM = 0;
//	}
//因为这边没有32.768这边
#if 0 //by tomi
	CLKCON &= ~0x04;					//Set sysclk 32kHz 1111 1011
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	CLKCON &= ~0x08;					//Close  sysclk 24MHz
#endif

#if 1 //mcu进入idle

//增加MCU保护的判断
	if(!( OIV|| UIV || FOPEN || Odiff ||Short_flag))
	{
		EA = 0;
		IEN0 = 0x10;   					//0001 0000 开启就开启串口中断，定时器中断
		IEN1 = 0x08;						//Enable INT4
		IENC = 0x10;						//Enable INT44 手动
		IENC |= 0x80;						//INT47			 ALARM中断 1000 0000
		EA = 1;
#if debug
		printf("-------------MCU into idle1------------\n");
#endif
		IntoIdle(); 						//MCU进入idle
	}

#else //这边是MCU进入PD
	EA = 0;
	IEN0 = 0x10;						//enable es0
	IEN1 = 0x08;						//Enable INT4
	IENC = 0x10;						//Enable INT44
//	IENC |= 0x40;						//INT46			  WAKEUP
	EA = 1;

	IntoPD();							//MCU into PD
#endif
}


/*******************************************************************************
Function: LowPowerCheck(void)
Description:  
Input:	 	
Output: 
Others:
*******************************************************************************/
void LowPowerCheck(void)
{
#if debug
		printf("\n -----LowPowerCheck----- bLEDOpen\n");    
#endif
	//uart发送来的进入低功耗
	if(bPCSleepFlg && bUartSndAckFlg)//这是串口发送过来要进入PD的命令
	{
		bPCSleepFlg = 0;
		bPDFlg = 1;	
#if debug
		printf("\n -----bPDFlg is 1(uart)----- \n");    
#endif		
	}
	//断线,mcu上传错误后，mcu要不断检测cto ，所以mcu不idle
  else if(bCTO)//由于现在CTO是外部MUC检测不说mcu不能idle
  {
			if(++ucPDTimerCnt >= E2ucDeleyLowPower)
			{
				ucPDTimerCnt = E2ucDeleyLowPower;
				bPDFlg = 1;		
#if debug
		printf("\n -----bPDFlg is 1(cto断线)----- \n");    
#endif		
			}
  }
	//这边要
	else if(!bLEDOpen)					//LED display off and then wait to enter a low-power
	{
#if 0
		printf("\n -----$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$----- \n");    
#endif	
		//不在充电也不在放电
		if(!bCHGING && !bDSGING)
		{
			//如果不在充电也不在放电,有欠压标志,AFE进入PD，mcu在没有其他标志位的时候，也进入idle，且只要插上电满25ms就会自动退出AFE低功耗
				if(bUV)
				{
#if debug
		printf("\n -----bPDFlg is 1(bUV欠压且不再充电也不在放电)----- \n");    
#endif	
					if(++ucPDTimerCnt >= E2ucDeleyLowPower) //20
					{
						ucPDTimerCnt = E2ucDeleyLowPower;
						bPDFlg = 1;		
						
#if debug
		printf("\n -----bPDFlg is 1(bUV欠压且不再充电也不在放电)----- \n");    
#endif		
					}
				}
				//如果不在放电也不在充电一段事件后自己也进入待机模式
				else
				{
#if 0
		printf("\n -----%%%%%%%%%%%%%%%%%%%%%%%%%----- \n");    
#endif						
						ucPDTimerCnt = 0;
//bAFE_OV, bCTO,bOTD#, bUTD#, bUV#,bOCD,bAFE_SC#,(bFD&&bDSGEnd),bLoadConectFlg,bOTC,bUTC,bHV#,bOCC#,bFC&&bCHGEnd,OIV,UIV,FOPEN,Odiff,Short_flag
								//需要tomi更改,什么情况下进入idle模式
						//	 if(!(bOTC | bUTC | bHV | bOCC | bOTD | bUTD | bUV | bOCD | bAFE_SC))
								//if(!(bOTC | bUTC | bHV | bOCC | bOTD | bUTD | bUV | bOCD | bOCD2 | bAFE_SC))
								if(!(bOTC | bUTC | bHV | bOCC | bOTD | bUTD | bUV | bOCD | bOCD2 | bAFE_SC | bAFE_OV ))
								{
#if debug
		printf("\n -----**************************ucIdleTimerCnt = %bd----- \n",ucIdleTimerCnt);    
#endif		
									if(++ucIdleTimerCnt >= E2ucDeleyLowPower)//E2ucDeleyLowPower =14
									{
		
										ucIdleTimerCnt = E2ucDeleyLowPower;
										bIdleFlg = 1;		
#if debug
		printf("\n -----bIdleFlg is 1(没有bOTC | bUTC | bHV | bOCC | bOTD | bUTD | bUV | bOCD | bAFE_SC错误)----- \n");    
#endif
									}
								}
								else
								{
										ucIdleTimerCnt = 0;
								}
				}
		}
		else
		{
			ucIdleTimerCnt = 0;
			ucPDTimerCnt = 0;
		}
	}
	else
	{
		ucIdleTimerCnt = 0;
		ucPDTimerCnt = 0;
	}

}


/*******************************************************************************
Function: LowPowerProcess(void)
Description:  
Input:	 	
Output: 
Others:
*******************************************************************************/
void LowPowerProcess(void)
{
	if(!bIdleFlg && !bPDFlg)
	{
		EA = 0;
		if(!(bKeyFlg | bAlarmFlg | bCADCFlg | bCalibrationFlg | bUpdataFlashFlg | bWakeupFlg | bISPFlg | bVADCFlg | bTimer1sFlg) && (ucResetFlag!=0x12))
		{
#if debug
		printf("\n ~~~~~~muc Idle~~~~~~\n");    
#endif
			EA = 1;
			IntoIdle();
		}
		EA = 1;	 	
	}
	else if(bPDFlg)
	{
#if debug
		printf("\n ~~~~~~AFE IntoPD~~~~~~\n");    
#endif
		SystemIntoPD();
	}
	else if(bIdleFlg)
	{
#if debug
		printf("\n ~~~~~~AFE mode2 ，MCU idle ~~~~~~\n");    
#endif
		//防止出现IO口仍开着的状态
		IO_CHG_MOS = 1;//关闭mos
		IO_DSG_MOS = 1;
		
		SystemIntoIdle();
	}

}

//roidmi的进入低功耗的方式
//(1)不开机:运输状态
//(2)开机：mcu （没有错误的时候，idle前mos管状态是开启的）idle，既不放电也不充电情况下，AFE过一段时间后自动进入模式2			可被充放电电流唤醒
//(3)开机：mcu idle,AFE进入POWER DOWN,因为sc和ov是硬件保护，所以直接进入powerdown也没关系（如果MUC要进PD，那么必须有按键）
//(4)开机：正常运行
//待机状态，不充电也不放电，仅仅开启短路保护



//(2)工作模式2，AFE检测到充放电小电流会产生ALarm中断，从而唤醒MCU
//前提条件不放电也不充电
void AFE_MODE2Idle(void)
{
	//AFE先进入模式2
	REG.AFESCONF1 &=~0x10; //0001 0000 //关闭WDT
	TWIWriteAFE(AFE_SCONF1,&REG.AFESCONF1);

	REG.AFEINTEN = 0x70; //0111 0000  ；硬件过冲，充放电状态中断使能，CADC中断使能位
	TWIWriteAFE(AFE_INT_EN,&REG.AFEINTEN);		
}

////(3)开机：mcu idle,AFE进入模式PD模式 
//void AFE_MCU_ManuIdle(void)
//{
//	REG.AFESCONF1 &=~0x10; //0001 0000 //关闭WDT
//	TWIWriteAFE(AFE_SCONF1,&REG.AFESCONF1);

//	REG.AFEINTEN = 0x70; //0111 0000  ；硬件过冲，充放电状态中断使能，CADC中断使能位
//	TWIWriteAFE(AFE_INT_EN,&REG.AFEINTEN);	
//	
//	
//}









