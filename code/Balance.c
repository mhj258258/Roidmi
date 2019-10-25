/********************************************************************************
Copyright (C), Sinowealth Electronic. Ltd.
Author: 	andyliu
Version: 	V0.0
Date: 		2014/05/30
History:
	V0.0		2014/05/30		 Preliminary
********************************************************************************/
#include "system.h"
#include "AfeMtp.h"


/*******************************************************************************
Function: BalManage(void)
Description:  
Input:	 	
Output: 
Others:
*******************************************************************************/
void BalManage(void)
{
	U8 i;

	if(!bBalancingFlg) //平衡过程中的标志位，该标志为1 时，不进行相关电压保护判断
	{
	    if(bBAL_EN)//平衡功能使能
	    {
				uiBalanceChannel = 0;
	    	if(((uiCellVmax>E2uiOVvol)&&(!bHV))||((uiCellVmax < E2uiOVRvol)&&(bHV))|| bOTC || bUTC || bOTD || bUTD) 
	    	{
	    		bBalanceFlg = 0;
	    		ucBalanceTimeCnt = 0;
	    	}
	    	else
		    {
					//4.1.1平衡功能，电芯最大电压大于平衡开启电压，电压差大于平衡电压差阈值，充电电流大于平衡电流阈值，维持事件也要大于平衡的开启时间
		    	if(((uiCellVmax>E2uiBalanceVol) && (uiCellVmax-uiCellVmin)>=E2uiBalanceVolDiff) && (Info.slCurr>=E2siBalCurrent))
	    		{
	    			if(!bBalanceFlg)
	    			{
	    				ucBalanceTimeCnt++;															 
	    			}
	    			if(ucBalanceTimeCnt >= BAL_DELAY_CNT)											 
	    			{
	    				for(i=ucCellNumOffset; i<ucCellNumOffset+ucCellNum; i++)
	    				{
	    					if((Info.uiVCell[i]-uiCellVmin) >= E2uiBalanceVolDiff)
	    					{
	    						uiBalanceChannel |= (1<<i);
	    					}
	    				}
	    				bBalanceFlg = 1;
	    			}
	    		}
	    		else
	    		{
	    			bBalanceFlg = 0;
	    			ucBalanceTimeCnt = 0;
	    		}
	        }
		}
	}	
}


/*******************************************************************************
Function: CloseBal(void)
Description:  
Input:	 	
Output: 
Others:
*******************************************************************************/
void CloseBal(void)
{
    REG.AFESCONF4 = 0;
    TWIWriteAFE(AFE_SCONF4, &REG.AFESCONF4);		   //	CleaerBalance=0x00
    REG.AFESCONF5 = 0;
    TWIWriteAFE(AFE_SCONF5, &REG.AFESCONF5);		   //when dec the VADC must close the balance 
}


/*******************************************************************************
Function: CheckCtoVol(void)
Description:  
Input:	 	
Output: 
Others:
*******************************************************************************/
bit CheckCtoVol(void)
{
    BOOL bResult=0;

	if(uiCellVmin < 180)
	{
        bResult = 1;
		if(++ucCtoCnt >= 10)		//断线 500ms
        {
					bCTO = 1;
        }      
	}
	else
	{
	 	ucCtoCnt = 0;
	}

    return bResult;
}


/*******************************************************************************
Function: BalProcess(void)
Description:  
Input:	 	
Output: 
Others:
*******************************************************************************/
void BalProcess(void)
{
    U16 BalChTemp;
    
    switch(ucBalanceStep)
    {
        case BALANCE_ENABLE:          //VADC采集的数据用于正常电压判断
            if(bCellOpenDecFlag || bBalanceFlg)//bCellOpenDecFlag  定时开启断线检测2s一次  bBalanceFlg 满足平衡条件后，置位开启平衡
	        {
	            if(bBalanceOddFlg)							  //odd balance
	            {
	                BalChTemp = uiBalanceChannel&0x5555;		//  0101 0101 0101 0101					
	            }
	            else											 //even balance
	            {
	                BalChTemp = uiBalanceChannel&0xAAAA;							
	            }
	            //开启对应的平衡回路
	            REG.AFESCONF4 = BalChTemp>>5;       //平衡回路控制位高5位
	            TWIWriteAFE(AFE_SCONF4, &REG.AFESCONF4);		   //CleaerBalance=0x00
	            REG.AFESCONF5 = BalChTemp;
	            TWIWriteAFE(AFE_SCONF5, &REG.AFESCONF5);		   //when dec the VADC must close the balance 
	                 
	            ucBalanceStep = BALANCE_START;
	            bBalancingFlg = 1;
	        }
            break;

        case BALANCE_START:         //VADC采集的电压数据丢掉
            ucBalanceStep = BALANCING;
            break;

        case BALANCING:             //VADC采集的电压数据判断断线
            if(!CheckCtoVol())      //如果发现有断线则连续检测断线，这边判断我有异议
	        {
								CloseBal();
                ucBalanceStep = BALANCE_DISABLE;
                bBalanceOddFlg = ~bBalanceOddFlg;
	        }
            break;

        case BALANCE_DISABLE:       //VADC采集的电压数据丢掉
            if(!CheckCtoVol())      //如果发现有断线则连续检测断线
	        {
                ucBalanceStep = BALANCE_NULL;
//                bBalancingFlg = 0;
					}
					else 
					{
									ucBalanceStep = BALANCE_ENABLE;
									bBalanceOddFlg = ~bBalanceOddFlg;
					}
            break;

        case BALANCE_NULL:          //VADC采集的数据用于正常电压判断
            ucBalanceStep = BALANCE_ENABLE;
            bCellOpenDecFlag = 0;
            bBalanceFlg = 0;
			bBalancingFlg = 0;		
            break;
	}
}


/*******************************************************************************
Function: CellOpenProcess(void)
Description:  
Input:	 	
Output: 
Others:
*******************************************************************************/
void CellOpenProcess(void)
{	
    if(bCTO_EN)//如果有断线检测使能
	{
        if(++ucCellOpenDecCnt >= 40)          //2S 那么就2s开启一次断线检测，分奇偶检测时候需要4s的时间
	{
            ucCellOpenDecCnt = 0;
            bCellOpenDecFlag = 1; //开断断线检测标志位
            uiBalanceChannel = 0x03ff;
		}
	}
}



