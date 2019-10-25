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
#include "Calibrate.h"

/*******************************************************************************
Function: VolProtect(void)
Description:  
Input:	 	
Output: 
Others:
*******************************************************************************/
void VolProtect(void)
{
	if(!bHV)
	{
		if(uiCellVmax > E2uiOVvol) //4.25
		{		
			if(++ucOVcnt >= OV_DELAY_CNT)//40*25 = 1000mS
			{
				bHV = 1;
				ucOVcnt = 0;
				ucOVRcnt = 0;
			}
		}
		else if(uiCellVmax < E2uiOVRvol) //< 4.15
		{
			if(ucOVcnt > 0)
			{
				ucOVcnt--;
			}
		}
	}
	else
	{
		if(uiCellVmax < E2uiOVRvol)//�ָ� < 4.15v
		{			
			if(++ucOVRcnt >= OVR_DELAY_CNT)
			{
				bHV = 0;
				ucOVcnt = 0;
				ucOVRcnt = 0;
			}
		}
		else if(uiCellVmax > E2uiOVvol) //> 4.25v
		{
			if(ucOVRcnt > 0)
			{
				ucOVRcnt--;
			}
		}
	}

	if(!bUV)
	{
		if(uiCellVmin < E2uiUVvol) //�ж���û��Ƿѹ��//< 2.7v
		{		
			if(++ucUVcnt >= UV_DELAY_CNT)
			{
				bUV = 1;
				ucUVcnt = 0;
				ucUVRcnt = 0;

	      bChkChgerFlg = 1;//Ƿѹ�Ļ�Ҫ�������
	      bChgerConectFlg = 0;
				ucChgerCnt = 0;
	      REG.AFESCONF1 |= 0x01;//������������
	      TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);	//ʹ�ܳ�������
			}
		}
		else if(uiCellVmin > E2uiUVRvol)//> 3v
		{
			if(ucUVcnt > 0)
			{
				ucUVcnt--;
			}
		}
	}
	else
	{
		if(uiCellVmin > E2uiUVRvol)//3000
		{			
			if(++ucUVRcnt >= UVR_DELAY_CNT)
			{
				bUV = 0;
				ucUVcnt = 0;
				ucUVRcnt = 0;
			}
		}
		else if(uiCellVmin < E2uiUVvol)//2700
		{
			if(ucUVRcnt > 0)
			{
				ucUVRcnt--;
			}
		}
	}
#if debug
		printf("\n VolProtect(void) : bUV=%x,bHV=%x,\n",((uiBatStatus&0x0002)>>1),(uiBatStatus&0x0001));   
#endif	
}


/*******************************************************************************
Function: TempeProtect(void)
Description:  
Input:	 	
Output: 
Others:
*******************************************************************************/
void TempeProtect(void)								//TempNum=0, Support one temperature 
{
	if(!bOTC)//������
	{
		if(uiTempeMax > E2uiTempOTC) //50
		{
			if(++ucOTCcnt >= TEMPE_DELAY_CNT) //120*25 3s
			{
				bOTC = 1;
				ucOTCcnt = 0;
				ucOTCRcnt = 0;
			}
		}
		else if(uiTempeMax < E2uiTempOTCR)//E2uiTempOTCRӦ��Ҫ��E2uiTempOTC��΢�͵㣬��Ϊ�ظ��¶�Ҫ�͵� 45
		{
			if(ucOTCcnt > 0)
			{
				ucOTCcnt--;
			}
		}
	}
	else
	{
		if(uiTempeMax < E2uiTempOTCR) //45 
		{
			if(++ucOTCRcnt >= TEMPER_DELAY_CNT)
			{
				bOTC = 0;
				ucOTCcnt = 0;
				ucOTCRcnt = 0;
			}
		}
		else if(uiTempeMax > E2uiTempOTC)
		{
			if(ucOTCRcnt > 0)
			{
				ucOTCRcnt--;
			}
		}
	}

	if(!bUTC)
	{
		if(uiTempeMin < E2uiTempUTC) //0
		{
			if(++ucUTCcnt >= TEMPE_DELAY_CNT)
			{
				bUTC = 1;
				ucUTCcnt = 0;
				ucUTCRcnt = 0;
			}
		}
		else if(uiTempeMin > E2uiTempUTCR)//5
		{
			if(ucUTCcnt > 0)
			{
				ucUTCcnt--;
			}
		}
	}
	else
	{
		if(uiTempeMin > E2uiTempUTCR)//5
		{
			if(++ucUTCRcnt >= TEMPER_DELAY_CNT)
			{
				bUTC = 0;
				ucUTCcnt = 0;
				ucUTCRcnt = 0;
			}
		}
		else if(uiTempeMin < E2uiTempUTC)//0
		{
			if(ucUTCRcnt > 0)
			{
				ucUTCRcnt--;
			}
		}
	}

	if(!bOTD)
	{
		if(uiTempeMax > E2uiTempOTD)//70  73
		{
			if(++ucOTDcnt >= TEMPE_DELAY_CNT)
			{
#if 0
		printf("\n bOTD = 1���ⲿuiTempeMax=%d,uiTempeMin=%d \n",uiTempeMax,uiTempeMin);    
#endif  		
				bOTD = 1;
				ucOTDcnt = 0;
				ucOTDRcnt = 0;
			}
		}
		else if(uiTempeMax < E2uiTempOTDR)
		{
			if(ucOTDcnt > 0)
			{
				ucOTDcnt--;
			}
		}
	}
	else
	{
		if(uiTempeMax < E2uiTempOTDR) //3311
		{
			if(++ucOTDRcnt >= TEMPER_DELAY_CNT)
			{
#if 0
		printf("\n bOTD = 0���ⲿuiTempeMax=%d,uiTempeMin=%d \n",uiTempeMax,uiTempeMin);    
#endif  	
				bOTD = 0;
				ucOTDcnt = 0;
				ucOTDRcnt = 0;
			}
		}
		else if(uiTempeMax > E2uiTempOTD)//70
		{
			if(ucOTDRcnt > 0)
			{
				ucOTDRcnt--;
			}
		}
	}

	if(!bUTD)
	{
		if(uiTempeMin < E2uiTempUTD)//-10
		{
			if(++ucUTDcnt >= TEMPE_DELAY_CNT)
			{
				bUTD = 1;
				ucUTDcnt = 0;
				ucUTDRcnt = 0;
			}
		}
		else if(uiTempeMin > E2uiTempUTDR)//-5
		{
			if(ucUTDcnt > 0)
			{
				ucUTDcnt--;
			}
		}
	}
	else
	{
		if(uiTempeMin > E2uiTempUTDR)//-5
		{
			if(++ucUTDRcnt >= TEMPER_DELAY_CNT)
			{
				bUTD = 0;
				ucUTDcnt = 0;
				ucUTDRcnt = 0;
			}
		}
		else if(uiTempeMin < E2uiTempUTD)//-10
		{
			if(ucUTDRcnt > 0)
			{
				ucUTDRcnt--;
			}
		}
	}
#if debug
		printf("\n TempeProtect(void) :bUTD=%x,bUTC=%x,bOTD=%x,bOTC=%x \n",((uiBatStatus&0x0800)>>11),((uiBatStatus&0x0400)>>10),((uiBatStatus&0x0200)>>9),((uiBatStatus&0x0100)>>8));   
#endif	
}


