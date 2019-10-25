/********************************************************************************
Copyright (C), Sinowealth Electronic. Ltd.
Author: 	andyliu
Version: 	V0.0
Date: 		2014/05/30
History:
	V0.0		2014/05/30		 Preliminary
********************************************************************************/
#include "system.h"


U8 code JumpISP[5]	={0x5A,0xA5,0x02,0xF4,0x00};        //ISP¹Ø¼ü×Ö¶¨Òå

//*****************************BIT MEMORY START***************************//
U16 bdata uiPackConfig;	
sbit bCellNum1		=	uiPackConfig^8;
sbit bCellNum2		=	uiPackConfig^9;
sbit bCellNum3		=	uiPackConfig^10;	
sbit bLEDNum0		=	uiPackConfig^11;		//00110111
sbit bLEDNum1		=	uiPackConfig^12;
sbit bTempNum		=	uiPackConfig^13;  //1£ºÖ§³Ö2¸öÎÂ¶Èµã¼ì²â£¬0£º1¸öÎÂ¶Èµã
sbit bCHGEnd		=	uiPackConfig^14; //³äµç½ØÖ¹¹Ø±Õ³äµçmosfet
sbit bDSGEnd		=	uiPackConfig^15; //·Åµç½ØÖ¹¹Ø±Õ·Åµçmos

sbit bSC_EN			=	uiPackConfig^0;			//00101011  Ó²¼þ¶ÌÂ·
sbit bOV_EN			=	uiPackConfig^1;			 //Ó²¼þ¹ý³ä
sbit bOCRC_EN		=	uiPackConfig^2;			//auto reset or nor when OCC   ¹ýÁ÷×Ô»Ö¸´Ê¹ÄÜ
sbit bBAL_EN		=	uiPackConfig^3; 		//Æ½ºâ¹¦ÄÜÊ¹ÄÜ
sbit bPF_EN			=	uiPackConfig^4;			//disable    ¶þ¼¶±£»¤Ê¹ÄÜ
sbit bCTO_EN		=	uiPackConfig^5;			//cell on  ¶ÏÏß¼ì²â
sbit bOCPM			=	uiPackConfig^6;			//disable   ¹ýÁ÷Í¬ÊÂ¹Ø±Õ³ä·Åµçmos
sbit bEnEEPRomBK	=	uiPackConfig^7;   //Ö§³ÖÊý¾Ý±¸·Ý¹¦ÄÜ

U16 bdata uiPackStatus;				
sbit bCHGMOS		=	uiPackStatus^8;			 //³äµçmos¿ª¹Ø 1:¿ªÆô  							0Î»
sbit bDSGMOS		=	uiPackStatus^9;			 //·Åµçmos													1
sbit bCHGING		=	uiPackStatus^10; //³äµç×´Ì¬															2	
sbit bDSGING		=	uiPackStatus^11; //·Åµç×´Ì¬															3
sbit bSlowDischarge	=	uiPackStatus^12; //pwmÕ¼¿Õ±ÈµÍ											4
sbit bMidDischarge	=	uiPackStatus^13; //Õ¼¿Õ±È¸ß													5
sbit bFastDischarge	= 	uiPackStatus^14;//Ò»Ö±¸ßµçÆ½£¬Õ¼¿Õ±È100%					6

sbit bFC	        =	uiPackStatus^0;			 //³äµç½ØÖ¹±êÖ¾Î»									8
sbit bFD	        =	uiPackStatus^1;			//·Åµç½ØÖ¹±êÖ¾Î»									9
sbit bVDQ        	=	uiPackStatus^2;			//Âú³äÈÝÁ¿¸üÐÂÓÐÐ§±êÖ¾Î»					10
sbit bCAL			=	uiPackStatus^7;  //1:ÒÑÐ£×¼ 0£ºÎ´Ð£×¼

U16 bdata uiBatStatus;				
sbit bHV			=	uiBatStatus^8;//¹ý³å±£»¤±êÖ¾Î»   												0
sbit bUV			=	uiBatStatus^9;//¹ý·Å±£»¤±êÖ¾Î»													1
sbit bOCC 			= 	uiBatStatus^10;//³äµç¹ýÁ÷±£»¤±êÖ¾Î» 1·¢Éú¹ý±£»¤
sbit bOCD 			= 	uiBatStatus^11;//·Åµç¹ýÁ÷±£»¤±êÖ¾Î»
//by tomi
sbit bOCD2 			= 	uiBatStatus^12;//·Åµç¹ýÁ÷±£»¤±êÖ¾Î»

