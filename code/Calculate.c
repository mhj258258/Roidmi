/********************************************************************************
Copyright (C), Sinowealth Electronic. Ltd.
Author: 	andyliu
Version: 	V0.0
Date: 		2014/05/30
History:
	V0.0		2014/05/30		 Preliminary
********************************************************************************/
#include "system.h"
#include "Calibrate.h"
#include "AfeMtp.h"


/*******************************************************************************
Function: U16 CalcuTemp(U16 getdata)
Description:  Based on collected voltage ratio to calculate the corresponding temperature(-50~110)
Input:	 	
Output: 
Others:
*******************************************************************************/
#define REF_RES_VAL	1000				////resistance 10K=1000*10,  The unit is 10 ohms
U16	code NTC103AT[161]=		   //this table  the temp corresponding to the (resistance value/10) of 103AT
{
	32950, 30970,29124,27404,25799,24770,23335,21995,20743,19572,18850, 17796, 16810, 15885, 15019,
	14410, 13633, 12904, 12219, 11576, 11130, 10550, 10005, 9492, 9009,8643, 8208, 7798, 7412, 7048, 6777, 6447, 6136,
	5842, 5564, 5341, 5090, 4852, 4627, 4414, 4247, 4053, 3870, 3696, 3532,3390, 3241, 3099, 2964, 2836, 2728, 2611,
	2501, 2395, 2295, 2205, 2114, 2027, 1944, 1866, 1796, 1724, 1655, 1590, 1527,1469, 1412, 1357, 1305, 1256, 1209,
	1163, 1120, 1078, 1038, 1000, 963, 928, 894, 862, 831, 801, 772, 745, 719, 694, 670, 646, 624, 603,583, 562, 543,
	525, 508, 491, 474, 459, 444, 430, 416, 402, 389, 377, 365, 354, 342, 331, 321, 311, 302, 292, 283, 275, 267,259,
	251, 243, 236, 229, 223, 216, 210, 204, 198, 192, 186, 181, 176, 171, 167, 162, 157, 153, 149, 145, 141, 137, 133,
	130, 127, 123, 120, 117, 114, 111, 108, 105, 102, 100, 97, 95, 92, 90, 88, 86, 83, 81, 79, 78, 76
};
U16 CalcuTemp(U16 getdata)
{
	U8	i;
	U16	Temperature;
    U32 tempcalcu;
 //等效电阻值
	tempcalcu= (U32)getdata*REF_RES_VAL/(4096-getdata);     //calculte the resistance value of 103AT
    if(tempcalcu > 10)
    {
        tempcalcu -= 10;
    }   
 
	if(tempcalcu >= NTC103AT[0])		            	   //look up table to find the resieter  correspond temp
	{
		Temperature = 2731-500;			
	}
	else if(tempcalcu <= NTC103AT[160])
	{
		Temperature = 2731+1100;
	}
	else
	{
		i = ucTempeMiddle;
		if(tempcalcu > NTC103AT[i])
		{
			for(i=ucTempeMiddle-1; i>=0; i--)
			{
				if(tempcalcu <= NTC103AT[i])			  //NTC103AT[i+1]<resis<NTC103AT[i]
				{
					break;
				}
			}
		}
		else
		{
			for(i=ucTempeMiddle+1; i<160; i++)
			{
				if(tempcalcu > NTC103AT[i])			      //NTC103AT[i-1]<resis<NTC103AT[i]
				{
					break;
				}
			}
			i--;
		}
		ucTempeMiddle = i;
		
		Temperature = (U16)(ucTempeMiddle-50)*10+(NTC103AT[i]-tempcalcu)*10/(NTC103AT[i]-NTC103AT[i+1])+2731;	
	}
	return Temperature;	
}


/*******************************************************************************
Function: 
Description:  
Input:	 	
Output: 
Others:
*******************************************************************************/
void CalcuTemperature(void)
{
    U16 TempeData;
   //校准参数等后期测试的时候放入该数值E2siTS1Offset = 0x000
    TempeData = CalcuTemp(AFE.uiTS1) + E2siTS1Offset;  //获取的是外部的温度
    UART_IRQ_DISABLE;
    Info.uiTemp1 = TempeData; //Info.uiTemp1 存放的是外部的温度值
    UART_IRQ_ENABLE;

	 //这边前面没有ICtemp1数值的获取等到后面做硬件了实际调
    TempeData = (U32)AFE.uiICTemp1*1623/1000+71;				//caculate ICTemp1		0.1623*TEMP-266
    UART_IRQ_DISABLE;
    Info.uiICTemp1 = TempeData;
    UART_IRQ_ENABLE;
  
    if(ucCellNum>5)												//Cellmun>5 ic must be AFE
    {    
        TempeData = (U32)AFE.uiICTemp2*1623/1000+71;				//caculate ICTemp2		0.1623*TEMP-266     0.17*temp1 -270
        UART_IRQ_DISABLE;
        Info.uiICTemp2 = TempeData;
        UART_IRQ_ENABLE;
    }
 
    uiTempeMin = Info.uiTemp1;
    uiTempeMax = Info.uiTemp1;
    if(bTempNum)											   //two tempe detect
    {
        TempeData = CalcuTemp(AFE.uiTS2) + E2siTS2Offset;
        UART_IRQ_DISABLE;
        Info.uiTemp2 = TempeData;
        UART_IRQ_ENABLE;

        if(TempeData < uiTempeMin)
        {
            uiTempeMin = TempeData;
        }
        else
        {
            uiTempeMax = TempeData;
        }

    }
#if tempdebug
		printf("\n 换算值：外部uiTempeMax=%d,uiTempeMin=%d \n",uiTempeMax,uiTempeMin);    
#endif  		
	
		
#if tempdebug
		printf("\n 温度换算值：外部Info.uiTemp1=%d,Info.uiTemp2=%d_____IC内部Info.uiICTemp1=%d Info.uiICTemp2=%d\n",(Info.uiTemp1-2731),(Info.uiTemp2-2731),(Info.uiICTemp1-2731),(Info.uiICTemp2-2731));    
#endif  
		
}


