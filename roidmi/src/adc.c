#include "System.h"
#include "AfeMtp.h"


/*************************************************************************************************
* 函数名: ADC_init
* 参  数: 无
* 返回值: 无
* 作  用: 
*************************************************************************************************/
void ADC_init(void)
{
/********ADC ini********/
	//ADC的时钟周期                采样时间选择
	//[4：7] :0000 1个系统周期    [3:0] 0000 采样时间为2个ad
	//        1010 32个系统周期          1111 15个Tad
  //				1111 192个系统周期
	ADT = 0xAF;//这边配置和21一样
	
	//ADCON1 
	//ADON(允许位) ADCIF（ad转换完成标志位） REFC（选择基准电压）  XTRGEN（外部中断2启动序列转换控制）	PCATRGEN  PWMTRGEN  TIMTRGEN  GO/DONE
	//  1            0												1->0					 				0																			0  			   0				0        0
	ADCON1 = 0x80;
	//ADCON2 
  //VBG(1.2V基准源) GRP[2:0](转换通道数量，暂定4个)  -  TGAP[2:0](一个通道转换完毕到下一个通道开始的间隔)
	// 0								011   		                     0	 001 （无等待时间）2个ADC周期
  ADCON2 = 0x31;
	//SEQCON
	//ALR(高4位存放在ADDxH)- - - - REG[2:0]（缓存器地址选择）
	// 1									 0 0 0 0   011（ADD0L和ADD0H）
	//SEQCON = 0x83;
	//ADCH1 通道配置寄存器
	//使用通道0和通道1
	//0000 0011  AN 0,3,5,8通道
	ADCH1 = 0x29;              // 0010 1001
	ADCH2 = 0x01;									//		001	  按照优先级存储到 SEQCH 0~3,所以优先级高的是5 ，然后是3，然后是0，然后是8
	//通道寄存器选择
	//SEQCHX
//	SEQCHx = 0x0000;//(CH0)切换的时候用 默认是通道0

}
/*************************************************************************************************
* 函数名: GetAD_Value
* 参  数: 通道数
* 返回值: 无
* 作  用: 
*************************************************************************************************/

//static U16 GetAD_Value(U8 ucAdcChannal)
//{
//		U16 uiConvertValue = 0x00; //变换后的值
//		
//		SEQCON = 0x80;//(0x80 | ucAdcChannal);//0,1,2,3
//		nop_();
//		SEQCHx = ucAdcChannal;
//	
//		_nop_();
//		_nop_();
//	//开启转换(这边没有用到数字比较功能)
//		ADCON1 |= 0x01;				   	//启动AD转换
//	//等待AD转换完成（10位数值）	
//		while(ADCON1&0x01);	
//	//ADD0L A7 A6 A5 A4 A3 A2 A1 A0  ADD0H是 A11 A10 A9 A8  
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

//参考
U16 GetAD_Value(U8 ucAdcChannal)
{
	U16 uiConvertValue = 0x00;
	
    ADT = 0x9A;     		//24uS
    ADCH1 = 0x29;           //??CH1/5/6   0010 1001   //AN0,AN3,AN5,AN8
	  ADCH2 = 0x01;												//0x62	0110 0010
    SEQCON = (0x90 | ucAdcChannal);  //1001 
	//ADCON2 
  //VBG(1.2V基准源) GRP[2:0](转换通道数量，暂定4个)  -  TGAP[2:0](一个通道转换完毕到下一个通道开始的间隔)
	// 0								011   		                     0	 001 （无等待时间）2个ADC周期

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
* 函数名: paixu
* 参  数: 数据地址 长度
* 返回值: 无
* 作  用: 
*************************************************************************************************/
//从小到大排序
static void paixu(U16 arr[],U8 len)//从头部开始，相邻元素之间做比较
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
函数名:	uint ADCRead(uchar channel)
描  述:	中位置平均滤波法(防脉冲干扰平均滤波）
        ADC转换程序，转换18次，去掉最小值和最大值，求16次平均值,
输入值: channel—模拟输入通道 
输出值: 无
返回值: 滤波后的值
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
	//从小到大排序
  paixu(ad_val,18);
  for(i=1;i<17;i++) 
 {	 	
	 sum +=	ad_val[i];//去头去尾
 }
	adc_data = sum>>4;//除以16

	return adc_data;
}



/*******************************************20190925新增地刷电流检测***************************************/



















