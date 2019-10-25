#ifndef __PROCESS_H_
#define __PROCESS_H_


/*****************软硬件讯息*******************/
#define HWversion 0x10 //1.0版本
#define SWversion 0x10 //1.0版本
#define TX_SN     {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,HWversion}

//	if(bAFE_OV #|| bCTO#)//断线保护，和硬件过冲					
//放电	if( bOTD# || bUTD# || bUV# || bOCD# || bAFE_SC# || (bFD&&bDSGEnd) || bLoadConectFlg || bCTO)  OCD2
//充电  if(bOTC# || bUTC# || bHV# || bOCC# || (bFC&&bCHGEnd))	//发生过报错 充电中报错		
//自定义 OIV#，UIV#,FOPEN#，Odiff#，自己的Short_flag短路标志
typedef union{
	struct fault_flag
	{
		U16 pack_UV  		  : 1; //欠压
		U16 pack_AFE_OV   : 1; //硬件过充
		U16 pack_AFE_SC		: 1; //短路
		U16 pack_OCC 			: 1; //充电过流
		U16 pack_UTD	  	: 1; //放电低温
		U16 pack_OTD 			: 1; //放电过温
		U16 pack_UTC 			: 1; //充电低温
		U16 pack_OTC 			: 1; //充电过温
		
		U16 pack_OCD 			: 1; //放电过流
		U16 pack_OCD2 		: 1; //放电过流2级
		U16 pack_FOPEN  	: 1; //保险丝熔断
		U16 pack_Odiff 		: 1; //单节压差超0.3
		U16 pack_HV 			: 1; //软件过充	
		U16 pack_CTO 			: 1; //电池断线
		U16 pack_UIV 			: 1; //充电器电压过低
		U16 pack_OIV 			: 1; //充电器电压过高
	} pack_flag_bit;
	U16 pack_flag_all;
}pack_fault_flag;



#define DEBUG 0





//tomi定义工作模式
#define	MODE_DISCHARGE		1 //放电模式
#define	MODE_CHARGING		  2	//充电模式
#define	MODE_ERROR			  3 //错误模式


typedef enum
{
	SYST_ERROR_STATE = 0,	
	SYS_OK_STATE ,	
} SYS_STATE_BIT;

void MCU_33V_Procude(void);
void MCU_Short_Clean(void);
void MCU_CHK_CDmos(void);
void updata_pack_fault_flag(void);

#endif









