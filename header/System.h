#ifndef __SYSTEM_H
#define __SYSTEM_H

#include "SH39F003.h"
#include "ABSACC.H"
#include "INTRINS.H"
#include "C51_TYPE.H"
#include "Main.h"
#include "Uart.h"
#include "Led.h"
#include "Memory.h"
#include "McuFlash.h"
#include "TwiIO.h"
#include "TwiModule.h"
#include "Common.h"
#include "Key.h"
#include <stdio.h>
/************by tomi*************/

//#define IO_CHG_MOS 			   P0_7 //mcu���������Ƿ���
//#define IO_SCP_RST  			 P3_2 //�����·����
//#define IO_DSG_MOS  			 P0_6 //mcu��������mos�Ƿ�ŵ�
//#define IO_Error_1  			 P3_1 //led����io�ں����ʾ
//#define IO_Error_2  			 P3_5 //led����io�ں����ʾ
//#define IO_Error_4  			 P2_6 //led����io�ں����ʾ
//#define IO_Error_8  			 P2_7 //led����io�ں����ʾ
//#define IO_PACK_VADC  		 P1_6 //��λ�ߵ�ƽ�˲�������pacl��ѹ�ɼ�
//#define IO_POWER_ON 			 P0_5 //MCU���ƹ����·����
      
/*************20190925************/
#define IO_PACK_VADC  		 P0_3 //��λ�ߵ�ƽ�˲�������pacl��ѹ�ɼ�,�Լ����Կ��Ƶ�����
#define IO_DSG_MOS  			 P0_4 //mcu��������mos�Ƿ�ŵ磬Ĭ��״̬���ŵ磬��ƽΪ��
#define IO_SCP_RST  			 P0_0 //�����·����
#define IO_POWER_ON 			 P3_2 //MCU���ƹ����·����
#define IO_Error_1  			 P3_5 //led����io�ں����ʾ
#define IO_Error_2  			 P2_6 //led����io�ں����ʾ
#define IO_Error_4  			 P2_7 //led����io�ں����ʾ
#define IO_Error_8  			 P0_7 //led����io�ں����ʾ
#define IO_CHG_MOS 			   P0_6 //mcu���������Ƿ���

//201911011 ������Ժ�
#define debug 0
#define tempdebug 0
#define chargedebug 0
#define elecdebug 1
#define discurdebug 1

void  debug_led1(void);
void  debug_led2(void);
void  debug_led4(void);
void  debug_led8(void);
void  AllInitial(void);//���ж���λ
//��������
//��1����ˢ����
//			*��ˢ�������
#define IO_Ground_CTRL 			P1_4 //Ĭ����1���տ�ʼ�ǹر�״̬
//			*��ˢ��·���
//#define IO_Ground_Short   	P1_5 //int40���������Ϊ�����
//��2��heapʶ��
#define IO_HEAP							P3_1			//ɨ��ʶ��ߵ͵�ƽ����Ϊ����
//��3����������
//#define IO_Speed_Change     P2_3			//ɨ��ʶ��ߵ͵�ƽ����Ϊ����
//��4�����ߵ��ԣ�buck����ʹ��
#define IO_BUCK_EN    		  P1_2	  //��ʱ����

/*************************�������蹦�ܼ�����*******************************/
#define PeriphFunc 1
#if PeriphFunc
//1.��ˢ���Ƽ�����
#define Ground_Conf  0 //��ʱ�Ȳ���
//2.buck����
#define BUCK_POWER_EN  0//��ʱ�Ȳ���
//3.����
#define Botton_Func  0
#endif

/**************************�����ⲿ�Զ��屣�����ܣ������ʱ�Ȳ�Ū��**************************/
#define PeriphProtect 1
#if PeriphProtect
//1.�������ѹ����
#define GetInputVol_Func  0
//2.�����������
#define GetInputCur_Func  0
//3.�ŵ��ⲿ��ѹ����
#define GetOutputVol_Func  0
//4.�ŵ��ⲿ��������
#define GetOutputCur_Func  0
//5.�ŵ��ⲿ��·���
#define GetOutputShort_Func  0

#endif



/********************************************************************/
#include "user_twi.h"
#if 1
#include "process.h"
#include "protect.h"
#include "adc.h"
#include "periph.h"
#include "pwm.h"
#include "funckey.h"
bit CheckCtoVol(void);
void roidmi_init(void);
extern U8 xdata ConectFlg_poron;
extern U8 xdata Short_flag;//��·��־
extern U8 xdata CHGING_cnt;
#endif


/************************8*******/
#define UpEepromWithDelay() {ucFlashWrValid = 0x55; ucUpDataLimitTime = 50;}
#define UpEepromNoDelay() {ucFlashWrValid = 0x55; ucUpDataLimitTime = 0;}


//����Ĵ�����Bank
#define BANK0_SEL	INSCON = 0x00;
//ѡ�����⹦�ܼĴ���ҳ1
#define BANK1_SEL	INSCON = 0x40; //null BKS0 null null  DIV MUL null DPS 
																 //0      1   0     0    0   0    0   0 



//����I/O
//#define DSG1PWM     		P2_5

//#define KEY_ON_IO      		P2_7
//#define KEY_LED_IO     		P3_0
//#define IO_BLEPW			P0_3


#endif

