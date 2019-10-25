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
	Info.uiTemp1 = 2731;				//0 �趨һ�����ֵ
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
    //�Ѵ�����eeprom��ֵȫ��ȡ������
	
    //Starting from the DATAFLASH_ADDR CODE address, read the DATAFLASH_LEN length of the data to the XDATA
	  if(!ReadMcuFlash(DATAFLASH_ADDR, DATAFLASH_MAP_ADDR, 512))    //��ȡ��һ�����ݿ������                  	
    {
			 //û�ж�ȡ��ֵ
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
                if(!WriteMcuFlash(DATAFLASH_ADDR))     //д��û�ɹ�                       
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
    Cleaner_AFE_Set;		//��ʼ��AFE�Ŀ��Ƶ�״̬λ											    

    //intial Info	    
		//E2uiPackConfigMap = -->0x2d 37
		//ǰ����Ǹ�λ�ֽڣ�������ǵ�λ�ֽ�
	  uiPackConfig = E2uiPackConfigMap;//0x0037;//0x2b, 0x37,	  0010 1011   0011 0111
		//uiPackConfig��ʼ��:00101101 00110111
																	//sbit bCellNum1		=	uiPackConfig^8; 1 	1 																	 
																	//sbit bCellNum2		=	uiPackConfig^9; 1		1 0
																	//sbit bCellNum3		=	uiPackConfig^10;1		0 1
																	//sbit bLEDNum0		=	uiPackConfig^11;	0		1 	//00110111
																	//sbit bLEDNum1		=	uiPackConfig^12;  1		0
																	//sbit bTempNum		=	uiPackConfig^13;  1		1 //1��֧��2���¶ȵ��⣬0��1���¶ȵ�
																	//sbit bCHGEnd		=	uiPackConfig^14;  0		0//����ֹ�رճ��mosfet
																	//sbit bDSGEnd		=	uiPackConfig^15;  0		0 //�ŵ��ֹ�رշŵ�mos
																				//0010 1101 0x2d
		
																	//sbit bSC_EN			=	uiPackConfig^0;	 1		//00101011  Ӳ����·�����Ե�λ���ֽ���ָ��λ�ģ�
																	//sbit bOV_EN			=	uiPackConfig^1;	 1		//Ӳ������
																	//sbit bOCRC_EN		=	uiPackConfig^2;	 1		//auto reset or nor when OCC   �����Իָ�ʹ��
																	//sbit bBAL_EN		=	uiPackConfig^3;  0	  //ƽ�⹦��ʹ��
																	//sbit bPF_EN			=	uiPackConfig^4;	 1    //disable    ��������ʹ��
																	//sbit bCTO_EN		=	uiPackConfig^5;	 1		//cell on  ���߼��
																	//sbit bOCPM			=	uiPackConfig^6;	 0		//disable   ����ͬ�¹رճ�ŵ�mos
																	//sbit bEnEEPRomBK	=	uiPackConfig^7;0    //֧�����ݱ��ݹ���
																					//  0011 0111 0x37
		
		
		
		
		//ϵͳ�ܵ�ѶϢ
	  Info.slCurr = 0;								//After the program is reset, the current default is "0",update after 1'seconds
	//	E2ulFCC=0X000,0X000,0x008,0x098,	0x0898 =2200mah
    Info.ulFCC = E2ulFCC;//ϵͳ�����������ϴ�һ�ε�ϵͳ������ֵ
	  Info.uiCycleCount = E2uiCycleCount;//ѭ���ŵ����
	  Info.uiPackStatus = uiPackStatus;//���״̬1 �տ�ʼ��0 
	  Info.uiBatStatus = uiBatStatus;//���״̬2 �տ�ʼ��0 
	  Info.uiPackConfig = uiPackConfig; //�Ѹ����úõĸ�ֵ��info�ṹ��
    
    if(bSC_EN)//ʹ��Ӳ����·����
    {
			//����AFE
        REG.AFESCONF1 |= 0x08;//ʹ��sc_en 
			//E2ucAFEProtectConfig = 0x074 0111 0100 ��ߺ�����ĳ�ʼ���䶯�ڣ�Ӳ����·������ѹ����ѡ��λ��200mv
        REG.AFESCONF6 |= (E2ucAFEProtectConfig&0x0f); //ǰ�˱�������  0x74  01:CADC�ɼ���Χ200mv 11:mcu������ 01��200Ӳ��������ѹ 00��Ӳ����·��ʱ50us
    }
    
    if(bOV_EN)//ʹ��Ӳ�����屣��
    {
        REG.AFESCONF1 |= 0x04;
			//0111 0100 ->Ӳ���������ó�128��ת������
        REG.AFESCONF7 |= (E2ucAFEProtectConfig&0xf0);
			
			//E2uiAFEOVvol = 0x011,0x030, = 0x1130=4400 =4.4v Ӳ������ʵ�жϵ�о�ĵ�ѹ	 ǰ�˹��屣����ѹ��ֵ
      //����ж����޸ĳ�4.2v = 1068 = 0x010,0x068
  			i = (U32)E2uiAFEOVvol*50/293; //�Ĵ���ֵ = ��ѹֵ/5.86mv
        REG.AFESCONF8 = i>>8;
        REG.AFESCONF9 = i;
    }
       
    //Intial cellNum �ж��ж��ٸ���о
		//0b0000 0000 0000 0111
    //ԭ��ucCellNum = (uiPackConfig&0x0007)+3;												//PackConfigMap last 3 bit set cellnum 3~10
    //�޸ģ�
#if debug
		TI = 0;
		TI = 1;	
		printf("\n uiPackConfig = %hx ----%hb\n",uiPackConfig,uiPackConfig);   
		printf("\n ��8λ��bEnEEPRomBK=%x,bOCPM=%x,bCTO_EN=%x,bPF_EN=%x,bBAL_EN=%x,bOCRC_EN=%x,bOV_EN=%x,bSC_EN=%x ��8λ bDSGEnd=%x,bCHGEnd=%x,bTempNum=%x,bLEDNum1=%x,bLEDNum0=%x,bCellNum3=%x,bCellNum2=%x,bCellNum1=%x\n",((uiPackConfig&0x8000)>>15),((uiPackConfig&0x4000)>>14),((uiPackConfig&0x2000)>>13),((uiPackConfig&0x1000)>>12),((uiPackConfig&0x0800)>>11),((uiPackConfig&0x0400)>>10),((uiPackConfig&0x0200)>>9),((uiPackConfig&0x0100)>>8)\
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
													//EA EADC ET3 ES0 EPCA1(�Ƚ�ģ��) EX1 EPCA0 EX0
													//0    0   1   1    0              0     0    0
	IEN1 = 0x18;						//Enable INT4  ,INT3 
													//ESCM(ʱ�Ӽ��) ELPD���͵�ѹ��⣩ EX2 EX3 EX4 EPWM1 EPWM0 ETWI
													//0                0                 0   1   1    0     0     0

#if 1
	//47 p1.0int afe alarm
	IENC = 0x80;						//Enable INT47	 
													//�ж�����ѡ��λ
													// 1000 0000
	//44 p1.1 int_scp��·���� 
	IENC |= 0x11;						//INT44		����һ��INT40��ˢ��·���	 
	//��ߵĴ�����ʽ��Ҫ�Ժ�ı�
	//�ⲿ�ж�3�� �����ش��� ��ؼ�⣬����������ص�ѹʱ���õ�
	//�ж�47 Alarm���룬AFE�����ж� ֻ��Ҫ����flag����
	//�ж�44 �ŵ��·��� �Լ��� �͵�ƽ��������ס
	
	//01 10 00 00 
	EXF0 = 0x60;						//Exint4��Falling edge trigger
													//������ʽ��
													//int4�½����ش�����int3 �����ش���
#endif	

	//IPHx 
	      //H:0 L:0 ���  H��0 L:1 �ȼ�1   H:1 L :0 �ȼ�2   H:1 L:1 �ȼ�3
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
	BANK1_SEL;      //���⹦�ܼĴ��� 1  T3�Ĵ�������BANK1
	//������õ�ʱ��Դ��32.768K
#if 0
	T3CON = 0x02;		//�ⲿ32.768kHz/128kHzΪʱ��Դ��1��Ƶ
#else
	T3CON = 0x30;	
#endif
									//0011 0000ԭ����00
									//     	T3PS[5:4]    00 1/1��Ƶ
									//                   01 1/8��Ƶ
									//                   10 1/64
									//                   11 1/256
	
									//      T3CLKS[1:0] 00 ---ϵͳʱ�ӣ�T3��������i/O
								  //									01 ---T3�˿������ⲿʱ�ӣ��Զ�����
									//									10 ---�ⲿ32.768��128k
									//                  11 ---����
				       
	//T3CON = 0x30;	//0011 0100
#if 0
	TL3 = 0xCD;		 //32.768��ʱ��
	TH3 = 0xFC;			//25mS
#else
	TL3 = 0xD8;		 //������ڲ�ʱ��Ϊ93750 ��ʱ��
	TH3 = 0xF6;			//25mS
#endif
	TR3 = 1;			//������ʱ��3
	BANK0_SEL;
}


/*******************************************************************************
Function: InitGPIO()
Description:
	P0[0]-----������Դ���ƣ�Ĭ������͵�ƽ
	P0[1]-----LED��ʾ
	P0[5:4]---TWIͨѶ�����룬�ⲿ������
	P0[7:6]---UART
	P1[1:0]-Ԥ���˿ڣ�����͵�ƽ
	P1[5:2]---JET51A������͵�ƽ
	P2[7]---Ԥ��I/O��Ĭ������͵�ƽ
	P2[4:0]---LED���ƣ�Ĭ������͵�ƽ
	P2[6]---ALARM�ж����룬���룬�ⲿ��������������Ϊ�ⲿ�ж�
	P3[1]-----IO_KEY��⣬Ĭ������������������Ϊ�ⲿ�ж�
	P3[4:3]-----�ⲿ��������
	P3[5]---LED���ƣ�Ĭ������͵�ƽ
Input:	NULL
Output: NULL
Others:
*******************************************************************************/
void InitGPIO(void)
{
	P0 = 0x50; //   0    1   0   1     0   0   0     0
	P0PCR = 0xD9; //��7 ��6 ��5 ��4    ��3 ��2 ��1 ��0λ   �ڲ���������
								//1	   1   0   1      1   0   0    1   
	P0CR = 0xD9;  //1    1   0   1      1   0   0    1      ���룺0 �����1
#if 1
	P1 = 0x10;//   0    1/0   0   1      0   0   0    0
	P1PCR = 0x15;//��7 ��6 ��5 ��4    ��3 ��2 ��1 ��0λ   �ڲ���������
#else
	P1 = 0x50;//   0    1/0   0   1      0   0   0    0
	P1PCR = 0x55;//��7 ��6 ��5 ��4    ��3 ��2 ��1 ��0λ   �ڲ���������
#endif	 			 //0	  1/0   0   1      0   1   0    1   
	P1CR = 0x14; //0    0   0   1      0   1   0    0      ���룺0 �����1

	
	P2 = 0x30;//   0    0   1   1      0   0   0    0
	P2PCR = 0xF8;//��7 ��6 ��5 ��4    ��3 ��2 ��1 ��0λ   �ڲ���������
							 //1	  1   1   1      1   0   0    1  
	P2CR = 0xE7; //1    1   1   0      0   1   1    1      ���룺0 �����1   //pin 2.1 scl
							 
	P3 = 0x00;   //   0    0   0   0      0   0   0   0
	P3PCR = 0x25;//	  0    0   1   0      0   1   0   1
	P3CR = 0x25;//    0    0   1   0      0   1   0   1
	
	//IO_BLEPW = 1;		//p0.3����1
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
  PWM2CON = 0xb0;						      	  //ϵͳʱ��/64  1011 0000 
  PWM2PL = 375000/2000;  //E2uiDSG1PWMFreq;	 		  //PWM2P=24000000/(64*E2uiDSG1PWMFreq);   4000HZ -->2K
	PWM2PH = (375000/2000) >> 8;//���ڿ���
  PWM2DL = (U16)(PWM2PL+PWM2PH*256)*E2ucDSG1PWMRatioL/100;  //ռ�ձ� 30% ->��   60%->�� 100%->��
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
	InitClk();						//��ʼ��ϵͳʱ��

	InitGPIO();						//��ʼ��GPIO

	InitTimer();					//��ʼ��Timer

	InitUART0();					//��ʼ��UARTģ��

	InitTwi();						//��ʼ��TWIģ��

	InitSysPara();					//��ʼ��ϵͳ����

	InitVar();						//��ʼ������
	
	InitPWM();						//��ʼ��PWM
//by tomi	
	roidmi_init();
		/**********by tomi******/
	//MCU_33V_Procude();//��������������ǵ����ã
	
//#if debug
//  printf("\n ALLinital done \n");    
//#endif	

	if(InitAFE())					//��ʼ�� AFE
	{

				ClearAFEFlg();

        if(WaitADCConvert())//�ȴ�������־λ
        {
					BatteryInfoManage();			//�����Ϣ������ȡ��ѹ���������¶�
    
					PorOnProtect();
    
					InitGasGauge();					//��ʼ��������Ϣ��RSOC

					//CaliCurOffsetSub();//��ȡ����ֵ
    
					InitIRQ();						//��ʼ���ж�
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
	
//	InitSealCalibrate();			//�ж��Ƿ�����У׼ȫ������

//	if(bEnEEPRomBK)					//���֧���ⲿRTC��EEPROM����Ҫ��ʼ��
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
	InitClk();						//��ʼ��ϵͳʱ�� 

	InitGPIO();						//��ʼ��GPIO
		/**********by tomi******/
	Delay1ms(5000);
#if 1
	IO_POWER_ON = 1;	
#endif
	ClrRam();						//���MCU RAM
	
	InitUART0();					//��ʼ��UART0ģ��

	InitTimer();					//��ʼ��Timer
	
	InitTwi();						//��ʼ��TWIģ��
	
	InitSysPara();					//��ʼ��ϵͳ����

  InitVar();						//��ʼ������//
	
  roidmi_init();
#if 1
	if(InitAFE())					//��ʼ�� AFE
	{
#if debug
	TI = 0;
	TI = 1;
  printf("\n AFE inital done \n");    
#endif
//		 IO_Error_2 = 1;
		 ClearAFEFlg();
		//���Ҫ����MCU�ı�־

        if(WaitADCConvert())
        {
					BatteryInfoManage();			//�����Ϣ������ȡ��ѹ���������¶�
			   //����ʱ�ı���
					PorOnProtect();
			   //�����Ƴ�ʼ��
					InitGasGauge();					//��ʼ��������Ϣ��RSOC
					//��ʱȡ������У׼
			  	//CaliCurOffsetSub();
#if 1 //�Ȱ�irq��ʼ��������
					InitIRQ();						//��ʼ���ж�
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
    if(CWORD[510/2] != 0x5AA5)	//�������һ��1K������������ֽ��Ƿ�Ϊ0x5AA5�������������Ϊ�����쳣��ֱ�ӽ���ISP
	{
     ISPProcess(); //�ȴ�ISP����tomi
	}

	//0011 1000  WDOF NULL PORF(�ϵ縴λ) LVRF    CLRF(reset����) WDT.2 WDT.1 WDT.0
	if(((RSTSTAT&0x38)!=0) || (ucResetFlag==0x12))	//POR+LVR+ResetPin+PC instruction
	{
	 	RSTSTAT	= 0x00;
		ucResetFlag = 0;
		
		AllInitial();//���ж���λ
#if 1
		printf("\n ALL init \n");    
#endif	
		
	}
	else			//WDT+OVL��λ ���Ź��ͳ��������λ
	{
	 	RSTSTAT	= 0x00;
		PartialInitial();//����ֵû�г�ʼ��
		
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
	IO_PACK_VADC = 0;//pack�����ѹ���
	IO_SCP_RST = 0;//�Լ��Ķ�·������
  packfault_flag.pack_flag_all = 0;
//���������ʼ��
//(1)ADC��ʼ��
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