sbit bOTC 			=	uiBatStatus^0;//³äµç¸ßÎÂ±£»¤													8
sbit bOTD 			= 	uiBatStatus^1;//·Åµç¸ßÎÂ±£»¤
sbit bUTC			=	uiBatStatus^2;//³äµçµÍÎÂ±£»¤
sbit bUTD			=	uiBatStatus^3;//·ÅµçµÍÎÂ±£»¤
sbit bAFE_OV		=	uiBatStatus^4;//Ó²¼þ¹ý³å±£»¤
sbit bAFE_SC		=	uiBatStatus^5;//Ó²¼þ¶ÌÂ·±£»¤
sbit bCTO			=	uiBatStatus^6; //Ô¤ÁôÎ»£¬¶ÏÏß±£»¤

BOOL bTimer1sFlg;
BOOL bTimer25msFlg;
BOOL bDataFlashFail;	
BOOL bWriteFlashFlg;
BOOL bUpdataFlashFlg;
BOOL bKeyFlg;
BOOL bCADCFlg;						//CADC×ªÂëÍê³É±êÖ¾
BOOL bVADCFlg;
BOOL bISPFlg;						//ISPÉý¼¶±êÖ¾£¬½øÈëISP³ÌÐò
BOOL bPCSleepFlg;					//PCÍ¨ÖªÏµÍ³½øÈëSLEEP±êÖ¾
BOOL bUartSndAckFlg;				//UARTÒÑ¾­·¢ËÍACK¸øÖ÷»ú
BOOL bCalibrationFlg;				//ÉÏÎ»»ú·¢ËÍÐ£×¼ÃüÁîºóÖÃÎ»¸Ã±êÖ¾
BOOL bAlarmFlg;						//AFEµÄALARM·¢Éú±êÖ¾
BOOL bLEDOpen;						//LEDÏÔÊ¾±êÖ¾
BOOL bLEDChgFlg;
BOOL bBLEOPEN;
BOOL bE2RTCON;
U8 data Timer3cnt;

BOOL bE2PProcessFlg;				//EEPROM´¦Àí±êÖ¾
BOOL bIdleFlg;						//ÏµÍ³½øÈëIDLE±êÖ¾

BOOL bDsgToChgFlg;					//·Åµç×ª»»Îª³äµç£¬ÐèÒª±¸·ÝÊý¾Ý
BOOL bChgToDsgFlg;					//³äµç×ª»»Îª·Åµç£¬ÐèÒª±¸·ÝÊý¾Ý
BOOL bE2PBKDsgEnd;					//·Åµç½áÊø±êÖ¾£¬ÐèÒª±¸·ÝÐÅÏ¢µ½Íâ¹ÒEEPROM
BOOL bE2PBKDsgEndValid;
BOOL bE2PBKChgStop;					//³äµç½áÊø±êÖ¾£¬ÐèÒª±¸·ÝÐÅÏ¢µ½Íâ¹ÒEEPROM
BOOL bE2PBKChgStart;				//³äµç¿ªÊ¼±êÖ¾£¬ÐèÒª±¸·ÝÐÅÏ¢µ½Íâ¹ÒEEPROM
BOOL bE2PBKRtc;						//RTC¶¨Ê±±¸·Ý±êÖ¾£¬ÐèÒª±¸·ÝÐÅÏ¢µ½Íâ¹ÒEEPROM
BOOL bE2PErase;						//²Á³ýÍâ¹ÒEEPROM±êÖ¾
BOOL bE2PRdData;					//¶ÁÈ¡Íâ¹ÒEEPROM±êÖ¾
BOOL bRTCRdTime;					//¶ÁÈ¡RTCÊ±¼ä±êÖ¾

//*****************************DATA MEMORY START***************************//
U8 idata STACK[0x100-STACK_ADDR]  	_at_	STACK_ADDR;		//¶ÑÕ»

U8	data	ucCellNum;
U16	data	uiCellVmax;
U16	data	uiCellVmin;
U16	data	uiVoltageMin;
U8	xdata	ucBalanceTimeCnt;
U16	xdata	uiBalanceChannel;
U8	xdata   AFESCONF2Bk;