/*******************************************************************************
Function: 
Description:  
1. Calculate a single battery voltage
2. Calculate the total voltage
Input:	 	
Output: 
Others:
*******************************************************************************/
void CalcuVoltage(void)
{
	U8 i;
	U32 TempPackVol=0, TempCellVol=0;
	uiCellVmax = 0;					  
	uiCellVmin = 6000;
#if debug
		TI = 0;
		TI = 1;	
		printf("ucCellNum = %bd\n",ucCellNum);    
#endif 
	for(i=ucCellNumOffset; i<(ucCellNum+ucCellNumOffset); i++)
	{
		//E2uiVPackGain =0x00A,0x022, = 2594校准用的 3800/2594
		TempCellVol = (U32)AFE.uiCell[i]*CALIVOL/E2uiVPackGain;		//Calculate a single battery voltage
	
		if(TempCellVol > uiCellVmax)										//calculate the max and min vlotage
		{
			uiCellVmax = TempCellVol;	
		}
		if(TempCellVol < uiCellVmin)
		{
			uiCellVmin = TempCellVol;	
		}

        if(!bBalancingFlg)										//if bCellOpenDecFlag this data will throw away
        {
					UART_IRQ_DISABLE;
					Info.uiVCell[i] = TempCellVol;
					UART_IRQ_ENABLE;
					TempPackVol += TempCellVol;	//这边就把每节电芯加起来了					
        }
	}
  //没有平衡 ，总的电量，如果是在平衡功能下就取消总电量值
	if(!bBalancingFlg)
    {
#if debug
		printf("\nbBalancingFlg is 0\n");    
#endif 
    	UART_IRQ_DISABLE;
    	Info.ulVoltage = TempPackVol;							  	//Calculate the total voltage
    	UART_IRQ_ENABLE;
    }

#if debug
		printf("\n 电芯总电压：Info.ulVoltage = %d \n",Info.ulVoltage);    
#endif  
#if debug
	  printf("\n 电芯电压值：TempCellVol = %ld,uiCellVmin = %d uiCellVmax = %d \n",TempCellVol,uiCellVmin,uiCellVmax);    
								
#endif  

	


}


/*******************************************************************************
Function: GetAFEData(void)
Description:  
Input:	 	
Output: 
Others:
*******************************************************************************/
bit GetAFEData(void)
{
	U8  i;
	BOOL result=1;
//8节电池的话，这边offset=2   ucCellNum+ucCellNumOffset = 8+2=10
	for(i=ucCellNumOffset; i<(ucCellNum+ucCellNumOffset); i++) //读取8组电池的数据
    {
			//0x0E+2*2就是从AFE_CELL3H开始
		if(!TWIReadAFE(AFE_CELL1H+2*i, (U8 xdata *)(&AFE.uiCell[i])))	//通过TWI读取VADC采集的电压值()
        {
					result = 0;
					return result;
				}
	}
	if(!TWIReadAFE(AFE_TS1H, (U8 xdata *)&AFE.uiTS1))
	{
		result = 0;
		return result;
	}
	if(!TWIReadAFE(AFE_TS2H, (U8 xdata *)&AFE.uiTS2))
	{
		result = 0;
		return result;
   }
//by tomi 添加IC温度检测
//-----------------------------------------
	if(!TWIReadAFE(AFE_TEMP1H, (U8 xdata *)&AFE.uiICTemp1))
	{
		result = 0;
		return result;
  }
	if(!TWIReadAFE(AFE_TEMP2H, (U8 xdata *)&AFE.uiICTemp2))
	{
		result = 0;
		return result;
	}
//-----------------------------------------		
	return result;
}


/*******************************************************************************
Function: BatteryInfoManage(void)
Description: First Calculate Temp1/Temp2/VCell1-VCell10/Current 
Input:
Output:
Others:
*******************************************************************************/
void BatteryInfoManage(void)
{
	if(GetAFEData())//获取电芯电压和温度（外部）
	{
		CalcuTemperature();
		
		CalcuVoltage();		

	}
}


