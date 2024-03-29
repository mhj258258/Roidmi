/********************************************************************************
Copyright (C), Sinowealth Electronic. Ltd.
Author: 	andyliu
Version: 	V0.0
Date: 		2014/05/30
History:
	V0.0		2014/05/30		 Preliminary
********************************************************************************/
#include "System.h"
#include "AfeMtp.h"
#include "ExtE2PRomRTC.h"
#include "GasGauge.h"
#include "ISP.h"


extern void SystemIntoPD(void);

/*******************************************************************************
Function:InitSealCalibrate() 
Description:  
Input: 	
Output: 
Others:
*******************************************************************************/
void InitSealCalibrate(void)
{
	if(E2ucCalibrated == CALI_FLAG)				//Initialize the Calibration flag
	{
		bCAL = 1;
	}
	else
	{
		bCAL = 0;
	}
}



/*******************************************************************************
Function:InitVar() 
Description:  
Input:	NULL 	
Output: NULL
Others:
*******************************************************************************/
void InitVar(void)
{
	bTimer1sFlg = 0;
	bTimer25msFlg = 0;
	bDataFlashFail = 0;
	bWriteFlashFlg = 0;
	bUpdataFlashFlg = 0;
	bCADCFlg = 0;
	bISPFlg = 0;

	bUartSndAckFlg = 0;
	bAlarmFlg = 0;
	bLEDOpen = 0;
	bLEDChgFlg = 0;
	bBLEOPEN = 0;
	bCalibrationFlg = 0;

	ucTimer3Cnt = 0;

	bChkLoadFlg = 0;
	bChkChgerFlg = 0;
	bChkChgerRFlg = 0;
	bLoadConectFlg = 0;
	bChgerConectFlg = 0;
	bUVBkFlg = 0;

	ucFlashWrValid = 0;
	ucWriteFlashCnt = 0;
	ucUartTimeoutCnt = 0;
	bIdleFlg = 0;
	bPDFlg = 0;
	bWakeupFlg = 0;
	ucIdleTimerCnt = 0;
	ucPDTimerCnt = 0;
	bKeyFlg = 0;
	ucLEDTimeCnt = 0;
	slCadcCurAverage = 0;
	ucUpDataLimitTime = 0;
	uiPackStatus = 0;
	uiBatStatus = 0;
	AFESCONF2Bk = 0;

	ucOVcnt = 0;
	ucOVRcnt = 0;
	ucUVcnt = 0;
	ucUVRcnt = 0;
	ucOTCcnt = 0;
	ucOTCRcnt = 0;
	ucUTCcnt = 0;
	ucUTCRcnt = 0;
	ucOTDcnt = 0;
	ucOTDRcnt = 0;
	ucUTDcnt = 0;
	ucUTDRcnt = 0;
	ucExtcaliSwitch1 = 0;
	ucExtcaliFlag = 0;

    bCellOpenDecFlag = 0;
    bBalancingFlg = 0;
    ucCellOpenDecCnt = 0;
    ucBalanceStep = BALANCE_ENABLE;
    ucCtoCnt = 0;

	ucCadcCnt = 0;
	MemorySet((U8 xdata *)slCadcCurBuf, 0, sizeof(slCadcCurBuf));

	bUartSndAckFlg = 0;

	ucBalanceTimeCnt = 0;
	uiBalanceChannel = 0;
	ucLEDTimeCnt = 0;
	bLEDOpen = 0;
	bPCSleepFlg = 0;//
	
	MemorySet((U8 xdata *)Info.uiVCell, 0, sizeof(Info.uiVCell));	//clr Info.uiVCell[]
	Info.slCurr = 0;
	Info.uiTemp1 = 2731;				//0 设定一个最大值
	Info.uiTemp2 = 2731;				//0
}


