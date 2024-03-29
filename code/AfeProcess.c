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
		if(uiCellVmax < E2uiOVRvol)//恢复 < 4.15v
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
		if(uiCellVmin < E2uiUVvol) //判断有没有欠压了//< 2.7v
		{		
			if(++ucUVcnt >= UV_DELAY_CNT)
			{
				bUV = 1;
				ucUVcnt = 0;
				ucUVRcnt = 0;

	      bChkChgerFlg = 1;//欠压的话要检测充电器
	      bChgerConectFlg = 0;
				ucChgerCnt = 0;
	      REG.AFESCONF1 |= 0x01;//开启充电器检测
	      TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);	//使能充电器检测
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
	if(!bOTC)//充电高温
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
		else if(uiTempeMax < E2uiTempOTCR)//E2uiTempOTCR应该要比E2uiTempOTC稍微低点，因为回复温度要低点 45
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
		printf("\n bOTD = 1：外部uiTempeMax=%d,uiTempeMin=%d \n",uiTempeMax,uiTempeMin);    
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
		printf("\n bOTD = 0：外部uiTempeMax=%d,uiTempeMin=%d \n",uiTempeMax,uiTempeMin);    
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
//电芯>4.4v
	if(bAFE_OV || bCTO)//断线保护，和硬件过冲												  
	{
		bCHGMOS = 0;
	}
	//放电状态：
	//(1)单体欠压检测(低于最低电压 触发)         bUV
	//(2)单体间压差检测（单体间压差>0.3V 触发）  OK
	//(3)三端保险丝是否熔断（熔断）              OK
	//(4)放电电流检测（过流，短路）              bAFE_SC
	//(5)高温检测（超温 触发）                   bOTD
	//(6)低温检测（超温 触发）                   bUTD
  //    																			 还剩: bOCD ||(bFD&&bDSGEnd) || bLoadConectFlg || bCTO
	//uiPackConfig初始化:00101101 00110111
//放电状态下的错误标志                         //放电截止且放电截止关闭放电mosfet   初始状态bLoadConectFlg = 0， 电池状态刚bstatus =0刚开始所以bCTO = 0
																																//bLoadConectFlg是1的话就会保护，bLoadConectFlg只有在检测发生放电的错误状态后，检测负载依然后负载连接，才会置1
	if( bOTD || bUTD || bUV || bOCD || bOCD2 || bAFE_SC || (bFD&&bDSGEnd) || bLoadConectFlg || bCTO)
	{																																//后期因为短路触发bLoadConectFlg的可能会去除这一项，不用检测负载，由插入适配器唤醒
		bDSGMOS = 0;   //放电mos关闭
//		PWM2CON &= ~0x01; //pwm禁止输出	
//		DSG1PWM = 0;
	}
	else
	{
		//tomi
		//添加放电正常的操作
		
//		DSG1PWM = 1;
//		if(ucDsgingSpeed != 2)
//		{
//			PWM2CON |= 0x01;
//		}
	}
   //packstatus初始状态都是0
    if(!bDSGING)//不是放电状态
    {
			  //(1)单体过压检测 (大于4.2V触发保护)                  bHV
				//(2)单体间压差检测（单体间压差>0.3V 触发）           OK
				//(3)三端保险丝是否熔断（熔断）         							OK
				//(4)充电器已连接（AFE充电器检测）                    待定
				//(5)充电器电压在正常范围（34.2±0.5V）								OK		Input_vol_judge();//判断充电器的充电电压
				//(6)充电电流检测（电流小于10ma 触发）                待定，这个其实就是bFC充满了
				//(7)放电电流检测（有放电电流 触发）!!!!!!!!!!!!!!!!  待定，因为已经是非放电状态
				//(8)高温检测（超温 触发）                            bOTC
				//(9)低温检测（超温 触发）									  				bUTC
			  //																										还剩:bOCC|| (bFC&&bCHGEnd)
        if(bOTC || bUTC || bHV || bOCC || (bFC&&bCHGEnd))	//发生过报错 充电中报错							
        {
#if debug
		printf("关闭充电mos1-----------\n");    
#endif
					bCHGMOS = 0;  //关闭充电mos
        }
    }
		//放电过流保护，硬件短路保护，检测标志位是1还有已经连接标志位是0
		//																											(bChkChgerFlg&&!bChgerConectFlg))
		if(bOCD || bOCD2 || bAFE_SC  || (bChkChgerFlg&&!bChgerConectFlg))//检测充电是否有充电，也需要关闭Cmos
		{
#if debug
		printf("关闭充电mos2-----------\n");    
#endif
					bCHGMOS = 0;//关闭充电mos
		}
		
#if (tempdebug|chargedebug|discurdebug)
		//注意这边低位字节放前面，高位字节存储在后�
		printf("\n 低8位：0,bCTO=%x,bAFE_SC=%x,bAFE_OV=%x�,bUTD=%x,bUTC=%x,bOTD=%x,bOTC=%x 高8位 0,0,0,bOCD2=%x,bOCD=%x,bOCC=%x,bUV=%x,bHV=%x,\n",((uiBatStatus&0x4000)>>14),((uiBatStatus&0x2000)>>13),((uiBatStatus&0x1000)>>12),((uiBatStatus&0x0800)>>11),\
		((uiBatStatus&0x0400)>>10),((uiBatStatus&0x0200)>>9),((uiBatStatus&0x0100)>>8),((uiBatStatus&0x0010)>>4),((uiBatStatus&0x0008)>>3),((uiBatStatus&0x0004)>>2),((uiBatStatus&0x0002)>>1),(uiBatStatus&0x0001));   
#endif 	
#if debug
		printf("\n bFD = %x,bFC= %x ,LoadConnet_FLG =%bd,bChgerConectFlg =%bd，bChkChgerFlg = %bd\n",((uiPackStatus&0x0200)>>9),((uiPackStatus&0x0100)>>8),LoadConnet_FLG,(bChgerConectFlg_test|bChgerConectFlg),(bChgerConectFlg_test|bChkChgerFlg));    
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
        REG.AFESCONF2 |= 0x01;   //充电mos控制位 开启
    }
    else
    {
        REG.AFESCONF2 &= ~0x01;    
    }

    if(bDSGMOS)
    {
        REG.AFESCONF2 |= 0x02;    //放电mos的开启
    }
    else
    {
        REG.AFESCONF2 &= ~0x02;    
    }
        
		//如果参数值和之前一样就不在写入
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
	if(bOCRC_EN)//过流自恢复使能
	{
		if(bOCD || bAFE_SC|| bOCD2)//放电过流保护标志位,短路标志位
		{
			if(++uiOCDRcnt > OCDR_DELAY_CNT)//>400*25 =10s
			{
               //这边可能要加判断
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
                    REG.AFESCONF1 |= 0x80;//清除标志寄存器各保护标志位
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
    if(!bChkLoadFlg)//检测负载标志位没有开启
    {
        if(bUV) //欠压了
        {
            bLoadConectFlg = 1;
            bUVBkFlg = 1;
        }
        else if(bUVBkFlg && !bUV) //上次欠压了，这次不欠压了-》断开mos后不欠压了，就开始负载检测
        {
						bUVBkFlg = 0;
            bChkLoadFlg = 1;
						ucLoadRCnt = 0;
						REG.AFESCONF1 |= 0x02; //负载检测
						TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);	 				//enable Load Detect	
        }

        if(bOCD || bAFE_SC ||bOCD2)//放电过流保护标志位，短路标志位
        {
            bChkLoadFlg = 1;
            bLoadConectFlg = 1; //如果检测这几个错误到了还有负载连接，就继续断开Dmos
						ucLoadRCnt = 0;
						REG.AFESCONF1 |= 0x02;//开启负载检测
						TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);	//使能负载检测
        }
    }
		//只有在关闭，bChkLoadFlg是1了，且Dmos关闭了，才能检测有没有负载
    else 
    {
    	  TWIReadAFE(AFE_BSTATUS, &REG.AFEBSTATUS);
        if((REG.AFEBSTATUS&0x02) == 0)//负载连接状态位 1：负载连接 0未连接负载
        {
            if(++ucLoadRCnt >= (E2ucDelayLoadR*5-1))             //4*125mS
            {
                bChkLoadFlg = 0;
                bLoadConectFlg = 0; 
								bChkChgerFlg = 0;
								bOCD = 0;//放电过流清零
						  	bOCD2 = 0;
                ucLoadRCnt = 0;    
								REG.AFESCONF1 &= ~0x02; //关闭负载检测功能
								TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);	 				// close Load Detect
							if(bAFE_SC)
							{
										bAFE_SC = 0;//清空检测标志位
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
        if((REG.AFEBSTATUS&0x01) != 0)//充电器连接状态是1，已连接
        {
					if(++ucChgerCnt >= E2ucDelayLoadR*5)
					{
						bChkChgerFlg = 0;
						bChgerConectFlg = 1;
						ucChgerCnt = 0;
						REG.AFESCONF1 &= ~0x01;//充电器检测位置0
						TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);	 				// close Charge detect		
		
					}
				}
	}

    if(bChkChgerRFlg)//发生过充电过流（大于自己设置的参数）
    {
    	  TWIReadAFE(AFE_BSTATUS, &REG.AFEBSTATUS);
        if((REG.AFEBSTATUS&0x01) == 0)//充电器未连接
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
									bAFE_OV = 0;//清空检测标志位
									REG.AFESCONF1 |= 0x80;
									TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);
									REG.AFESCONF1 &= ~0x80;
									TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);
							}
            }
        }
	}