/*******************************************************************************
Function: ChkMosStatus(void)
Description:  
Input:	 	
Output: 
Others:
*******************************************************************************/
void ChkMosStatus(void)
{
		U8 bChgerConectFlg_test = 0x00;
    bCHGMOS = 1;
    bDSGMOS = 1;
//��о>4.4v
	if(bAFE_OV || bCTO)//���߱�������Ӳ������												  
	{
		bCHGMOS = 0;
	}
	//�ŵ�״̬��
	//(1)����Ƿѹ���(������͵�ѹ ����)         bUV
	//(2)�����ѹ���⣨�����ѹ��>0.3V ������  OK
	//(3)���˱���˿�Ƿ��۶ϣ��۶ϣ�              OK
	//(4)�ŵ������⣨��������·��              bAFE_SC
	//(5)���¼�⣨���� ������                   bOTD
	//(6)���¼�⣨���� ������                   bUTD
  //    																			 ��ʣ: bOCD ||(bFD&&bDSGEnd) || bLoadConectFlg || bCTO
	//uiPackConfig��ʼ��:00101101 00110111
//�ŵ�״̬�µĴ����־                         //�ŵ��ֹ�ҷŵ��ֹ�رշŵ�mosfet   ��ʼ״̬bLoadConectFlg = 0�� ���״̬��bstatus =0�տ�ʼ����bCTO = 0
																																//bLoadConectFlg��1�Ļ��ͻᱣ����bLoadConectFlgֻ���ڼ�ⷢ���ŵ�Ĵ���״̬�󣬼�⸺����Ȼ�������ӣ��Ż���1
	if( bOTD || bUTD || bUV || bOCD || bOCD2 || bAFE_SC || (bFD&&bDSGEnd) || bLoadConectFlg || bCTO)
	{																																//������Ϊ��·����bLoadConectFlg�Ŀ��ܻ�ȥ����һ����ü�⸺�أ��ɲ�������������
		bDSGMOS = 0;   //�ŵ�mos�ر�
//		PWM2CON &= ~0x01; //pwm��ֹ���	
//		DSG1PWM = 0;
	}
	else
	{
		//tomi
		//��ӷŵ������Ĳ���
		
//		DSG1PWM = 1;
//		if(ucDsgingSpeed != 2)
//		{
//			PWM2CON |= 0x01;
//		}
	}
   //packstatus��ʼ״̬����0
    if(!bDSGING)//���Ƿŵ�״̬
    {
			  //(1)�����ѹ��� (����4.2V��������)                  bHV
				//(2)�����ѹ���⣨�����ѹ��>0.3V ������           OK
				//(3)���˱���˿�Ƿ��۶ϣ��۶ϣ�         							OK
				//(4)����������ӣ�AFE�������⣩                    ����
				//(5)�������ѹ��������Χ��34.2��0.5V��								OK		Input_vol_judge();//�жϳ�����ĳ���ѹ
				//(6)��������⣨����С��10ma ������                �����������ʵ����bFC������
				//(7)�ŵ������⣨�зŵ���� ������!!!!!!!!!!!!!!!!  ��������Ϊ�Ѿ��ǷǷŵ�״̬
				//(8)���¼�⣨���� ������                            bOTC
				//(9)���¼�⣨���� ������									  				bUTC
			  //																										��ʣ:bOCC|| (bFC&&bCHGEnd)
        if(bOTC || bUTC || bHV || bOCC || (bFC&&bCHGEnd))	//���������� ����б���							
        {
#if debug
		printf("�رճ��mos1-----------\n");    
#endif
					bCHGMOS = 0;  //�رճ��mos
        }
    }
		//�ŵ����������Ӳ����·����������־λ��1�����Ѿ����ӱ�־λ��0
		//																											(bChkChgerFlg&&!bChgerConectFlg))
		if(bOCD || bOCD2 || bAFE_SC  || (bChkChgerFlg&&!bChgerConectFlg))//������Ƿ��г�磬Ҳ��Ҫ�ر�Cmos
		{
#if debug
		printf("�رճ��mos2-----------\n");    
#endif
					bCHGMOS = 0;//�رճ��mos
		}
		
#if (tempdebug|chargedebug|discurdebug)
		//ע����ߵ�λ�ֽڷ�ǰ�棬��λ�ֽڴ洢�ں��
		printf("\n ��8λ��0,bCTO=%x,bAFE_SC=%x,bAFE_OV=%x�,bUTD=%x,bUTC=%x,bOTD=%x,bOTC=%x ��8λ 0,0,0,bOCD2=%x,bOCD=%x,bOCC=%x,bUV=%x,bHV=%x,\n",((uiBatStatus&0x4000)>>14),((uiBatStatus&0x2000)>>13),((uiBatStatus&0x1000)>>12),((uiBatStatus&0x0800)>>11),\
		((uiBatStatus&0x0400)>>10),((uiBatStatus&0x0200)>>9),((uiBatStatus&0x0100)>>8),((uiBatStatus&0x0010)>>4),((uiBatStatus&0x0008)>>3),((uiBatStatus&0x0004)>>2),((uiBatStatus&0x0002)>>1),(uiBatStatus&0x0001));   
#endif 	
#if debug
		printf("\n bFD = %x,bFC= %x ,LoadConnet_FLG =%bd,bChgerConectFlg =%bd��bChkChgerFlg = %bd\n",((uiPackStatus&0x0200)>>9),((uiPackStatus&0x0100)>>8),LoadConnet_FLG,(bChgerConectFlg_test|bChgerConectFlg),(bChgerConectFlg_test|bChkChgerFlg));    
#endif 
#if (tempdebug|chargedebug|discurdebug)
		printf("\n ChkMosStatus(void)uiPackStatus=%x,bCHGMOS=%x,bDSGMOS=%x \n",uiPackStatus,(uiPackStatus&0x0001),(uiPackStatus&0x0002)>>1);    
#endif  	
}


