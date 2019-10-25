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
		if(uiCellVmax < E2uiOVRvol)//»Ö¸´ < 4.15v
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
		if(uiCellVmin < E2uiUVvol) //ÅÐ¶ÏÓÐÃ»ÓÐÇ·Ñ¹ÁË//< 2.7v
		{		
			if(++ucUVcnt >= UV_DELAY_CNT)
			{
				bUV = 1;
				ucUVcnt = 0;
				ucUVRcnt = 0;

	      bChkChgerFlg = 1;//Ç·Ñ¹µÄ»°Òª¼ì²â³äµçÆ÷
	      bChgerConectFlg = 0;
				ucChgerCnt = 0;
	      REG.AFESCONF1 |= 0x01;//¿ªÆô³äµçÆ÷¼ì²â
	      TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);	//Ê¹ÄÜ³äµçÆ÷¼ì²â
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
	if(!bOTC)//³äµç¸ßÎÂ
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
		else if(uiTempeMax < E2uiTempOTCR)//E2uiTempOTCRÓ¦¸ÃÒª±ÈE2uiTempOTCÉÔÎ¢µÍµã£¬ÒòÎª»Ø¸´ÎÂ¶ÈÒªµÍµã 45
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
		printf("\n bOTD = 1£ºÍâ²¿uiTempeMax=%d,uiTempeMin=%d \n",uiTempeMax,uiTempeMin);    
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
		printf("\n bOTD = 0£ºÍâ²¿uiTempeMax=%d,uiTempeMin=%d \n",uiTempeMax,uiTempeMin);    
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
//µçÐ¾>4.4v
	if(bAFE_OV || bCTO)//¶ÏÏß±£»¤£¬ºÍÓ²¼þ¹ý³å												  
	{
		bCHGMOS = 0;
	}
	//·Åµç×´Ì¬£º
	//(1)µ¥ÌåÇ·Ñ¹¼ì²â(µÍÓÚ×îµÍµçÑ¹ ´¥·¢)         bUV
	//(2)µ¥Ìå¼äÑ¹²î¼ì²â£¨µ¥Ìå¼äÑ¹²î>0.3V ´¥·¢£©  OK
	//(3)Èý¶Ë±£ÏÕË¿ÊÇ·ñÈÛ¶Ï£¨ÈÛ¶Ï£©              OK
	//(4)·ÅµçµçÁ÷¼ì²â£¨¹ýÁ÷£¬¶ÌÂ·£©              bAFE_SC
	//(5)¸ßÎÂ¼ì²â£¨³¬ÎÂ ´¥·¢£©                   bOTD
	//(6)µÍÎÂ¼ì²â£¨³¬ÎÂ ´¥·¢£©                   bUTD
  //    																			 »¹Ê£: bOCD ||(bFD&&bDSGEnd) || bLoadConectFlg || bCTO
	//uiPackConfig³õÊ¼»¯:00101101 00110111
//·Åµç×´Ì¬ÏÂµÄ´íÎó±êÖ¾                         //·Åµç½ØÖ¹ÇÒ·Åµç½ØÖ¹¹Ø±Õ·Åµçmosfet   ³õÊ¼×´Ì¬bLoadConectFlg = 0£¬ µç³Ø×´Ì¬¸Õbstatus =0¸Õ¿ªÊ¼ËùÒÔbCTO = 0
																																//bLoadConectFlgÊÇ1µÄ»°¾Í»á±£»¤£¬bLoadConectFlgÖ»ÓÐÔÚ¼ì²â·¢Éú·ÅµçµÄ´íÎó×´Ì¬ºó£¬¼ì²â¸ºÔØÒÀÈ»ºó¸ºÔØÁ¬½Ó£¬²Å»áÖÃ1
	if( bOTD || bUTD || bUV || bOCD || bOCD2 || bAFE_SC || (bFD&&bDSGEnd) || bLoadConectFlg || bCTO)
	{																																//ºóÆÚÒòÎª¶ÌÂ·´¥·¢bLoadConectFlgµÄ¿ÉÄÜ»áÈ¥³ýÕâÒ»Ïî£¬²»ÓÃ¼ì²â¸ºÔØ£¬ÓÉ²åÈëÊÊÅäÆ÷»½ÐÑ
		bDSGMOS = 0;   //·Åµçmos¹Ø±Õ
//		PWM2CON &= ~0x01; //pwm½ûÖ¹Êä³ö	
//		DSG1PWM = 0;
	}
	else
	{
		//tomi
		//Ìí¼Ó·ÅµçÕý³£µÄ²Ù×÷
		
//		DSG1PWM = 1;
//		if(ucDsgingSpeed != 2)
//		{
//			PWM2CON |= 0x01;
//		}
	}
   //packstatus³õÊ¼×´Ì¬¶¼ÊÇ0
    if(!bDSGING)//²»ÊÇ·Åµç×´Ì¬
    {
			  //(1)µ¥Ìå¹ýÑ¹¼ì²â (´óÓÚ4.2V´¥·¢±£»¤)                  bHV
				//(2)µ¥Ìå¼äÑ¹²î¼ì²â£¨µ¥Ìå¼äÑ¹²î>0.3V ´¥·¢£©           OK
				//(3)Èý¶Ë±£ÏÕË¿ÊÇ·ñÈÛ¶Ï£¨ÈÛ¶Ï£©         							OK
				//(4)³äµçÆ÷ÒÑÁ¬½Ó£¨AFE³äµçÆ÷¼ì²â£©                    ´ý¶¨
				//(5)³äµçÆ÷µçÑ¹ÔÚÕý³£·¶Î§£¨34.2¡À0.5V£©								OK		Input_vol_judge();//ÅÐ¶Ï³äµçÆ÷µÄ³äµçµçÑ¹
				//(6)³äµçµçÁ÷¼ì²â£¨µçÁ÷Ð¡ÓÚ10ma ´¥·¢£©                ´ý¶¨£¬Õâ¸öÆäÊµ¾ÍÊÇbFC³äÂúÁË
				//(7)·ÅµçµçÁ÷¼ì²â£¨ÓÐ·ÅµçµçÁ÷ ´¥·¢£©!!!!!!!!!!!!!!!!  ´ý¶¨£¬ÒòÎªÒÑ¾­ÊÇ·Ç·Åµç×´Ì¬
				//(8)¸ßÎÂ¼ì²â£¨³¬ÎÂ ´¥·¢£©                            bOTC
				//(9)µÍÎÂ¼ì²â£¨³¬ÎÂ ´¥·¢£©									  				bUTC
			  //																										»¹Ê£:bOCC|| (bFC&&bCHGEnd)
        if(bOTC || bUTC || bHV || bOCC || (bFC&&bCHGEnd))	//·¢Éú¹ý±¨´í ³äµçÖÐ±¨´í							
        {
#if debug
		printf("¹Ø±Õ³äµçmos1-----------\n");    
#endif
					bCHGMOS = 0;  //¹Ø±Õ³äµçmos
        }
    }
		//·Åµç¹ýÁ÷±£»¤£¬Ó²¼þ¶ÌÂ·±£»¤£¬¼ì²â±êÖ¾Î»ÊÇ1»¹ÓÐÒÑ¾­Á¬½Ó±êÖ¾Î»ÊÇ0
		//																											(bChkChgerFlg&&!bChgerConectFlg))
		if(bOCD || bOCD2 || bAFE_SC  || (bChkChgerFlg&&!bChgerConectFlg))//¼ì²â³äµçÊÇ·ñÓÐ³äµç£¬Ò²ÐèÒª¹Ø±ÕCmos
		{
#if debug
		printf("¹Ø±Õ³äµçmos2-----------\n");    
#endif
					bCHGMOS = 0;//¹Ø±Õ³äµçmos
		}
		
#if (tempdebug|chargedebug|discurdebug)
		//×¢ÒâÕâ±ßµÍÎ»×Ö½Ú·ÅÇ°Ãæ£¬¸ßÎ»×Ö½Ú´æ´¢ÔÚºóÃ
		printf("\n µÍ8Î»£º0,bCTO=%x,bAFE_SC=%x,bAFE_OV=%xæ,bUTD=%x,bUTC=%x,bOTD=%x,bOTC=%x ¸ß8Î» 0,0,0,bOCD2=%x,bOCD=%x,bOCC=%x,bUV=%x,bHV=%x,\n",((uiBatStatus&0x4000)>>14),((uiBatStatus&0x2000)>>13),((uiBatStatus&0x1000)>>12),((uiBatStatus&0x0800)>>11),\
		((uiBatStatus&0x0400)>>10),((uiBatStatus&0x0200)>>9),((uiBatStatus&0x0100)>>8),((uiBatStatus&0x0010)>>4),((uiBatStatus&0x0008)>>3),((uiBatStatus&0x0004)>>2),((uiBatStatus&0x0002)>>1),(uiBatStatus&0x0001));   
#endif 	
#if debug
		printf("\n bFD = %x,bFC= %x ,LoadConnet_FLG =%bd,bChgerConectFlg =%bd£¬bChkChgerFlg = %bd\n",((uiPackStatus&0x0200)>>9),((uiPackStatus&0x0100)>>8),LoadConnet_FLG,(bChgerConectFlg_test|bChgerConectFlg),(bChgerConectFlg_test|bChkChgerFlg));    
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
        REG.AFESCONF2 |= 0x01;   //³äµçmos¿ØÖÆÎ» ¿ªÆô
    }
    else
    {
        REG.AFESCONF2 &= ~0x01;    
    }

    if(bDSGMOS)
    {
        REG.AFESCONF2 |= 0x02;    //·ÅµçmosµÄ¿ªÆô
    }
    else
    {
        REG.AFESCONF2 &= ~0x02;    
    }
        
		//Èç¹û²ÎÊýÖµºÍÖ®Ç°Ò»Ñù¾Í²»ÔÚÐ´Èë
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
	if(bOCRC_EN)//¹ýÁ÷×Ô»Ö¸´Ê¹ÄÜ
	{
		if(bOCD || bAFE_SC|| bOCD2)//·Åµç¹ýÁ÷±£»¤±êÖ¾Î»,¶ÌÂ·±êÖ¾Î»
		{
			if(++uiOCDRcnt > OCDR_DELAY_CNT)//>400*25 =10s
			{
               //Õâ±ß¿ÉÄÜÒª¼ÓÅÐ¶Ï
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
                    REG.AFESCONF1 |= 0x80;//Çå³ý±êÖ¾¼Ä´æÆ÷¸÷±£»¤±êÖ¾Î»
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
    if(!bChkLoadFlg)//¼ì²â¸ºÔØ±êÖ¾Î»Ã»ÓÐ¿ªÆô
    {
        if(bUV) //Ç·Ñ¹ÁË
        {
            bLoadConectFlg = 1;
            bUVBkFlg = 1;
        }
        else if(bUVBkFlg && !bUV) //ÉÏ´ÎÇ·Ñ¹ÁË£¬Õâ´Î²»Ç·Ñ¹ÁË-¡·¶Ï¿ªmosºó²»Ç·Ñ¹ÁË£¬¾Í¿ªÊ¼¸ºÔØ¼ì²â
        {
						bUVBkFlg = 0;
            bChkLoadFlg = 1;
						ucLoadRCnt = 0;
						REG.AFESCONF1 |= 0x02; //¸ºÔØ¼ì²â
						TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);	 				//enable Load Detect	
        }

        if(bOCD || bAFE_SC ||bOCD2)//·Åµç¹ýÁ÷±£»¤±êÖ¾Î»£¬¶ÌÂ·±êÖ¾Î»
        {
            bChkLoadFlg = 1;
            bLoadConectFlg = 1; //Èç¹û¼ì²âÕâ¼¸¸ö´íÎóµ½ÁË»¹ÓÐ¸ºÔØÁ¬½Ó£¬¾Í¼ÌÐø¶Ï¿ªDmos
						ucLoadRCnt = 0;
						REG.AFESCONF1 |= 0x02;//¿ªÆô¸ºÔØ¼ì²â
						TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);	//Ê¹ÄÜ¸ºÔØ¼ì²â
        }
    }
		//Ö»ÓÐÔÚ¹Ø±Õ£¬bChkLoadFlgÊÇ1ÁË£¬ÇÒDmos¹Ø±ÕÁË£¬²ÅÄÜ¼ì²âÓÐÃ»ÓÐ¸ºÔØ
    else 
    {
    	  TWIReadAFE(AFE_BSTATUS, &REG.AFEBSTATUS);
        if((REG.AFEBSTATUS&0x02) == 0)//¸ºÔØÁ¬½Ó×´Ì¬Î» 1£º¸ºÔØÁ¬½Ó 0Î´Á¬½Ó¸ºÔØ
        {
            if(++ucLoadRCnt >= (E2ucDelayLoadR*5-1))             //4*125mS
            {
                bChkLoadFlg = 0;
                bLoadConectFlg = 0; 
								bChkChgerFlg = 0;
								bOCD = 0;//·Åµç¹ýÁ÷ÇåÁã
						  	bOCD2 = 0;
                ucLoadRCnt = 0;    
								REG.AFESCONF1 &= ~0x02; //¹Ø±Õ¸ºÔØ¼ì²â¹¦ÄÜ
								TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);	 				// close Load Detect
							if(bAFE_SC)
							{
										bAFE_SC = 0;//Çå¿Õ¼ì²â±êÖ¾Î»
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
        if((REG.AFEBSTATUS&0x01) != 0)//³äµçÆ÷Á¬½Ó×´Ì¬ÊÇ1£¬ÒÑÁ¬½Ó
        {
					if(++ucChgerCnt >= E2ucDelayLoadR*5)
					{
						bChkChgerFlg = 0;
						bChgerConectFlg = 1;
						ucChgerCnt = 0;
						REG.AFESCONF1 &= ~0x01;//³äµçÆ÷¼ì²âÎ»ÖÃ0
						TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);	 				// close Charge detect		
		
					}
				}
	}

    if(bChkChgerRFlg)//·¢Éú¹ý³äµç¹ýÁ÷£¨´óÓÚ×Ô¼ºÉèÖÃµÄ²ÎÊý£©
    {
    	  TWIReadAFE(AFE_BSTATUS, &REG.AFEBSTATUS);
        if((REG.AFEBSTATUS&0x01) == 0)//³äµçÆ÷Î´Á¬½Ó
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
									bAFE_OV = 0;//Çå¿Õ¼ì²â±êÖ¾Î»
									REG.AFESCONF1 |= 0x80;
									TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);
									REG.AFESCONF1 &= ~0x80;
									TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);
							}
            }
        }
	}
