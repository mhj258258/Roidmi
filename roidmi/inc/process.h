#ifndef __PROCESS_H_
#define __PROCESS_H_


/*****************��Ӳ��ѶϢ*******************/
#define HWversion 0x10 //1.0�汾
#define SWversion 0x10 //1.0�汾
#define TX_SN     {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,HWversion}

//	if(bAFE_OV #|| bCTO#)//���߱�������Ӳ������					
//�ŵ�	if( bOTD# || bUTD# || bUV# || bOCD# || bAFE_SC# || (bFD&&bDSGEnd) || bLoadConectFlg || bCTO)  OCD2
//���  if(bOTC# || bUTC# || bHV# || bOCC# || (bFC&&bCHGEnd))	//���������� ����б���		
//�Զ��� OIV#��UIV#,FOPEN#��Odiff#���Լ���Short_flag��·��־
typedef union{
	struct fault_flag
	{
		U16 pack_UV  		  : 1; //Ƿѹ
		U16 pack_AFE_OV   : 1; //Ӳ������
		U16 pack_AFE_SC		: 1; //��·
		U16 pack_OCC 			: 1; //������
		U16 pack_UTD	  	: 1; //�ŵ����
		U16 pack_OTD 			: 1; //�ŵ����
		U16 pack_UTC 			: 1; //������
		U16 pack_OTC 			: 1; //������
		
		U16 pack_OCD 			: 1; //�ŵ����
		U16 pack_OCD2 		: 1; //�ŵ����2��
		U16 pack_FOPEN  	: 1; //����˿�۶�
		U16 pack_Odiff 		: 1; //����ѹ�0.3
		U16 pack_HV 			: 1; //�������	
		U16 pack_CTO 			: 1; //��ض���
		U16 pack_UIV 			: 1; //�������ѹ����
		U16 pack_OIV 			: 1; //�������ѹ����
	} pack_flag_bit;
	U16 pack_flag_all;
}pack_fault_flag;



#define DEBUG 0





//tomi���幤��ģʽ
#define	MODE_DISCHARGE		1 //�ŵ�ģʽ
#define	MODE_CHARGING		  2	//���ģʽ
#define	MODE_ERROR			  3 //����ģʽ


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