#if debug
		printf("\n -----充电器连接bChgerConectFlg =%bd，bChkChgerFlg = %bd----- \n",(bChgerConectFlg_test|bChgerConectFlg),(bChgerConectFlg_test|bChkChgerFlg));    
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
		//判断是否充满
	    if(!bFC)//充电截止标志位
	    {
				//0x010,0x068,充电电压大于4.2v 且电流要小于 100mA 算充满状态
	     	if((uiCellVmax>E2uiChgEndVol) && (Info.slCurr<=E2siChgEndCur))   //charge end voltage  大于4.2V小于100ma 1s*5 
	    	{
					//E2ucDelayChgEnd  0x005,								// U8  xdata DelayChgEnd   		_at_ 	CHG_PARA_MAP_ADDR+10;	5
	    		if(++ucChgEndcnt >= E2ucDelayChgEnd)
	    		{
	    			bFC = 1;
#if debug
		printf("\n -----bFC 充电截止-----uiCellVmax = %d \n",uiCellVmax);    
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
	//放电截止
	    if(!bFD)
	    {
				//0x00B,0x054,	2.9V
	    	if(uiCellVmin<E2uiDsgEndVol)	  //discharge end voltage  放电截止1s*5秒小于2.9V
	    	{
	    		if(++ucDsgEndcnt >= E2ucDelayDsgEnd)
	    		{
	    			bFD = 1;
#if debug
		printf("\n -----bFD 放电截止----- \n");    
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
	if(!bOCC)//没有发生 充电过流标志位
    {
			//0x000,0x000,0x00B,0x0B8,		 3000mA 充电电流
			if(slCadcCurrent > E2slOCCvol)			   //CADC  cycle is 64ms 大于自己设置的过流值
			{
							if(++ucOCCcnt > OCC_DELAY_CNT)
							{
								bOCC = 1;
								ucOCCcnt = 0;

								bChkChgerRFlg = 1;//重新检测是否充电
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
//放电过流保护位
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
//by tomi 二级电流保护
//放电过流保护位
	if(!bOCD2)
    {			
			if(slCadcCurrent < E2slOCD2vol)			   //CADC  40A 延时100ms
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

	TWIReadAFE(AFE_CURH, (U8 xdata *)&AFE.siCurr);	//通过TWI读取CADC采集的电流值
	
#if 0
	if((AFE.siCurr&0x1000) != 0)					//根据bit12判断是否为负数(放电电流为负值)，如为负值高位补0
	{
		AFE.siCurr|=0xE000; //1110 0000 0000 0000 第12位：1表示放电，0表示充电
	}
	slCadcCurrent = (S32)CALICUR*(AFE.siCurr-E2siCadcOffset)/E2siCadcGain;

	slCadcCurBuf[ucCadcCnt] = slCadcCurrent;		//对连续采集的16次电流取平均值，作为当前电流值
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
	if((AFE.siCurr&0x1000) != 0)					//根据bit12判断是否为负数(放电电流为负值)，如为负值高位补1
	{
		AFE.siCurr|=0xE000; //1110 0000 0000 0000 第12位：1表示放电，0表示充电
	}
#if (chargedebug|discurdebug)
		printf("\n 单次电流值AFE.siCurr = %d \n",AFE.siCurr);    
#endif		
	//slCadcCurrent = (S32)AFE.siCurr;
	slCadcCurrent = (S32)CALICUR*(AFE.siCurr-E2siCadcOffset)/E2siCadcGain;   //       (AFE.siCurr-3)/ (-82)
#if (chargedebug|discurdebug)
		printf("\n 电流----------------slCadcCurrent ------------------= %ld \n",slCadcCurrent);    
#endif	
	slCadcCurBuf[ucCadcCnt] = slCadcCurrent;
	if(AverTime <= 16)//防内存超出
	{	
		if(++ucCadcCnt >= AverTime)
		{
			ucCadcCnt = 0;
		}
		if(ucCadcCnt == 0)//到达了16次
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
		printf("\n 平均电流 CurrProcess(void): slCadcCurAverage = %ld \n",slCadcCurAverage);    
#endif	

	bDSGING = 0;
	bCHGING = 0;
	//< -100mA  
	if(slCadcCurAverage < (-E2siDfilterCur))//放电状态下平均电流大于E2siDfilterCur
	{
#if debug
		printf("\n 小于 -100mA \n");    
#endif	
		bDSGING = 1; //正在放电 
		UART_IRQ_DISABLE;
		Info.slCurr = slCadcCurAverage;
		UART_IRQ_ENABLE;
	}
	//rs2 -> rs1充电状态，定义电流为正
	else if(slCadcCurAverage > E2siDfilterCur) //100
	{
#if debug
		printf("\n 大于 100mA \n");    
#endif	
		bCHGING = 1;	 //正在充电	
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
//若正在充电，要做的动作，一般是LED指示
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
    else //不在充电
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
				printf("\n CurrProcess uiPackStatus =%hx ---低8位：bCAL =%x,0,0,0,0,bVDQ=%x,bFD=%x,bFC=%x 高8位:0,bFastDischarge=%x,bMidDischarge=%x,bSlowDischarge=%x,bDSGING=%x,bCHGING=%x,bDSGMOS=%x,bCHGMOS=%x\n",uiPackStatus,((uiPackStatus&0x8000)>>15),((uiPackStatus&0x0400)>>10),((uiPackStatus&0x0200)>>9),((uiPackStatus&0x0100)>>8),\
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

	for(i=3; i<=11; i=i+2)		//从可写寄存器03H开始比较   // 03H  05H 07H 09H 0BH
	{
		TWIReadAFE(i, RdBuf); 

        if((RdBuf[0]!=*(&REG.AFEFLAG1+i)) || (RdBuf[1]!=*(&REG.AFEFLAG1+i+1)))
        {
					if(i != 7) //07H是数据位
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
	TWIReadAFE(AFE_FLAG1, &REG.AFEFLAG1);	//读取AFE寄存器FLAG1 & FLAG2中的Alarm标志

	if((REG.AFEFLAG2&0x02) != 0)	//CADC 电流转换完成 
	{
		bCADCFlg = 1;
#if debug
		printf("\n Cur trans OK! \n");    
#endif	
	}

	if((REG.AFEFLAG2&0x01) != 0)	//VADC 电压转换完成
	{
		bVADCFlg = 1;
#if debug
		printf("\n VOL trans OK! \n");    
#endif	
	}

	if((REG.AFEFLAG1&0x08) != 0)	//AFE 短路保护
	{
		bAFE_SC = 1;
		Info.uiPackStatus = uiPackStatus;
	}

	if((REG.AFEFLAG1&0x04) != 0)	//AFE 过压保护
	{
		bAFE_OV = 1;
		//by tomi
		bChkChgerRFlg = 1; //自恢复
		Info.uiPackStatus = uiPackStatus;
	}

	if((REG.AFEFLAG2&0x04) != 0)	//判断FLAG2的bit2是否为1，如为1则表示AFE发生过LVR，需要重新初始化AFE寄存器
	{
		InitAFE();
#if debug
		printf("AFE的rst发生过LVR \n");    
#endif	
	}

#if debug
		printf("\n uiPackStatus =%hx ---低8位：bCAL =%x,0,0,0,0,bVDQ=%x,bFD=%x,bFC=%x 高8位:0,bFastDischarge=%x,bMidDischarge=%x,bSlowDischarge=%x,bDSGING=%x,bCHGING=%x,bDSGMOS=%x,bCHGMOS=%x\n",uiPackStatus,((uiPackStatus&0x8000)>>15),((uiPackStatus&0x0400)>>10),((uiPackStatus&0x0200)>>9),((uiPackStatus&0x0100)>>8),\
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
	//单芯过充	E2uiOVvol > 0x010,0x09A,4250 > 4.25V -->4.175
	if(uiCellVmax > E2uiOVvol)//E2uiOVvol过充电保护阈值    
	{		
		bHV = 1;
#if debug
		TI = 0;
		TI = 1;	
		printf("\n --过充bHV-- \n");   
#endif  
	}
 //E2uiUVvol = 0x00A,0x08C,	2.7v
	if(uiCellVmin < E2uiUVvol)//欠压保护阈值
	{		
		bUV = 1;
	  bChkChgerFlg = 1;//检查充电器标志置1
	  bChgerConectFlg = 0;//已连接标志清空
		ucChgerCnt = 0;
		//使能充电器检测
		REG.AFESCONF1 |= 0x01;
		TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);	 				//enable charger detect 
#if debug
		printf("\n --欠压bUV--\n");   
#endif  
	}

	if(uiTempeMax > E2uiTempOTC)//	0x00C,0x09F,						// U16 xdata TempOTC      		_at_ 	CHG_TEMP_PARA_MAP_ADDR;		3231-2731=500
	{
		bOTC = 1;
#if debug
		printf("\n --充过温bOTC--\n");   
#endif  
	}

	if(uiTempeMin < E2uiTempUTC) //0摄氏度
	{
		bUTC = 1;
#if debug
		printf("\n --充低温bUTC-\n");   
#endif  
	}
	if(uiTempeMax > E2uiTempOTD)//70 摄氏度    3461
	{
		bOTD = 1;
#if debug
		printf("\n --放高温bOTD-\n");   
#endif 
	}

	if(uiTempeMin < E2uiTempUTD)//-10摄氏度 
	{
		bUTD = 1;
#if debug	
		printf("\n --放低温bUTD--\n");   
#endif 
	}
//
//sbit bHV			=	uiBatStatus^8;//过冲保护标志位
//sbit bUV			=	uiBatStatus^9;//过放保护标志位
//sbit bOCC 			= 	uiBatStatus^10;//充电过流保护标志位 1发生过保护
//sbit bOCD 			= 	uiBatStatus^11;//放电过流保护标志位
//sbit bOCD2 			= 	uiBatStatus^12;//放电过流2保护标志位
	
//sbit bOTC 			=	uiBatStatus^0;//充电高温保护
//sbit bOTD 			= 	uiBatStatus^1;//放电高温保护
//sbit bUTC			=	uiBatStatus^2;//充电低温保护
//sbit bUTD			=	uiBatStatus^3;//放电低温保护
//sbit bAFE_OV		=	uiBatStatus^4;//硬件过冲保护
//sbit bAFE_SC		=	uiBatStatus^5;//硬件短路保护
//sbit bCTO			=	uiBatStatus^6; //预留位，断线保护

#if debug
		TI = 0;
		TI = 1;	
		printf("\n PorOnProtect() uiBatStatus = %x\n",uiBatStatus);   
#endif  

#if debug
		//注意这边低位字节放前面，高位字节存储在后面
		printf("\n 低8位：0,bCTO=%x,bAFE_SC=%x,bAFE_OV=%x,bUTD=%x,bUTC=%x,bOTD=%x,bOTC=%x 高8位 0,0,0,bOCD2=%x,bOCD=%x,bOCC=%x,bUV=%x,bHV=%x,\n",((uiBatStatus&0x4000)>>14),((uiBatStatus&0x2000)>>13),((uiBatStatus&0x1000)>>12),((uiBatStatus&0x0800)>>11),\
		((uiBatStatus&0x0400)>>10),((uiBatStatus&0x0200)>>9),((uiBatStatus&0x0100)>>8),((uiBatStatus&0x0010)>>4),((uiBatStatus&0x0008)>>3),((uiBatStatus&0x0004)>>2),((uiBatStatus&0x0002)>>1),(uiBatStatus&0x0001));   
#endif  
	
		/*********by tomi********/

		Input_charger_detect_poron();//判断充电器有没有插 

		if(ConectFlg_poron == 1)
		{
#if 1 
			printf("\n 充电器已连接\n");
#endif
			Input_vol_judge_poron();//判断充电器的充电电压	
		}	
		else//如果充电器不连接了,清空错误标志位
		{			
#if 1 
			printf("\n 清空OIV UIV\n");
#endif			
			OIV = 0;
			UIV = 0;
		}
		
	 // Chk_Fuse_poron(); //检查有没有熔断（上电瞬间不检查熔断）
	  Chk_Cell_VolDIFF_poron();//压差保护

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
	//不是断线或者是在平衡状态下
	if(!(bCellOpenDecFlag || bBalanceFlg))//在断线检测和平衡的状态下，不做电压保护
	{
		//电压保护程序（bHV）
		VolProtect();			
	}
		//温度保护程序
		TempeProtect();		
//-------------------------------------------------------------------------------------------------	
#if 0
		/*********by tomi********/
	//运行起来后， （1）在~(AFE_CHG_MOS  = 1且IO_CHG_MOS= 0) 条件（2）：上次没有充电器这次有充电器了 ，还有一种是充电mos没有开启
		if(!((IO_CHG_MOS == 0) && ((uiPackStatus&0x0001) == 1)) )//在都关闭的状态下
		{
#if debug
			printf(" \n");
#endif
			Input_charger_detect_poron();//判断充电器有没有插
			if(ConectFlg_poron == 1)
			{
				Input_vol_judge();//判断充电器的充电电压
			}	
			else
			{
				OIV = 0;
				UIV = 0;
			}
		}
#endif	
		/*********by tomi********/
//运行起来后， （1）在~(AFE_CHG_MOS  = 1且IO_CHG_MOS= 0) 条件（2）：上次没有充电器这次有充电器了 ，还有一种是充电mos没有开启
		if(bCHGING == 0)//如果不在充电了
		{		
			if(++CHGING_cnt >= 4)
			{
				CHGING_cnt = 0;
				IO_CHG_MOS = 1;//关闭充电mos
				Input_charger_detect();//判断充电器有没有插
				if(ConectFlg_poron == 1)
				{
					//1.CMOS没有打开
					if(!((IO_CHG_MOS == 0) && ((uiPackStatus&0x0001) == 1)) )//在都关闭的状态下
					{
#if chargedebug
						printf("Input_vol_judge延时检测\n");
#endif	
						if(ConectFlg_poron_prev == 1)//
						{
							Input_vol_judge();//判断充电器的充电电压，有延时的判断
						}
						else//如果上次是第一次插
						{
							Input_vol_judge_poron();//因为只采集一次所以不用等待恢复
						}
								
					}	
					//2.CMOS打开了
	//				else
	//				{
	//					//就要判断，上一次是否没插 充电器
	//					if(ConectFlg_poron_prev == 0)
	//					{
	//#if chargedebug
	//						printf("Input_vol_judge_poron单次检测\n");
	//#endif							
	//						Input_vol_judge_poron();//单次检测
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
		
	  Chk_Fuse(); //检查有没有熔断
  	Chk_Cell_VolDIFF();


		
		MCU_CHK_CDmos();
//-------------------------------------------------------------------------------------------------	
	
	//mos状态检测和处理
    ChkMosStatus();		
    ControlMos(); 
#if 1
		updata_pack_fault_flag(); //25ms执行一次
#endif
	
		Info.uiBatStatus = uiBatStatus;
		Info.uiPackStatus = uiPackStatus;
//过流自恢复使能
    RecoveryOC();

    CheckLoad();
//不断的去检测充电器是否被插入
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
	REG.AFESCONF10 = 0x33;						//进入Power Down模式
	TWIWriteAFE(AFE_SCONF10, &REG.AFESCONF10);
	REG.AFESCONF1 |= 0x20; 
	TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);
}