/*******************************************************************************
Function: ControlMos(void)
Description:  
Input:	 	
Output: 
Others:
*******************************************************************************/
void ControlMos(void)
{
    if(bCHGMOS)
    {
        REG.AFESCONF2 |= 0x01;   //���mos����λ ����
    }
    else
    {
        REG.AFESCONF2 &= ~0x01;    
    }

    if(bDSGMOS)
    {
        REG.AFESCONF2 |= 0x02;    //�ŵ�mos�Ŀ���
    }
    else
    {
        REG.AFESCONF2 &= ~0x02;    
    }
        
		//�������ֵ��֮ǰһ���Ͳ���д��
	if(REG.AFESCONF2 != AFESCONF2Bk)			//If the two variables of the same, is not written to AFE
	{
		AFESCONF2Bk = REG.AFESCONF2;
    TWIWriteAFE(AFE_SCONF2, &REG.AFESCONF2);	 					
	}


}


/*******************************************************************************
Function: RecoveryOC(void)
Description:  
Input:	 	
Output: 
Others:
*******************************************************************************/
void RecoveryOC(void)
{
	if(bOCRC_EN)//�����Իָ�ʹ��
	{
		if(bOCD || bAFE_SC|| bOCD2)//�ŵ����������־λ,��·��־λ
		{
			if(++uiOCDRcnt > OCDR_DELAY_CNT)//>400*25 =10s
			{
               //��߿���Ҫ���ж�
								bChkLoadFlg = 0;
                ucLoadRCnt = 0;    
								uiOCDRcnt = 0;
								bOCD = 0;
								//by tomi
								ucOCD2cnt	= 0;
								bOCD2 = 0;
                if(bAFE_SC)
                {
    			        	bAFE_SC = 0;
                    REG.AFESCONF1 |= 0x80;//�����־�Ĵ�����������־λ
                    TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);
                    REG.AFESCONF1 &= ~0x80;
                    TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);
                }
			}
		}
		//by tomi 
		if(bOCC | bAFE_OV)
		{			
			if(++uiOCCRcnt > OCCR_DELAY_CNT)
			{
				uiOCCRcnt = 0;
				bOCC = 0;
				//by tomi 
				bAFE_OV = 0;
				
	      bChkChgerRFlg = 0;
				ucChgerRCnt = 0;
			}
		}
	}
}


