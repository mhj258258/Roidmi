#include "System.h"
#include "AfeMtp.h"


/*************************************************************************************************
* ������: ADC_init
* ��  ��: ��
* ����ֵ: ��
* ��  ��: 
*************************************************************************************************/
void ADC_init(void)
{
/********ADC ini********/
	//ADC��ʱ������                ����ʱ��ѡ��
	//[4��7] :0000 1��ϵͳ����    [3:0] 0000 ����ʱ��Ϊ2��ad
	//        1010 32��ϵͳ����          1111 15��Tad
  //				1111 192��ϵͳ����
	ADT = 0xAF;//������ú�21һ��
	
	//ADCON1 
	//ADON(����λ) ADCIF��adת����ɱ�־λ�� REFC��ѡ���׼��ѹ��  XTRGEN���ⲿ�ж�2��������ת�����ƣ�	PCATRGEN  PWMTRGEN  TIMTRGEN  GO/DONE
	//  1            0												1->0					 				0																			0  			   0				0        0
	ADCON1 = 0x80;
	//ADCON2 
  //VBG(1.2V��׼Դ) GRP[2:0](ת��ͨ���������ݶ�4��)  -  TGAP[2:0](һ��ͨ��ת����ϵ���һ��ͨ����ʼ�ļ��)
	// 0								011   		                     0	 001 ���޵ȴ�ʱ�䣩2��ADC����
  ADCON2 = 0x31;
	//SEQCON
	//ALR(��4λ�����ADDxH)- - - - REG[2:0]����������ַѡ��
	// 1									 0 0 0 0   011��ADD0L��ADD0H��
	//SEQCON = 0x83;
	//ADCH1 ͨ�����üĴ���
	//ʹ��ͨ��0��ͨ��1
	//0000 0011  AN 0,3,5,8ͨ��
	ADCH1 = 0x29;              // 0010 1001
	ADCH2 = 0x01;									//		001	  �������ȼ��洢�� SEQCH 0~3,�������ȼ��ߵ���5 ��Ȼ����3��Ȼ����0��Ȼ����8
	//ͨ���Ĵ���ѡ��
	//SEQCHX
//	SEQCHx = 0x0000;//(CH0)�л���ʱ���� Ĭ����ͨ��0

}
/*************************************************************************************************
* ������: GetAD_Value
* ��  ��: ͨ����
* ����ֵ: ��
* ��  ��: 
*************************************************************************************************/

//static U16 GetAD_Value(U8 ucAdcChannal)
//{
//		U16 uiConvertValue = 0x00; //�任���ֵ
//		
//		SEQCON = 0x80;//(0x80 | ucAdcChannal);//0,1,2,3
//		nop_();
//		SEQCHx = ucAdcChannal;
//	
//		_nop_();
//		_nop_();
//	//����ת��(���û���õ����ֱȽϹ���)
//		ADCON1 |= 0x01;				   	//����ADת��
//	//�ȴ�ADת����ɣ�10λ��ֵ��	
//		while(ADCON1&0x01);	
//	//ADD0L A7 A6 A5 A4 A3 A2 A1 A0  ADD0H�� A11 A10 A9 A8  
//		uiConvertValue |= ((ADDxH&0x0f)<<8);		
//		_nop_();
//		uiConvertValue |= ADDxL;	
//#if 0
//		TI = 0;
//		TI = 1;	
//		printf("\n ADDxH = %bd ,ADDxL =%bd ,uiConvertValue = %hd\n",ADDxH,ADDxL,(uiConvertValue&0x0fff));    
//#endif	
//	
//		return(uiConvertValue&0x0fff);
//}

//�ο�
U16 GetAD_Value(U8 ucAdcChannal)
{
	U16 uiConvertValue = 0x00;
	
    ADT = 0x9A;     		//24uS
    ADCH1 = 0x29;           //??CH1/5/6   0010 1001   //AN0,AN3,AN5,AN8
	  ADCH2 = 0x01;												//0x62	0110 0010
    SEQCON = (0x90 | ucAdcChannal);  //1001 
	//ADCON2 
  //VBG(1.2V��׼Դ) GRP[2:0](ת��ͨ���������ݶ�4��)  -  TGAP[2:0](һ��ͨ��ת����ϵ���һ��ͨ����ʼ�ļ��)
	// 0								011   		                     0	 001 ���޵ȴ�ʱ�䣩2��ADC����

    ADCON2 = 0x31;  //0101 SEQCH0-SEQCH5
		_nop_();
		_nop_();
	  SEQCHx &= 0x00; 
    SEQCHx |= ucAdcChannal;
    ADCON1 = 0x80;
		_nop_();
	  _nop_();
		_nop_();
		ADCON1 |= 0x01;
		while(ADCON1&0x01){_nop_();}
		ADCON1 &= ~(0xc0);						//?ADC Flag,???ADC????   1100 0000
    SEQCON = (0x80 | ucAdcChannal);
		uiConvertValue = ((U16)ADDxH<<8) | ADDxL;	//get the data of ADC
	
	return(uiConvertValue);
}



/*************************************************************************************************
* ������: paixu
* ��  ��: ���ݵ�ַ ����
* ����ֵ: ��
* ��  ��: 
*************************************************************************************************/
//��С��������
static void paixu(U16 arr[],U8 len)//��ͷ����ʼ������Ԫ��֮�����Ƚ�
{
	int i,j,t;
		for(i=0;i<len-1;i++)
			for(j=0;j<len-1-i;j++)
			{
				if(arr[j]>arr[j+1])
				{
					t=arr[j];arr[j]=arr[j+1];arr[j+1]=t;
				}
			}
}



/*********************************************************
������:	uint ADCRead(uchar channel)
��  ��:	��λ��ƽ���˲���(���������ƽ���˲���
        ADCת������ת��18�Σ�ȥ����Сֵ�����ֵ����16��ƽ��ֵ,
����ֵ: channel��ģ������ͨ�� 
���ֵ: ��
����ֵ: �˲����ֵ
**********************************************************/
U16 ADCRead(U8 ucAdcChannal)
{

	U16 xdata sum=0, ad_temp = 0;   
	U16 xdata ad_val[18]={0,};
	U8 i;
	U16  adc_data;

  for(i=0;i<18;i++) 
 {	 	
	 ad_val[i] = GetAD_Value(ucAdcChannal);		
	 
	//Delay1ms(100);
#if 0
	 TI = 0;
	 TI = 1;
	 printf("ad_val[%bd] = %hd \n",i, ad_val[i] );
#endif
 }
	//��С��������
  paixu(ad_val,18);
  for(i=1;i<17;i++) 
 {	 	
	 sum +=	ad_val[i];//ȥͷȥβ
 }
	adc_data = sum>>4;//����16

	return adc_data;
}



/*******************************************20190925������ˢ�������***************************************/



















