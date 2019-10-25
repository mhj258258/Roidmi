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

	if(!bBalancingFlg) //ƽ������еı�־λ���ñ�־Ϊ1 ʱ����������ص�ѹ�����ж�
	{
	    if(bBAL_EN)//ƽ�⹦��ʹ��
	    {
				uiBalanceChannel = 0;
	    	if(((uiCellVmax>E2uiOVvol)&&(!bHV))||((uiCellVmax < E2uiOVRvol)&&(bHV))|| bOTC || bUTC || bOTD || bUTD) 
	    	{
	    		bBalanceFlg = 0;
	    		ucBalanceTimeCnt = 0;
	    	}
	    	else
		    {
					//4.1.1ƽ�⹦�ܣ���о����ѹ����ƽ�⿪����ѹ����ѹ�����ƽ���ѹ����ֵ������������ƽ�������ֵ��ά���¼�ҲҪ����ƽ��Ŀ���ʱ��
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
		if(++ucCtoCnt >= 10)		//���� 500ms
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
        case BALANCE_ENABLE:          //VADC�ɼ�����������������ѹ�ж�
            if(bCellOpenDecFlag || bBalanceFlg)//bCellOpenDecFlag  ��ʱ�������߼��2sһ��  bBalanceFlg ����ƽ����������λ����ƽ��
	        {
	            if(bBalanceOddFlg)							  //odd balance
	            {
	                BalChTemp = uiBalanceChannel&0x5555;		//  0101 0101 0101 0101					
	            }
	            else											 //even balance
	            {
	                BalChTemp = uiBalanceChannel&0xAAAA;							
	            }
	            //������Ӧ��ƽ���·
	            REG.AFESCONF4 = BalChTemp>>5;       //ƽ���·����λ��5λ
	            TWIWriteAFE(AFE_SCONF4, &REG.AFESCONF4);		   //CleaerBalance=0x00
	            REG.AFESCONF5 = BalChTemp;
	            TWIWriteAFE(AFE_SCONF5, &REG.AFESCONF5);		   //when dec the VADC must close the balance 
	                 
	            ucBalanceStep = BALANCE_START;
	            bBalancingFlg = 1;
	        }
            break;

        case BALANCE_START:         //VADC�ɼ��ĵ�ѹ���ݶ���
            ucBalanceStep = BALANCING;
            break;

        case BALANCING:             //VADC�ɼ��ĵ�ѹ�����ж϶���
            if(!CheckCtoVol())      //��������ж��������������ߣ�����ж���������
	        {
								CloseBal();
                ucBalanceStep = BALANCE_DISABLE;
                bBalanceOddFlg = ~bBalanceOddFlg;
	        }
            break;

        case BALANCE_DISABLE:       //VADC�ɼ��ĵ�ѹ���ݶ���
            if(!CheckCtoVol())      //��������ж���������������
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

        case BALANCE_NULL:          //VADC�ɼ�����������������ѹ�ж�
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
    if(bCTO_EN)//����ж��߼��ʹ��
	{
        if(++ucCellOpenDecCnt >= 40)          //2S ��ô��2s����һ�ζ��߼�⣬����ż���ʱ����Ҫ4s��ʱ��
	{
            ucCellOpenDecCnt = 0;
            bCellOpenDecFlag = 1; //���϶��߼���־λ
            uiBalanceChannel = 0x03ff;
		}
	}
}



