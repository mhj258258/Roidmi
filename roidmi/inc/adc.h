#ifndef __ADC_H_
#define __ADC_H_





//for adc
//#define CHG_VOL  0x0000 //这个没变
//#define PACK_VOL 0x0001

/*******20190925********/
#define PACK_VOL 		0x08  //更换成p1.6口 AN8
#define CHG_VOL 		0x03  //更换成P0.5口 AN3
#define Gbrush_VOL  0x05  //AN5 暂时还未用到
#define CHG_CUR  		0x00  //外部充电电留检测 AN0 暂时还未用到




void ADC_init(void);
U16 ADCRead(U8 ucAdcChannal);



#endif









