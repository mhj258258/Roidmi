/********************************************************************************
Copyright (C), Sinowealth Electronic. Ltd.
Author: 	andyliu
Version: 	V0.0
Date: 		2014/05/30
History:
	V0.0		2014/05/30		 Preliminary
********************************************************************************/
#include "system.h"

//U8 xdata Short_flag = 0;
//extern U8 xdata Sflag;


/*******************************************************************************
Function: 
Description:  INT45
Input:	 	
Output: 
Others:
*******************************************************************************/
void InterruptINT4(void) interrupt 10
{
	//AFE��������
	if(IF47)		                   //AFE ALARM
	{
		IF47 = 0;
		if(bPDFlg||bIdleFlg) //�����Ϊ�˵͹���ģʽ2����mcu��
		{
			InitClk();
			bWakeupFlg = 1;
		}
#if 1
		bAlarmFlg = 1;	
#endif
	}
	if(IF44)		                   
	{
		IF44 = 0;
		if(bPDFlg || bIdleFlg)			   //WakeUp
		{
			InitClk();	 	

			bWakeupFlg = 1;	
		}
	}
	if(IF40)		                   //A��ˢ���
	{
		IF40 = 0;
	}

	/***********20190925��ˢ��·�ж�***********/

}



/*******************************************************************************
Function: InterruptTimer0()
Description:  20ms
Input:	 	
Output: 
Others:
*******************************************************************************/
#if 1
void InterruptTimer3(void)	interrupt	5
{
	BANK1_SEL;
	TR3 = 0;
//	TL3 = 0xC2;		 
//	TH3 = 0xB6;			//25mS
	TL3 = 0xD8;		 //������ڲ�ʱ��Ϊ93750 ��ʱ��
	TH3 = 0xF6;			//25mS
 	TF3 = 0;
//	TR3 = 1;
	BANK0_SEL;

  bTimer25msFlg = 1;

//    if(bLEDChgFlg)						//charging flicker led 25*20=500mS
//    {
//        if(++ucLedFlickCnt >= 20)
//        {
//            ucLedFlickCnt = 0;
//						LED_CHARGE ^= 1;
//        }
//    }
//	else
//	{
//		LED_CHARGE = 0;
//	}

//    if(bBLEOPEN)						// BLE flicker 25*10=250mS
//    {
//        if(++ucBLEFlickCnt >= 10)
//        {
//            ucBLEFlickCnt = 0;
//			LED1 ^= 1;
//        }
//    }
//	else
//	{
//		LED1 = 0;
//	}	
//  IO_Error_1 = 1;
	if(++ucTimer3Cnt >= 40)					  //1S
	{
		ucTimer3Cnt = 0;
		bTimer1sFlg = 1;
	}

	if(++ucUartTimeoutCnt >= 8)			//If not Uart communication within 200ms, then clear 
	{
		REN = 1;	//��������λ
		ucUartBufPT = 0;
	}
	
	BANK1_SEL;
	TR3 = 1;
	BANK0_SEL;
//	ScanKey();						  //Scankey not keyprocess ,in scankey will judge if have press key and set keyflag
}
#endif

void InterruptINT3(void) interrupt 11	 
{
//20190925
	if(IE3)//�ŵ��·���
	{
		IE3= 0;
	  //��⵽��·����Ҫ���Ķ���
		//���ַŵ�mos���عر�
//		IO_Error_8 = 1;
		IO_DSG_MOS = 1;
		Short_flag = 1;//ʲôʱ����������
		//Sflag = 1;
	}	
   
}
