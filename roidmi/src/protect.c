#include "System.h"
#include "AfeMtp.h"

//输入电压的最大最小值检测
#define Input_vol_max 2319		//347   //34.7V对应的AD值2810，
#define Input_vol_min 2297		//337   //2729测试使用
#define Input_vol_maxR 2314	//345  //2793
#define Input_vol_minR 2301	//335	//2712

#define allsistor  1200 //输出总的电阻
#define partsistor 100	//分压的电阻


/*************************************************************************************************
* 函数名: Input_charger_detect_poron
* 参  数: 无
* 返回值: 无
* 作  用: 检测是否有适配器插入
*************************************************************************************************/
void Input_charger_detect_poron(void)
{
		REG.AFESCONF1 |= 0x01;
		TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);	 				//enable charger detect 
		Delay1ms(100);
		TWIReadAFE(AFE_BSTATUS, &REG.AFEBSTATUS);

			if((REG.AFEBSTATUS&0x01) != 0)//充电器连接状态是1，已连接
			{
				//Delay1ms(2);
				if((REG.AFEBSTATUS&0x01) != 0)//充电器连接状态是1，已连接
				{
					ConectFlg_poron = 1;
				}
				else
				{
					ConectFlg_poron = 0;
				}
			}
			else		
			{
					ConectFlg_poron = 0;
			}
			REG.AFESCONF1 &= ~0x01;//充电器检测位置0
			TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);//清除

#if chargedebug
			printf("\n ConectFlg_poron = %bd \n",ConectFlg_poron);    
#endif	
	
}


void Input_charger_detect(void)
{
		REG.AFESCONF1 |= 0x01;
		TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);	 				//enable charger detect 
		if(++charge_detect_cnt >= 2)//这边是延时的作用
		{
			charge_detect_cnt = 0;
			TWIReadAFE(AFE_BSTATUS, &REG.AFEBSTATUS);

			if((REG.AFEBSTATUS&0x01) != 0)//充电器连接状态是1，已连接
			{
				//Delay1ms(2);
				if((REG.AFEBSTATUS&0x01) != 0)//充电器连接状态是1，已连接
				{
					ConectFlg_poron = 1;
				}
				else
				{
					ConectFlg_poron = 0;
				}
			}
			else		
			{
					ConectFlg_poron = 0;
			}
			REG.AFESCONF1 &= ~0x01;//充电器检测位置0
			TWIWriteAFE(AFE_SCONF1, &REG.AFESCONF1);//清除

#if chargedebug
			printf("\n ConectFlg_poron = %bd \n",ConectFlg_poron);    
#endif	
	}
}






