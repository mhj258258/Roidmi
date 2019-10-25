/********************************************************************************
Copyright (C), Sinowealth Electronic. Ltd.
Author: 	andyliu
Version: 	V0.0
Date: 		2014/05/30
History:
	V0.0		2014/08/28		 Preliminary
********************************************************************************/
#include "System.h"
#include "AfeMtp.h"


/*******************************************************************************
Function:TWIWriteAFE(U8 WrAddr, U8 xdata *WrBuf) 
Description:  
Input:	 	
Output: 
Others:
*******************************************************************************/
//目的是为了加强容错处理
//if(!TWIWriteAFE(AFE_INT_EN+i, &REG.AFEINTEN+i)) //0x03+i  中断地址+i
bit TWIWriteAFE(U8 WrAddr, U8 xdata *WrBuf)					//write only one byte once
{
    BOOL result = 0;							
    U8 times = 0;

    while(times++ < TRY_TIMES)
    {
			//从机固定地址 0011 011x是0写操作
			//AFE_CHIP_ID = 1b<<1  
			result = TwiWrite(AFE_CHIP_ID, WrAddr, 1, WrBuf);
			if(result)
			{
				break;
			}
			else
			{
				Delay1ms(1);
				//Delay1ms(2);
			}
    }
    
    return  result;
}


/*******************************************************************************
Function:TWIReadAFE(U8 RdAddr, U8 xdata *RdBuf) 
Description:  
Input:		
Output: 
Others:
*******************************************************************************/
bit TWIReadAFE(U8 RdAddr, U8 xdata *RdBuf)		//读AFE
{
    BOOL result = 0;							
    U8 times = 0;

    while(times++ < TRY_TIMES)					//如果读数出错，则Try 5次
    {
		result = TwiRead(AFE_CHIP_ID, RdAddr, 2, RdBuf);	//读取数据长度固定为2Bytes
		if(result)								//如果读数成功且CRC校验正确，则退出循环返回成功标标志
		{
			break;
		}
		else									//如果TWI读数或CRC校验失败，则延时1mS之后再读取
		{
			Delay1ms(1);
		}
    }
    
    return  result;
}


/*******************************************************************************
Function:ClearAFEFlg(void) 
Description:  
Input:	NULL 	
Output: NULL
Others:
*******************************************************************************/
void ClearAFEFlg(void)
{
	TWIReadAFE(AFE_FLAG1, &REG.AFEFLAG1);		//read AFE FLAG1 search for which state  trigged ALARM
	if((REG.AFEFLAG1!=0) || (REG.AFEFLAG2!=0))	//读取两个flag							
	{
        REG.AFESCONF1 |= 0x80;//清除位置1
        TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);
        REG.AFESCONF1 &= ~0x80;
        TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);
	}
}


/*******************************************************************************
Function:InitAFE() 
Description:  
Input:	NULL 	
Output: NULL
Others:
*******************************************************************************/
bit InitAFE(void)//把REG表里的数据写入到AFE中
{
	U8 i;
	BOOL result=1;
#if 0
		TI = 0;
		TI = 1;	
		printf("\n InitAFE \n");    
#endif
	
	for(i=0; i<11; i++)//从AFE_INT_EN地址03h开始的后11位地址
	{
		if(!TWIWriteAFE(AFE_INT_EN+i, &REG.AFEINTEN+i)) //0x03+i  中断地址+i
		{
        result = 0;
    		//break;
		}
	}

	return result;
}


/*******************************************************************************
Function:EnableAFEWdtCadc()  
Description:使能CHG&DSG&PCHG输出，且使能WDT和CADC模块
Input: 	
Output: 
Others:
*******************************************************************************/
//void EnableAFEWdtCadc(void)
//{
//	ucMTPConfVal |= 0x7c;
//	MTPWrite(MTP_CONF, 1, &ucMTPConfVal);
//}