/********************************************************************************
Copyright (C), Sinowealth Electronic. Ltd.
Author: 	andyliu
Version: 	V0.0
Date: 		2014/05/30
History:
	V0.0		2014/05/30		 Preliminary
********************************************************************************/
#include "system.h"
#include "ISP.h"
#include "stdio.h"
#include "string.h"
bit tomi_rx_handle(void);
/*******************************************************************************
Function: Page1WrRdFuncTable()
Description:
Input:  NULL	
Output: NULL
Others: NULL
*******************************************************************************/
U16 code Page1WrRdFuncTable[18]=
{
    SYS_PARA_MAP_ADDR,               //SubClassID 0x00
    SYSINFO_MAP_ADDR,               //0x01
    CHG_PARA_MAP_ADDR,               //0x02
    DSG_PARA_MAP_ADDR,               //0x03
    0x00,
    DSG_PWM_PARA_MAP_ADDR,            //0x04
    CHG_TEMP_PARA_MAP_ADDR,           //0x05
    DSG_TEMP_PARA_MAP_ADDR,           //0x06                           //0x07
    BAL_PARA_MAP_ADDR,               //0x08
    0x00,                           //0x09
    AFE_PARA_MAP_ADDR,               //0x0A
    CALI_PARA_MAP_ADDR,              //0x0B
    DataflashCheck_Map_ADDR,        //0x0C
    0x00,                           //0x0D
    0x00,                           //0x0E
    0x00,                           //0x0F
    0x00,                           //0x10
    0x00,                           //0x11
};
    
/*******************************************************************************
Function: Page2WrRdFuncTable()
Description:
Input:  NULL	
Output: NULL
Others: NULL
*******************************************************************************/
U16 code Page2WrRdFuncTable[18]=
{
    SYS_PARA_MAP_ADDR+32,            //SubClassID 0x00
    SYSINFO_MAP_ADDR+32,            //0x01
    CHG_PARA_MAP_ADDR+32,            //0x02
    DSG_PARA_MAP_ADDR+32,            //0x03
	0x00,
    DSG_PWM_PARA_MAP_ADDR+32,         //0x04
    CHG_TEMP_PARA_MAP_ADDR+32,        //0x05
    DSG_TEMP_PARA_MAP_ADDR+32,        //0x06                           //0x07
    BAL_PARA_MAP_ADDR+32,            //0x08
    0x00,                           //0x09
    AFE_PARA_MAP_ADDR+32,            //0x0A
    CALI_PARA_MAP_ADDR+32,           //0x0B
    DataflashCheck_Map_ADDR+32,     //0x0C
    0x00,                           //0x0D
    0x00,                           //0x0E
    0x00,                           //0x0F
    0x00,                           //0x10
    0x00,                           //0x11
};

/*******************************************************************************
Function: InitUART0()
Description: Init UART1  
Input:  NULL
Output: NULL
Others:
*******************************************************************************/
void InitUART0(void)
{
  
  UART0CR = 0x54; //tx和rx的引脚映射配置
									//0011 0010 -->   0101 0100
									//tx：011映射到p0.7  -->  P2.5:101
								//  rx：010 映射到 p0.6  --> P2.4:100
	
	PCON = 0x00;	// [7]SMOD 0:方式2中，波特率位系统时钟的1/64
								// [6]SSTA 0:设置为sm0，sm1，sm2
	SCON = 0x50;	//0101 0000
								//配置Uart工作在模式1 [7:6]   00 :方式0 同步方式，固定波特率 SSTAT = 0
								//													  01 :方式1 8位异步方式，可变波特率
								//										[5]     0:在方式1下，禁止停止位确认检验 ，任何停止位都会置位RI
								//										[4] REN 1  0:接收禁止， 1：接收允许
	SBRTH = 0xff;		//配置波特率参数，波特率9600
	SBRTL = 0x64;		//计算公式：(int)X=FSY/(16*波特率) ;  SBRT=32768-X  ;   SFINE=（FSY/波特率）-16*X   FSY=24M
	SFINE = 0x04;   //波特率发生器微调
	SADDR = 0x0A;   //Euart0的从机地址
	SADEN = 0xFF;


	ucUartBufPT = 0;
	ucUartTimeoutCnt = 0;
}


/*******************************************************************************
Function:ISPHandshake() 
Description:  Processing ISP Handshake
Input:	 NULL
Output: NULL
Others:
*******************************************************************************/
//U8 code ISPCODE[8] = {0x05,0x0a,0x09,0x06,'I','S','P'};
U8 code ISPCODE[10] = {0,7,0x05,0x0a,0x09,0x06,'I','S','P'};
void ISPHandshake(void)
{
	U8 i;
	
	for(i=0; i<9; i++)
	{
		if(ucUartBuf[2+i] != ISPCODE[i])
		{
			UART_SEND_NACK;
			return;                  
		}
	}
	bISPFlg = 1;
	bUartSndAckFlg = 0;
	UART_SEND_ACK;
}


