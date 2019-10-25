
/**
  ********************************  SH39F004  *********************************
  * @文件名     ： main.c
  * @作者       ： tomi
  * @库版本     ：
  * @文件版本   ： V1.0.0
  * @日期       ： 2019年7月31日
  * @摘要       ： 主函数 - BMS
  ******************************************************************************/
/*----------------------------------------------------------------------------
  更新日志:modify
		
  ----------------------------------------------------------------------------*/
/* ----------------------------------包含的头文件 ----------------------------*/


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
	  //ucresetflag是0x12的话并且已经发完就会强制初始化
		if((ucResetFlag==0x12) && bUartSndAckFlg)		//Software reset occurs, and ended UART communication
		{
#if debug
		printf("\n -------reset-------\n");    
#endif	
			EA = 0;
			ResetInit();					//Reset MCU
		}
    //ips程序不用管
//		if(bISPFlg && bUartSndAckFlg)
//		{
//			bISPFlg = 0;
//			ISPProcess();
//		}
		//唤醒标志位被触发了，按键int4标志位46或alarm 标志位45被触发
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
		
		//ALARMPROCESS来判断是否有事件发生
		if(bAlarmFlg)//有报警 针对AFE才发生的
		{
			//在这里面要添加我们自己的报警
		 	bAlarmFlg = 0;
			AlarmProcess();
		}
	
		if(bCADCFlg)//判断充电状态还是放电状态
		{
#if debug
		printf("\n bCADCFlg is  1\n");    
#endif	
			bCADCFlg = 0;
			CurrProcess();					//Calculate the current value and determines the state of charge and discharge
		}

//如果电压转换完成的话
		if(bVADCFlg)
		{			
      bVADCFlg = 0;
#if debug
		printf("\n bVADCFlg is  1\n");    
#endif	
			BatteryInfoManage();		  	//Get battery voltage and temperature information
			
			//by tomi
			//新增断线保护
			
			CheckCtoVol();

		//平衡有两种功能，一个就是平衡电压，还有一个是断线检测
		//	BalManage();					//Process external voltage balance

		//	CellOpenProcess(); //断线检测开始标志位

		//	BalProcess();//开始5步的，断线检测时序
		}

//串口发送校准值来进行校准的
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
//eeprom暂时不关注
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
			//按键检测,什么时候检测?
			//1.充电时候不检测2.放电mos关闭的时候
#if 1
			if(bCHGING == 0)
			{
					Scan_key();
			}	
#endif	
#if 0		
			//LED状态显示
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
      //AFE寄存器防护
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
#if 0  //测试唤醒的按键中断 
 //  	Delay1ms(1000);
//		IO_Error_8 = 0;
#endif
	}

}