/*******************************************************************************
Function: CheckLoad(void)
Description:  
Input:	 	
Output: 
Others:
*******************************************************************************/
void CheckLoad(void)
{
	 U8 bLoadConectFlg_test = 0x00;
    if(!bChkLoadFlg)//��⸺�ر�־λû�п���
    {
        if(bUV) //Ƿѹ��
        {
            bLoadConectFlg = 1;
            bUVBkFlg = 1;
        }
        else if(bUVBkFlg && !bUV) //�ϴ�Ƿѹ�ˣ���β�Ƿѹ��-���Ͽ�mos��Ƿѹ�ˣ��Ϳ�ʼ���ؼ��
        {
						bUVBkFlg = 0;
            bChkLoadFlg = 1;
						ucLoadRCnt = 0;
						REG.AFESCONF1 |= 0x02; //���ؼ��
						TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);	 				//enable Load Detect	
        }

        if(bOCD || bAFE_SC ||bOCD2)//�ŵ����������־λ����·��־λ
        {
            bChkLoadFlg = 1;
            bLoadConectFlg = 1; //�������⼸�������˻��и������ӣ��ͼ����Ͽ�Dmos
						ucLoadRCnt = 0;
						REG.AFESCONF1 |= 0x02;//�������ؼ��
						TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);	//ʹ�ܸ��ؼ��
        }
    }
		//ֻ���ڹرգ�bChkLoadFlg��1�ˣ���Dmos�ر��ˣ����ܼ����û�и���
    else 
    {
    	  TWIReadAFE(AFE_BSTATUS, &REG.AFEBSTATUS);
        if((REG.AFEBSTATUS&0x02) == 0)//��������״̬λ 1���������� 0δ���Ӹ���
        {
            if(++ucLoadRCnt >= (E2ucDelayLoadR*5-1))             //4*125mS
            {
                bChkLoadFlg = 0;
                bLoadConectFlg = 0; 
								bChkChgerFlg = 0;
								bOCD = 0;//�ŵ��������
						  	bOCD2 = 0;
                ucLoadRCnt = 0;    
								REG.AFESCONF1 &= ~0x02; //�رո��ؼ�⹦��
								TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);	 				// close Load Detect
							if(bAFE_SC)
							{
										bAFE_SC = 0;//��ռ���־λ
										REG.AFESCONF1 |= 0x80;
										TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);
										REG.AFESCONF1 &= ~0x80;
										TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);
							}
            }
        }
    }
		LoadConnet_FLG = (bLoadConectFlg_test|bLoadConectFlg);
#if debug
		printf("\n -----bLoadConectFlg =%bd----- \n",(bLoadConectFlg_test|bLoadConectFlg));    
#endif

}


/*******************************************************************************
Function: CheckCharger(void)
Description:  
Input:	 	
Output: 
Others:
*******************************************************************************/
void CheckCharger(void)
{
	U8 bChgerConectFlg_test = 0x00;
    if(bChkChgerFlg)
	{
    	  TWIReadAFE(AFE_BSTATUS, &REG.AFEBSTATUS);
        if((REG.AFEBSTATUS&0x01) != 0)//���������״̬��1��������
        {
					if(++ucChgerCnt >= E2ucDelayLoadR*5)
					{
						bChkChgerFlg = 0;
						bChgerConectFlg = 1;
						ucChgerCnt = 0;
						REG.AFESCONF1 &= ~0x01;//��������λ��0
						TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);	 				// close Charge detect		
		
					}
				}
	}

    if(bChkChgerRFlg)//�������������������Լ����õĲ�����
    {
    	  TWIReadAFE(AFE_BSTATUS, &REG.AFEBSTATUS);
        if((REG.AFEBSTATUS&0x01) == 0)//�����δ����
        {
            if(++ucChgerRCnt >= E2ucDelayLoadR*5)             //4*125mS
            {
              bChkChgerRFlg = 0;
							bOCC = 0;					
              ucChgerRCnt = 0;    
        	    REG.AFESCONF1 &= ~0x01;
            	TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);	 				// close Charge detect
							
							//by tomi
							if(bAFE_SC)
							{
									bAFE_OV = 0;//��ռ���־λ
									REG.AFESCONF1 |= 0x80;
									TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);
									REG.AFESCONF1 &= ~0x80;
									TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);
							}
            }
        }
	}
#if debug
		printf("\n -----���������bChgerConectFlg =%bd��bChkChgerFlg = %bd----- \n",(bChgerConectFlg_test|bChgerConectFlg),(bChgerConectFlg_test|bChkChgerFlg));    
#endif
}


/*******************************************************************************
Function: VolProcess()
Description:  
Input:	 	
Output: 
Others:
*******************************************************************************/	
void VolProcess(void)
{
	if(!bBalancingFlg)
	{
		//�ж��Ƿ����
	    if(!bFC)//����ֹ��־λ
	    {
				//0x010,0x068,����ѹ����4.2v �ҵ���ҪС�� 100mA �����״̬
	     	if((uiCellVmax>E2uiChgEndVol) && (Info.slCurr<=E2siChgEndCur))   //charge end voltage  ����4.2VС��100ma 1s*5 
	    	{
					//E2ucDelayChgEnd  0x005,								// U8  xdata DelayChgEnd   		_at_ 	CHG_PARA_MAP_ADDR+10;	5
	    		if(++ucChgEndcnt >= E2ucDelayChgEnd)
	    		{
	    			bFC = 1;
#if debug
		printf("\n -----bFC ����ֹ-----uiCellVmax = %d \n",uiCellVmax);    
#endif
	    			ucChgEndcnt = 0;
	          ucChgEndRcnt = 0;
	    		}
	    	}
	        else
	        {
						if(ucChgEndcnt > 0)
						{
							ucChgEndcnt--;
						}
	        }
	    }
	    else
	    {
	    	if(uiCellVmin < E2uiChgEndVol) //charge end recover voltage
	    	{
	    		if(++ucChgEndRcnt >= E2ucDelayChgEnd)
	    		{
	    			bFC = 0;
	          ucChgEndcnt = 0;
	    			ucChgEndRcnt = 0;
	    		}
	    	}
	        else
	        {
						if(ucChgEndRcnt > 0)
						{
							ucChgEndRcnt--;
						}
	        }
	    }
	//�ŵ��ֹ
	    if(!bFD)
	    {
				//0x00B,0x054,	2.9V
	    	if(uiCellVmin<E2uiDsgEndVol)	  //discharge end voltage  �ŵ��ֹ1s*5��С��2.9V
	    	{
	    		if(++ucDsgEndcnt >= E2ucDelayDsgEnd)
	    		{
	    			bFD = 1;
#if debug
		printf("\n -----bFD �ŵ��ֹ----- \n");    
#endif
	    			ucDsgEndcnt = 0;
	          ucDsgEndRcnt = 0;
	    		}
	    	}
	        else
	        {
	    		if(ucDsgEndcnt > 0)
	    		{
	    			ucDsgEndcnt--;
	    		}
	        }
	    }
	    else
	    {
	    	if(uiCellVmin > E2uiDsgEndVol)	   //discharge end recover voltage
	    	{										  
	    		if(++ucDsgEndRcnt >= E2ucDelayDsgEnd)
	    		{
	    			bFD = 0;
	    			ucDsgEndcnt = 0;
	    			ucDsgEndRcnt = 0;
	    		}
	    	}
	        else
	        {
						if(ucDsgEndRcnt > 0)
						{
							ucDsgEndRcnt--;
						}
	        }
	    }
	}
#if debug
		printf("\n -----VolProcess(void)-----bFD = %x,bFC= %x\n",((uiPackStatus&0x0200)>>9),((uiPackStatus&0x0100)>>8));    
#endif	
}


