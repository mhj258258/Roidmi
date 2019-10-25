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

		REG.AFEINTEN = 0x6C;					//Enable SC��Ӳ����·��, OV��Ӳ�����䱣����, CD����ŵ�״̬�жϣ� �жϱ���ʹ��
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
	
#if 1 //����PD��������1,���ŵ粻���״̬��UV��2.�������� 3.CTO����
#if debug
			printf("-------------AFE IntoPowerDown-----------\n");
#endif	

	IntoPowerDown();		//ֻ�в�������uart�����ܻ���-->��������AFE��pd��
	
#if 0
/-------------------------------------------------------------
	if(!( OIV|| UIV || FOPEN || Odiff ||Short_flag))
	{
		EA = 0;
		IEN0 = 0x10;							//���ڣ���ʱ��
		IEN1 = 0x00;	
		EA = 1;
#if debug
			printf("-------------MCU into idle PD����------------\n");
#endif
		IntoIdle(); 						//MCU����idle
	}
/--------------------------------------------------------------	
#else //����AFE���ر��ˣ�MCU������Ҳû��ʲô����
		//������߾͹ر����е�
		
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
	REG.AFESCONF1 &= ~0x10;					//�ر�WDT
	TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);

	REG.AFEINTEN = 0x70;					//����SC,OV,CD��Alarm����
	TWIWriteAFE(AFE_INT_EN, &REG.AFEINTEN);
#if debug
			printf("-------------�����˵͹���ģʽ2------------\n");
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
//��Ϊ���û��32.768���
#if 0 //by tomi
	CLKCON &= ~0x04;					//Set sysclk 32kHz 1111 1011
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	CLKCON &= ~0x08;					//Close  sysclk 24MHz
#endif

#if 1 //mcu����idle

//����MCU�������ж�
	if(!( OIV|| UIV || FOPEN || Odiff ||Short_flag))
	{
		EA = 0;
		IEN0 = 0x10;   					//0001 0000 �����Ϳ��������жϣ���ʱ���ж�
		IEN1 = 0x08;						//Enable INT4
		IENC = 0x10;						//Enable INT44 �ֶ�
		IENC |= 0x80;						//INT47			 ALARM�ж� 1000 0000
		EA = 1;
#if debug
		printf("-------------MCU into idle1------------\n");
#endif
		IntoIdle(); 						//MCU����idle
	}

#else //�����MCU����PD
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
	//uart�������Ľ���͹���
	if(bPCSleepFlg && bUartSndAckFlg)//���Ǵ��ڷ��͹���Ҫ����PD������
	{
		bPCSleepFlg = 0;
		bPDFlg = 1;	
#if debug
		printf("\n -----bPDFlg is 1(uart)----- \n");    
#endif		
	}
	//����,mcu�ϴ������mcuҪ���ϼ��cto ������mcu��idle
  else if(bCTO)//��������CTO���ⲿMUC��ⲻ˵mcu����idle
  {
			if(++ucPDTimerCnt >= E2ucDeleyLowPower)
			{
				ucPDTimerCnt = E2ucDeleyLowPower;
				bPDFlg = 1;		
#if debug
		printf("\n -----bPDFlg is 1(cto����)----- \n");    
#endif		
			}
  }
	//���Ҫ
	else if(!bLEDOpen)					//LED display off and then wait to enter a low-power
	{
#if 0
		printf("\n -----$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$----- \n");    
#endif	
		//���ڳ��Ҳ���ڷŵ�
		if(!bCHGING && !bDSGING)
		{
			//������ڳ��Ҳ���ڷŵ�,��Ƿѹ��־,AFE����PD��mcu��û��������־λ��ʱ��Ҳ����idle����ֻҪ���ϵ���25ms�ͻ��Զ��˳�AFE�͹���
				if(bUV)
				{
#if debug
		printf("\n -----bPDFlg is 1(bUVǷѹ�Ҳ��ٳ��Ҳ���ڷŵ�)----- \n");    
#endif	
					if(++ucPDTimerCnt >= E2ucDeleyLowPower) //20
					{
						ucPDTimerCnt = E2ucDeleyLowPower;
						bPDFlg = 1;		
						
#if debug
		printf("\n -----bPDFlg is 1(bUVǷѹ�Ҳ��ٳ��Ҳ���ڷŵ�)----- \n");    
#endif		
					}
				}
				//������ڷŵ�Ҳ���ڳ��һ���¼����Լ�Ҳ�������ģʽ
				else
				{
#if 0
		printf("\n -----%%%%%%%%%%%%%%%%%%%%%%%%%----- \n");    
#endif						
						ucPDTimerCnt = 0;
//bAFE_OV, bCTO,bOTD#, bUTD#, bUV#,bOCD,bAFE_SC#,(bFD&&bDSGEnd),bLoadConectFlg,bOTC,bUTC,bHV#,bOCC#,bFC&&bCHGEnd,OIV,UIV,FOPEN,Odiff,Short_flag
								//��Ҫtomi����,ʲô����½���idleģʽ
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
		printf("\n -----bIdleFlg is 1(û��bOTC | bUTC | bHV | bOCC | bOTD | bUTD | bUV | bOCD | bAFE_SC����)----- \n");    
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
		printf("\n ~~~~~~AFE mode2 ��MCU idle ~~~~~~\n");    
#endif
		//��ֹ����IO���Կ��ŵ�״̬
		IO_CHG_MOS = 1;//�ر�mos
		IO_DSG_MOS = 1;
		
		SystemIntoIdle();
	}

}

//roidmi�Ľ���͹��ĵķ�ʽ
//(1)������:����״̬
//(2)������mcu ��û�д����ʱ��idleǰmos��״̬�ǿ����ģ�idle���Ȳ��ŵ�Ҳ���������£�AFE��һ��ʱ����Զ�����ģʽ2			�ɱ���ŵ��������
//(3)������mcu idle,AFE����POWER DOWN,��Ϊsc��ov��Ӳ������������ֱ�ӽ���powerdownҲû��ϵ�����MUCҪ��PD����ô�����а�����
//(4)��������������
//����״̬�������Ҳ���ŵ磬����������·����



//(2)����ģʽ2��AFE��⵽��ŵ�С���������ALarm�жϣ��Ӷ�����MCU
//ǰ���������ŵ�Ҳ�����
void AFE_MODE2Idle(void)
{
	//AFE�Ƚ���ģʽ2
	REG.AFESCONF1 &=~0x10; //0001 0000 //�ر�WDT
	TWIWriteAFE(AFE_SCONF1,&REG.AFESCONF1);

	REG.AFEINTEN = 0x70; //0111 0000  ��Ӳ�����壬��ŵ�״̬�ж�ʹ�ܣ�CADC�ж�ʹ��λ
	TWIWriteAFE(AFE_INT_EN,&REG.AFEINTEN);		
}

////(3)������mcu idle,AFE����ģʽPDģʽ 
//void AFE_MCU_ManuIdle(void)
//{
//	REG.AFESCONF1 &=~0x10; //0001 0000 //�ر�WDT
//	TWIWriteAFE(AFE_SCONF1,&REG.AFESCONF1);

//	REG.AFEINTEN = 0x70; //0111 0000  ��Ӳ�����壬��ŵ�״̬�ж�ʹ�ܣ�CADC�ж�ʹ��λ
//	TWIWriteAFE(AFE_INT_EN,&REG.AFEINTEN);	
//	
//	
//}









