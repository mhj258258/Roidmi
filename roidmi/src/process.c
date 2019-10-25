#include "System.h"
#include "AfeMtp.h"

//extern U8 xdata Short_flag;
U8 xdata Short_flag = 0; 
//U8 xdata Sflag = 0;
//extern U8 xdata Sflag;
/*******************************************************************************************
* 函数名: MCU_33V_Procude
* 参  数: 无
* 返回值: 无
* 作  用: 作为开机调试使用
*********************************************************************************************/
void MCU_33V_Procude(void) 
{
	IO_POWER_ON = 1;
}


/*******************************************************************************************
* 函数名: MCU_Short_Clean
* 参  数: 无
* 返回值: 无
* 作  用: 清除自己外部的短路标志，这是用来和内部的短路比较调试用的
*********************************************************************************************/
void MCU_Short_Clean(void)
{
	IO_SCP_RST = 1;
	Short_flag = 0;//短路清除
}


/*******************************************************************************************
* 函数名: MCU_CHK_CDmos
* 参  数: 无
* 返回值: 无
* 作  用: 用来判断自己的控制mos管开关
*********************************************************************************************/

void MCU_CHK_CDmos(void)
{
	U8 ALLOW_CHG_FLAG = 0;
	U8 ALLOW_DSG_FLAG = 0;
	//刚开始状态下MCU控制开关为关闭状态
//	IO_CHG_MOS = 1;
//	IO_DSG_MOS = 1;
	//什么情况下
	//OIV = 1不允许充电 FOPEN = 1//保险丝熔断
	if( OIV|| UIV || FOPEN || Odiff|| (ConectFlg_poron == 0) )//如果不充电
	{
		IO_CHG_MOS = 1;//不充电
		ALLOW_CHG_FLAG = 0;
	}
  else
	{
		IO_CHG_MOS = 0;//允许充电
		ALLOW_CHG_FLAG = 1;
	}
	
//Short_flag =1说明短路
	if( FOPEN || Odiff || Short_flag)
	{
#if debug
		printf("\n Short_flag = %bd",Short_flag);   
#endif	
		IO_DSG_MOS = 1;//不放电
		ALLOW_DSG_FLAG = 0;
	}
  else
	{
		IO_DSG_MOS = 0;//允许放电
		ALLOW_DSG_FLAG = 1;
	}
	
#if (chargedebug|discurdebug)
		printf("\n Short_flag= %bd,FOPEN = %bd,Odiff =%bd,UIV =%bd ,OIV= %bd \n",Short_flag,FOPEN,Odiff,UIV,OIV);   
#endif
#if (chargedebug|discurdebug)
		printf("\n ALLOW_CHG_FLAG= %bd ALLOW_DSG_FLAG = %bd\n",ALLOW_CHG_FLAG,ALLOW_DSG_FLAG);   
#endif

}


#if 1
/*******************************************************************************************
* 函数名: updata_pack_fault_flag
* 参  数: 无
* 返回值: 无
* 作  用: 用来更新错误标志位，需要uart发送的
*********************************************************************************************/
extern pack_fault_flag packfault_flag;
//	if(bAFE_OV #|| bCTO#)//断线保护，和硬件过冲					
//放电	if( bOTD# || bUTD# || bUV# || bOCD# || bAFE_SC# || (bFD&&bDSGEnd) || bLoadConectFlg || bCTO)
//充电  if(bOTC# || bUTC# || bHV# || bOCC# || (bFC&&bCHGEnd))	//发生过报错 充电中报错		
//自定义 OIV#，UIV#,FOPEN#，Odiff#，自己的Short_flag短路标志