#if debug
		printf("\n -----³äµçÆ÷Á¬½ÓbChgerConectFlg =%bd£¬bChkChgerFlg = %bd----- \n",(bChgerConectFlg_test|bChgerConectFlg),(bChgerConectFlg_test|bChkChgerFlg));    
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
		//ÅÐ¶ÏÊÇ·ñ³äÂú
	    if(!bFC)//³äµç½ØÖ¹±êÖ¾Î»
	    {
				//0x010,0x068,³äµçµçÑ¹´óÓÚ4.2v ÇÒµçÁ÷ÒªÐ¡ÓÚ 100mA Ëã³äÂú×´Ì¬
	     	if((uiCellVmax>E2uiChgEndVol) && (Info.slCurr<=E2siChgEndCur))   //charge end voltage  ´óÓÚ4.2VÐ¡ÓÚ100ma 1s*5 
	    	{
					//E2ucDelayChgEnd  0x005,								// U8  xdata DelayChgEnd   		_at_ 	CHG_PARA_MAP_ADDR+10;	5
	    		if(++ucChgEndcnt >= E2ucDelayChgEnd)
	    		{
	    			bFC = 1;
#if debug
		printf("\n -----bFC ³äµç½ØÖ¹-----uiCellVmax = %d \n",uiCellVmax);    
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
	//·Åµç½ØÖ¹
	    if(!bFD)
	    {
				//0x00B,0x054,	2.9V
	    	if(uiCellVmin<E2uiDsgEndVol)	  //discharge end voltage  ·Åµç½ØÖ¹1s*5ÃëÐ¡ÓÚ2.9V
	    	{
	    		if(++ucDsgEndcnt >= E2ucDelayDsgEnd)
	    		{
	    			bFD = 1;
#if debug
		printf("\n -----bFD ·Åµç½ØÖ¹----- \n");    
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
	if(!bOCC)//Ã»ÓÐ·¢Éú ³äµç¹ýÁ÷±êÖ¾Î»
    {
			//0x000,0x000,0x00B,0x0B8,		 3000mA ³äµçµçÁ÷
			if(slCadcCurrent > E2slOCCvol)			   //CADC  cycle is 64ms ´óÓÚ×Ô¼ºÉèÖÃµÄ¹ýÁ÷Öµ
			{
							if(++ucOCCcnt > OCC_DELAY_CNT)
							{
								bOCC = 1;
								ucOCCcnt = 0;

								bChkChgerRFlg = 1;//ÖØÐÂ¼ì²âÊÇ·ñ³äµç
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
//·Åµç¹ýÁ÷±£»¤Î»
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
//by tomi ¶þ¼¶µçÁ÷±£»¤
//·Åµç¹ýÁ÷±£»¤Î»
	if(!bOCD2)
    {			
			if(slCadcCurrent < E2slOCD2vol)			   //CADC  40A ÑÓÊ±100ms
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

	TWIReadAFE(AFE_CURH, (U8 xdata *)&AFE.siCurr);	//Í¨¹ýTWI¶ÁÈ¡CADC²É¼¯µÄµçÁ÷Öµ
	
#if 0
	if((AFE.siCurr&0x1000) != 0)					//¸ù¾Ýbit12ÅÐ¶ÏÊÇ·ñÎª¸ºÊý(·ÅµçµçÁ÷Îª¸ºÖµ)£¬ÈçÎª¸ºÖµ¸ßÎ»²¹0
	{
		AFE.siCurr|=0xE000; //1110 0000 0000 0000 µÚ12Î»£º1±íÊ¾·Åµç£¬0±íÊ¾³äµç
	}
	slCadcCurrent = (S32)CALICUR*(AFE.siCurr-E2siCadcOffset)/E2siCadcGain;

	slCadcCurBuf[ucCadcCnt] = slCadcCurrent;		//¶ÔÁ¬Ðø²É¼¯µÄ16´ÎµçÁ÷È¡Æ½¾ùÖµ£¬×÷Îªµ±Ç°µçÁ÷Öµ
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
	if((AFE.siCurr&0x1000) != 0)					//¸ù¾Ýbit12ÅÐ¶ÏÊÇ·ñÎª¸ºÊý(·ÅµçµçÁ÷Îª¸ºÖµ)£¬ÈçÎª¸ºÖµ¸ßÎ»²¹1
	{
		AFE.siCurr|=0xE000; //1110 0000 0000 0000 µÚ12Î»£º1±íÊ¾·Åµç£¬0±íÊ¾³äµç
	}
#if (chargedebug|discurdebug)
		printf("\n µ¥´ÎµçÁ÷ÖµAFE.siCurr = %d \n",AFE.siCurr);    
#endif		
	//slCadcCurrent = (S32)AFE.siCurr;
	slCadcCurrent = (S32)CALICUR*(AFE.siCurr-E2siCadcOffset)/E2siCadcGain;   //       (AFE.siCurr-3)/ (-82)
#if (chargedebug|discurdebug)
		printf("\n µçÁ÷----------------slCadcCurrent ------------------= %ld \n",slCadcCurrent);    
#endif	
	slCadcCurBuf[ucCadcCnt] = slCadcCurrent;
	if(AverTime <= 16)//·ÀÄÚ´æ³¬³ö
	{	
		if(++ucCadcCnt >= AverTime)
		{
			ucCadcCnt = 0;
		}
		if(ucCadcCnt == 0)//µ½´ïÁË16´Î
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
		printf("\n Æ½¾ùµçÁ÷ CurrProcess(void): slCadcCurAverage = %ld \n",slCadcCurAverage);    
#endif	

	bDSGING = 0;
	bCHGING = 0;
	//< -100mA  
	if(slCadcCurAverage < (-E2siDfilterCur))//·Åµç×´Ì¬ÏÂÆ½¾ùµçÁ÷´óÓÚE2siDfilterCur
	{
#if debug
		printf("\n Ð¡ÓÚ -100mA \n");    
#endif	
		bDSGING = 1; //ÕýÔÚ·Åµç 
		UART_IRQ_DISABLE;
		Info.slCurr = slCadcCurAverage;
		UART_IRQ_ENABLE;
	}
	//rs2 -> rs1³äµç×´Ì¬£¬¶¨ÒåµçÁ÷ÎªÕý
	else if(slCadcCurAverage > E2siDfilterCur) //100
	{
#if debug
		printf("\n ´óÓÚ 100mA \n");    
#endif	
		bCHGING = 1;	 //ÕýÔÚ³äµç	
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
//ÈôÕýÔÚ³äµç£¬Òª×öµÄ¶¯×÷£¬Ò»°ãÊÇLEDÖ¸Ê¾
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
    else //²»ÔÚ³äµç
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
				printf("\n CurrProcess uiPackStatus =%hx ---µÍ8Î»£ºbCAL =%x,0,0,0,0,bVDQ=%x,bFD=%x,bFC=%x ¸ß8Î»:0,bFastDischarge=%x,bMidDischarge=%x,bSlowDischarge=%x,bDSGING=%x,bCHGING=%x,bDSGMOS=%x,bCHGMOS=%x\n",uiPackStatus,((uiPackStatus&0x8000)>>15),((uiPackStatus&0x0400)>>10),((uiPackStatus&0x0200)>>9),((uiPackStatus&0x0100)>>8),\
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

	for(i=3; i<=11; i=i+2)		//´Ó¿ÉÐ´¼Ä´æÆ÷03H¿ªÊ¼±È½Ï   // 03H  05H 07H 09H 0BH
	{
		TWIReadAFE(i, RdBuf); 

        if((RdBuf[0]!=*(&REG.AFEFLAG1+i)) || (RdBuf[1]!=*(&REG.AFEFLAG1+i+1)))
        {
					if(i != 7) //07HÊÇÊý¾ÝÎ»
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
	TWIReadAFE(AFE_FLAG1, &REG.AFEFLAG1);	//¶ÁÈ¡AFE¼Ä´æÆ÷FLAG1 & FLAG2ÖÐµÄAlarm±êÖ¾

	if((REG.AFEFLAG2&0x02) != 0)	//CADC µçÁ÷×ª»»Íê³É 
	{
		bCADCFlg = 1;
#if debug
		printf("\n Cur trans OK! \n");    
#endif	
	}

	if((REG.AFEFLAG2&0x01) != 0)	//VADC µçÑ¹×ª»»Íê³É
	{
		bVADCFlg = 1;
#if debug
		printf("\n VOL trans OK! \n");    
#endif	
	}

	if((REG.AFEFLAG1&0x08) != 0)	//AFE ¶ÌÂ·±£»¤
	{
		bAFE_SC = 1;
		Info.uiPackStatus = uiPackStatus;
	}

	if((REG.AFEFLAG1&0x04) != 0)	//AFE ¹ýÑ¹±£»¤
	{
		bAFE_OV = 1;
		//by tomi
		bChkChgerRFlg = 1; //×Ô»Ö¸´
		Info.uiPackStatus = uiPackStatus;
	}

	if((REG.AFEFLAG2&0x04) != 0)	//ÅÐ¶ÏFLAG2µÄbit2ÊÇ·ñÎª1£¬ÈçÎª1Ôò±íÊ¾AFE·¢Éú¹ýLVR£¬ÐèÒªÖØÐÂ³õÊ¼»¯AFE¼Ä´æÆ÷
	{
		InitAFE();
#if debug
		printf("AFEµÄrst·¢Éú¹ýLVR \n");    
#endif	
	}

#if debug
		printf("\n uiPackStatus =%hx ---µÍ8Î»£ºbCAL =%x,0,0,0,0,bVDQ=%x,bFD=%x,bFC=%x ¸ß8Î»:0,bFastDischarge=%x,bMidDischarge=%x,bSlowDischarge=%x,bDSGING=%x,bCHGING=%x,bDSGMOS=%x,bCHGMOS=%x\n",uiPackStatus,((uiPackStatus&0x8000)>>15),((uiPackStatus&0x0400)>>10),((uiPackStatus&0x0200)>>9),((uiPackStatus&0x0100)>>8),\
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
	//µ¥Ð¾¹ý³ä	E2uiOVvol > 0x010,0x09A,4250 > 4.25V -->4.175
	if(uiCellVmax > E2uiOVvol)//E2uiOVvol¹ý³äµç±£»¤ãÐÖµ    
	{		
		bHV = 1;
#if debug
		TI = 0;
		TI = 1;	
		printf("\n --¹ý³äbHV-- \n");   
#endif  
	}
 //E2uiUVvol = 0x00A,0x08C,	2.7v
	if(uiCellVmin < E2uiUVvol)//Ç·Ñ¹±£»¤ãÐÖµ
	{		
		bUV = 1;
	  bChkChgerFlg = 1;//¼ì²é³äµçÆ÷±êÖ¾ÖÃ1
	  bChgerConectFlg = 0;//ÒÑÁ¬½Ó±êÖ¾Çå¿Õ
		ucChgerCnt = 0;
		//Ê¹ÄÜ³äµçÆ÷¼ì²â
		REG.AFESCONF1 |= 0x01;
		TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);	 				//enable charger detect 
#if debug
		printf("\n --Ç·Ñ¹bUV--\n");   
#endif  
	}

	if(uiTempeMax > E2uiTempOTC)//	0x00C,0x09F,						// U16 xdata TempOTC      		_at_ 	CHG_TEMP_PARA_MAP_ADDR;		3231-2731=500
	{
		bOTC = 1;
#if debug
		printf("\n --³ä¹ýÎÂbOTC--\n");   
#endif  
	}

	if(uiTempeMin < E2uiTempUTC) //0ÉãÊÏ¶È
	{
		bUTC = 1;
#if debug
		printf("\n --³äµÍÎÂbUTC-\n");   
#endif  
	}
	if(uiTempeMax > E2uiTempOTD)//70 ÉãÊÏ¶È    3461
	{
		bOTD = 1;
#if debug
		printf("\n --·Å¸ßÎÂbOTD-\n");   
#endif 
	}

	if(uiTempeMin < E2uiTempUTD)//-10ÉãÊÏ¶È 
	{
		bUTD = 1;
#if debug	
		printf("\n --·ÅµÍÎÂbUTD--\n");   
#endif 
	}
//
//sbit bHV			=	uiBatStatus^8;//¹ý³å±£»¤±êÖ¾Î»
//sbit bUV			=	uiBatStatus^9;//¹ý·Å±£»¤±êÖ¾Î»
//sbit bOCC 			= 	uiBatStatus^10;//³äµç¹ýÁ÷±£»¤±êÖ¾Î» 1·¢Éú¹ý±£»¤
//sbit bOCD 			= 	uiBatStatus^11;//·Åµç¹ýÁ÷±£»¤±êÖ¾Î»
//sbit bOCD2 			= 	uiBatStatus^12;//·Åµç¹ýÁ÷2±£»¤±êÖ¾Î»
	
//sbit bOTC 			=	uiBatStatus^0;//³äµç¸ßÎÂ±£»¤
//sbit bOTD 			= 	uiBatStatus^1;//·Åµç¸ßÎÂ±£»¤
//sbit bUTC			=	uiBatStatus^2;//³äµçµÍÎÂ±£»¤
//sbit bUTD			=	uiBatStatus^3;//·ÅµçµÍÎÂ±£»¤
//sbit bAFE_OV		=	uiBatStatus^4;//Ó²¼þ¹ý³å±£»¤
//sbit bAFE_SC		=	uiBatStatus^5;//Ó²¼þ¶ÌÂ·±£»¤
//sbit bCTO			=	uiBatStatus^6; //Ô¤ÁôÎ»£¬¶ÏÏß±£»¤

#if debug
		TI = 0;
		TI = 1;	
		printf("\n PorOnProtect() uiBatStatus = %x\n",uiBatStatus);   
#endif  

#if debug
		//×¢ÒâÕâ±ßµÍÎ»×Ö½Ú·ÅÇ°Ãæ£¬¸ßÎ»×Ö½Ú´æ´¢ÔÚºóÃæ
		printf("\n µÍ8Î»£º0,bCTO=%x,bAFE_SC=%x,bAFE_OV=%x,bUTD=%x,bUTC=%x,bOTD=%x,bOTC=%x ¸ß8Î» 0,0,0,bOCD2=%x,bOCD=%x,bOCC=%x,bUV=%x,bHV=%x,\n",((uiBatStatus&0x4000)>>14),((uiBatStatus&0x2000)>>13),((uiBatStatus&0x1000)>>12),((uiBatStatus&0x0800)>>11),\
		((uiBatStatus&0x0400)>>10),((uiBatStatus&0x0200)>>9),((uiBatStatus&0x0100)>>8),((uiBatStatus&0x0010)>>4),((uiBatStatus&0x0008)>>3),((uiBatStatus&0x0004)>>2),((uiBatStatus&0x0002)>>1),(uiBatStatus&0x0001));   
#endif  
	
		/*********by tomi********/

		Input_charger_detect_poron();//ÅÐ¶Ï³äµçÆ÷ÓÐÃ»ÓÐ²å 

		if(ConectFlg_poron == 1)
		{
#if 1 
			printf("\n ³äµçÆ÷ÒÑÁ¬½Ó\n");
#endif
			Input_vol_judge_poron();//ÅÐ¶Ï³äµçÆ÷µÄ³äµçµçÑ¹	
		}	
		else//Èç¹û³äµçÆ÷²»Á¬½ÓÁË,Çå¿Õ´íÎó±êÖ¾Î»
		{			
#if 1 
			printf("\n Çå¿ÕOIV UIV\n");
#endif			
			OIV = 0;
			UIV = 0;
		}
		
	 // Chk_Fuse_poron(); //¼ì²éÓÐÃ»ÓÐÈÛ¶Ï£¨ÉÏµçË²¼ä²»¼ì²éÈÛ¶Ï£©
	  Chk_Cell_VolDIFF_poron();//Ñ¹²î±£»¤

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
	//²»ÊÇ¶ÏÏß»òÕßÊÇÔÚÆ½ºâ×´Ì¬ÏÂ
	if(!(bCellOpenDecFlag || bBalanceFlg))//ÔÚ¶ÏÏß¼ì²âºÍÆ½ºâµÄ×´Ì¬ÏÂ£¬²»×öµçÑ¹±£»¤
	{
		//µçÑ¹±£»¤³ÌÐò£¨bHV£©
		VolProtect();			
	}
		//ÎÂ¶È±£»¤³ÌÐò
		TempeProtect();		
//-------------------------------------------------------------------------------------------------	
#if 0
		/*********by tomi********/
	//ÔËÐÐÆðÀ´ºó£¬ £¨1£©ÔÚ~(AFE_CHG_MOS  = 1ÇÒIO_CHG_MOS= 0) Ìõ¼þ£¨2£©£ºÉÏ´ÎÃ»ÓÐ³äµçÆ÷Õâ´ÎÓÐ³äµçÆ÷ÁË £¬»¹ÓÐÒ»ÖÖÊÇ³äµçmosÃ»ÓÐ¿ªÆô
		if(!((IO_CHG_MOS == 0) && ((uiPackStatus&0x0001) == 1)) )//ÔÚ¶¼¹Ø±ÕµÄ×´Ì¬ÏÂ
		{
#if debug
			printf(" \n");
#endif
			Input_charger_detect_poron();//ÅÐ¶Ï³äµçÆ÷ÓÐÃ»ÓÐ²å
			if(ConectFlg_poron == 1)
			{
				Input_vol_judge();//ÅÐ¶Ï³äµçÆ÷µÄ³äµçµçÑ¹
			}	
			else
			{
				OIV = 0;
				UIV = 0;
			}
		}
#endif	
		/*********by tomi********/
//ÔËÐÐÆðÀ´ºó£¬ £¨1£©ÔÚ~(AFE_CHG_MOS  = 1ÇÒIO_CHG_MOS= 0) Ìõ¼þ£¨2£©£ºÉÏ´ÎÃ»ÓÐ³äµçÆ÷Õâ´ÎÓÐ³äµçÆ÷ÁË £¬»¹ÓÐÒ»ÖÖÊÇ³äµçmosÃ»ÓÐ¿ªÆô
		if(bCHGING == 0)//Èç¹û²»ÔÚ³äµçÁË
		{		
			if(++CHGING_cnt >= 4)
			{
				CHGING_cnt = 0;
				IO_CHG_MOS = 1;//¹Ø±Õ³äµçmos
				Input_charger_detect();//ÅÐ¶Ï³äµçÆ÷ÓÐÃ»ÓÐ²å
				if(ConectFlg_poron == 1)
				{
					//1.CMOSÃ»ÓÐ´ò¿ª
					if(!((IO_CHG_MOS == 0) && ((uiPackStatus&0x0001) == 1)) )//ÔÚ¶¼¹Ø±ÕµÄ×´Ì¬ÏÂ
					{
#if chargedebug
						printf("Input_vol_judgeÑÓÊ±¼ì²â\n");
#endif	
						if(ConectFlg_poron_prev == 1)//
						{
							Input_vol_judge();//ÅÐ¶Ï³äµçÆ÷µÄ³äµçµçÑ¹£¬ÓÐÑÓÊ±µÄÅÐ¶Ï
						}
						else//Èç¹ûÉÏ´ÎÊÇµÚÒ»´Î²å
						{
							Input_vol_judge_poron();//ÒòÎªÖ»²É¼¯Ò»´ÎËùÒÔ²»ÓÃµÈ´ý»Ö¸´
						}
								
					}	
					//2.CMOS´ò¿ªÁË
	//				else
	//				{
	//					//¾ÍÒªÅÐ¶Ï£¬ÉÏÒ»´ÎÊÇ·ñÃ»²å ³äµçÆ÷
	//					if(ConectFlg_poron_prev == 0)
	//					{
	//#if chargedebug
	//						printf("Input_vol_judge_poronµ¥´Î¼ì²â\n");
	//#endif							
	//						Input_vol_judge_poron();//µ¥´Î¼ì²â
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
		
	  Chk_Fuse(); //¼ì²éÓÐÃ»ÓÐÈÛ¶Ï
  	Chk_Cell_VolDIFF();


		
		MCU_CHK_CDmos();
//-------------------------------------------------------------------------------------------------	
	
	//mos×´Ì¬¼ì²âºÍ´¦Àí
    ChkMosStatus();		
    ControlMos(); 
#if 1
		updata_pack_fault_flag(); //25msÖ´ÐÐÒ»´Î
#endif
	
		Info.uiBatStatus = uiBatStatus;
		Info.uiPackStatus = uiPackStatus;
//¹ýÁ÷×Ô»Ö¸´Ê¹ÄÜ
    RecoveryOC();

    CheckLoad();
//²»¶ÏµÄÈ¥¼ì²â³äµçÆ÷ÊÇ·ñ±»²åÈë
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
	REG.AFESCONF10 = 0x33;						//½øÈëPower DownÄ£Ê½
	TWIWriteAFE(AFE_SCONF10, &REG.AFESCONF10);
	REG.AFESCONF1 |= 0x20; 
	TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);
}