/*******************************************************************************
Function:UartHandshake() 
Description:  PC and slave handshake
Input:  NULL
Output: NULL
Others:
*******************************************************************************/
void UartHandshake(void)
{
	if(ucUartBuf[2] == 0x00)        //Testing equipment is properly
	{
		UART_SEND_ACK;
	}
	else
	{
		UART_SEND_NACK;
	}
}


 /*******************************************************************************
Function:UartWriteInfo() 
Description:  write the information: MCU Info
Input:  *prt---Data storage address
Output: NULL
Others:
*******************************************************************************/
void UartWriteInfo(U8 xdata *ptr)                   //PC write to MCU
{
	U8 i;
	
	if(ucUartBuf[3+ucUartBuf[UART_LENGTH]] == CRC8cal(&ucUartBuf, ucUartBuf[UART_LENGTH]+3))
	{
		for(i=0; i<ucUartBuf[UART_LENGTH]; i++)
		{
			RSTSTAT = 0x00;
			*ptr = ucUartBuf[3+i];
			ptr++;
		}
		
		bWriteFlashFlg = 1;         //Updated parameters, and written to the flash
//        UpEepromWithDelay();
		ucWriteFlashCnt=0;
		
		UART_SEND_ACK;
	}
	else
	{
		UART_SEND_NACK;
	}
}


 /*******************************************************************************
Function:UartReadInfo() 
Description:  Read the information: Battery Info, MCU Info, MTP Info
Input:  *prt---Data storage address
Output: NULL
Others:
*******************************************************************************/
void UartReadInfo(U8 xdata *ptr)
{
	U8 i;

	if(ucUartBuf[UART_LENGTH] > 140)
	{
		ucUartBuf[UART_LENGTH] = 0;
	}
	
	for(i=0; i<ucUartBuf[UART_LENGTH]; i++)
	{
		RSTSTAT = 0x00;
		ucUartBuf[3+i] =  *ptr;
		ptr++;
	}
	ucUartBuf[3+ucUartBuf[UART_LENGTH]] = CRC8cal(&ucUartBuf,ucUartBuf[UART_LENGTH]+3);
	
	UART_SEND_DATA;			//Start Send Data; Set UART REG
}

/*******************************************************************************
Function:WriteManufacture() 
Description:  write the information: User-defined,  For example, a software reset and forced into sleep
Input:  *prt---Data storage address
Output: NULL
Others:
*******************************************************************************/
void WriteManufacture(void)
{ 
	if(ucUartBuf[3+ucUartBuf[UART_LENGTH]] == CRC8cal(&ucUartBuf, ucUartBuf[UART_LENGTH]+3))
	{
		if(ucUartBuf[4] == 0x41)            //0x0041: Reset system
		{
			ucResetFlag = 0x12;
		}
		else if(ucUartBuf[4] == 0x05)         //0x0005: Enter sleep mode
		{
			bPCSleepFlg = 1;
		}
		bUartSndAckFlg = 0;
		UART_SEND_ACK;
	}
	else
	{
		UART_SEND_NACK;
	}
}


void ReadSubClassID(void)
{
	if(ucUartBuf[3+ucUartBuf[UART_LENGTH]] == CRC8cal(&ucUartBuf, ucUartBuf[UART_LENGTH]+3))
	{
		ucSubClassID=ucUartBuf[3];
		
		bUartSndAckFlg = 0;
		UART_SEND_ACK;
	}
	else
	{
		UART_SEND_NACK;
	}
}