void updata_pack_fault_flag(void)
{
		U16 packfaulttemp;
		packfault_flag.pack_flag_bit.pack_UV 			= bUV			; //欠压
		packfault_flag.pack_flag_bit.pack_AFE_OV 	= bAFE_OV	; //硬件过充
		packfault_flag.pack_flag_bit.pack_AFE_SC	=	bAFE_SC	; //短路
		packfault_flag.pack_flag_bit.pack_OCC 		=	bOCC		; //充电过流
		packfault_flag.pack_flag_bit.pack_UTD	  	=	bUTD		; //放电低温
		packfault_flag.pack_flag_bit.pack_OTD 		=	bOTD		; //放电过温
		packfault_flag.pack_flag_bit.pack_UTC 		=	bUTC		; //充电低温
		packfault_flag.pack_flag_bit.pack_OTC 		=	bOTC		; //充电过温
		
		packfault_flag.pack_flag_bit.pack_OCD 		=	bOCD 		; //放电过流
		packfault_flag.pack_flag_bit.pack_OCD2 		=	bOCD2		;
		packfault_flag.pack_flag_bit.pack_FOPEN  	=	FOPEN		; //保险丝熔断
		packfault_flag.pack_flag_bit.pack_Odiff 	=	Odiff		; //单节压差超0.3
		packfault_flag.pack_flag_bit.pack_HV 			=	bHV			; //软件过充	
		packfault_flag.pack_flag_bit.pack_CTO 		=	bCTO		; //电池断线
		packfault_flag.pack_flag_bit.pack_UIV 		= UIV			; //充电器电压过低
		packfault_flag.pack_flag_bit.pack_OIV 		=	OIV			; //充电器电压过高


		//	for(i=0;i<16;i++)
		if(++packfault_cnt >= 5)//2.5s  *16一次是16  2.5*16=40正一次，反一次*2  80*25=2s
		{		
			IO_Error_1 = 0;
			IO_Error_2 = 0;
			IO_Error_4 = 0;
			IO_Error_8 = 0;				
			packfault_cnt = 0;
			if(packfault_i >= 16)
			{
				packfault_i = 0;
			}				
			packfaulttemp = (packfault_flag.pack_flag_all&(0x0001 << packfault_i));
			packfault_i++;
		

				switch(packfaulttemp) // 1111 1111 1111 1111  ffff
				{
					case 0x0001:  
						IO_Error_1 = 0;
						IO_Error_2 = 0;
						IO_Error_4 = 0;
						IO_Error_8 = ~IO_Error_8;
						//Delay1ms(2000);
						break;
					case 0x0002:
						IO_Error_1 = 0;
						IO_Error_2 = 0;
						IO_Error_4 = ~IO_Error_4;
						IO_Error_8 = 0;
						//Delay1ms(2000);
						break;
					case 0x0004:
						IO_Error_1 = 0;
						IO_Error_2 = 0;
						IO_Error_4 = ~IO_Error_4;
						IO_Error_8 = ~IO_Error_8;
						//Delay1ms(2000);
						break;
					case 0x0008:
						IO_Error_1 = 0;
						IO_Error_2 = ~IO_Error_2;
						IO_Error_4 = 0;
						IO_Error_8 = 0;
						//Delay1ms(2000);
						break;
					case 0x0010:
						IO_Error_1 = 0;
						IO_Error_2 = ~IO_Error_2;
						IO_Error_4 = 0;
						IO_Error_8 = ~IO_Error_8;
						//Delay1ms(2000);
						break;
					case 0x0020:
						IO_Error_1 = 0;
						IO_Error_2 = ~IO_Error_2;
						IO_Error_4 = ~IO_Error_4;
						IO_Error_8 = 0;
						//Delay1ms(2000);
						break;
					case 0x0040:
						IO_Error_1 = 0;
						IO_Error_2 = ~IO_Error_2;
						IO_Error_4 = ~IO_Error_4;
						IO_Error_8 = ~IO_Error_8;
						//Delay1ms(2000);
						break;
					case 0x0080:
						IO_Error_1 = ~IO_Error_1;
						IO_Error_2 = 0;
						IO_Error_4 = 0;
						IO_Error_8 = 0;
						//Delay1ms(2000);
						break;
					case 0x0100:
						IO_Error_1 = ~IO_Error_1;
						IO_Error_2 = 0;
						IO_Error_4 = 0;
						IO_Error_8 = ~IO_Error_8;
						//Delay1ms(2000);
						break;
					case 0x0200:
						IO_Error_1 = ~IO_Error_1;
						IO_Error_2 = 0;
						IO_Error_4 = ~IO_Error_4;
						IO_Error_8 = 0;
						//Delay1ms(2000);
						break;
					case 0x0400:
						IO_Error_1 = ~IO_Error_1;
						IO_Error_2 = 0;
						IO_Error_4 = ~IO_Error_4;
						IO_Error_8 = ~IO_Error_8;
						//Delay1ms(2000);
						break;
					case 0x0800:
						IO_Error_1 = ~IO_Error_1;
						IO_Error_2 = ~IO_Error_2;
						IO_Error_4 = 0;
						IO_Error_8 = 0;
						//Delay1ms(2000);
						break;
					case 0x1000:
						IO_Error_1 = ~IO_Error_1;
						IO_Error_2 = ~IO_Error_2;
						IO_Error_4 = 0;
						IO_Error_8 = ~IO_Error_8;
						//Delay1ms(2000);
						break;
					case 0x2000:
						IO_Error_1 = ~IO_Error_1;
						IO_Error_2 = ~IO_Error_2;
						IO_Error_4 = ~IO_Error_4;
						IO_Error_8 = 0;
						//Delay1ms(2000);
						break;
					case 0x4000:
						IO_Error_1 = ~IO_Error_1;
						IO_Error_2 = ~IO_Error_2;
						IO_Error_4 = ~IO_Error_4;
						IO_Error_8 = ~IO_Error_8;
					//Delay1ms(2000);
						break;
					case 0x8000:
						if(++fault_cntoiv >= 2)
						{
							fault_cntoiv = 0;
							IO_Error_1 = ~IO_Error_1;
							IO_Error_2 = ~IO_Error_2;
							IO_Error_4 = ~IO_Error_4;
							IO_Error_8 = ~IO_Error_8;
						}
						break;
					default:
						IO_Error_1 = 0;
						IO_Error_2 = 0;
						IO_Error_4 = 0;
						IO_Error_8 = 0;
						break;
				}
			
			}

		
}





#endif
/****************************************20190925增加地刷保护******************************************/
		
	
