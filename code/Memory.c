/********************************************************************************
Copyright (C), Sinowealth Electronic. Ltd.
Author: 	andyliu
Version: 	V0.0
Date: 		2014/05/30
History:
	V0.0		2014/05/30		 Preliminary
********************************************************************************/
#include "system.h"


U8 code JumpISP[5]	={0x5A,0xA5,0x02,0xF4,0x00};        //ISP关键字定义

//*****************************BIT MEMORY START***************************//
U16 bdata uiPackConfig;	
sbit bCellNum1		=	uiPackConfig^8;
sbit bCellNum2		=	uiPackConfig^9;
sbit bCellNum3		=	uiPackConfig^10;	
sbit bLEDNum0		=	uiPackConfig^11;		//00110111
sbit bLEDNum1		=	uiPackConfig^12;
sbit bTempNum		=	uiPackConfig^13;  //1：支持2个温度点检测，0：1个温度点
sbit bCHGEnd		=	uiPackConfig^14; //充电截止关闭充电mosfet
sbit bDSGEnd		=	uiPackConfig^15; //放电截止关闭放电mos

sbit bSC_EN			=	uiPackConfig^0;			//00101011  硬件短路
sbit bOV_EN			=	uiPackConfig^1;			 //硬件过充
sbit bOCRC_EN		=	uiPackConfig^2;			//auto reset or nor when OCC   过流自恢复使能
sbit bBAL_EN		=	uiPackConfig^3; 		//平衡功能使能
sbit bPF_EN			=	uiPackConfig^4;			//disable    二级保护使能
sbit bCTO_EN		=	uiPackConfig^5;			//cell on  断线检测
sbit bOCPM			=	uiPackConfig^6;			//disable   过流同事关闭充放电mos
sbit bEnEEPRomBK	=	uiPackConfig^7;   //支持数据备份功能

U16 bdata uiPackStatus;				
sbit bCHGMOS		=	uiPackStatus^8;			 //充电mos开关 1:开启  							0位
sbit bDSGMOS		=	uiPackStatus^9;			 //放电mos													1
sbit bCHGING		=	uiPackStatus^10; //充电状态															2	
sbit bDSGING		=	uiPackStatus^11; //放电状态															3
sbit bSlowDischarge	=	uiPackStatus^12; //pwm占空比低											4
sbit bMidDischarge	=	uiPackStatus^13; //占空比高													5
sbit bFastDischarge	= 	uiPackStatus^14;//一直高电平，占空比100%					6

sbit bFC	        =	uiPackStatus^0;			 //充电截止标志位									8
sbit bFD	        =	uiPackStatus^1;			//放电截止标志位									9
sbit bVDQ        	=	uiPackStatus^2;			//满充容量更新有效标志位					10
sbit bCAL			=	uiPackStatus^7;  //1:已校准 0：未校准

U16 bdata uiBatStatus;				
sbit bHV			=	uiBatStatus^8;//过冲保护标志位   												0
sbit bUV			=	uiBatStatus^9;//过放保护标志位													1
sbit bOCC 			= 	uiBatStatus^10;//充电过流保护标志位 1发生过保护
sbit bOCD 			= 	uiBatStatus^11;//放电过流保护标志位
//by tomi
sbit bOCD2 			= 	uiBatStatus^12;//放电过流保护标志位

sbit bOTC 			=	uiBatStatus^0;//充电高温保护													8
sbit bOTD 			= 	uiBatStatus^1;//放电高温保护
sbit bUTC			=	uiBatStatus^2;//充电低温保护
sbit bUTD			=	uiBatStatus^3;//放电低温保护
sbit bAFE_OV		=	uiBatStatus^4;//硬件过冲保护
sbit bAFE_SC		=	uiBatStatus^5;//硬件短路保护
sbit bCTO			=	uiBatStatus^6; //预留位，断线保护

BOOL bTimer1sFlg;
BOOL bTimer25msFlg;
BOOL bDataFlashFail;	
BOOL bWriteFlashFlg;
BOOL bUpdataFlashFlg;
BOOL bKeyFlg;
BOOL bCADCFlg;						//CADC转码完成标志
BOOL bVADCFlg;
BOOL bISPFlg;						//ISP升级标志，进入ISP程序
BOOL bPCSleepFlg;					//PC通知系统进入SLEEP标志
BOOL bUartSndAckFlg;				//UART已经发送ACK给主机
BOOL bCalibrationFlg;				//上位机发送校准命令后置位该标志
BOOL bAlarmFlg;						//AFE的ALARM发生标志
BOOL bLEDOpen;						//LED显示标志
BOOL bLEDChgFlg;
BOOL bBLEOPEN;
BOOL bE2RTCON;
U8 data Timer3cnt;

