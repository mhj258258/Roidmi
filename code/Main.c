
/**
  ********************************  SH39F004  *********************************
  * @�ļ���     �� main.c
  * @����       �� tomi
  * @��汾     ��
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2019��7��31��
  * @ժҪ       �� ������ - BMS
  ******************************************************************************/
/*----------------------------------------------------------------------------
  ������־:modify
		
  ----------------------------------------------------------------------------*/
/* ----------------------------------������ͷ�ļ� ----------------------------*/


#include "system.h"
#include "AfeMtp.h"
void Uart_Send_Byte(U8 DATA)
{
	ES0 = 0; 
	SBUF = DATA;
	while(TI != 1);
	TI = 0;
	ES0 = 1;
}
void Uart_Send_string(U8 *s)
{
	while(*s)
	{
		Uart_Send_Byte(*s++);
	}
}
char putchar(char c)
{
	Uart_Send_Byte(c);
	return c;
}

U8 xdata AFE_INT_EN_buf = 0;
extern U8 xdata KEY_state;
extern U8 xdata Short_flag;
void main(void)
{
	
	Initial();								//Initialize the system

	while(1)
	{ 
#if DEBUG
		TWIReadAFE(AFE_INT_EN, &AFE_INT_EN_buf);
//		Delay1ms(500);
		printf("\n AFE_INT_EN_buf = %bx \n",AFE_INT_EN_buf);    //0110 1100
#endif	

#if 0
		printf("\n test VOL = %hd \n",ADCRead(PACK_VOL));    
#endif	
#if 0
		printf("\n test CHG VOL = %hd \n",ADCRead(CHG_VOL));    
#endif	
#if debug
		printf("\n ----------------------------------------------------main hanshu ----------------------------------------------------------\n");    
#endif	
#if 0 //debug timer
		
//		Delay1ms(2000);
		if(bTimer1sFlg == 1)
		{
#if debug
		printf("\n 1s dinsghi \n");    
#endif	
			bTimer1sFlg = 0;
		//	IO_Error_4 = ~IO_Error_4;
		}
#endif
		

		RSTSTAT = 0x00;
	  //ucresetflag��0x12�Ļ������Ѿ�����ͻ�ǿ�Ƴ�ʼ��
		if((ucResetFlag==0x12) && bUartSndAckFlg)		//Software reset occurs, and ended UART communication
		{
#if debug
		printf("\n -------reset-------\n");    
#endif	
			EA = 0;
			ResetInit();					//Reset MCU
		}
    //ips�����ù�
//		if(bISPFlg && bUartSndAckFlg)
//		{
//			bISPFlg = 0;
//			ISPProcess();
//		}
		//���ѱ�־λ�������ˣ�����int4��־λ46��alarm ��־λ45������
		if(bWakeupFlg)						//The charger / load, keys, communication can wake up the system
		{
#if debug
			printf("\n -----WakeUpProcess-----\n");    
#endif	
			bWakeupFlg = 0;
			WakeUpProcess();				//System to be awakened
		}

#if 0
		if(bKeyFlg)							//timeinterrupt will dectect keyprocess(),  if detected effective key
		{	
			bKeyFlg = 0;
			KeyProcess();
		}
#endif
		
		//ALARMPROCESS���ж��Ƿ����¼�����
		if(bAlarmFlg)//�б��� ���AFE�ŷ�����
		{
			//��������Ҫ��������Լ��ı���
		 	bAlarmFlg = 0;
			AlarmProcess();
		}
	
		if(bCADCFlg)//�жϳ��״̬���Ƿŵ�״̬
		{
#if debug
		printf("\n bCADCFlg is  1\n");    
#endif	
			bCADCFlg = 0;
			CurrProcess();					//Calculate the current value and determines the state of charge and discharge
		}

//�����ѹת����ɵĻ�
		if(bVADCFlg)
		{			
      bVADCFlg = 0;
#if debug
		printf("\n bVADCFlg is  1\n");    
#endif	
			BatteryInfoManage();		  	//Get battery voltage and temperature information
			
			//by tomi
			//�������߱���
			
			CheckCtoVol();

		//ƽ�������ֹ��ܣ�һ������ƽ���ѹ������һ���Ƕ��߼��
		//	BalManage();					//Process external voltage balance

		//	CellOpenProcess(); //���߼�⿪ʼ��־λ

		//	BalProcess();//��ʼ5���ģ����߼��ʱ��
		}

//���ڷ���У׼ֵ������У׼��
#if 0
		if(bCalibrationFlg)
		{
			bCalibrationFlg = 0;
			CaliProcess();					//PC calibration
		}
#endif
		
		if(bUpdataFlashFlg)
		{
#if discurdebug
		printf("\n -----UpdataFlash-----\n");    
#endif	
			bUpdataFlashFlg = 0;
			FlashProcess();
		}

#if 0
//eeprom��ʱ����ע
		if(bEnEEPRomBK && bE2PProcessFlg)
		{
			bE2PProcessFlg = 0;
			E2PRomBKProcess();
		}
#endif
		

		if(bTimer25msFlg)
		{
#if 0
			printf("-------------25ms flag-------------\n");
#endif
      bTimer25msFlg = 0;						
   		  
 			ProtectProcess();			 	//Detection of battery charge and discharge cut-off voltage
			//�������,ʲôʱ����?
			//1.���ʱ�򲻼��2.�ŵ�mos�رյ�ʱ��
#if 1
			if(bCHGING == 0)
			{
					Scan_key();
			}	
#endif	
#if 0		
			//LED״̬��ʾ
			LED_Proc();
#endif	
    }

		if(bTimer1sFlg)						//timer is 1s
		{
#if 0
			printf("-------------1s flag-------------\n");
#endif
			bTimer1sFlg = 0;

			RamCheckProcess();              //Detect the xdata block data			

			//LEDPowerOff();					//When the LED display 4s, turn off the LED

			VolProcess();					//Detection of battery charge and discharge cut-off voltage

			GaugeManage();					//Calculate the battery charge and discharge capacity
      //AFE�Ĵ�������
			AFERamCheck();	                //Check AFE Register		

	  	WriteFlashCheck();
#if 0		
            if(bEnEEPRomBK)
            {
							E2PRomBKCheck();			//Detecting whether to backup EEPRom
            }
#endif

			LowPowerCheck();				//Detecting whether to enter a low-power

		}

		LowPowerProcess();
#if 0  //���Ի��ѵİ����ж� 
 //  	Delay1ms(1000);
//		IO_Error_8 = 0;
#endif
	}

}