/*******************************************************************************
Function: CurrProtect(void)
Description:  
Input:	 	
Output: 
Others:
*******************************************************************************/
void CurrProtect(void)
{
	if(!bOCC)//û�з��� ��������־λ
    {
			//0x000,0x000,0x00B,0x0B8,		 3000mA ������
			if(slCadcCurrent > E2slOCCvol)			   //CADC  cycle is 64ms �����Լ����õĹ���ֵ
			{
							if(++ucOCCcnt > OCC_DELAY_CNT)
							{
								bOCC = 1;
								ucOCCcnt = 0;

								bChkChgerRFlg = 1;//���¼���Ƿ���
								ucChgerRCnt = 0;
								REG.AFESCONF1 |= 0x01;
								TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);	 				// close VADC
							}
			}
			else
			{
								if(ucOCCcnt > 0)
								{
										ucOCCcnt--;	
								}
			}
    }
//�ŵ��������λ
	if(!bOCD)
    {
				//0xFF,0xFF,0x0B1,0x0E0,  
			if(slCadcCurrent < E2slOCDvol)			   //CADC  cycle is 64ms
			{
							if(++ucOCDcnt > OCD_DELAY_CNT)
							{
									ucOCDcnt = 0;
									bOCD = 1;
							}
			}
			else
			{
					if(ucOCDcnt > 0)
					{
						ucOCDcnt--;	
					}
			}
		}			
//by tomi ������������
//�ŵ��������λ
	if(!bOCD2)
    {			
			if(slCadcCurrent < E2slOCD2vol)			   //CADC  40A ��ʱ100ms
			{
					if(++ucOCD2cnt > OCD2_DELAY_CNT)
					{
							ucOCD2cnt = 0;
							bOCD2 = 1;
					}
			}
			else
			{
					if(ucOCD2cnt > 0)
					{
						ucOCD2cnt--;	
					}
			}
		}		

		
}