/*******************************************************************************
Function:UartRdCmdProcess() 
Description:  Processing Uart read command  
Input:   NULL
Output: NULL
Others:
*******************************************************************************/
void UartRdCmdProcess(void)
{
    U8 xdata *rdaddr;
    
    switch(ucUartBuf[UART_CMD_NO])
    {
        case 0xA0:
            UartHandshake();
        break;

        case CELL1:
            UartReadInfo((U8 xdata *)&Info.uiVCell[ucCellNumOffset]);
        break;

        case CELL2:
            UartReadInfo((U8 xdata *)&Info.uiVCell[ucCellNumOffset+1]);
        break;

        case CELL3:
            UartReadInfo((U8 xdata *)&Info.uiVCell[ucCellNumOffset+2]);
        break;

        case CELL4:
            UartReadInfo((U8 xdata *)&Info.uiVCell[ucCellNumOffset+3]);
        break;

        case CELL5:
            UartReadInfo((U8 xdata *)&Info.uiVCell[ucCellNumOffset+4]);
        break;

        case CELL6:
            UartReadInfo((U8 xdata *)&Info.uiVCell[ucCellNumOffset+5]);
        break;

        case CELL7:
            UartReadInfo((U8 xdata *)&Info.uiVCell[ucCellNumOffset+6]);
        break;

        case CELL8:
            UartReadInfo((U8 xdata *)&Info.uiVCell[ucCellNumOffset+7]);
        break;

        case CELL9:
            UartReadInfo((U8 xdata *)&Info.uiVCell[ucCellNumOffset+8]);
        break;

        case CELL10:
            UartReadInfo((U8 xdata *)&Info.uiVCell[ucCellNumOffset+9]);
        break;
		
	    	case TOTAL_VOLTAGE:
            UartReadInfo((U8 xdata *)&Info.ulVoltage);
        break;

        case CADC_CURRENT:
            UartReadInfo((U8 xdata *)&Info.slCurr);
        break;
		
        case EXT_TEMP1:
            UartReadInfo((U8 xdata *)&Info.uiTemp1);
        break;

        case EXT_TEMP2:
            UartReadInfo((U8 xdata *)&Info.uiTemp2);
        break;

        case DIE_TEMP1:
            UartReadInfo((U8 xdata *)&Info.uiICTemp1);
        break;

        case DIE_TEMP2:
         UartReadInfo((U8 xdata *)&Info.uiICTemp2);
        break;

        case FULL_CHG_CAP:
            UartReadInfo((U8 xdata *)&Info.ulFCC);
        break;

        case REMAIN_CAP:
            UartReadInfo((U8 xdata *)&Info.ulRC);
        break;

        case R_SOC:
            UartReadInfo((U8 xdata *)&Info.uiRSOC);
        break;

        case CYCLE_COUNT:
            UartReadInfo((U8 xdata *)&Info.uiCycleCount);
        break;

        case PACK_STATUS:
            UartReadInfo((U8 xdata *)&Info.uiPackStatus);
        break;

        case BATTERY_STATUS:
            UartReadInfo((U8 xdata *)&Info.uiBatStatus);
        break;

        case PACK_CONFIG:
            UartReadInfo((U8 xdata *)&Info.uiPackConfig);
        break;

        case MANUFACTURE_COMMAND:
            UartReadInfo((U8 xdata *)&Info.uiManuCommand);
        break;

        case ISP_COMMAND:
            ISPHandshake();
        break;
		
        default:    //read extern EEPRom data
            if(ucUartBuf[UART_LENGTH] >= 128)
						{
							bE2PProcessFlg = 1;
							bE2PRdData = 1;
						}
			else if(ucUartBuf[UART_CMD_NO] == SUB_PAGE1 && ucSubClassID == RTC_SUBID)
            {
							bE2PProcessFlg = 1;
							bRTCRdTime = 1;
						}
			else if(ucUartBuf[UART_CMD_NO] == SUB_PAGE1)
            {
              rdaddr = (U8 xdata *)Page1WrRdFuncTable[ucSubClassID];
              UartReadInfo(rdaddr);
            }
            else if(ucUartBuf[UART_CMD_NO] == SUB_PAGE2)
            {
              rdaddr = (U8 xdata *)Page2WrRdFuncTable[ucSubClassID];
              UartReadInfo(rdaddr);
            }
			break;
    }
}


/*******************************************************************************
Function:CalibVoltage() 
Description:  Prepare calibration voltage, include pack voltage & cell voltage
Input:	 NULL
Output: NULL
Others:
*******************************************************************************/
void CalibVoltage(void)
{	
	if(ucUartBuf[3+ucUartBuf[UART_LENGTH]] == CRC8cal(&ucUartBuf, ucUartBuf[UART_LENGTH]+3))
	{
		bCalibrationFlg = 1;
		ulExtVPack = ((U16)ucUartBuf[3]<<8)|ucUartBuf[4];
		ucExtcaliSwitch1 |= 0x01;

		UART_SEND_ACK;
	}
	else
	{
		UART_SEND_NACK;	
	}
}


/*******************************************************************************
Function:CalibCurrent() 
Description:  Prepare calibration zero current and discharge current
Input:	 NULL
Output: NULL
Others:
*******************************************************************************/
void CalibCurrent(void)
{
	if(ucUartBuf[3+ucUartBuf[UART_LENGTH]] == CRC8cal(&ucUartBuf, ucUartBuf[UART_LENGTH]+3))
	{
		bCalibrationFlg = 1;
		slExtCur = ((U32)ucUartBuf[3]<<24)
						| ((U32)ucUartBuf[4]<<16)
						| ((U32)ucUartBuf[5]<<8)
						| ((U32)ucUartBuf[6]);
		ucExtcaliSwitch1 |= 0x10;

		UART_SEND_ACK;
	}
	else
	{
		UART_SEND_NACK;
	}
}