//Check Load and Charger
BOOL bChkLoadFlg;
BOOL bChkChgerFlg;
BOOL bChkChgerRFlg;
BOOL bLoadConectFlg;
BOOL bChgerConectFlg;
BOOL bUVBkFlg;
U8  xdata   ucLoadRCnt;
U8  xdata   ucChgerRCnt;
U8  xdata   ucChgerCnt;
U8  xdata   ucLedChgCnt;
U8  xdata   ucLedFlickCnt;
U8  xdata   ucBLEFlickCnt;

//ProtectCnt
U8	xdata	ucOVcnt			_at_	ProtectCnt;
U8	xdata  	ucOVRcnt		_at_	ProtectCnt+1;
U8	xdata  	ucUVcnt			_at_	ProtectCnt+2;
U8	xdata  	ucUVRcnt		_at_	ProtectCnt+3;
U8	xdata  	ucChgEndcnt		_at_	ProtectCnt+4;
U8	xdata  	ucChgEndRcnt	_at_	ProtectCnt+5;
U8	xdata  	ucDsgEndcnt		_at_	ProtectCnt+6;
U8	xdata  	ucDsgEndRcnt	_at_	ProtectCnt+7;
U8	xdata  	ucOTCcnt		_at_	ProtectCnt+8;
U8	xdata  	ucOTCRcnt		_at_	ProtectCnt+9;
U8	xdata  	ucUTCcnt		_at_	ProtectCnt+10;
U8	xdata  	ucUTCRcnt	    _at_	ProtectCnt+11;
U8	xdata  	ucOTDcnt		_at_	ProtectCnt+12;
U8	xdata  	ucOTDRcnt		_at_	ProtectCnt+13;
U8	xdata  	ucUTDcnt		_at_	ProtectCnt+14;
U8	xdata  	ucUTDRcnt		_at_	ProtectCnt+15;

//Flash 
U8	xdata	ucFlashWrValid;
U8	xdata	ucWriteFlashCnt;

//UART
BOOL		bUartReadFlg;
BOOL		bUartWriteFlg;
BOOL 		bUartNeedAckFlg;             //UART Need to send ACK flag
U8	xdata	ucSubClassID;

//System low power
BOOL		bIdleFlg;
BOOL 		bPDFlg;
BOOL		bWakeupFlg;
U8	xdata	ucIdleTimerCnt;
U8	xdata	ucPDTimerCnt;
U8  xdata	ucResetFlag; 
U8  data	ucTimer3Cnt;

//Calibrate
U16	xdata	ulExtVPack;
U8  xdata	ucExtcaliSwitch1;
U8  xdata 	ucExtcaliFlag;				//calibration flag
S32 xdata	slExtCur;
U16 xdata	uiExtTemp1;
U16 xdata	uiExtTemp2;

//Gauge
U16 xdata	uiRCCharge;					//Charge capacity statistics
U16 xdata	uiRCDischarge;				//Discharge capacity statistics
U32 xdata	ulDsgCycleCount;			//Discharge capacity statistics, for update E2uiCycleCount
U32 xdata	ulFCCCount;					//The effective discharge capacity statistics, for updating E2ulFCC
//BOOL bVDQFlag;


//CADC
U8	xdata	ucOCCcnt;
U8	xdata	ucOCDcnt;
//by tomi
U8	xdata	ucOCD2cnt;

U16	xdata	uiOCCRcnt;
U16 xdata	uiOCDRcnt;

U8  xdata 	ucCadcCnt;
S32 xdata 	slCadcCurBuf[16];

U8	data	ucCellNumOffset;
U8	xdata 	ucTempeMiddle;

//Balance
BOOL		bBalanceFlg;
BOOL		bBalanceOddFlg;
U16 xdata	uiBalanceChannel;
//U8	xdata	ucBalanceCnt;

//KEY
U8	data 	ucKeyValidCnt;
U8	data 	ucKeyInvalidCnt;
U8	data 	ucKeyValue;
U8	data 	ucKeyOldValue;
U8	data 	ucKeyValueBK;

U8	xdata	ucLEDTimeCnt;
S32 xdata   slCadcCurrent;
S32 xdata   slCadcCurAverage;
U8  xdata   ucDsgingSpeed;

U16 xdata	uiTempeMin;
U16 xdata	uiTempeMax;