BOOL bE2PProcessFlg;				//EEPROM处理标志
BOOL bIdleFlg;						//系统进入IDLE标志

BOOL bDsgToChgFlg;					//放电转换为充电，需要备份数据
BOOL bChgToDsgFlg;					//充电转换为放电，需要备份数据
BOOL bE2PBKDsgEnd;					//放电结束标志，需要备份信息到外挂EEPROM
BOOL bE2PBKDsgEndValid;
BOOL bE2PBKChgStop;					//充电结束标志，需要备份信息到外挂EEPROM
BOOL bE2PBKChgStart;				//充电开始标志，需要备份信息到外挂EEPROM
BOOL bE2PBKRtc;						//RTC定时备份标志，需要备份信息到外挂EEPROM
BOOL bE2PErase;						//擦除外挂EEPROM标志
BOOL bE2PRdData;					//读取外挂EEPROM标志
BOOL bRTCRdTime;					//读取RTC时间标志

//*****************************DATA MEMORY START***************************//
U8 idata STACK[0x100-STACK_ADDR]  	_at_	STACK_ADDR;		//堆栈

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
BOOL		bCellOpenDecFlag;//开断断线检测标志位
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
//DataFlash中区块地址定义
/**************************************************************************************/
//系统信息区开始 SubClassID=0x00		langth=48
U16 xdata E2uiPackConfigMap		_at_ 	SYS_PARA_MAP_ADDR; 
U16 xdata	E2uiVOC[10]             _at_ 	SYS_PARA_MAP_ADDR+2;//电池包电量10~100%对应的电压值mv
U32 xdata	E2ulDesignCapacity	    _at_ 	SYS_PARA_MAP_ADDR+22;
U32 xdata	E2ulFCC							_at_ 	SYS_PARA_MAP_ADDR+26; //系统满容量（mAh）2200
U32 xdata	E2ulCycleThreshold		_at_ 	SYS_PARA_MAP_ADDR+30;//系统单词循环放电总量
U16 xdata	E2uiCycleCount			_at_ 	SYS_PARA_MAP_ADDR+34;
U16 xdata	E2uiLearnLowTemp		_at_ 	SYS_PARA_MAP_ADDR+36;
U16 xdata	E2uiNearFCC				_at_ 	SYS_PARA_MAP_ADDR+38;
S16 xdata	E2siDfilterCur			_at_ 	SYS_PARA_MAP_ADDR+40;
U8  xdata	E2ucDeleyLowPower       _at_ 	SYS_PARA_MAP_ADDR+42;
U8  xdata	E2ucChgBKDelay			_at_	SYS_PARA_MAP_ADDR+43;
S16 xdata	E2uiChgBKCur			_at_	SYS_PARA_MAP_ADDR+44;
U8  xdata	E2ucRTCBKDelay			_at_	SYS_PARA_MAP_ADDR+46;
U8  xdata	ucRamCheckFlg1			_at_	SYS_PARA_MAP_ADDR+47;

//用户自定义参数区开始 SubClassID=0x01	langth=50
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

//充电参数区开始 SubClassID=0x02		langth=18		
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

//放电参数区开始 SubClassID=0x03		langth=21
U16 xdata  	E2uiUVvol          		_at_ 	DSG_PARA_MAP_ADDR;
U16 xdata  	E2uiUVRvol          	_at_ 	DSG_PARA_MAP_ADDR+2;
U8  xdata  	E2ucDelayUV        		_at_ 	DSG_PARA_MAP_ADDR+4;					  // all the delay unit is 500mS
U8  xdata  	E2ucDelayUVR        	_at_ 	DSG_PARA_MAP_ADDR+5;
U16 xdata  	E2uiDsgEndVol       	_at_ 	DSG_PARA_MAP_ADDR+6;//放电截止电压
U8  xdata  	E2ucDelayDsgEnd     	_at_ 	DSG_PARA_MAP_ADDR+8;//放电截止延时
S32 xdata		E2slOCDvol		  	 	_at_ 	DSG_PARA_MAP_ADDR+9;//放电过流1保护电流
U8	xdata		E2ucDelayOCD	 	  	_at_ 	DSG_PARA_MAP_ADDR+13;
S32 xdata		E2slOCD2vol		  	 	_at_ 	DSG_PARA_MAP_ADDR+14;
U8  xdata		E2ucDelayOCD2	 	  	_at_ 	DSG_PARA_MAP_ADDR+18;
U8  xdata 	E2ucDelayLoadR	 	  	_at_ 	DSG_PARA_MAP_ADDR+19;
U8  xdata		ucRamCheckFlg4			_at_	DSG_PARA_MAP_ADDR+20;