/*******************************************************************************
Function: InitSysPara()
Description: DataFlash Data write to XRAM  
Input:	NULL 	
Output: NULL
Others:
*******************************************************************************/
void InitSysPara(void)
{
    U32 i;
    //把存在类eeprom的值全部取出来了
	
    //Starting from the DATAFLASH_ADDR CODE address, read the DATAFLASH_LEN length of the data to the XDATA
	  if(!ReadMcuFlash(DATAFLASH_ADDR, DATAFLASH_MAP_ADDR, 512))    //读取第一个数据块的区域                  	
    {
			 //没有读取到值
        if(!ReadMcuFlash(DATAFLASH_ADDR2, DATAFLASH_MAP_ADDR, 512))         
        {
            while(1)
            {
                RSTSTAT	= 0x00;
                SystemIntoPD();
            }
        }
				else
        {
            FLASHCON = 0x01;
            if(CWORD[DATAFLASH_OK_FLG1_ADDR/2] != 0x5AA5)
            {
                UpEepromNoDelay();
                if(!WriteMcuFlash(DATAFLASH_ADDR))     //写入没成功                       
                {
                    UpEepromNoDelay();                  
                    WriteMcuFlash(DATAFLASH_ADDR);	
                }
            }
            FLASHCON = 0x00;	
        }
    }
    else
	  {
			FLASHCON = 0x01;
			if(CWORD[DATAFLASH_OK_FLG2_ADDR/2] != 0x5AA5)
			{
				UpEepromNoDelay();
				if(!WriteMcuFlash(DATAFLASH_ADDR2))
				{
					UpEepromNoDelay();
					WriteMcuFlash(DATAFLASH_ADDR2);	
				}
			}
			FLASHCON = 0x00;		
	  }

    ucFlashWrValid = 0x00;
		
    //intial AFE data
    Cleaner_AFE_Set;		//初始化AFE的控制的状态位											    

    //intial Info	    
		//E2uiPackConfigMap = -->0x2d 37
		//前面的是高位字节，后面的是低位字节
	  uiPackConfig = E2uiPackConfigMap;//0x0037;//0x2b, 0x37,	  0010 1011   0011 0111
		//uiPackConfig初始化:00101101 00110111
																	//sbit bCellNum1		=	uiPackConfig^8; 1 	1 																	 
																	//sbit bCellNum2		=	uiPackConfig^9; 1		1 0
																	//sbit bCellNum3		=	uiPackConfig^10;1		0 1
																	//sbit bLEDNum0		=	uiPackConfig^11;	0		1 	//00110111
																	//sbit bLEDNum1		=	uiPackConfig^12;  1		0
																	//sbit bTempNum		=	uiPackConfig^13;  1		1 //1：支持2个温度点检测，0：1个温度点
																	//sbit bCHGEnd		=	uiPackConfig^14;  0		0//充电截止关闭充电mosfet
																	//sbit bDSGEnd		=	uiPackConfig^15;  0		0 //放电截止关闭放电mos
																				//0010 1101 0x2d
		
																	//sbit bSC_EN			=	uiPackConfig^0;	 1		//00101011  硬件短路（所以低位的字节是指低位的）
																	//sbit bOV_EN			=	uiPackConfig^1;	 1		//硬件过充
																	//sbit bOCRC_EN		=	uiPackConfig^2;	 1		//auto reset or nor when OCC   过流自恢复使能
																	//sbit bBAL_EN		=	uiPackConfig^3;  0	  //平衡功能使能
																	//sbit bPF_EN			=	uiPackConfig^4;	 1    //disable    二级保护使能
																	//sbit bCTO_EN		=	uiPackConfig^5;	 1		//cell on  断线检测
																	//sbit bOCPM			=	uiPackConfig^6;	 0		//disable   过流同事关闭充放电mos
																	//sbit bEnEEPRomBK	=	uiPackConfig^7;0    //支持数据备份功能
																					//  0011 0111 0x37
		
		
		
		
		//系统总的讯息
	  Info.slCurr = 0;								//After the program is reset, the current default is "0",update after 1'seconds
	//	E2ulFCC=0X000,0X000,0x008,0x098,	0x0898 =2200mah
    Info.ulFCC = E2ulFCC;//系统满充容量，上次一次的系统满容量值
	  Info.uiCycleCount = E2uiCycleCount;//循环放电次数
	  Info.uiPackStatus = uiPackStatus;//电池状态1 刚开始是0 
	  Info.uiBatStatus = uiBatStatus;//电池状态2 刚开始是0 
	  Info.uiPackConfig = uiPackConfig; //把刚配置好的赋值给info结构体
    
    if(bSC_EN)//使能硬件短路保护
    {
			//设置AFE
        REG.AFESCONF1 |= 0x08;//使能sc_en 
			//E2ucAFEProtectConfig = 0x074 0111 0100 这边和上面的初始化变动在，硬件短路保护电压设置选择位：200mv
        REG.AFESCONF6 |= (E2ucAFEProtectConfig&0x0f); //前端保护设置  0x74  01:CADC采集范围200mv 11:mcu脉冲宽度 01：200硬件保护电压 00：硬件短路延时50us
    }
    
    if(bOV_EN)//使能硬件过冲保护
    {
        REG.AFESCONF1 |= 0x04;
			//0111 0100 ->硬件过冲配置成128个转换周期
        REG.AFESCONF7 |= (E2ucAFEProtectConfig&0xf0);
			
			//E2uiAFEOVvol = 0x011,0x030, = 0x1130=4400 =4.4v 硬件过充实判断单芯的电压	 前端过冲保护电压阈值
      //如果判断先修改成4.2v = 1068 = 0x010,0x068
  			i = (U32)E2uiAFEOVvol*50/293; //寄存器值 = 电压值/5.86mv
        REG.AFESCONF8 = i>>8;
        REG.AFESCONF9 = i;
    }
       
    //Intial cellNum 判断有多少个电芯
		//0b0000 0000 0000 0111
    //原来ucCellNum = (uiPackConfig&0x0007)+3;												//PackConfigMap last 3 bit set cellnum 3~10
    //修改：
#if debug
		TI = 0;
		TI = 1;	
		printf("\n uiPackConfig = %hx ----%hb\n",uiPackConfig,uiPackConfig);   
		printf("\n 低8位：bEnEEPRomBK=%x,bOCPM=%x,bCTO_EN=%x,bPF_EN=%x,bBAL_EN=%x,bOCRC_EN=%x,bOV_EN=%x,bSC_EN=%x 高8位 bDSGEnd=%x,bCHGEnd=%x,bTempNum=%x,bLEDNum1=%x,bLEDNum0=%x,bCellNum3=%x,bCellNum2=%x,bCellNum1=%x\n",((uiPackConfig&0x8000)>>15),((uiPackConfig&0x4000)>>14),((uiPackConfig&0x2000)>>13),((uiPackConfig&0x1000)>>12),((uiPackConfig&0x0800)>>11),((uiPackConfig&0x0400)>>10),((uiPackConfig&0x0200)>>9),((uiPackConfig&0x0100)>>8)\
		,((uiPackConfig&0x0080)>>7),((uiPackConfig&0x0040)>>6),((uiBatStatus&0x0020)>>5),((uiBatStatus&0x0010)>>4),((uiBatStatus&0x0008)>>3),((uiPackConfig&0x0004)>>2),((uiPackConfig&0x0002)>>1),(uiPackConfig&0x0001));   
		
#endif		

		ucCellNum = ((uiPackConfig&0x0005))+3;	 //5+3=8
		if(ucCellNum==5 || ucCellNum==10)
    {
        ucCellNumOffset = 0;
    }
    else if(ucCellNum==4 || ucCellNum==9)											//Nomal order	  VC1-VCn
    {
        ucCellNumOffset = 1;
    }
    else
    {
        ucCellNumOffset = 2;
    }
}