/*************************************************************************************************
* 函数名: Input_vol_judge
* 参  数: 无
* 返回值: 无
* 作  用: 对充电的电压值进行判断
*************************************************************************************************/
U8 xdata OIV = 0,UIV = 0 ;//IptEN = 0;
void Input_vol_judge_poron(void) //单次检测
{
	U16 xdata Input_Vol = 0;
//	if(IO_CHG_MOS == 1)
	{
		Input_Vol = ADCRead(CHG_VOL); //
#if chargedebug
		printf("\n Input_Vol_poron = %d \n",Input_Vol);    
#endif	
		if(((Input_Vol)>Input_vol_max))//>34.7V和<33.7V
		{
			OIV = 1;//输入电压过压 
			UIV = 0;
		}
		else if((Input_Vol)< Input_vol_min)
		{
			UIV = 1;
			OIV = 0;
		}
		else
		{
			OIV = 0;//允许充电
			UIV = 0;
		}

	}
#if debug
		printf("\n por输入适配电压  UIV =%bd OIV= %bd \n",UIV,OIV);    
#endif
}
U8 xdata ChkIptOCnt = 0;
U8 xdata ChkIptUCnt = 0;
U8 xdata ChkIptOCntR = 0;
U8 xdata ChkIptUCntR = 0;
#define DelayChkIpt 2//40
#if 0
void Input_vol_judge(void)
{
	U16 xdata Input_Vol = 0;
//	if(IO_CHG_MOS == 1)
	{
		Input_Vol = ADCRead(CHG_VOL);
#if chargedebug
		printf("\n Input_Vol = %d \n",Input_Vol);    
#endif	
		if(((Input_Vol)>Input_vol_max))//>34.7V和<33.7V
		{
			if(++ChkIptOCnt > DelayChkIpt)
			{
				ChkIptOCnt = 0;
				OIV = 1;//输入电压过压 
			}
			ChkIptRCnt = 0;
		}
		else if((Input_Vol)< Input_vol_min)
		{
			if(++ChkIptUCnt > DelayChkIpt)
			{
				ChkIptUCnt = 0;
				UIV = 1;
			}
			ChkIptRCnt = 0;
		}
		else if(((Input_Vol) > Input_vol_minR)&&((Input_Vol) < Input_vol_maxR))
		{
			ChkIptUCnt = 0;
			ChkIptOCnt = 0;
			if(++ChkIptRCnt > DelayChkIpt)
			{
				ChkIptRCnt = 0;
				OIV = 0;//允许充电
				UIV = 0;
			}

		}
		else
		{
			;//保持
		}

	}
#if debug
		printf("\n 输入适配电压 UIV =%bd OIV= %bd \n",UIV,OIV);    
#endif
}
#else
void Input_vol_judge(void)
{
	U16 xdata Input_Vol = 0;
//	if(IO_CHG_MOS == 1)
	
		Input_Vol = ADCRead(CHG_VOL);
#if chargedebug
		printf("\n -----Input_Vol----- = %d \n",Input_Vol);    
#endif
	
		if(!OIV)
		{
			if(((Input_Vol) > Input_vol_max))//>34.7V和<33.7V
			{
				if(++ChkIptOCnt >= DelayChkIpt)
				{
					ChkIptOCnt = 0;
					ChkIptOCntR = 0;
					OIV = 1;//输入电压过压 
				}				
			}		
			else if((Input_Vol) < Input_vol_maxR)
			{
				if(ChkIptOCnt > 0)
				{
					ChkIptOCnt--;
				}		
			}
		}
		else
		{
			if(((Input_Vol) < Input_vol_maxR))
			{
				if(++ChkIptOCntR >= DelayChkIpt)
				{
					ChkIptOCnt = 0;
					ChkIptOCntR = 0;
					OIV = 0;//输入电压过压 
				}				
			}		
			else if((Input_Vol) > Input_vol_max)
			{
				if(ChkIptOCntR > 0)
				{
					ChkIptOCntR--;
				}		
			}
		}
		
		
		
		if(!UIV)
		{
			if(((Input_Vol) < Input_vol_min))//>34.7V和<33.7V
			{
				if(++ChkIptUCnt >= DelayChkIpt)
				{
					ChkIptUCnt = 0;
					ChkIptUCntR = 0;
					UIV = 1;//输入电压过压 
				}				
			}		
			else if((Input_Vol) > Input_vol_minR)
			{
				if(ChkIptUCnt > 0)
				{
					ChkIptUCnt--;
				}		
			}
		}
		else
		{

			if(((Input_Vol) > Input_vol_minR))
			{
				if(++ChkIptUCntR >= DelayChkIpt)
				{
					ChkIptUCnt = 0;
					ChkIptUCntR = 0;
					UIV = 0;//输入电压过压 
				}				
			}		
			else if((Input_Vol) < Input_vol_min)
			{
				if(ChkIptUCntR > 0)
				{
					ChkIptUCntR--;
				}		
			}
		}
	
	
#if chargedebug
		printf("\n 输入适配电压 UIV =%bd OIV= %bd \n",UIV,OIV);    
#endif
}
#endif


/*************************************************************************************************
* 函数名: Chk_Fuse
* 参  数: 无
* 返回值: 
* 作  用: 检测保险丝状态
*************************************************************************************************/

U8 xdata FOPEN = 0;//默认状态是导通状态
#define OffsetpackVol 2000

void Chk_Fuse_poron(void)
{
	U32 xdata Pack_Vol = 0;
	//U32 xdata Pack_Vol2 = 0;
	//开启输出电压检测
	IO_PACK_VADC = 1;//这边可能里面要加延时
	Delay1ms(10);
	Pack_Vol = (U32)ADCRead(PACK_VOL);
//	Delay1ms(100);
#if debug
		printf("\n AD 值Pack_Vol  = %ld \n",(U32)Pack_Vol);    
#endif	
	//换算成电压值
//	Pack_Vol =  ((Pack_Vol*33*(1000/10)/4096)*(allsistor/partsistor));//转换成mv
		Pack_Vol = (((Pack_Vol*3300)/4096)*(allsistor/partsistor));//转换成mv
#if debug
		printf("\n 转换的Pack_Vol = %ld \n",(U32)Pack_Vol);    
#endif	
	//Pack_VoL值不在正常范围内的话
	
	if((Pack_Vol < (Info.ulVoltage-OffsetpackVol))||(Pack_Vol > (Info.ulVoltage+OffsetpackVol)))
	{		
			FOPEN = 1;//fuse状态是断开状态
	}
	else
	{
			FOPEN = 0;
	}
	//关闭检测
	IO_PACK_VADC = 0;//暂时配置0
#if debug
	printf("\n por保险丝状态 FOPEN =%bd \n",FOPEN);    
#endif
}