//Cell tap open 
BOOL		bCellOpenDecFlag;//¿ª¶Ï¶ÏÏß¼ì²â±êÖ¾Î»
BOOL		bBalancingFlg;
U8 idata	ucCellOpenDecCnt;
U8 idata	ucBalanceStep;
U8 idata	ucCtoCnt;

//Charge/Discharge Management
SYSINFOR xdata Info;					//System information, For PC display
AFEDATA  xdata AFE;						//Used to store AFE ADC Data collection
AFEREG   xdata REG;						//Used to set AFE register and read AFE register

RTC_VAR xdata RTC;						//For External RTC
U16 xdata	uiE2PDataAddr;
U8  xdata	ucRTCBKTime1;
U16 xdata	uiRTCBKTime2;
U8  xdata	ucRTCBuf[7];
U8  idata	ucUpDataLimitTime;
U8  xdata	ucExtRTC[6];				//During calibration, the received RTC Time

U16	xdata uiCHGValidTime;

/**************************************************************************************/
//DataFlashÖÐÇø¿éµØÖ·¶¨Òå
/**************************************************************************************/
//ÏµÍ³ÐÅÏ¢Çø¿ªÊ¼ SubClassID=0x00		langth=48
U16 xdata E2uiPackConfigMap		_at_ 	SYS_PARA_MAP_ADDR; 
U16 xdata	E2uiVOC[10]             _at_ 	SYS_PARA_MAP_ADDR+2;//µç³Ø°üµçÁ¿10~100%¶ÔÓ¦µÄµçÑ¹Öµmv
U32 xdata	E2ulDesignCapacity	    _at_ 	SYS_PARA_MAP_ADDR+22;
U32 xdata	E2ulFCC							_at_ 	SYS_PARA_MAP_ADDR+26; //ÏµÍ³ÂúÈÝÁ¿£¨mAh£©2200
U32 xdata	E2ulCycleThreshold		_at_ 	SYS_PARA_MAP_ADDR+30;//ÏµÍ³µ¥´ÊÑ­»··Åµç×ÜÁ¿
U16 xdata	E2uiCycleCount			_at_ 	SYS_PARA_MAP_ADDR+34;
U16 xdata	E2uiLearnLowTemp		_at_ 	SYS_PARA_MAP_ADDR+36;
U16 xdata	E2uiNearFCC				_at_ 	SYS_PARA_MAP_ADDR+38;
S16 xdata	E2siDfilterCur			_at_ 	SYS_PARA_MAP_ADDR+40;
U8  xdata	E2ucDeleyLowPower       _at_ 	SYS_PARA_MAP_ADDR+42;
U8  xdata	E2ucChgBKDelay			_at_	SYS_PARA_MAP_ADDR+43;
S16 xdata	E2uiChgBKCur			_at_	SYS_PARA_MAP_ADDR+44;
U8  xdata	E2ucRTCBKDelay			_at_	SYS_PARA_MAP_ADDR+46;
U8  xdata	ucRamCheckFlg1			_at_	SYS_PARA_MAP_ADDR+47;

//ÓÃ»§×Ô¶¨Òå²ÎÊýÇø¿ªÊ¼ SubClassID=0x01	langth=50
U16 xdata	E2uiSWVersion       	_at_ 	SYSINFO_MAP_ADDR;			
U16 xdata	E2uiHWVersion       	_at_ 	SYSINFO_MAP_ADDR+2;			
U8  xdata	E2ucID              	_at_ 	SYSINFO_MAP_ADDR+4;				
U8  xdata	E2ucMNFName[12]     	_at_ 	SYSINFO_MAP_ADDR+5;			
U32 xdata	E2ulMNFDate				_at_ 	SYSINFO_MAP_ADDR+17;					
U16 xdata	E2uiSerialNum    		_at_ 	SYSINFO_MAP_ADDR+21;			
U8  xdata	E2ucDeviceName[12]  	_at_ 	SYSINFO_MAP_ADDR+23;	
U8  xdata	E2ucDeviceChem[12]  	_at_ 	SYSINFO_MAP_ADDR+35;		
U16 xdata	E2uiChemID          	_at_ 	SYSINFO_MAP_ADDR+47;
U8  xdata	ucRamCheckFlg2			_at_	SYSINFO_MAP_ADDR+49;