/*******************************************************************************
Function: InitIRQ() 
Description: Init EUART,INT0,INT1,TIMER0 Interrupt  
Input:	NULL 	
Output: NULL
Others:
*******************************************************************************/

void InitIRQ(void)
{
	EA = 0;

 	IEN0 = 0x30;						//Enable UART0,Timer3 Interrupt
													//EA EADC ET3 ES0 EPCA1(比较模块) EX1 EPCA0 EX0
													//0    0   1   1    0              0     0    0
	IEN1 = 0x18;						//Enable INT4  ,INT3 
													//ESCM(时钟监控) ELPD（低电压检测） EX2 EX3 EX4 EPWM1 EPWM0 ETWI
													//0                0                 0   1   1    0     0     0

#if 1
	//47 p1.0int afe alarm
	IENC = 0x80;						//Enable INT47	 
													//中断允许选择位
													// 1000 0000
	//44 p1.1 int_scp短路保护 
	IENC |= 0x11;						//INT44		多了一个INT40地刷短路检测	 
	//这边的触发方式需要以后改变
	//外部中断3是 上升沿触发 电池检测，用来采样电池电压时候用的
	//中断47 Alarm输入，AFE报警中断 只需要配置flag就行
	//中断44 放电短路检测 自己的 低电平触发后锁住
	
	//01 10 00 00 
	EXF0 = 0x60;						//Exint4：Falling edge trigger
													//触发方式：
													//int4下降沿沿触发，int3 上升沿触发
#endif	

	//IPHx 
	      //H:0 L:0 最低  H：0 L:1 等级1   H:1 L :0 等级2   H:1 L:1 等级3
	//		 PINT PADC PT3 PS0 PCF1 PX1 PCF0 PX0  		
	//IPH0						0  	0
	//IPL0						1		0
	//		 PSCM PLPDL PX2 PX3 PX4 PPWM PPWM0 PTWI
	//IPH1		0		0		0	   1	  1   0     0      0
	//IPL1								 0		1
	

#if 1
	IPL0 = 0x10;           //30
	IPH0 = 0x00;
	
	IPL1 = 0x08;
	IPH1 = 0x18;						//set uart priority = 3, other interrupt priority  = 0
#else
	IPL0 = 0x10;           //
	IPH0 = 0x00;
	
	IPL1 = 0x08;
	IPH1 = 0x18;						//set uart priority = 3, other interrupt priority  = 0
#endif
	EA = 1;
}