U8 xdata ChkFuseCnt = 0;
U8 xdata ChkFuseRCnt = 0;
#define DelayChkFuse 2//40
void Chk_Fuse(void)
{
	U32 xdata Pack_Vol = 0;
	//开启输出电压检测
	IO_PACK_VADC = 1;
	Pack_Vol = (U32)ADCRead(PACK_VOL);
	//Pack_VoL值不在正常范围内的话
		Pack_Vol = (((Pack_Vol*3300)/4096)*(allsistor/partsistor));//转换成mv
#if debug
		printf("\n Chk_Fuse:转换的Pack_Vol = %ld \n",(U32)Pack_Vol);    
#endif	
	if((Pack_Vol < (Info.ulVoltage-OffsetpackVol))||(Pack_Vol > (Info.ulVoltage+OffsetpackVol)))
	{
#if debug
		printf("\n ----ChkFuseCnt=%bd ---\n",ChkFuseCnt);    
#endif
		if(++ChkFuseCnt > DelayChkFuse)
		{
			ChkFuseCnt = 0;
			//关闭检测
			IO_PACK_VADC = 0;
			FOPEN = 1;//fuse状态是断开状态
#if debug
		printf("\n FOPEN is 1 !!!!!!!!!!!!\n");    
#endif
		}
		ChkFuseRCnt = 0;
	}
	else
	{
		if(++ChkFuseRCnt > DelayChkFuse)
		{
			ChkFuseRCnt = 0;
			FOPEN = 0;
		}
		ChkFuseCnt = 0;
	}
#if debug
		printf("\n 保险丝状态 FOPEN =%bd \n",FOPEN);    
#endif
}


/*************************************************************************************************
* 函数名: Chk_Cell_VolDIFF
* 参  数: uiCellVmax uiCellVmin
* 返回值: 
* 作  用: 检测单体电压差 Info.uiVCell[i]
*************************************************************************************************/
U8 xdata Odiff = 0;//默认状态是不超过0.3V的
void Chk_Cell_VolDIFF_poron(void)
{
	U16 VOL_Diff = 0;
	VOL_Diff = uiCellVmax - uiCellVmin;
#if debug
		printf("\n VOL_Diff_poron =%hd uiCellVmax  = %hd,uiCellVmin = %hd----\n",VOL_Diff,uiCellVmax,uiCellVmin);    
#endif
	if(VOL_Diff > 300)//大于0.3v
	{
			Odiff = 1; //错误标志
	}
	else
	{
			Odiff = 0;
	}
#if debug
		printf("\n por单体电压差Odiff = %bd \n",Odiff);    
#endif	
	
}




U8 xdata ChkDiffCnt = 0;
U8 xdata ChkDiffRCnt = 0;
#define DelayChkDiff  2  //40
void Chk_Cell_VolDIFF(void)
{
	U16 VOL_Diff = 0;
	VOL_Diff = uiCellVmax - uiCellVmin;
#if debug
		printf("\n -----VOL_Diff =%hd uiCellVmax  = %hd,uiCellVmin = %hd----\n",VOL_Diff,uiCellVmax,uiCellVmin);    
#endif
	if(VOL_Diff > 300)//大于0.3v
	{
#if debug
		printf("\n ----ChkDiffCnt=%bd ---\n",ChkDiffCnt);    
#endif
		if(++ChkDiffCnt > DelayChkDiff)
		{
			ChkDiffCnt = 0;
			Odiff = 1; //错误标志
#if debug
		printf("\n Odiff is 1 !!!!!!!!!!!!\n");    
#endif
		}
		ChkDiffRCnt = 0;
	}
	else
	{
		if(++ChkDiffRCnt > DelayChkDiff)
		{
			ChkDiffRCnt = 0;
			Odiff = 0;
		}
		ChkDiffCnt = 0;
	}
#if debug
		printf("\n 单体电压差Odiff = %bd \n",Odiff);    
#endif	
}






/******************************************20190925地刷电流检测************************************************/













