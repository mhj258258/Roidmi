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
void GaugeManage(void) //1sִ��һ��
{
	U16 FccDelta;
//������ڳ�磬�бȽ����صĴ��󱨾�����������־����λ
	if(bCHGING | bAFE_SC | bOCC | bOCD | bOCD2)						//If the charge or discharge overcurrent
	{
		bVDQ = 0;	//��������������Ч��־λ
	}
	else
	{ 
		//E2uiNearFCC ��Ч�ŵ翪ʼʱ��ʣ������������������ֵ�����ֵ���ⲿ���õ�
		//ulFCC ��������   ulRCʣ�����
		//�������������������¶�          //200
		if(!bVDQ && (Info.ulRC>(Info.ulFCC-E2uiNearFCC)) && (uiTempeMin>E2uiLearnLowTemp)) //E2uiLearnLowTemp:2731
		{
    		bVDQ = 1;//��������������Ч��־λ
    		ulFCCCount = 0;
		}
	}
#if elecdebug
	printf("��о������������bVDQ = %x\n",((uiPackStatus&0x0400)>>10));
#endif
//���ڳ��
	if(bCHGING)										//charging  I*t = 1s
	{
		uiRCCharge += slCadcCurAverage;//��ÿ�ε���ֵ������					3600 /1s
		while(uiRCCharge >= VALUE_mAH)		//����ǻ��㣺��3600����λ����1mah����Ϊ3600*��1/3600��  i*h =1maH�����ʵ���Ǽ������maH��
		{
			uiRCCharge -= VALUE_mAH;			
			if(Info.ulRC < Info.ulFCC)//ʣ�����С�ڳ�������
			{
				UART_IRQ_DISABLE;
				Info.ulRC++;							//have charged 1mAh
				UART_IRQ_ENABLE;
			}
			else	  //ʣ��������ڵ��ڳ������� 
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
		uiRCDischarge += -slCadcCurAverage;//����ǻ��ַŵ�ĵ���
		if(uiRCDischarge >= VALUE_mAH) //�ڷŵ��ʱ���ڼ���������ֵ
		{
			while(uiRCDischarge >= VALUE_mAH)					
			{
				uiRCDischarge -= VALUE_mAH;
				ulDsgCycleCount++;		//�ŵ����		����mAh
#if elecdebug
	printf("�ŵ���� mAh 	= %ld,info.ulRC = %ld \n",ulDsgCycleCount,Info.ulRC);
#endif
				if(Info.ulRC > 0)
				{
					UART_IRQ_DISABLE;
					Info.ulRC--; //ʣ��ĵ����ݼ�
					UART_IRQ_ENABLE;
				}
				if(bVDQ)	//��������������Ч��־λ��������ʼ����������
				{
					ulFCCCount++; //�ܵ�����
				}
			}
	
#if elecdebug
	printf("ʣ����� Info.ulRC= %ld\n",Info.ulRC );
#endif

			//2000 �����2000mAh��
			if(ulDsgCycleCount >= E2ulCycleThreshold)				//�ŵ�������ܵ�mah�� > ϵͳ����ѭ���ŵ�������mAh��Ҳ���Ƿŵ��Ѿ�������
			{
				ulDsgCycleCount -= E2ulCycleThreshold; 
				E2uiCycleCount++; //ѭ���ŵ���� //�����ֵҪ�洢����
 			  Info.uiCycleCount = E2uiCycleCount;
				bWriteFlashFlg = 1;									//write E2uiCycleCount to flash flag
				ucWriteFlashCnt=0;
//		        UpEepromWithDelay();
			}
		}
	}
	//��������Ǹ���RSOC��ֵ����Ϊ����ulrc�Ѿ��䶯��
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

	
	if(bFD)	//Discharge end  ������֮���ٸ�������ֵ
	{
#if elecdebug
			printf("bFD --------\n" );
#endif
		if(bVDQ)														        //E2ulFCC updata
		{
#if elecdebug
			printf("bVDQ --------\n" );
#endif
			FccDelta = (Info.ulFCC/10)*FCC_UPDATE_PERCENT;//�������� * 30%
			
			if(ulFCCCount > Info.ulFCC+FccDelta)// > FCC*130%
			{
				ulFCCCount = Info.ulFCC + FccDelta;
			}
			else if(ulFCCCount+FccDelta < Info.ulFCC) //< FCC*30%
			{
				ulFCCCount = Info.ulFCC - FccDelta;
			}
			UART_IRQ_DISABLE;
			Info.ulFCC = ulFCCCount;			//�����ܵ�����ֵ			//write E2ulFCC to flash
			UART_IRQ_ENABLE;
			//д��EE
			E2ulFCC = ulFCCCount;        //�����ܵ�����ֵ ,Ҫд��EEPROM
#if elecdebug
			printf("����֮���������ֵ Info.ulFCC = %ld��E2ulFCC = %ld\n",Info.ulFCC,E2ulFCC );
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
		Info.ulRC = Info.ulFCC*Info.uiRSOC/100;//�ŵ��ֹ����ʣ���������Ϊ�����10%
		UART_IRQ_ENABLE;
	}
	else if(bFC)	//Charge end//�ŵ��ֹ����ʣ���������Ϊ�����100%
	{
#if elecdebug
			printf("bFC --------\n" );
#endif
		UART_IRQ_DISABLE;
		Info.ulRC = Info.ulFCC;//����ʣ�����ֵ
		Info.uiRSOC = 100;
		UART_IRQ_ENABLE;
	}
	
	UART_IRQ_DISABLE;
	if(bUV)				//����״̬��ʣ���������Ϊ0%			
	{
#if elecdebug
			printf("bUV --------\n" );
#endif
		Info.ulRC = 0;																
		Info.uiRSOC = 0;
	}
	else if(bHV)			//����״̬��ʣ���������Ϊ100%													
	{
#if elecdebug
			printf("bHV --------\n" );
#endif
		Info.ulRC = Info.ulFCC;
		Info.uiRSOC = 100;
	}
	//���ϸ���ʣ������������Ǹ���Ŀǰ�ĵ��������ðٷֱ�����ʾĿǰ�ĵ�����
	UART_IRQ_ENABLE;
	
#if elecdebug
	printf("ʣ����� Info.ulRC= %ld,�����ٷֱ�Info.uiRSOC = %d ��Info.ulFCC = %ld��E2ulFCC = %ld ,ulFCCCount = %ld\n",Info.ulRC ,Info.uiRSOC, Info.ulFCC, E2ulFCC,ulFCCCount);
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
		if(Info.ulVoltage < E2uiVOC[i]*ucCellNum)//����ܵĵ�ѹҪ�ȣ����ڣ��趨�ĵ�ѹֵ��*��������ѹС
		{
#if debug
		printf("\n E2uiVOC[%bd]= %d,E2uiDsgEndVol= %d \n",i,E2uiVOC[i],E2uiDsgEndVol);    
#endif
			if(i == 0)
			{
				if(Info.ulVoltage < E2uiDsgEndVol*ucCellNum)//С�ڽ�ֹ��ѹ  2900
				{
					Info.uiRSOC = 0;	
				}
				else
				{
					//������ܵ�ѹ - ���ڷŵ��ֹ��ѹ*10��*10/����һ�ڵ�ص�ѹ - ���ڷŵ��ֹ��ѹ��*10
				 	Info.uiRSOC = (U32)(Info.ulVoltage-E2uiDsgEndVol*ucCellNum)*10/((E2uiVOC[0]-E2uiDsgEndVol)*ucCellNum);	
				}
				break;
			}
			else
			{
				//ʣ������İٷֱ�
				Info.uiRSOC = 10*i + (U32)(Info.ulVoltage-E2uiVOC[i-1]*ucCellNum)*10/((E2uiVOC[i]-E2uiVOC[i-1])*ucCellNum);
#if elecdebug	
		printf("\n i=%bd  ʣ������ٷֱ�Info.uiRSOC = %d,Info.ulVoltage =%d,E2uiVOC[%bd] = %d ,E2uiVOC[%bd]=%d \n",i,Info.uiRSOC,Info.ulVoltage,i,E2uiVOC[i],i-1,E2uiVOC[i-1]);    
#endif
				break;
			}
		}
	}
//ʣ����� = ������*�ٷֱ�
	Info.ulRC = E2ulFCC*Info.uiRSOC/100;
	
#if elecdebug
		printf("\n E2ulFCC =%ld ʣ�����Info.ulRC = %ld \n",E2ulFCC,Info.ulRC);    
#endif
}