/*******************************************************************************
Function: InitTimer()
Description: Init Timer3,Set Timer3 time is 25ms 
Input:	NULL 	
Output: NULL
Others:
*******************************************************************************/
void InitTimer(void)
{
	BANK1_SEL;      //特殊功能寄存器 1  T3寄存器属于BANK1
	//这边设置的时钟源是32.768K
#if 0
	T3CON = 0x02;		//外部32.768kHz/128kHz为时钟源，1分频
#else
	T3CON = 0x30;	
#endif
									//0011 0000原来是00
									//     	T3PS[5:4]    00 1/1分频
									//                   01 1/8分频
									//                   10 1/64
									//                   11 1/256
	
									//      T3CLKS[1:0] 00 ---系统时钟，T3引脚用作i/O
								  //									01 ---T3端口输入外部时钟，自动上拉
									//									10 ---外部32.768或128k
									//                  11 ---保留
				       
	//T3CON = 0x30;	//0011 0100
#if 0
	TL3 = 0xCD;		 //32.768的时钟
	TH3 = 0xFC;			//25mS
#else
	TL3 = 0xD8;		 //这边是内部时钟为93750 的时候
	TH3 = 0xF6;			//25mS
#endif
	TR3 = 1;			//启动定时器3
	BANK0_SEL;
}


/*******************************************************************************
Function: InitGPIO()
Description:
	P0[0]-----蓝牙电源控制，默认输出低电平
	P0[1]-----LED显示
	P0[5:4]---TWI通讯，输入，外部已上拉
	P0[7:6]---UART
	P1[1:0]-预留端口，输出低电平
	P1[5:2]---JET51A，输出低电平
	P2[7]---预留I/O，默认输出低电平
	P2[4:0]---LED控制，默认输出低电平
	P2[6]---ALARM中断输入，输入，外部已上拉，且配置为外部中断
	P3[1]-----IO_KEY检测，默认输入上拉，且配置为外部中断
	P3[4:3]-----外部晶振，输入
	P3[5]---LED控制，默认输出低电平
Input:	NULL
Output: NULL
Others:
*******************************************************************************/
void InitGPIO(void)
{
	P0 = 0x50; //   0    1   0   1     0   0   0     0
	P0PCR = 0xD9; //第7 第6 第5 第4    第3 第2 第1 第0位   内部上拉允许
								//1	   1   0   1      1   0   0    1   
	P0CR = 0xD9;  //1    1   0   1      1   0   0    1      输入：0 输出：1
#if 1
	P1 = 0x10;//   0    1/0   0   1      0   0   0    0
	P1PCR = 0x15;//第7 第6 第5 第4    第3 第2 第1 第0位   内部上拉允许
#else
	P1 = 0x50;//   0    1/0   0   1      0   0   0    0
	P1PCR = 0x55;//第7 第6 第5 第4    第3 第2 第1 第0位   内部上拉允许
#endif	 			 //0	  1/0   0   1      0   1   0    1   
	P1CR = 0x14; //0    0   0   1      0   1   0    0      输入：0 输出：1

	
	P2 = 0x30;//   0    0   1   1      0   0   0    0
	P2PCR = 0xF8;//第7 第6 第5 第4    第3 第2 第1 第0位   内部上拉允许
							 //1	  1   1   1      1   0   0    1  
	P2CR = 0xE7; //1    1   1   0      0   1   1    1      输入：0 输出：1   //pin 2.1 scl
							 
	P3 = 0x00;   //   0    0   0   0      0   0   0   0
	P3PCR = 0x25;//	  0    0   1   0      0   1   0   1
	P3CR = 0x25;//    0    0   1   0      0   1   0   1
	
	//IO_BLEPW = 1;		//p0.3口置1
}


