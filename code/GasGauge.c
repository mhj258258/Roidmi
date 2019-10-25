/********************************************************************************
Copyright (C), Sinowealth Electronic. Ltd.
Author: 	andyliu
Version: 	V0.0
Date: 		2014/05/30
History:
	V0.0		2014/05/30		 Preliminary
********************************************************************************/
#include "system.h"
#include "GasGauge.h"


/*******************************************************************************
Function: GaugeManage(void)
Description: 
Input: 
Output: 
Others:
********************************************************************************/
void GaugeManage(void) //1s执行一次
{
	U16 FccDelta;
//如果正在充电，有比较严重的错误报警，满容量标志不置位
	if(bCHGING | bAFE_SC | bOCC | bOCD | bOCD2)						//If the charge or discharge overcurrent
	{
		bVDQ = 0;	//满充容量更新有效标志位
	}
	else
	{ 
		//E2uiNearFCC 有效放电开始时，剩余容量与满充容量差值，这个值是外部设置的
		//ulFCC 充满电量   ulRC剩余电量
		//满容量更新允许的最低温度          //200
		if(!bVDQ && (Info.ulRC>(Info.ulFCC-E2uiNearFCC)) && (uiTempeMin>E2uiLearnLowTemp)) //E2uiLearnLowTemp:2731
		{
    		bVDQ = 1;//满充容量更新有效标志位
    		ulFCCCount = 0;
		}
	}
#if elecdebug
	printf("电芯满充容量更新bVDQ = %x\n",((uiPackStatus&0x0400)>>10));
#endif
//正在充电
	if(bCHGING)										//charging  I*t = 1s
	{
		uiRCCharge += slCadcCurAverage;//把每次电流值加起来					3600 /1s
		while(uiRCCharge >= VALUE_mAH)		//这边是换算：满3600个单位才是1mah，因为3600*（1/3600）  i*h =1maH这边其实就是计算多少maH了
		{
			uiRCCharge -= VALUE_mAH;			
			if(Info.ulRC < Info.ulFCC)//剩余电量小于充满电量
			{
				UART_IRQ_DISABLE;
				Info.ulRC++;							//have charged 1mAh
				UART_IRQ_ENABLE;
			}
			else	  //剩余电量大于等于充满电量 
			{
				UART_IRQ_DISABLE;
				Info.ulRC = Info.ulFCC;				
				UART_IRQ_ENABLE;			
				uiRCCharge = 0;
			}
		}
	}
	else if(bDSGING)													//discharging
	{
		uiRCDischarge += -slCadcCurAverage;//这边是积分放电的电流
		if(uiRCDischarge >= VALUE_mAH) //在放电的时候在计算满容量值
		{
			while(uiRCDischarge >= VALUE_mAH)					
			{
				uiRCDischarge -= VALUE_mAH;
				ulDsgCycleCount++;		//放电次数		多少mAh
#if elecdebug
	printf("放电次数 mAh 	= %ld,info.ulRC = %ld \n",ulDsgCycleCount,Info.ulRC);
#endif
				if(Info.ulRC > 0)
				{
					UART_IRQ_DISABLE;
					Info.ulRC--; //剩余的电量递减
					UART_IRQ_ENABLE;
				}
				if(bVDQ)	//满充容量更新有效标志位，充满后开始计数的容量
				{
					ulFCCCount++; //总的容量
				}
			}
	
#if elecdebug
	printf("剩余电量 Info.ulRC= %ld\n",Info.ulRC );
#endif

			//2000 如果有2000mAh了
			if(ulDsgCycleCount >= E2ulCycleThreshold)				//放电次数（总的mah） > 系统单次循环放电总量（mAh）也就是放电已经放完了
			{
				ulDsgCycleCount -= E2ulCycleThreshold; 
				E2uiCycleCount++; //循环放电次数 //这个数值要存储起来
 			  Info.uiCycleCount = E2uiCycleCount;
				bWriteFlashFlg = 1;									//write E2uiCycleCount to flash flag
				ucWriteFlashCnt=0;
//		        UpEepromWithDelay();
			}
		}
	}
	//下面这个是更新RSOC的值，因为上面ulrc已经变动了
	if(Info.ulRC < Info.ulFCC)											//Calculate RSOC
	{
		UART_IRQ_DISABLE;
		Info.uiRSOC = (U32)Info.ulRC*100/Info.ulFCC;
		UART_IRQ_ENABLE;
	}
	else												
	{
		UART_IRQ_DISABLE;
		Info.uiRSOC = 100;
		UART_IRQ_ENABLE;
	}
#if elecdebug
	printf("Info.uiRSOC = %d\n",Info.uiRSOC );
#endif

	
	if(bFD)	//Discharge end  过放了之后再更新容量值
	{
#if elecdebug
			printf("bFD --------\n" );
#endif
		if(bVDQ)														        //E2ulFCC updata
		{
#if elecdebug
			printf("bVDQ --------\n" );
#endif
			FccDelta = (Info.ulFCC/10)*FCC_UPDATE_PERCENT;//满充容量 * 30%
			
			if(ulFCCCount > Info.ulFCC+FccDelta)// > FCC*130%
			{
				ulFCCCount = Info.ulFCC + FccDelta;
			}
			else if(ulFCCCount+FccDelta < Info.ulFCC) //< FCC*30%
			{
				ulFCCCount = Info.ulFCC - FccDelta;
			}
			UART_IRQ_DISABLE;
			Info.ulFCC = ulFCCCount;			//更新总的容量值			//write E2ulFCC to flash
			UART_IRQ_ENABLE;
			//写入EE
			E2ulFCC = ulFCCCount;        //更新总的容量值 ,要写入EEPROM
#if elecdebug
			printf("过放之后更新容量值 Info.ulFCC = %ld，E2ulFCC = %ld\n",Info.ulFCC,E2ulFCC );
#endif
			bVDQ = 0;
      bWriteFlashFlg = 1;					//Updated calibration parameters, and written to the flash
    	ucWriteFlashCnt=0;
			UpEepromWithDelay();
		}


		UART_IRQ_DISABLE;
		if(Info.uiRSOC > 10) 
		{
			Info.uiRSOC = 10;
		}
		Info.ulRC = Info.ulFCC*Info.uiRSOC/100;//放电截止，将剩余电量调整为满充的10%
		UART_IRQ_ENABLE;
	}
	else if(bFC)	//Charge end//放电截止，将剩余电量调整为满充的100%
	{
#if elecdebug
			printf("bFC --------\n" );
#endif
		UART_IRQ_DISABLE;
		Info.ulRC = Info.ulFCC;//更新剩余电量值
		Info.uiRSOC = 100;
		UART_IRQ_ENABLE;
	}
	
	UART_IRQ_DISABLE;
	if(bUV)				//过放状态将剩余电量调整为0%			
	{
#if elecdebug
			printf("bUV --------\n" );
#endif
		Info.ulRC = 0;																
		Info.uiRSOC = 0;
	}
	else if(bHV)			//过充状态将剩余电量调整为100%													
	{
#if elecdebug
			printf("bHV --------\n" );
#endif
		Info.ulRC = Info.ulFCC;
		Info.uiRSOC = 100;
	}
	//以上更新剩余电量的作用是更新目前的电量，是用百分比来表示目前的电量的
	UART_IRQ_ENABLE;
	
#if elecdebug
	printf("剩余电量 Info.ulRC= %ld,电量百分比Info.uiRSOC = %d ，Info.ulFCC = %ld，E2ulFCC = %ld ,ulFCCCount = %ld\n",Info.ulRC ,Info.uiRSOC, Info.ulFCC, E2ulFCC,ulFCCCount);
#endif
}