/*******************************************************************************
Function: CurrProcess(void)
Description:  
Input:	 	
Output: 
Others:
*******************************************************************************/
#define AverTime  4
void CurrProcess(void)
{
	U8 i;
	S32 Tempdata=0;

	TWIReadAFE(AFE_CURH, (U8 xdata *)&AFE.siCurr);	//ͨ��TWI��ȡCADC�ɼ��ĵ���ֵ
	
#if 0
	if((AFE.siCurr&0x1000) != 0)					//����bit12�ж��Ƿ�Ϊ����(�ŵ����Ϊ��ֵ)����Ϊ��ֵ��λ��0
	{
		AFE.siCurr|=0xE000; //1110 0000 0000 0000 ��12λ��1��ʾ�ŵ磬0��ʾ���
	}
	slCadcCurrent = (S32)CALICUR*(AFE.siCurr-E2siCadcOffset)/E2siCadcGain;

	slCadcCurBuf[ucCadcCnt] = slCadcCurrent;		//�������ɼ���16�ε���ȡƽ��ֵ����Ϊ��ǰ����ֵ
	if(++ucCadcCnt >= 16)
	{
		ucCadcCnt = 0;
	}
	for(i=0; i<16; i++)
	{
		Tempdata += slCadcCurBuf[i];
	}
	slCadcCurAverage = Tempdata/16;	
	
#else
	
	//slCadcCurrent = AFE.siCurr&(~0xE000);
	if((AFE.siCurr&0x1000) != 0)					//����bit12�ж��Ƿ�Ϊ����(�ŵ����Ϊ��ֵ)����Ϊ��ֵ��λ��1
	{
		AFE.siCurr|=0xE000; //1110 0000 0000 0000 ��12λ��1��ʾ�ŵ磬0��ʾ���
	}
#if (chargedebug|discurdebug)
		printf("\n ���ε���ֵAFE.siCurr = %d \n",AFE.siCurr);    
#endif		
	//slCadcCurrent = (S32)AFE.siCurr;
	slCadcCurrent = (S32)CALICUR*(AFE.siCurr-E2siCadcOffset)/E2siCadcGain;   //       (AFE.siCurr-3)/ (-82)
#if (chargedebug|discurdebug)
		printf("\n ����----------------slCadcCurrent ------------------= %ld \n",slCadcCurrent);    
#endif	
	slCadcCurBuf[ucCadcCnt] = slCadcCurrent;
	if(AverTime <= 16)//���ڴ泬��
	{	
		if(++ucCadcCnt >= AverTime)
		{
			ucCadcCnt = 0;
		}
		if(ucCadcCnt == 0)//������16��
		{
			for(i=0; i<AverTime; i++)
			{
				Tempdata += slCadcCurBuf[i];
			}
			slCadcCurAverage = Tempdata/AverTime;
		}
	}	
#endif  
#if (chargedebug|discurdebug)
		printf("\n ƽ������ CurrProcess(void): slCadcCurAverage = %ld \n",slCadcCurAverage);    
#endif	

	bDSGING = 0;
	bCHGING = 0;
	//< -100mA  
	if(slCadcCurAverage < (-E2siDfilterCur))//�ŵ�״̬��ƽ����������E2siDfilterCur
	{
#if debug
		printf("\n С�� -100mA \n");    
#endif	
		bDSGING = 1; //���ڷŵ� 
		UART_IRQ_DISABLE;
		Info.slCurr = slCadcCurAverage;
		UART_IRQ_ENABLE;
	}
	//rs2 -> rs1���״̬���������Ϊ��
	else if(slCadcCurAverage > E2siDfilterCur) //100
	{
#if debug
		printf("\n ���� 100mA \n");    
#endif	
		bCHGING = 1;	 //���ڳ��	
		UART_IRQ_DISABLE;
		Info.slCurr = slCadcCurAverage;
		UART_IRQ_ENABLE;
	}
	else
	{
		UART_IRQ_DISABLE;
		Info.slCurr = 0;
		UART_IRQ_ENABLE;
	}
//�����ڳ�磬Ҫ���Ķ�����һ����LEDָʾ
    if(bCHGING)						//debounce: flick led charging
    {
#if chargedebug
				printf("\n is charging \n");
#endif
//        if(!bLEDChgFlg)
//        {
//            if(++ucLedChgCnt > 8)
//            {
//                bLEDChgFlg = 1;
//                ucLedChgCnt = 0;
//            }
//        }
//        else
//        {
//            if(ucLedChgCnt > 0)
//            {
//                ucLedChgCnt--;
//            }
//        }
    }
    else //���ڳ��
    {
#if chargedebug
				printf("\n is  not charging \n");
#endif
//        if(!bLEDChgFlg)
//        {
//            if(ucLedChgCnt > 0)
//            {
//                ucLedChgCnt--;
//            }
//        }
//        else
//        {
//            if(++ucLedChgCnt > 8)
//            {
//                bLEDChgFlg = 0;
//                ucLedChgCnt = 0;
//            }
//        }    
    }
#if 0
				printf("\n CurrProcess uiPackStatus =%hx ---��8λ��bCAL =%x,0,0,0,0,bVDQ=%x,bFD=%x,bFC=%x ��8λ:0,bFastDischarge=%x,bMidDischarge=%x,bSlowDischarge=%x,bDSGING=%x,bCHGING=%x,bDSGMOS=%x,bCHGMOS=%x\n",uiPackStatus,((uiPackStatus&0x8000)>>15),((uiPackStatus&0x0400)>>10),((uiPackStatus&0x0200)>>9),((uiPackStatus&0x0100)>>8),\
		((uiPackStatus&0x0040)>>6),((uiPackStatus&0x0020)>>5),((uiPackStatus&0x0010)>>4),((uiPackStatus&0x0008)>>3),((uiPackStatus&0x0004)>>2),((uiPackStatus&0x0002)>>1),(uiPackStatus&0x0001));      
#endif
	CurrProtect();
}


/*******************************************************************************
Function: AFERamCheck(void)
Description: 
Input:	 	
Output: 
Others:
*******************************************************************************/
void AFERamCheck(void)
{
	U8 xdata RdBuf[2];
	U8 i;

	for(i=3; i<=11; i=i+2)		//�ӿ�д�Ĵ���03H��ʼ�Ƚ�   // 03H  05H 07H 09H 0BH
	{
		TWIReadAFE(i, RdBuf); 

        if((RdBuf[0]!=*(&REG.AFEFLAG1+i)) || (RdBuf[1]!=*(&REG.AFEFLAG1+i+1)))
        {
					if(i != 7) //07H������λ
					{
									InitAFE();		
									break;
					}
        }
	}
}


/*******************************************************************************
Function: AlarmProcess(void)
Description:  
Input:	NULL 	
Output: NULL
Others:
*******************************************************************************/
void AlarmProcess(void)
{
	TWIReadAFE(AFE_FLAG1, &REG.AFEFLAG1);	//��ȡAFE�Ĵ���FLAG1 & FLAG2�е�Alarm��־

	if((REG.AFEFLAG2&0x02) != 0)	//CADC ����ת����� 
	{
		bCADCFlg = 1;
#if debug
		printf("\n Cur trans OK! \n");    
#endif	
	}

	if((REG.AFEFLAG2&0x01) != 0)	//VADC ��ѹת�����
	{
		bVADCFlg = 1;
#if debug
		printf("\n VOL trans OK! \n");    
#endif	
	}

	if((REG.AFEFLAG1&0x08) != 0)	//AFE ��·����
	{
		bAFE_SC = 1;
		Info.uiPackStatus = uiPackStatus;
	}

	if((REG.AFEFLAG1&0x04) != 0)	//AFE ��ѹ����
	{
		bAFE_OV = 1;
		//by tomi
		bChkChgerRFlg = 1; //�Իָ�
		Info.uiPackStatus = uiPackStatus;
	}

	if((REG.AFEFLAG2&0x04) != 0)	//�ж�FLAG2��bit2�Ƿ�Ϊ1����Ϊ1���ʾAFE������LVR����Ҫ���³�ʼ��AFE�Ĵ���
	{
		InitAFE();
#if debug
		printf("AFE��rst������LVR \n");    
#endif	
	}

#if debug
		printf("\n uiPackStatus =%hx ---��8λ��bCAL =%x,0,0,0,0,bVDQ=%x,bFD=%x,bFC=%x ��8λ:0,bFastDischarge=%x,bMidDischarge=%x,bSlowDischarge=%x,bDSGING=%x,bCHGING=%x,bDSGMOS=%x,bCHGMOS=%x\n",uiPackStatus,((uiPackStatus&0x8000)>>15),((uiPackStatus&0x0400)>>10),((uiPackStatus&0x0200)>>9),((uiPackStatus&0x0100)>>8),\
		((uiPackStatus&0x0040)>>6),((uiPackStatus&0x0020)>>5),((uiPackStatus&0x0010)>>4),((uiPackStatus&0x0008)>>3),((uiPackStatus&0x0004)>>2),((uiPackStatus&0x0002)>>1),(uiPackStatus&0x0001));   

#endif 
	
}	
	