//³äµç²ÎÊýÇø¿ªÊ¼ SubClassID=0x02		langth=18		
U16 xdata  	E2uiOVvol				_at_ 	CHG_PARA_MAP_ADDR;
U16 xdata  	E2uiOVRvol         		_at_ 	CHG_PARA_MAP_ADDR+2;
U8  xdata  	E2ucDelayOV       		_at_ 	CHG_PARA_MAP_ADDR+4;
U8  xdata  	E2ucDelayOVR    		_at_ 	CHG_PARA_MAP_ADDR+5;
U16 xdata  	E2uiChgEndVol   	    _at_ 	CHG_PARA_MAP_ADDR+6;
S16 xdata		E2siChgEndCur	      	_at_ 	CHG_PARA_MAP_ADDR+8;
U8  xdata  	E2ucDelayChgEnd   		_at_ 	CHG_PARA_MAP_ADDR+10;
S32 xdata		E2slOCCvol 				_at_ 	CHG_PARA_MAP_ADDR+11;
U8  xdata   E2ucDelayOCC			_at_ 	CHG_PARA_MAP_ADDR+15;
U8  xdata  	E2ucDelayOCCR			_at_ 	CHG_PARA_MAP_ADDR+16;
U8  xdata		ucRamCheckFlg3			_at_	CHG_PARA_MAP_ADDR+17;

//·Åµç²ÎÊýÇø¿ªÊ¼ SubClassID=0x03		langth=21
U16 xdata  	E2uiUVvol          		_at_ 	DSG_PARA_MAP_ADDR;
U16 xdata  	E2uiUVRvol          	_at_ 	DSG_PARA_MAP_ADDR+2;
U8  xdata  	E2ucDelayUV        		_at_ 	DSG_PARA_MAP_ADDR+4;					  // all the delay unit is 500mS
U8  xdata  	E2ucDelayUVR        	_at_ 	DSG_PARA_MAP_ADDR+5;
U16 xdata  	E2uiDsgEndVol       	_at_ 	DSG_PARA_MAP_ADDR+6;//·Åµç½ØÖ¹µçÑ¹
U8  xdata  	E2ucDelayDsgEnd     	_at_ 	DSG_PARA_MAP_ADDR+8;//·Åµç½ØÖ¹ÑÓÊ±
S32 xdata		E2slOCDvol		  	 	_at_ 	DSG_PARA_MAP_ADDR+9;//·Åµç¹ýÁ÷1±£»¤µçÁ÷
U8	xdata		E2ucDelayOCD	 	  	_at_ 	DSG_PARA_MAP_ADDR+13;
S32 xdata		E2slOCD2vol		  	 	_at_ 	DSG_PARA_MAP_ADDR+14;
U8  xdata		E2ucDelayOCD2	 	  	_at_ 	DSG_PARA_MAP_ADDR+18;
U8  xdata 	E2ucDelayLoadR	 	  	_at_ 	DSG_PARA_MAP_ADDR+19;
U8  xdata		ucRamCheckFlg4			_at_	DSG_PARA_MAP_ADDR+20;

//·ÅµçPWM²ÎÊýÇø¿ªÊ¼ SubClassID=0x04		langth=5
U16 xdata 	E2uiDSG1PWMFreq	   		_at_ 	DSG_PWM_PARA_MAP_ADDR; 
U8  xdata  	E2ucDSG1PWMRatioL   	_at_ 	DSG_PWM_PARA_MAP_ADDR+2;			
U8  xdata  	E2ucDSG1PWMRatioH   	_at_ 	DSG_PWM_PARA_MAP_ADDR+3;		
U8  xdata		ucRamCheckFlg5			_at_ 	DSG_PWM_PARA_MAP_ADDR+4;

//³äµçÎÂ¶È±£»¤²ÎÊý¿ªÊ¼ SubClassID=0x05	langth=11
U16 xdata  	E2uiTempOTC      		_at_ 	CHG_TEMP_PARA_MAP_ADDR;
U16 xdata  	E2uiTempOTCR      		_at_ 	CHG_TEMP_PARA_MAP_ADDR+2;
U16 xdata  	E2uiTempUTC        	    _at_ 	CHG_TEMP_PARA_MAP_ADDR+4;
U16 xdata  	E2uiTempUTCR       	    _at_ 	CHG_TEMP_PARA_MAP_ADDR+6;
U8  xdata  	E2ucDelayTemp      		_at_ 	CHG_TEMP_PARA_MAP_ADDR+8; //CHG_TEMP_PARA_MAP_ADDR+8;	3
U8  xdata  	E2ucDelayTempR	      	_at_ 	CHG_TEMP_PARA_MAP_ADDR+9;
U8  xdata		ucRamCheckFlg6			_at_ 	CHG_TEMP_PARA_MAP_ADDR+10;