void CalibCurOffset(void)
{
	if(ucUartBuf[3+ucUartBuf[UART_LENGTH]] == CRC8cal(&ucUartBuf, ucUartBuf[UART_LENGTH]+3))
	{
		bCalibrationFlg = 1;					//Calibration offset Current
		ucExtcaliSwitch1 |= 0x20;

		UART_SEND_ACK;
	}
	else
	{
		UART_SEND_NACK;	
	}
}


/*******************************************************************************
Function:CalibTemperature() 
Description:  Prepare calibration temperature
Input:	 NULL
Output: NULL
Others:
*******************************************************************************/
void CalibTS1(void)
{
	if(ucUartBuf[3+ucUartBuf[UART_LENGTH]] == CRC8cal(&ucUartBuf, ucUartBuf[UART_LENGTH]+3))
	{
		bCalibrationFlg = 1;					//Calibration TS1
		uiExtTemp1 = ((U16)ucUartBuf[3]<<8)|ucUartBuf[4];
		ucExtcaliSwitch1 |= 0x04;

		UART_SEND_ACK;
	}
	else
	{
		UART_SEND_NACK;	
	}
}


void CalibTS2(void)
{
	if(ucUartBuf[3+ucUartBuf[UART_LENGTH]] == CRC8cal(&ucUartBuf, ucUartBuf[UART_LENGTH]+3))
	{
		bCalibrationFlg = 1;					//Calibration TS2
		uiExtTemp2 = ((U16)ucUartBuf[3]<<8)|ucUartBuf[4];
		ucExtcaliSwitch1 |= 0x08;

		UART_SEND_ACK;
	}
	else
	{
		UART_SEND_NACK;	
	}
}


/*******************************************************************************
Function: CaliRTC(void)
Description:  
Input:	 	
Output: 
Others:
*******************************************************************************/
void CalibRTC(void)
{
	U8 i;
	
	if(ucUartBuf[3+ucUartBuf[UART_LENGTH]] == CRC8cal(&ucUartBuf, ucUartBuf[UART_LENGTH]+3))
	{
		for(i=0; i<6; i++)
		{
			bCalibrationFlg = 1;
			ucExtRTC[i] = ucUartBuf[3+i];
			ucExtcaliSwitch1 |= 0x80;
		}
	}
}


/*******************************************************************************
Function:UartWrCmdProcess() 
Description:  Processing Uart write command  
Input:   NULL
Output: NULL
Others:
*******************************************************************************/
void UartWrCmdProcess(void)
{
    U8 xdata *rdaddr;
    switch(ucUartBuf[UART_CMD_NO])
    {
        case MANUFACTURE_COMMAND:
            WriteManufacture();
        break;
        case DATA_FLASH_COMMAND:  
            ReadSubClassID();               //accept command is dataflashcommand 0x77
        break;
        case CALI_CUR_COMMAND:
            CalibCurrent();
        break;
        case CALI_VOL_COMMAND:
            CalibVoltage();
        break;
        case CALI_ZERO_CUR_COMMAND:
            CalibCurOffset();
        break;
        case CALI_TS1_COMMAND:
            CalibTS1();
        break;
        case CALI_TS2_COMMAND:
            CalibTS2();
        break;

        case CALI_RTC_COMMAND:
            CalibRTC();
        break;
//        case ISP_COMMAND:
//            ISPHandshake();
//        break;
        case SUB_PAGE1:
            rdaddr = (U8 xdata *)Page1WrRdFuncTable[ucSubClassID];
            UartWriteInfo(rdaddr);
        break;
        case SUB_PAGE2:
            rdaddr = (U8 xdata *)Page2WrRdFuncTable[ucSubClassID];
            UartWriteInfo(rdaddr);
        break;
        
        case CMD_E2PROM_ERASE:
            if(ucUartBuf[UART_DATA] == 0x55)    //Testing equipment is properly
            {
                bE2PProcessFlg = 1;
                bE2PErase = 1;
                UART_SEND_ACK;
            }
            else
            {
                UART_SEND_NACK;
            }
        break;
    }
}