/*******************************************************************************
Function:WaitADCConvert(void) 
Description:  
Input:	NULL 	
Output: NULL
Others:
*******************************************************************************/
bit WaitADCConvert(void)
{
    U8 i;
    BOOL result=0;

    while(i++ < 60)
    {
        Delay1ms(5);
    	TWIReadAFE(AFE_FLAG1, &REG.AFEFLAG1);		//read AFE FLAG1 search for which state  trigged ALARM
    	if((REG.AFEFLAG2&0x01) != 0)				//VADC interrupt
    	{
			while(i++ < 60)
    		{
		        Delay1ms(5);
		    	TWIReadAFE(AFE_FLAG1, &REG.AFEFLAG1);		//read AFE FLAG1 search for which state  trigged ALARM
		    	if((REG.AFEFLAG2&0x02) != 0)				//CADC interrupt
		    	{
		    		result = 1;
		            break;
		    	}
    		}
            break;
    	}
    }

    return result;
}


/*******************************************************************************
Function: PorOnProtect(void)
Description:  
Input:	NULL 	
Output: NULL
Others:
*******************************************************************************/
void PorOnProtect(void)
{
	//��о����	E2uiOVvol > 0x010,0x09A,4250 > 4.25V -->4.175
	if(uiCellVmax > E2uiOVvol)//E2uiOVvol����籣����ֵ    
	{		
		bHV = 1;
#if debug
		TI = 0;
		TI = 1;	
		printf("\n --����bHV-- \n");   
#endif  
	}
 //E2uiUVvol = 0x00A,0x08C,	2.7v
	if(uiCellVmin < E2uiUVvol)//Ƿѹ������ֵ
	{		
		bUV = 1;
	  bChkChgerFlg = 1;//���������־��1
	  bChgerConectFlg = 0;//�����ӱ�־���
		ucChgerCnt = 0;
		//ʹ�ܳ�������
		REG.AFESCONF1 |= 0x01;
		TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);	 				//enable charger detect 
#if debug
		printf("\n --ǷѹbUV--\n");   
#endif  
	}

	if(uiTempeMax > E2uiTempOTC)//	0x00C,0x09F,						// U16 xdata TempOTC      		_at_ 	CHG_TEMP_PARA_MAP_ADDR;		3231-2731=500
	{
		bOTC = 1;
#if debug
		printf("\n --�����bOTC--\n");   
#endif  
	}

	if(uiTempeMin < E2uiTempUTC) //0���϶�
	{
		bUTC = 1;
#if debug
		printf("\n --�����bUTC-\n");   
#endif  
	}
	if(uiTempeMax > E2uiTempOTD)//70 ���϶�    3461
	{
		bOTD = 1;
#if debug
		printf("\n --�Ÿ���bOTD-\n");   
#endif 
	}

	if(uiTempeMin < E2uiTempUTD)//-10���϶� 
	{
		bUTD = 1;
#if debug	
		printf("\n --�ŵ���bUTD--\n");   
#endif 
	}
//
//sbit bHV			=	uiBatStatus^8;//���屣����־λ
//sbit bUV			=	uiBatStatus^9;//���ű�����־λ
//sbit bOCC 			= 	uiBatStatus^10;//������������־λ 1����������
//sbit bOCD 			= 	uiBatStatus^11;//�ŵ����������־λ
//sbit bOCD2 			= 	uiBatStatus^12;//�ŵ����2������־λ
	
//sbit bOTC 			=	uiBatStatus^0;//�����±���
//sbit bOTD 			= 	uiBatStatus^1;//�ŵ���±���
//sbit bUTC			=	uiBatStatus^2;//�����±���
//sbit bUTD			=	uiBatStatus^3;//�ŵ���±���
//sbit bAFE_OV		=	uiBatStatus^4;//Ӳ�����屣��
//sbit bAFE_SC		=	uiBatStatus^5;//Ӳ����·����
//sbit bCTO			=	uiBatStatus^6; //Ԥ��λ�����߱���

#if debug
		TI = 0;
		TI = 1;	
		printf("\n PorOnProtect() uiBatStatus = %x\n",uiBatStatus);   
#endif  

#if debug
		//ע����ߵ�λ�ֽڷ�ǰ�棬��λ�ֽڴ洢�ں���
		printf("\n ��8λ��0,bCTO=%x,bAFE_SC=%x,bAFE_OV=%x,bUTD=%x,bUTC=%x,bOTD=%x,bOTC=%x ��8λ 0,0,0,bOCD2=%x,bOCD=%x,bOCC=%x,bUV=%x,bHV=%x,\n",((uiBatStatus&0x4000)>>14),((uiBatStatus&0x2000)>>13),((uiBatStatus&0x1000)>>12),((uiBatStatus&0x0800)>>11),\
		((uiBatStatus&0x0400)>>10),((uiBatStatus&0x0200)>>9),((uiBatStatus&0x0100)>>8),((uiBatStatus&0x0010)>>4),((uiBatStatus&0x0008)>>3),((uiBatStatus&0x0004)>>2),((uiBatStatus&0x0002)>>1),(uiBatStatus&0x0001));   
