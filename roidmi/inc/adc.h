#ifndef __ADC_H_
#define __ADC_H_





//for adc
//#define CHG_VOL  0x0000 //���û��
//#define PACK_VOL 0x0001

/*******20190925********/
#define PACK_VOL 		0x08  //������p1.6�� AN8
#define CHG_VOL 		0x03  //������P0.5�� AN3
#define Gbrush_VOL  0x05  //AN5 ��ʱ��δ�õ�
#define CHG_CUR  		0x00  //�ⲿ��������� AN0 ��ʱ��δ�õ�




void ADC_init(void);
U16 ADCRead(U8 ucAdcChannal);



#endif