/*******************************************************************************
Function:  InterruptUART()
Description:  Receive and transmit data via UART
	ucUartBuf[0]--Slave Addr
	ucUartBuf[1]--CMD No.
	ucUartBuf[2]--Offset
	ucUartBuf[3]--Data Length
	ucUartBuf[4...]--Data
Input:	 	
Output: 
Others:
*******************************************************************************/
U8 xdata rx_length = 0;
U8 xdata tx_length = 0;
bit Arry_EN = 0;
void InterruptUART(void) interrupt 4
{
	if(RI)                              //RT1 interrupt means MCU recept data though UART
	{
		ucUartBuf[ucUartBufPT] = SBUF;                  //transfer SBUF to the ucUartBuf
		ucUartBufPT++;
	//	IO_Error_8 = 1;
		if(ucUartBufPT >= 80)
		{
			ucUartBufPT = 0;
		}
		
#if 0
		if(ucUartBufPT == 1)
		{//判断是不是从机地址
			if((ucUartBuf[UART_SLAVE_ADDR]&0xFE) == SADDR)      //received first data is not the slave ADDR
			{
				if((ucUartBuf[UART_SLAVE_ADDR]&0x01)==0)         //bit 7 of UartADDR is R/W 0--R, 1--W
				{
					bUartReadFlg = 1;					//this communication is read
				}
				else
				{
					bUartWriteFlg = 1;					//this communication is write        
				}
			}
			else
			{
				ucUartBufPT = 0;                       //ucUartBufPT reset to 0  
			}
		}
		if(bUartReadFlg)
		{
			if(ucUartBufPT == 3 
				&& ucUartBuf[UART_CMD_NO] != ISP_COMMAND)
			{
				UartRdCmdProcess();                 //Read the command process			
			}
			else if	(ucUartBuf[UART_CMD_NO] == ISP_COMMAND
						&& ucUartBufPT > (ucUartBuf[3]+3))
			{
				ucUartBufPT = 0;
                ucUartSndLength = 1;
				ISPHandshake();
			}
		}
		
		if(bUartWriteFlg)
		{
			if(ucUartBufPT > (ucUartBuf[UART_LENGTH]+3))        //If a write operation, and complete all the data has been received
			{
				UartWrCmdProcess();					//Write the command peocess
				bUartWriteFlg = 0;					//PC write MCU communiaction over
				ucUartBufPT = 0;
			}
		}
#else
		
		//判断是第四个数据，是数据的长度，一帧的长度就是数据长度+6
		if(ucUartBufPT == 4)
		{
			rx_length = ucUartBuf[3] + 6;
		}	
		if(ucUartBufPT >= rx_length)//正确获取了一帧数据
		{
			rx_length = 0;
			ucUartBufPT = 0;
#if 0
			if(tomi_rx_handle());
		//	memset(ucUartBuf,0,24);
#endif
		}
#endif
		RI = 0;
		bUartSndAckFlg = 0;
	}
	
	if(TI)
	{
#if debug
		while(!TI)//等待发送数据完成。非0（是1）就等待
		TI = 0;
#endif
	}
#if 0
		if((ucUartBufPT==0) || (ucUartBufPT>=ucUartBuf[UART_LENGTH]+3))
		{
			REN = 1;					//Allow UART receive data
			ucUartBufPT = 0;
			bUartReadFlg = 0;			//PC read MCU communication over
			ucUartBuf[0] = 0;
			ucUartBuf[1] = 0;
			ucUartBuf[2] = 0;
			ucUartBuf[3] = 0;
		
		}
		else
		{
			ucUartBufPT++;
			SBUF = ucUartBuf[ucUartBufPT];
		}

		if(Arry_EN == 1)//如果发送的是队列数据
		{
				//这个是发送一个个数组的数据
			if(ucUartBufPT == 4)//从第0个开始数
			{
				tx_length = ucUartBuf[4] + 7;
			}
			//刚开始的时候已经把0发送出去了，发送出去了之后进入了中断
			if(ucUartBufPT > tx_length)
			{
				Arry_EN = 0;
				tx_length = 0;
				TI = 0;
				REN = 1;					//Allow UART receive data
				ucUartBufPT = 0;
				bUartReadFlg = 0;			//PC read MCU communication over
				memset(ucUartBuf,0,tx_length);
			}
			else
			{		
				TI = 0;
				ucUartBufPT++;//开始发送下一个
				SBUF = ucUartBuf[ucUartBufPT]; 
			}
		}
		else
		{
			while(!TI);
			TI = 0;
		}
	  

		
		//TI = 0;
		bUartSndAckFlg = 1;
	}
		
	ucIdleTimerCnt = 0;
	ucPDTimerCnt = 0;			//If communication is clear the count
	ucUartTimeoutCnt = 0; //200ms会清除一下指针，也就是说200ms后统计一下数据
#endif
}

/***********************by tomi*****************************/
U8 auchCRCHi[] =
{
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};


U8  auchCRCLo[] =
{
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
	0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
	0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
	0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
	0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
	0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
	0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
	0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
	0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
	0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
	0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
	0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
	0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
	0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
	0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
	0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
	0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
	0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};
U16 CRC16(U8 *puchMsg, U16 usDataLen)
{
	U8 uchCRCHi = 0xFF ; 	//
	U8 uchCRCLo = 0xFF ;
	U8 uIndex ;

	while (usDataLen--)
	{
		uIndex = uchCRCHi ^ *puchMsg++ ;
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ;
		uchCRCLo = auchCRCLo[uIndex] ;
	}
	return (uchCRCHi << 8 | uchCRCLo) ;
}