/*******************************************************************************
Function:InitGasGauge() 
Description: Calculate the remaining capacity according to pack voltage  
Input:  NULL	
Output: NULL
Others:
*******************************************************************************/
void InitGasGauge(void)
{
	U8 i;
	
	Info.uiRSOC = 100;
	for(i=0; i<10; i++)
	{
#if debug
		printf("\n E2uiVOC[%bd]= %d, \n",i,E2uiVOC[i]);    //3600*8=25600
#endif		
		if(Info.ulVoltage < E2uiVOC[i]*ucCellNum)//如果总的电压要比（单节（设定的电压值）*节数）电压小
		{
#if debug
		printf("\n E2uiVOC[%bd]= %d,E2uiDsgEndVol= %d \n",i,E2uiVOC[i],E2uiDsgEndVol);    
#endif
			if(i == 0)
			{
				if(Info.ulVoltage < E2uiDsgEndVol*ucCellNum)//小于截止电压  2900
				{
					Info.uiRSOC = 0;	
				}
				else
				{
					//（电池总电压 - 单节放电截止电压*10）*10/（第一节电池电压 - 单节放电截止电压）*10
				 	Info.uiRSOC = (U32)(Info.ulVoltage-E2uiDsgEndVol*ucCellNum)*10/((E2uiVOC[0]-E2uiDsgEndVol)*ucCellNum);	
				}
				break;
			}
			else
			{
				//剩余电量的百分比
				Info.uiRSOC = 10*i + (U32)(Info.ulVoltage-E2uiVOC[i-1]*ucCellNum)*10/((E2uiVOC[i]-E2uiVOC[i-1])*ucCellNum);
#if elecdebug	
		printf("\n i=%bd  剩余电亮百分比Info.uiRSOC = %d,Info.ulVoltage =%d,E2uiVOC[%bd] = %d ,E2uiVOC[%bd]=%d \n",i,Info.uiRSOC,Info.ulVoltage,i,E2uiVOC[i],i-1,E2uiVOC[i-1]);    
#endif
				break;
			}
		}
	}
//剩余电量 = 总容量*百分比
	Info.ulRC = E2ulFCC*Info.uiRSOC/100;
	
#if elecdebug
		printf("\n E2ulFCC =%ld 剩余电量Info.ulRC = %ld \n",E2ulFCC,Info.ulRC);    
#endif
}

