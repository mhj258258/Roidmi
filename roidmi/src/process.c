#include "System.h"
#include "AfeMtp.h"

//extern U8 xdata Short_flag;
U8 xdata Short_flag = 0; 
//U8 xdata Sflag = 0;
//extern U8 xdata Sflag;
/*******************************************************************************************
* ������: MCU_33V_Procude
* ��  ��: ��
* ����ֵ: ��
* ��  ��: ��Ϊ��������ʹ��
*********************************************************************************************/
void MCU_33V_Procude(void) 
{
	IO_POWER_ON = 1;
}


/*******************************************************************************************
* ������: MCU_Short_Clean
* ��  ��: ��
* ����ֵ: ��
* ��  ��: ����Լ��ⲿ�Ķ�·��־�������������ڲ��Ķ�·�Ƚϵ����õ�
*********************************************************************************************/
void MCU_Short_Clean(void)
{
	IO_SCP_RST = 1;
	Short_flag = 0;//��·���
}


/*******************************************************************************************
* ������: MCU_CHK_CDmos
* ��  ��: ��
* ����ֵ: ��
* ��  ��: �����ж��Լ��Ŀ���mos�ܿ���
*********************************************************************************************/

void MCU_CHK_CDmos(void)
{
	U8 ALLOW_CHG_FLAG = 0;
	U8 ALLOW_DSG_FLAG = 0;
	//�տ�ʼ״̬��MCU���ƿ���Ϊ�ر�״̬
//	IO_CHG_MOS = 1;
//	IO_DSG_MOS = 1;
	//ʲô�����
	//OIV = 1�������� FOPEN = 1//����˿�۶�
	if( OIV|| UIV || FOPEN || Odiff|| (ConectFlg_poron == 0) )//��������
	{
		IO_CHG_MOS = 1;//�����
		ALLOW_CHG_FLAG = 0;
	}
  else
	{
		IO_CHG_MOS = 0;//������
		ALLOW_CHG_FLAG = 1;
	}
	
//Short_flag =1˵����·
	if( FOPEN || Odiff || Short_flag)
	{
#if debug
		printf("\n Short_flag = %bd",Short_flag);   
#endif	
		IO_DSG_MOS = 1;//���ŵ�
		ALLOW_DSG_FLAG = 0;
	}
  else
	{
		IO_DSG_MOS = 0;//����ŵ�
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
* ������: updata_pack_fault_flag
* ��  ��: ��
* ����ֵ: ��
* ��  ��: �������´����־λ����Ҫuart���͵�
*********************************************************************************************/
extern pack_fault_flag packfault_flag;
//	if(bAFE_OV #|| bCTO#)//���߱�������Ӳ������					
//�ŵ�	if( bOTD# || bUTD# || bUV# || bOCD# || bAFE_SC# || (bFD&&bDSGEnd) || bLoadConectFlg || bCTO)
//���  if(bOTC# || bUTC# || bHV# || bOCC# || (bFC&&bCHGEnd))	//���������� ����б���		
//�Զ��� OIV#��UIV#,FOPEN#��Odiff#���Լ���Short_flag��·��־

void updata_pack_fault_flag(void)
{
		U16 packfaulttemp;
		packfault_flag.pack_flag_bit.pack_UV 			= bUV			; //Ƿѹ
		packfault_flag.pack_flag_bit.pack_AFE_OV 	= bAFE_OV	; //Ӳ������
		packfault_flag.pack_flag_bit.pack_AFE_SC	=	bAFE_SC	; //��·
		packfault_flag.pack_flag_bit.pack_OCC 		=	bOCC		; //������
		packfault_flag.pack_flag_bit.pack_UTD	  	=	bUTD		; //�ŵ����
		packfault_flag.pack_flag_bit.pack_OTD 		=	bOTD		; //�ŵ����
		packfault_flag.pack_flag_bit.pack_UTC 		=	bUTC		; //������
		packfault_flag.pack_flag_bit.pack_OTC 		=	bOTC		; //������
		
		packfault_flag.pack_flag_bit.pack_OCD 		=	bOCD 		; //�ŵ����
		packfault_flag.pack_flag_bit.pack_OCD2 		=	bOCD2		;
		packfault_flag.pack_flag_bit.pack_FOPEN  	=	FOPEN		; //����˿�۶�
		packfault_flag.pack_flag_bit.pack_Odiff 	=	Odiff		; //����ѹ�0.3
		packfault_flag.pack_flag_bit.pack_HV 			=	bHV			; //�������	
		packfault_flag.pack_flag_bit.pack_CTO 		=	bCTO		; //��ض���
		packfault_flag.pack_flag_bit.pack_UIV 		= UIV			; //�������ѹ����
		packfault_flag.pack_flag_bit.pack_OIV 		=	OIV			; //�������ѹ����


		//	for(i=0;i<16;i++)
		if(++packfault_cnt >= 5)//2.5s  *16һ����16  2.5*16=40��һ�Σ���һ��*2  80*25=2s
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
/****************************************20190925���ӵ�ˢ����******************************************/
		
	