typedef struct{
	U16 addr ;
	U8 fuc_num ;//功能码
	U8 *p;
	U8 fram_len;//帧长度
	U8 error_code;//错误码
	U8 data_len;
	int rx_crc;
}rx_from_master;


#if 0
/*************************************************************************************************
* 函数名: tomi_rx_handle
* 参  数: 无
* 返回值: 无
* 作  用:接收到协议后，进行回复
*************************************************************************************************/
//0x23功能码
//填16位的SN码
U8 xdata tx_sn_data[16]= TX_SN;
extern pack_fault_flag packfault_flag;
bit tomi_rx_handle(void)
{
			rx_from_master xdata receive = {
			0,
			0,
			ucUartBuf,
			0,
			0,
			0,
			0,
		};
		unsigned int RcvCRC = 0;
		U8 xdata tx_valid_crc_num = 0;
		//U8 xdata tx_buff[24]={0};
		U8 xdata tx_error_para = 0x00;
	/**********tx填写的值***************/

		//0x24功能码
	//	数据码01、02内容为:电压值uint16数据类型，单位为mV；
		U16 xdata vol_data = Info.ulVoltage;
	//	数据码03、04内容为:电流值sint16数据类型，单位为mA；
		S16 xdata cur_data = (S16)Info.slCurr;
	//	数据码05、06内容为:温度值sint08数据类型，单位为摄氏度；电池剩余容量值uint08数据类型，百分比表示(%)
		S8 xdata tmp = (S8)Info.uiTemp1;
		U8 xdata rvol =(U8)Info.uiRSOC;
	//	数据码07、08 内容为:电池总容量值uint16数据类型，单位mAh；
		U16 xdata bat_cap = (U16)Info.ulFCC;
	//	数据码09、10 内容为:充电的次数
		U16 xdata chg_num = Info.uiCycleCount;
	//	数据码11、12 内容为:剩余用电时长,uint16数据类型，单位为s,仅在电流大于200mA时计算,若电流小于200mA,则该数据值恒为0xFFFF;
		U16 xdata r_time = (U16)Info.ulRC;
	//	数据码13、14 内容为:电芯厂家：三星  0xA0        LG 0x93，字节14为预留，数据对齐；

		//0x27功能码


		U8  xdata manu = 0;
		U8  xdata reserve = 0;
		receive.addr = ucUartBuf[0]+(ucUartBuf[1] *256);
		receive.fuc_num = ucUartBuf[2];
		receive.fram_len = rx_length ;
		receive.data_len = ucUartBuf[3];
		receive.rx_crc = (ucUartBuf[receive.fram_len-2])+((ucUartBuf[receive.fram_len-1])*256);
		printf("Rreceive.fram_len =%d \n\r", receive.fram_len);
		printf("Rreceive.rx_crc =%x \n\r",receive.rx_crc);
		printf("receive.addr =%x \n\r", receive.addr);
		if(receive.addr  == 0x4D4D)
		{
			printf("receive.fuc_num=%x \n\r", receive.fuc_num);
			printf("(receive.fram_len-2 ) =%d",(receive.fram_len-2 ));
			/*****************CRC16*********************/
			RcvCRC = CRC16(ucUartBuf,(receive.fram_len-2 ));
			printf("RcvCRC =%x \n\r", (U16)RcvCRC);

			if(RcvCRC == receive.rx_crc)
			{
				printf(" pass crc16 \n\r");
				switch(receive.fuc_num)
				{
				  case 0x21://主机控制指令
				  if((receive.fram_len == 8)&&(receive.data_len == 2))
				  {
					 receive.p[0] = ucUartBuf[4];
					 receive.p[1] = ucUartBuf[5];
					  // run_mode为0x01且mode为0xFF表示让从机从升级模式跳转到app模式
					  // run_mode为0x02且mode为0xFF表示让从机从app模式跳转到升级模式
					  // run_mode为其他值时,无需处理对run_mode进行处理
					  if((receive.p[0]== 0x01))//&&(mode ==0xff)
					  {
							//还未添加该功能
					  }
					  else if((receive.p[0]== 0x02))//&&(mode ==0xff)
					  {

					  }
						
						//充电器电压判断
						if((receive.p[1]== 0x01))//&&(mode ==0xff)
					  {

					  }
					  else if((receive.p[1]== 0x10))//&&(mode ==0xff)
					  {

					  }				
								
					  /****************respond********************/
					  ucUartBuf[0] = 0x4242 & 0xff;
					  printf(" ucUartBuf[0]=%x \n\r",ucUartBuf[0]);
					  ucUartBuf[1] = (0x4242 >> 8)&0xff;
					  ucUartBuf[2] = 0x21;
					  ucUartBuf[3] = 0x01;
					  ucUartBuf[4] = 0;
					  tx_valid_crc_num = 5 + ucUartBuf[4];
					  ucUartBuf[5] = CRC16(ucUartBuf,tx_valid_crc_num)&0xff;
					  printf(" ucUartBuf[5]=%x \n\r",ucUartBuf[5]);
					  ucUartBuf[6] = (CRC16(ucUartBuf,tx_valid_crc_num) >> 8)&0xff;
					  printf(" ucUartBuf[6]=%x \n\r",ucUartBuf[6]);
						UART_SEND_DATA_ARRY;
				  }
				  else
				  {
					  return 0;
				  }
				  break;
					 /******************************/
				  case 0x22://主机查询电池包版本信息
					  printf(" 0x22judge \n\r");
					  if((receive.fram_len == 6)&&(receive.data_len == 0))
						  {
							  /****************respond********************/
							  ucUartBuf[0] = 0x4242 & 0xff;
							  ucUartBuf[1] = (0x4242 >> 8)&0xff;
							  ucUartBuf[2] = 0x22;
							  ucUartBuf[3] = 0x01;
							  ucUartBuf[4] = 3;
							  tx_valid_crc_num = 5 + ucUartBuf[4];
							  //填硬件版本
							  ucUartBuf[5] = HWversion;
							  //填软件版本
							  ucUartBuf[6] = SWversion;
							  //填运行模式
							  ucUartBuf[7] = 0x01;//暂时定义为app模式，后面如果要用到任务升级的话，就定义一个变量
							  ucUartBuf[8] = CRC16(ucUartBuf,tx_valid_crc_num)&0xff;
							  ucUartBuf[9] = (CRC16(ucUartBuf,tx_valid_crc_num) >> 8)&0xff;
								UART_SEND_DATA_ARRY;
						  }
						  else
						  {
							  return 0;
						  }
					  break;
					  /******************************/
				  case 0x23://主机查询电池包SN信息
					  printf(" 0x23judge \n\r");
					  if((receive.fram_len == 6)&&(receive.data_len == 0))
						  {
							  /****************respond********************/
							  ucUartBuf[0] = 0x4242 & 0xff;
							  ucUartBuf[1] = (0x4242 >> 8)&0xff;
							  ucUartBuf[2] = 0x23;
							  ucUartBuf[3] = 0x01;
							  ucUartBuf[4] = 0x0f;
							  tx_valid_crc_num = 5 + ucUartBuf[4];
							  //填16位 SN码
							  memcpy(&ucUartBuf[5],tx_sn_data,16);
							  ucUartBuf[21] = CRC16(ucUartBuf,tx_valid_crc_num)&0xff;
							  ucUartBuf[22] = (CRC16(ucUartBuf,tx_valid_crc_num) >> 8)&0xff;
							  UART_SEND_DATA_ARRY;
						  }
						  else
						  {
							  return 0;
						  }
					  break;
					  /******************************/
				  case 0x24://主机查询电池包信息1
					  if((receive.fram_len == 6)&&(receive.data_len == 0))
						  {
							  /****************respond********************/
							  ucUartBuf[0] = 0x4242 & 0xff;
							  ucUartBuf[1] = (0x4242 >> 8)&0xff;
							  ucUartBuf[2] = 0x24;
							  ucUartBuf[3] = 0x01;
							  ucUartBuf[4] = 0x0d;
							  tx_valid_crc_num = 5 + ucUartBuf[4];
	//						  数据码01、02内容为:电压值uint16数据类型，单位为mV；
							  ucUartBuf[5] = vol_data&0xff;
							  ucUartBuf[6] = (vol_data >> 8)&0xff;
	//						  数据码03、04内容为:电流值sint16数据类型，单位为mA；
							  ucUartBuf[7] = vol_data&0xff;//存放的是低8位的字节，解码的时候按照sint解码
							  ucUartBuf[8] = (vol_data >> 8)&0xff;
	//						  数据码05、06内容为:温度值sint08数据类型，单位为摄氏度；电池剩余容量值uint08数据类型，百
	//						  分比表示(%)
							  ucUartBuf[9] = (U8)tmp;//解码按照sint08解码
							  ucUartBuf[10] = rvol;
	//						  数据码07、08 内容为:电池总容量值uint16数据类型，单位mAh；
							  ucUartBuf[11] = bat_cap&0xff;
							  ucUartBuf[12] = (bat_cap >> 8)&0xff;
	//						  数据码09、10 内容为:充电的次数
							  ucUartBuf[13] = chg_num&0xff;
							  ucUartBuf[14] = (chg_num >> 8)&0xff;
	//						  数据码11、12 内容为:剩余用电时长,uint16数据类型，单位为s,仅在电流大于200mA时计算,若电流小于200mA,则该数据值恒为0xFFFF;
							  if(r_time < 200)
							  {
								 ucUartBuf[15] = 0xff;
								 ucUartBuf[16] = 0xff;
							  }
							  else
							  {
								 ucUartBuf[15] = r_time&0xff;
								 ucUartBuf[16] = (r_time >> 8)&0xff;
							  }

	//						  数据码13、14 内容为:电芯厂家：三星  0xA0        LG 0x93，字节14为预留，数据对齐；
							  ucUartBuf[17] = manu;
							  ucUartBuf[18] = 0xff;
							  //填运行模式
							  ucUartBuf[19] = CRC16(ucUartBuf,tx_valid_crc_num)&0xff;
							  ucUartBuf[20] = (CRC16(ucUartBuf,tx_valid_crc_num) >> 8)&0xff;
							  UART_SEND_DATA_ARRY;
						  }
						  else
						  {
							  return 0;
						  }

					  break;
					
				  case 0x25://主机查询电池包信息2暂未使用
					  break;
					  /******************************/
				  case 0x26://主机查询电池包故障信息
					  if((receive.fram_len == 6)&&(receive.data_len == 0))
					 	 {
					 			/****************respond********************/
					 		 ucUartBuf[0] = 0x4242 & 0xff;
					 		 ucUartBuf[1] = (0x4242 >> 8)&0xff;
					 		 ucUartBuf[2] = 0x26;
					 		 ucUartBuf[3] = 0x01;
					 		 ucUartBuf[4] = 0x02;
					 		 tx_valid_crc_num = 5 + ucUartBuf[4];
					 		 ucUartBuf[5] = (packfault_flag.pack_flag_all)&0xff;
					 		 ucUartBuf[6] = ((packfault_flag.pack_flag_all) >> 8)&0xff;
							 ucUartBuf[7] = CRC16(ucUartBuf,tx_valid_crc_num)&0xff;
					 		 ucUartBuf[8] = (CRC16(ucUartBuf,tx_valid_crc_num) >> 8)&0xff;
					 		 UART_SEND_DATA_ARRY;
					 	}
					 	else
					 	{
					 		return 0;
					 	}
					  break;
					  /******************************/
				  case 0x27://主机查询电池包导致输出关断的历史故障信息
					  if((receive.fram_len == 6)&&(receive.data_len == 0))
						{
							/****************respond********************/
							 ucUartBuf[0] = 0x4242 & 0xff;
							 ucUartBuf[1] = (0x4242 >> 8)&0xff;
							 ucUartBuf[2] = 0x27;
							 ucUartBuf[3] = 0x01;
							 ucUartBuf[4] = 0x01;
							 tx_valid_crc_num = 5 + ucUartBuf[4];
							 ucUartBuf[5] = (packfault_flag.pack_flag_all)&0xff;//这边历史错误码，待定
							 ucUartBuf[6] = CRC16(ucUartBuf,tx_valid_crc_num)&0xff;
							 ucUartBuf[7] = (CRC16(ucUartBuf,tx_valid_crc_num) >> 8)&0xff;
							 UART_SEND_DATA_ARRY;
						}
						else
						{
							 return 0;
						}
					  break;
				  case 0x28://主机清除电池包导致输出关断的历史故障信息
					  if((receive.fram_len == 6)&&(receive.data_len == 0))
						{
							/****************respond********************/
							 ucUartBuf[0] = 0x4242 & 0xff;
							 ucUartBuf[1] = (0x4242 >> 8)&0xff;
							 ucUartBuf[2] = 0x28;
							 ucUartBuf[3] = 0x01;
							 ucUartBuf[4] = 0x00;
							 tx_valid_crc_num = 5 + ucUartBuf[4];
							 ucUartBuf[5] = CRC16(ucUartBuf,tx_valid_crc_num)&0xff;
							 ucUartBuf[6] = (CRC16(ucUartBuf,tx_valid_crc_num) >> 8)&0xff;
							 UART_SEND_DATA_ARRY;
						}
						else
						{
							 return 0;
						}
					  break;

				  default:
						//默认状态下：就是不是上面所述的任何一种功能码
							 ucUartBuf[0] = 0x4242 & 0xff;
							 ucUartBuf[1] = (0x4242 >> 8)&0xff;
							 ucUartBuf[2] = 0x28;
							 ucUartBuf[3] = 0x02;//返回无效功能码
							 ucUartBuf[4] = 0x00;
							 tx_valid_crc_num = 5 + ucUartBuf[4];
							 ucUartBuf[5] = CRC16(ucUartBuf,tx_valid_crc_num)&0xff;
							 printf(" ucUartBuf[5]=%x \n\r",ucUartBuf[5]);
							 ucUartBuf[6] = (CRC16(ucUartBuf,tx_valid_crc_num) >> 8)&0xff;
							 printf(" ucUartBuf[6]=%x \n\r",ucUartBuf[6]);	
							 UART_SEND_DATA_ARRY					
					     break;

				}
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
		return 1;

}

#endif