/*******************************************************************************
Function: InitClk()
Description: Set system clock = 16.6M/12 
Input:	NULL 	
Output: NULL
Others:
*******************************************************************************/
void InitClk(void)
{
 	CLKCON	= 0x08;								//SETB HFON    0000 1000
	//CLKCON	= 0x0C;								//SETB HFON    0000 1100
	_nop_(); 
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	//Delay1ms(11);
	CLKCON |= 0x04;								//SETB	FS, SYSCLK=24M 
	//CLKCON |= 0x04;
	//CLKCON : 0000 1110
}


/*******************************************************************************
Function: InitPWM()
Description: InitPWM2 
Input:	NULL   	
Output: NULL
Others:
*******************************************************************************/
void InitPWM(void)
{
  PWM2CON = 0xb0;						      	  //系统时钟/64  1011 0000 
  PWM2PL = 375000/2000;  //E2uiDSG1PWMFreq;	 		  //PWM2P=24000000/(64*E2uiDSG1PWMFreq);   4000HZ -->2K
	PWM2PH = (375000/2000) >> 8;//周期控制
  PWM2DL = (U16)(PWM2PL+PWM2PH*256)*E2ucDSG1PWMRatioL/100;  //占空比 30% ->低   60%->中 100%->高
	PWM2DH = ((U16)(PWM2PL+PWM2PH*256)*E2ucDSG1PWMRatioL/100) >> 8;
//	bSlowDischarge = 0;
//	bMidDischarge = 0;
//	ucDsgingSpeed = 2;
//	bFastDischarge = 1;
}


/*******************************************************************************
Function:PartialInitial()  
Description:  
Input: 	
Output: 
Others:
*******************************************************************************/
void PartialInitial(void)
{
	InitClk();						//初始化系统时钟

	InitGPIO();						//初始化GPIO

	InitTimer();					//初始化Timer

	InitUART0();					//初始化UART模块

	InitTwi();						//初始化TWI模块

	InitSysPara();					//初始化系统参数

	InitVar();						//初始化变量
	
	InitPWM();						//初始化PWM
//by tomi	
	roidmi_init();
		/**********by tomi******/
	//MCU_33V_Procude();//供电自锁，这边是调试用�
	
//#if debug
//  printf("\n ALLinital done \n");    
//#endif	

	if(InitAFE())					//初始化 AFE
	{

				ClearAFEFlg();

        if(WaitADCConvert())//等待产生标志位
        {
					BatteryInfoManage();			//电池信息管理：获取电压、电流、温度
    
					PorOnProtect();
    
					InitGasGauge();					//初始化容量信息，RSOC

					//CaliCurOffsetSub();//获取电流值
    
					InitIRQ();						//初始化中断
        }
        else
        {
					IntoPowerDown();
        }
	}
	else
	{
		IntoPowerDown();
	}
	
//	InitSealCalibrate();			//判断是否做过校准全部参数

//	if(bEnEEPRomBK)					//如果支持外部RTC和EEPROM，需要初始化
//	{
//		InitE2PRom();
//		InitRTC();
//	}

}