//放电PWM参数区开始 SubClassID=0x04		langth=5
U16 xdata 	E2uiDSG1PWMFreq	   		_at_ 	DSG_PWM_PARA_MAP_ADDR; 
U8  xdata  	E2ucDSG1PWMRatioL   	_at_ 	DSG_PWM_PARA_MAP_ADDR+2;			
U8  xdata  	E2ucDSG1PWMRatioH   	_at_ 	DSG_PWM_PARA_MAP_ADDR+3;		
U8  xdata		ucRamCheckFlg5			_at_ 	DSG_PWM_PARA_MAP_ADDR+4;

//充电温度保护参数开始 SubClassID=0x05	langth=11
U16 xdata  	E2uiTempOTC      		_at_ 	CHG_TEMP_PARA_MAP_ADDR;
U16 xdata  	E2uiTempOTCR      		_at_ 	CHG_TEMP_PARA_MAP_ADDR+2;
U16 xdata  	E2uiTempUTC        	    _at_ 	CHG_TEMP_PARA_MAP_ADDR+4;
U16 xdata  	E2uiTempUTCR       	    _at_ 	CHG_TEMP_PARA_MAP_ADDR+6;
U8  xdata  	E2ucDelayTemp      		_at_ 	CHG_TEMP_PARA_MAP_ADDR+8; //CHG_TEMP_PARA_MAP_ADDR+8;	3
U8  xdata  	E2ucDelayTempR	      	_at_ 	CHG_TEMP_PARA_MAP_ADDR+9;
U8  xdata		ucRamCheckFlg6			_at_ 	CHG_TEMP_PARA_MAP_ADDR+10;

//放电温度保护参数开始 SubClassID=0x06	langth=9
U16 xdata  	E2uiTempOTD        		_at_ 	DSG_TEMP_PARA_MAP_ADDR;
U16 xdata  	E2uiTempOTDR       		_at_ 	DSG_TEMP_PARA_MAP_ADDR+2;
U16 xdata  	E2uiTempUTD        		_at_ 	DSG_TEMP_PARA_MAP_ADDR+4;
U16 xdata  	E2uiTempUTDR       		_at_ 	DSG_TEMP_PARA_MAP_ADDR+6;
U8  xdata		ucRamCheckFlg7			_at_ 	DSG_TEMP_PARA_MAP_ADDR+8;

//平衡参数区开始 SubClassID=0x08		langth=8
U16 xdata	E2uiBalanceVol      	_at_	BAL_PARA_MAP_ADDR;		
U16 xdata	E2uiBalanceVolDiff  	_at_	BAL_PARA_MAP_ADDR+2;		
S16 xdata	E2siBalCurrent			_at_	BAL_PARA_MAP_ADDR+4;	
U8	xdata	E2ucBalanceDelay    	_at_	BAL_PARA_MAP_ADDR+6;	
U8  xdata	ucRamCheckFlg8			_at_ 	BAL_PARA_MAP_ADDR+7;

//AFE参数区开始 SubClassID=0x0A			langth=4
U8	xdata	E2ucAFEProtectConfig	_at_	AFE_PARA_MAP_ADDR;
U16 xdata	E2uiAFEOVvol			_at_	AFE_PARA_MAP_ADDR+1;
U8  xdata	ucRamCheckFlg9			_at_ 	AFE_PARA_MAP_ADDR+3;

//校准参数区开始 SubClassID=0x0B		langth=12
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
U8 xdata 	ConectFlg_poron;//充电器连接标志
U8 xdata 	charge_detect_cnt;//充电器连接标�
U8 xdata  CHGING_cnt;
U8 xdata  packfault_cnt;
U8 xdata  packfault_i;
U8 xdata  fault_cntoiv;