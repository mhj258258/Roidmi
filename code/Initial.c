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
	Info.uiTemp1 = 2731;				//0 Éè¶¨Ò»¸ö×î´óÖµ
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
    //°Ñ´æÔÚÀàeepromµÄÖµÈ«²¿È¡³öÀ´ÁË
	
    //Starting from the DATAFLASH_ADDR CODE address, read the DATAFLASH_LEN length of the data to the XDATA
	  if(!ReadMcuFlash(DATAFLASH_ADDR, DATAFLASH_MAP_ADDR, 512))    //¶ÁÈ¡µÚÒ»¸öÊý¾Ý¿éµÄÇøÓò                  	
    {
			 //Ã»ÓÐ¶ÁÈ¡µ½Öµ
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
                if(!WriteMcuFlash(DATAFLASH_ADDR))     //Ð´ÈëÃ»³É¹¦                       
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
    Cleaner_AFE_Set;		//³õÊ¼»¯AFEµÄ¿ØÖÆµÄ×´Ì¬Î»											    

    //intial Info	    
		//E2uiPackConfigMap = -->0x2d 37
		//Ç°ÃæµÄÊÇ¸ßÎ»×Ö½Ú£¬ºóÃæµÄÊÇµÍÎ»×Ö½Ú
	  uiPackConfig = E2uiPackConfigMap;//0x0037;//0x2b, 0x37,	  0010 1011   0011 0111
		//uiPackConfig³õÊ¼»¯:00101101 00110111
																	//sbit bCellNum1		=	uiPackConfig^8; 1 	1 																	 
																	//sbit bCellNum2		=	uiPackConfig^9; 1		1 0
																	//sbit bCellNum3		=	uiPackConfig^10;1		0 1
																	//sbit bLEDNum0		=	uiPackConfig^11;	0		1 	//00110111
																	//sbit bLEDNum1		=	uiPackConfig^12;  1		0
																	//sbit bTempNum		=	uiPackConfig^13;  1		1 //1£ºÖ§³Ö2¸öÎÂ¶Èµã¼ì²â£¬0£º1¸öÎÂ¶Èµã
																	//sbit bCHGEnd		=	uiPackConfig^14;  0		0//³äµç½ØÖ¹¹Ø±Õ³äµçmosfet
																	//sbit bDSGEnd		=	uiPackConfig^15;  0		0 //·Åµç½ØÖ¹¹Ø±Õ·Åµçmos
																				//0010 1101 0x2d
		
																	//sbit bSC_EN			=	uiPackConfig^0;	 1		//00101011  Ó²¼þ¶ÌÂ·£¨ËùÒÔµÍÎ»µÄ×Ö½ÚÊÇÖ¸µÍÎ»µÄ£©
																	//sbit bOV_EN			=	uiPackConfig^1;	 1		//Ó²¼þ¹ý³ä
																	//sbit bOCRC_EN		=	uiPackConfig^2;	 1		//auto reset or nor when OCC   ¹ýÁ÷×Ô»Ö¸´Ê¹ÄÜ
																	//sbit bBAL_EN		=	uiPackConfig^3;  0	  //Æ½ºâ¹¦ÄÜÊ¹ÄÜ
																	//sbit bPF_EN			=	uiPackConfig^4;	 1    //disable    ¶þ¼¶±£»¤Ê¹ÄÜ
																	//sbit bCTO_EN		=	uiPackConfig^5;	 1		//cell on  ¶ÏÏß¼ì²â
																	//sbit bOCPM			=	uiPackConfig^6;	 0		//disable   ¹ýÁ÷Í¬ÊÂ¹Ø±Õ³ä·Åµçmos
																	//sbit bEnEEPRomBK	=	uiPackConfig^7;0    //Ö§³ÖÊý¾Ý±¸·Ý¹¦ÄÜ
																					//  0011 0111 0x37
		
		
		
		
		//ÏµÍ³×ÜµÄÑ¶Ï¢
	  Info.slCurr = 0;								//After the program is reset, the current default is "0",update after 1'seconds
	//	E2ulFCC=0X000,0X000,0x008,0x098,	0x0898 =2200mah
    Info.ulFCC = E2ulFCC;//ÏµÍ³Âú³äÈÝÁ¿£¬ÉÏ´ÎÒ»´ÎµÄÏµÍ³ÂúÈÝÁ¿Öµ
	  Info.uiCycleCount = E2uiCycleCount;//Ñ­»··Åµç´ÎÊý
	  Info.uiPackStatus = uiPackStatus;//µç³Ø×´Ì¬1 ¸Õ¿ªÊ¼ÊÇ0 
	  Info.uiBatStatus = uiBatStatus;//µç³Ø×´Ì¬2 ¸Õ¿ªÊ¼ÊÇ0 
	  Info.uiPackConfig = uiPackConfig; //°Ñ¸ÕÅäÖÃºÃµÄ¸³Öµ¸øinfo½á¹¹Ìå
    
    if(bSC_EN)//Ê¹ÄÜÓ²¼þ¶ÌÂ·±£»¤
    {
			//ÉèÖÃAFE
        REG.AFESCONF1 |= 0x08;//Ê¹ÄÜsc_en 
			//E2ucAFEProtectConfig = 0x074 0111 0100 Õâ±ßºÍÉÏÃæµÄ³õÊ¼»¯±ä¶¯ÔÚ£¬Ó²¼þ¶ÌÂ·±£»¤µçÑ¹ÉèÖÃÑ¡ÔñÎ»£º200mv
        REG.AFESCONF6 |= (E2ucAFEProtectConfig&0x0f); //Ç°¶Ë±£»¤ÉèÖÃ  0x74  01:CADC²É¼¯·¶Î§200mv 11:mcuÂö³å¿í¶È 01£º200Ó²¼þ±£»¤µçÑ¹ 00£ºÓ²¼þ¶ÌÂ·ÑÓÊ±50us
    }
    
    if(bOV_EN)//Ê¹ÄÜÓ²¼þ¹ý³å±£»¤
    {
        REG.AFESCONF1 |= 0x04;
			//0111 0100 ->Ó²¼þ¹ý³åÅäÖÃ³É128¸ö×ª»»ÖÜÆÚ
        REG.AFESCONF7 |= (E2ucAFEProtectConfig&0xf0);
			
			//E2uiAFEOVvol = 0x011,0x030, = 0x1130=4400 =4.4v Ó²¼þ¹ý³äÊµÅÐ¶Ïµ¥Ð¾µÄµçÑ¹	 Ç°¶Ë¹ý³å±£»¤µçÑ¹ãÐÖµ
      //Èç¹ûÅÐ¶ÏÏÈÐÞ¸Ä³É4.2v = 1068 = 0x010,0x068
  			i = (U32)E2uiAFEOVvol*50/293; //¼Ä´æÆ÷Öµ = µçÑ¹Öµ/5.86mv
        REG.AFESCONF8 = i>>8;
        REG.AFESCONF9 = i;
    }
       
    //Intial cellNum ÅÐ¶ÏÓÐ¶àÉÙ¸öµçÐ¾
		//0b0000 0000 0000 0111
    //Ô­À´ucCellNum = (uiPackConfig&0x0007)+3;												//PackConfigMap last 3 bit set cellnum 3~10
    //ÐÞ¸Ä£º
#if debug
		TI = 0;
		TI = 1;	
		printf("\n uiPackConfig = %hx ----%hb\n",uiPackConfig,uiPackConfig);   
		printf("\n µÍ8Î»£ºbEnEEPRomBK=%x,bOCPM=%x,bCTO_EN=%x,bPF_EN=%x,bBAL_EN=%x,bOCRC_EN=%x,bOV_EN=%x,bSC_EN=%x ¸ß8Î» bDSGEnd=%x,bCHGEnd=%x,bTempNum=%x,bLEDNum1=%x,bLEDNum0=%x,bCellNum3=%x,bCellNum2=%x,bCellNum1=%x\n",((uiPackConfig&0x8000)>>15),((uiPackConfig&0x4000)>>14),((uiPackConfig&0x2000)>>13),((uiPackConfig&0x1000)>>12),((uiPackConfig&0x0800)>>11),((uiPackConfig&0x0400)>>10),((uiPackConfig&0x0200)>>9),((uiPackConfig&0x0100)>>8)\
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
													//EA EADC ET3 ES0 EPCA1(±È½ÏÄ£¿é) EX1 EPCA0 EX0
													//0    0   1   1    0              0     0    0
	IEN1 = 0x18;						//Enable INT4  ,INT3 
													//ESCM(Ê±ÖÓ¼à¿Ø) ELPD£¨µÍµçÑ¹¼ì²â£© EX2 EX3 EX4 EPWM1 EPWM0 ETWI
													//0                0                 0   1   1    0     0     0

#if 1
	//47 p1.0int afe alarm
	IENC = 0x80;						//Enable INT47	 
													//ÖÐ¶ÏÔÊÐíÑ¡ÔñÎ»
													// 1000 0000
	//44 p1.1 int_scp¶ÌÂ·±£»¤ 
	IENC |= 0x11;						//INT44		¶àÁËÒ»¸öINT40µØË¢¶ÌÂ·¼ì²â	 
	//Õâ±ßµÄ´¥·¢·½Ê½ÐèÒªÒÔºó¸Ä±ä
	//Íâ²¿ÖÐ¶Ï3ÊÇ ÉÏÉýÑØ´¥·¢ µç³Ø¼ì²â£¬ÓÃÀ´²ÉÑùµç³ØµçÑ¹Ê±ºòÓÃµÄ
	//ÖÐ¶Ï47 AlarmÊäÈë£¬AFE±¨¾¯ÖÐ¶Ï Ö»ÐèÒªÅäÖÃflag¾ÍÐÐ
	//ÖÐ¶Ï44 ·Åµç¶ÌÂ·¼ì²â ×Ô¼ºµÄ µÍµçÆ½´¥·¢ºóËø×¡
	
	//01 10 00 00 
	EXF0 = 0x60;						//Exint4£ºFalling edge trigger
													//´¥·¢·½Ê½£º
													//int4ÏÂ½µÑØÑØ´¥·¢£¬int3 ÉÏÉýÑØ´¥·¢
#endif	

	//IPHx 
	      //H:0 L:0 ×îµÍ  H£º0 L:1 µÈ¼¶1   H:1 L :0 µÈ¼¶2   H:1 L:1 µÈ¼¶3
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
	BANK1_SEL;      //ÌØÊâ¹¦ÄÜ¼Ä´æÆ÷ 1  T3¼Ä´æÆ÷ÊôÓÚBANK1
	//Õâ±ßÉèÖÃµÄÊ±ÖÓÔ´ÊÇ32.768K
#if 0
	T3CON = 0x02;		//Íâ²¿32.768kHz/128kHzÎªÊ±ÖÓÔ´£¬1·ÖÆµ
#else
	T3CON = 0x30;	
#endif
									//0011 0000Ô­À´ÊÇ00
									//     	T3PS[5:4]    00 1/1·ÖÆµ
									//                   01 1/8·ÖÆµ
									//                   10 1/64
									//                   11 1/256
	
									//      T3CLKS[1:0] 00 ---ÏµÍ³Ê±ÖÓ£¬T3Òý½ÅÓÃ×÷i/O
								  //									01 ---T3¶Ë¿ÚÊäÈëÍâ²¿Ê±ÖÓ£¬×Ô¶¯ÉÏÀ­
									//									10 ---Íâ²¿32.768»ò128k
									//                  11 ---±£Áô
				       
	//T3CON = 0x30;	//0011 0100
#if 0
	TL3 = 0xCD;		 //32.768µÄÊ±ÖÓ
	TH3 = 0xFC;			//25mS
#else
	TL3 = 0xD8;		 //Õâ±ßÊÇÄÚ²¿Ê±ÖÓÎª93750 µÄÊ±ºò
	TH3 = 0xF6;			//25mS
#endif
	TR3 = 1;			//Æô¶¯¶¨Ê±Æ÷3
	BANK0_SEL;
}


/*******************************************************************************
Function: InitGPIO()
Description:
	P0[0]-----À¶ÑÀµçÔ´¿ØÖÆ£¬Ä¬ÈÏÊä³öµÍµçÆ½
	P0[1]-----LEDÏÔÊ¾
	P0[5:4]---TWIÍ¨Ñ¶£¬ÊäÈë£¬Íâ²¿ÒÑÉÏÀ­
	P0[7:6]---UART
	P1[1:0]-Ô¤Áô¶Ë¿Ú£¬Êä³öµÍµçÆ½
	P1[5:2]---JET51A£¬Êä³öµÍµçÆ½
	P2[7]---Ô¤ÁôI/O£¬Ä¬ÈÏÊä³öµÍµçÆ½
	P2[4:0]---LED¿ØÖÆ£¬Ä¬ÈÏÊä³öµÍµçÆ½
	P2[6]---ALARMÖÐ¶ÏÊäÈë£¬ÊäÈë£¬Íâ²¿ÒÑÉÏÀ­£¬ÇÒÅäÖÃÎªÍâ²¿ÖÐ¶Ï
	P3[1]-----IO_KEY¼ì²â£¬Ä¬ÈÏÊäÈëÉÏÀ­£¬ÇÒÅäÖÃÎªÍâ²¿ÖÐ¶Ï
	P3[4:3]-----Íâ²¿¾§Õñ£¬ÊäÈë
	P3[5]---LED¿ØÖÆ£¬Ä¬ÈÏÊä³öµÍµçÆ½
Input:	NULL
Output: NULL
Others:
*******************************************************************************/
void InitGPIO(void)
{
	P0 = 0x50; //   0    1   0   1     0   0   0     0
	P0PCR = 0xD9; //µÚ7 µÚ6 µÚ5 µÚ4    µÚ3 µÚ2 µÚ1 µÚ0Î»   ÄÚ²¿ÉÏÀ­ÔÊÐí
								//1	   1   0   1      1   0   0    1   
	P0CR = 0xD9;  //1    1   0   1      1   0   0    1      ÊäÈë£º0 Êä³ö£º1
#if 1
	P1 = 0x10;//   0    1/0   0   1      0   0   0    0
	P1PCR = 0x15;//µÚ7 µÚ6 µÚ5 µÚ4    µÚ3 µÚ2 µÚ1 µÚ0Î»   ÄÚ²¿ÉÏÀ­ÔÊÐí
#else
	P1 = 0x50;//   0    1/0   0   1      0   0   0    0
	P1PCR = 0x55;//µÚ7 µÚ6 µÚ5 µÚ4    µÚ3 µÚ2 µÚ1 µÚ0Î»   ÄÚ²¿ÉÏÀ­ÔÊÐí
#endif	 			 //0	  1/0   0   1      0   1   0    1   
	P1CR = 0x14; //0    0   0   1      0   1   0    0      ÊäÈë£º0 Êä³ö£º1

	
	P2 = 0x30;//   0    0   1   1      0   0   0    0
	P2PCR = 0xF8;//µÚ7 µÚ6 µÚ5 µÚ4    µÚ3 µÚ2 µÚ1 µÚ0Î»   ÄÚ²¿ÉÏÀ­ÔÊÐí
							 //1	  1   1   1      1   0   0    1  
	P2CR = 0xE7; //1    1   1   0      0   1   1    1      ÊäÈë£º0 Êä³ö£º1   //pin 2.1 scl
							 
	P3 = 0x00;   //   0    0   0   0      0   0   0   0
	P3PCR = 0x25;//	  0    0   1   0      0   1   0   1
	P3CR = 0x25;//    0    0   1   0      0   1   0   1
	
	//IO_BLEPW = 1;		//p0.3¿ÚÖÃ1
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
  PWM2CON = 0xb0;						      	  //ÏµÍ³Ê±ÖÓ/64  1011 0000 
  PWM2PL = 375000/2000;  //E2uiDSG1PWMFreq;	 		  //PWM2P=24000000/(64*E2uiDSG1PWMFreq);   4000HZ -->2K
	PWM2PH = (375000/2000) >> 8;//ÖÜÆÚ¿ØÖÆ
  PWM2DL = (U16)(PWM2PL+PWM2PH*256)*E2ucDSG1PWMRatioL/100;  //Õ¼¿Õ±È 30% ->µÍ   60%->ÖÐ 100%->¸ß
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
	InitClk();						//³õÊ¼»¯ÏµÍ³Ê±ÖÓ

	InitGPIO();						//³õÊ¼»¯GPIO

	InitTimer();					//³õÊ¼»¯Timer

	InitUART0();					//³õÊ¼»¯UARTÄ£¿é

	InitTwi();						//³õÊ¼»¯TWIÄ£¿é

	InitSysPara();					//³õÊ¼»¯ÏµÍ³²ÎÊý

	InitVar();						//³õÊ¼»¯±äÁ¿
	
	InitPWM();						//³õÊ¼»¯PWM
//by tomi	
	roidmi_init();
		/**********by tomi******/
	//MCU_33V_Procude();//¹©µç×ÔËø£¬Õâ±ßÊÇµ÷ÊÔÓÃ£
	
//#if debug
//  printf("\n ALLinital done \n");    
//#endif	

	if(InitAFE())					//³õÊ¼»¯ AFE
	{

				ClearAFEFlg();

        if(WaitADCConvert())//µÈ´ý²úÉú±êÖ¾Î»
        {
					BatteryInfoManage();			//µç³ØÐÅÏ¢¹ÜÀí£º»ñÈ¡µçÑ¹¡¢µçÁ÷¡¢ÎÂ¶È
    
					PorOnProtect();
    
					InitGasGauge();					//³õÊ¼»¯ÈÝÁ¿ÐÅÏ¢£¬RSOC

					//CaliCurOffsetSub();//»ñÈ¡µçÁ÷Öµ
    
					InitIRQ();						//³õÊ¼»¯ÖÐ¶Ï
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
	
//	InitSealCalibrate();			//ÅÐ¶ÏÊÇ·ñ×ö¹ýÐ£×¼È«²¿²ÎÊý

//	if(bEnEEPRomBK)					//Èç¹ûÖ§³ÖÍâ²¿RTCºÍEEPROM£¬ÐèÒª³õÊ¼»¯
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
	InitClk();						//³õÊ¼»¯ÏµÍ³Ê±ÖÓ 

	InitGPIO();						//³õÊ¼»¯GPIO
		/**********by tomi******/
	Delay1ms(5000);
#if 1
	IO_POWER_ON = 1;	
#endif
	ClrRam();						//Çå¿ÕMCU RAM
	
	InitUART0();					//³õÊ¼»¯UART0Ä£¿é

	InitTimer();					//³õÊ¼»¯Timer
	
	InitTwi();						//³õÊ¼»¯TWIÄ£¿é
	
	InitSysPara();					//³õÊ¼»¯ÏµÍ³²ÎÊý

  InitVar();						//³õÊ¼»¯±äÁ¿//
	
  roidmi_init();
#if 1
	if(InitAFE())					//³õÊ¼»¯ AFE
	{
#if debug
	TI = 0;
	TI = 1;
  printf("\n AFE inital done \n");    
#endif
//		 IO_Error_2 = 1;
		 ClearAFEFlg();
		//Õâ±ßÒªÔö¼ÓMCUµÄ±êÖ¾

        if(WaitADCConvert())
        {
					BatteryInfoManage();			//µç³ØÐÅÏ¢¹ÜÀí£º»ñÈ¡µçÑ¹¡¢µçÁ÷¡¢ÎÂ¶È
			   //Æô¶¯Ê±µÄ±£»¤
					PorOnProtect();
			   //ÈÝÁ¿¼Æ³õÊ¼»¯
					InitGasGauge();					//³õÊ¼»¯ÈÝÁ¿ÐÅÏ¢£¬RSOC
					//ÔÝÊ±È¡ÏûµçÁ÷Ð£×¼
			  	//CaliCurOffsetSub();
#if 1 //ÏÈ°Ñirq³õÊ¼»¯·ÅÉÏÃæ
					InitIRQ();						//³õÊ¼»¯ÖÐ¶Ï
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
    if(CWORD[510/2] != 0x5AA5)	//³ÌÐò¼ì²âµÚÒ»¸ö1KÊý¾Ý×îºóÁ½¸ö×Ö½ÚÊÇ·ñÎª0x5AA5£¬Èç¹û²»ÊÇÔòÈÏÎª³ÌÐòÒì³££¬Ö±½Ó½øÈëISP
	{
     ISPProcess(); //µÈ´ýISPÏÂÔØtomi
	}

	//0011 1000  WDOF NULL PORF(ÉÏµç¸´Î») LVRF    CLRF(resetÒý½Å) WDT.2 WDT.1 WDT.0
	if(((RSTSTAT&0x38)!=0) || (ucResetFlag==0x12))	//POR+LVR+ResetPin+PC instruction
	{
	 	RSTSTAT	= 0x00;
		ucResetFlag = 0;
		
		AllInitial();//ËùÓÐ¶¼¸´Î»
#if 1
		printf("\n ALL init \n");    
#endif	
		
	}
	else			//WDT+OVL¸´Î» ¿´ÃÅ¹·ºÍ³ÌÐòÒç³ö¸´Î»
	{
	 	RSTSTAT	= 0x00;
		PartialInitial();//±äÁ¿ÖµÃ»ÓÐ³õÊ¼»¯
		
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
	IO_PACK_VADC = 0;//packÊä³öµçÑ¹¼ì²â
	IO_SCP_RST = 0;//×Ô¼ºµÄ¶ÌÂ·Çå³ýÈí¼þ
  packfault_flag.pack_flag_all = 0;
//Ôö¼ÓÍâÉè³õÊ¼»¯
//(1)ADC³õÊ¼»¯
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