#endif  
	
		/*********by tomi********/

		Input_charger_detect_poron();//�жϳ������û�в� 

		if(ConectFlg_poron == 1)
		{
#if 1 
			printf("\n �����������\n");
#endif
			Input_vol_judge_poron();//�жϳ�����ĳ���ѹ	
		}	
		else//����������������,��մ����־λ
		{			
#if 1 
			printf("\n ���OIV UIV\n");
#endif			
			OIV = 0;
			UIV = 0;
		}
		
	 // Chk_Fuse_poron(); //�����û���۶ϣ��ϵ�˲�䲻����۶ϣ�
	  Chk_Cell_VolDIFF_poron();//ѹ���

		MCU_CHK_CDmos();
	/***************************/

    ChkMosStatus();				 
    ControlMos();
#if 1
		updata_pack_fault_flag();
#endif
}


/*******************************************************************************
Function: ProtectProcess(void)
Description:  
Input:	 	
Output: 
Others:
*******************************************************************************/
void ProtectProcess(void)
{
	//���Ƕ��߻�������ƽ��״̬��
	if(!(bCellOpenDecFlag || bBalanceFlg))//�ڶ��߼���ƽ���״̬�£�������ѹ����
	{
		//��ѹ��������bHV��
		VolProtect();			
	}
		//�¶ȱ�������
		TempeProtect();		
//-------------------------------------------------------------------------------------------------	
#if 0
		/*********by tomi********/
	//���������� ��1����~(AFE_CHG_MOS  = 1��IO_CHG_MOS= 0) ������2�����ϴ�û�г��������г������ ������һ���ǳ��mosû�п���
		if(!((IO_CHG_MOS == 0) && ((uiPackStatus&0x0001) == 1)) )//�ڶ��رյ�״̬��
		{
#if debug
			printf(" \n");
#endif
			Input_charger_detect_poron();//�жϳ������û�в�
			if(ConectFlg_poron == 1)
			{
				Input_vol_judge();//�жϳ�����ĳ���ѹ
			}	
			else
			{
				OIV = 0;
				UIV = 0;
			}
		}
#endif	
		/*********by tomi********/
//���������� ��1����~(AFE_CHG_MOS  = 1��IO_CHG_MOS= 0) ������2�����ϴ�û�г��������г������ ������һ���ǳ��mosû�п���
		if(bCHGING == 0)//������ڳ����
		{		
			if(++CHGING_cnt >= 4)
			{
				CHGING_cnt = 0;
				IO_CHG_MOS = 1;//�رճ��mos
				Input_charger_detect();//�жϳ������û�в�
				if(ConectFlg_poron == 1)
				{
					//1.CMOSû�д�
					if(!((IO_CHG_MOS == 0) && ((uiPackStatus&0x0001) == 1)) )//�ڶ��رյ�״̬��
					{
#if chargedebug
						printf("Input_vol_judge��ʱ���\n");
#endif	
						if(ConectFlg_poron_prev == 1)//
						{
							Input_vol_judge();//�жϳ�����ĳ���ѹ������ʱ���ж�
						}
						else//����ϴ��ǵ�һ�β�
						{
							Input_vol_judge_poron();//��Ϊֻ�ɼ�һ�����Բ��õȴ��ָ�
						}
								
					}	
					//2.CMOS����
	//				else
	//				{
	//					//��Ҫ�жϣ���һ���Ƿ�û�� �����
	//					if(ConectFlg_poron_prev == 0)
	//					{
	//#if chargedebug
	//						printf("Input_vol_judge_poron���μ��\n");
	//#endif							
	//						Input_vol_judge_poron();//���μ��
	//					}
	//				}
				}
				else
				{	
#if chargedebug
						printf("&&&&&&&&&&&&&&2\n");
#endif				
					OIV = 0;
					UIV = 0;
				}			
			ConectFlg_poron_prev = ConectFlg_poron;
#if chargedebug
			printf("ConectFlg_poron_prev2=%bd\n",ConectFlg_poron_prev);
#endif	
			}			
		}		
		else
		{
			CHGING_cnt = 0;
		}
		
	  Chk_Fuse(); //�����û���۶�
  	Chk_Cell_VolDIFF();


		
		MCU_CHK_CDmos();
//-------------------------------------------------------------------------------------------------	
	
	//mos״̬���ʹ���
    ChkMosStatus();		
    ControlMos(); 
#if 1
		updata_pack_fault_flag(); //25msִ��һ��
#endif
	
		Info.uiBatStatus = uiBatStatus;
		Info.uiPackStatus = uiPackStatus;
//�����Իָ�ʹ��
    RecoveryOC();

    CheckLoad();
//���ϵ�ȥ��������Ƿ񱻲���
    CheckCharger();

}


/*******************************************************************************
Function: IntoPowerDown(void)
Description:  
Input:	 	
Output: 
Others:	   FOR UART 
*******************************************************************************/
void IntoPowerDown(void)
{
	REG.AFESCONF10 = 0x33;						//����Power Downģʽ
	TWIWriteAFE(AFE_SCONF10, &REG.AFESCONF10);
	REG.AFESCONF1 |= 0x20; 
	TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);
}