/*******************************************************************************
Function: AllInitial()
Description:  
Input:	NULL 	
Output: NULL
Others:
*******************************************************************************/
void AllInitial(void)
{
	InitClk();						//初始化系统时钟 

	InitGPIO();						//初始化GPIO
		/**********by tomi******/
	Delay1ms(5000);
#if 1
	IO_POWER_ON = 1;	
#endif
	ClrRam();						//清空MCU RAM
	
	InitUART0();					//初始化UART0模块

	InitTimer();					//初始化Timer
	
	InitTwi();						//初始化TWI模块
	
	InitSysPara();					//初始化系统参数

  InitVar();						//初始化变量//
	
  roidmi_init();
#if 1
	if(InitAFE())					//初始化 AFE
	{
#if debug
	TI = 0;
	TI = 1;
  printf("\n AFE inital done \n");    
#endif
//		 IO_Error_2 = 1;
		 ClearAFEFlg();
		//这边要增加MCU的标志

        if(WaitADCConvert())
        {
					BatteryInfoManage();			//电池信息管理：获取电压、电流、温度
			   //启动时的保护
					PorOnProtect();
			   //容量计初始化
					InitGasGauge();					//初始化容量信息，RSOC
					//暂时取消电流校准
			  	//CaliCurOffsetSub();
#if 1 //先把irq初始化放上面
					InitIRQ();						//初始化中断
#endif
					
				}
        else
        {
					IntoPowerDown();
#if debug	
					printf("\n IntoPowerDown1 \n");    
#endif  
        }
	}
	else
	{
		IntoPowerDown();
#if debug
					printf("\n IntoPowerDown2\n");    
#endif  
	}
#endif
}


/*******************************************************************************
Function: Initial()
Description:  ALLInitial()--Initial all data; PartialInitial()--Initial part data;
Input: 	NULL
Output: NULL
Others:
*******************************************************************************/
void Initial(void)
{
	FLASHCON = 0x00;
//tomi pass
    if(CWORD[510/2] != 0x5AA5)	//程序检测第一个1K数据最后两个字节是否为0x5AA5，如果不是则认为程序异常，直接进入ISP
	{
     ISPProcess(); //等待ISP下载tomi
	}

	//0011 1000  WDOF NULL PORF(上电复位) LVRF    CLRF(reset引脚) WDT.2 WDT.1 WDT.0
	if(((RSTSTAT&0x38)!=0) || (ucResetFlag==0x12))	//POR+LVR+ResetPin+PC instruction
	{
	 	RSTSTAT	= 0x00;
		ucResetFlag = 0;
		
		AllInitial();//所有都复位
#if 1
		printf("\n ALL init \n");    
#endif	
		
	}
	else			//WDT+OVL复位 看门狗和程序溢出复位
	{
	 	RSTSTAT	= 0x00;
		PartialInitial();//变量值没有初始化
		
#if debug
		TI = 0;
		TI = 1;	
		printf("\n Part init \n");    
#endif	
	}

}

/*****************************by tomi**********************************/
#if 1
//pack_fault_flag packfault_flag = {.pack_flag_all = 0};
pack_fault_flag packfault_flag;
void roidmi_init(void)
{
	IO_CHG_MOS = 1;
	IO_DSG_MOS = 1;
	IO_PACK_VADC = 0;//pack输出电压检测
	IO_SCP_RST = 0;//自己的短路清除软件
  packfault_flag.pack_flag_all = 0;
//增加外设初始化
//(1)ADC初始化
	//ADC_init();
}
#endif

//==============================debug====================
void  debug_led1(void)
{
	IO_Error_1 = 1;
	Delay1ms(5000);
	IO_Error_1 = 0;
	Delay1ms(5000);
	IO_Error_1 = 1;
	Delay1ms(5000);
	IO_Error_1 = 0;
	Delay1ms(5000);

}

void  debug_led2(void)
{
	IO_Error_2 = 1;
	Delay1ms(5000);
	IO_Error_2 = 0;
	Delay1ms(5000);
	IO_Error_2 = 1;
	Delay1ms(5000);
	IO_Error_2 = 0;
	Delay1ms(5000);

}

void  debug_led4(void)
{
	IO_Error_4 = 1;
	Delay1ms(5000);
	IO_Error_4 = 0;
	Delay1ms(5000);
	IO_Error_4 = 1;
	Delay1ms(5000);
	IO_Error_4 = 0;
	Delay1ms(5000);
}
void  debug_led8(void)
{
	IO_Error_8 = 1;
	Delay1ms(5000);
	IO_Error_8 = 0;
	Delay1ms(5000);
	IO_Error_8 = 1;
	Delay1ms(5000);
	IO_Error_8 = 0;
	Delay1ms(5000);
}