//·ÅµçÎÂ¶È±£»¤²ÎÊý¿ªÊ¼ SubClassID=0x06	langth=9
U16 xdata  	E2uiTempOTD        		_at_ 	DSG_TEMP_PARA_MAP_ADDR;
U16 xdata  	E2uiTempOTDR       		_at_ 	DSG_TEMP_PARA_MAP_ADDR+2;
U16 xdata  	E2uiTempUTD        		_at_ 	DSG_TEMP_PARA_MAP_ADDR+4;
U16 xdata  	E2uiTempUTDR       		_at_ 	DSG_TEMP_PARA_MAP_ADDR+6;
U8  xdata		ucRamCheckFlg7			_at_ 	DSG_TEMP_PARA_MAP_ADDR+8;

//Æ½ºâ²ÎÊýÇø¿ªÊ¼ SubClassID=0x08		langth=8
U16 xdata	E2uiBalanceVol      	_at_	BAL_PARA_MAP_ADDR;		
U16 xdata	E2uiBalanceVolDiff  	_at_	BAL_PARA_MAP_ADDR+2;		
S16 xdata	E2siBalCurrent			_at_	BAL_PARA_MAP_ADDR+4;	
U8	xdata	E2ucBalanceDelay    	_at_	BAL_PARA_MAP_ADDR+6;	
U8  xdata	ucRamCheckFlg8			_at_ 	BAL_PARA_MAP_ADDR+7;

//AFE²ÎÊýÇø¿ªÊ¼ SubClassID=0x0A			langth=4
U8	xdata	E2ucAFEProtectConfig	_at_	AFE_PARA_MAP_ADDR;
U16 xdata	E2uiAFEOVvol			_at_	AFE_PARA_MAP_ADDR+1;
U8  xdata	ucRamCheckFlg9			_at_ 	AFE_PARA_MAP_ADDR+3;

//Ð£×¼²ÎÊýÇø¿ªÊ¼ SubClassID=0x0B		langth=12
U16 xdata 	E2uiVPackGain			_at_	CALI_PARA_MAP_ADDR;		
S16 xdata 	E2siCadcGain			_at_ 	CALI_PARA_MAP_ADDR+2;
S16 xdata 	E2siCadcOffset			_at_	CALI_PARA_MAP_ADDR+4;				
S16 xdata 	E2siTS1Offset			_at_	CALI_PARA_MAP_ADDR+6;
S16 xdata 	E2siTS2Offset			_at_	CALI_PARA_MAP_ADDR+8;
U8  xdata		E2ucCalibrated			_at_	CALI_PARA_MAP_ADDR+10;
U8  xdata		ucRamCheckFlg10			_at_	CALI_PARA_MAP_ADDR+11;

U8  xdata   Reserved[RESERVED_DATA_LEN]     _at_    RESERVED_DATA_MAP_ADDR;

//DataflashCheck
U16 xdata		E2uiCheckFlag			_at_    DATAFLASH_MAP_ADDR+510;		


U8 xdata	ucUartBuf[150] 			_at_ 0x400;			//For UART transmit or acceptance buffer
U8 xdata	ucUartBufPT 			_at_ 0x400+150;		//Pointing to the current UART Buffer
U8 xdata	ucUartSndLength 		_at_ 0x400+151;		//UART Buffer send length
U8 xdata	ucUartTimeoutCnt 		_at_ 0x400+152;		//UART timerout cnt, If not Uart communication within 2s, then clear ucUartBufPT




//by tomi 
U8 xdata	ConectFlg_poron_prev;
U8 xdata	LoadConnet_FLG;
U8 xdata 	ConectFlg_poron;//³äµçÆ÷Á¬½Ó±êÖ¾
U8 xdata 	charge_detect_cnt;//³äµçÆ÷Á¬½Ó±êÖ
U8 xdata  CHGING_cnt;
U8 xdata  packfault_cnt;
U8 xdata  packfault_i;
U8 xdata  fault_cntoiv;