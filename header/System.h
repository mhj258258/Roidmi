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

//#define IO_CHG_MOS 			   P0_7 //mcu单独控制是否充电
//#define IO_SCP_RST  			 P3_2 //清除短路保护
//#define IO_DSG_MOS  			 P0_6 //mcu单独控制mos是否放电
//#define IO_Error_1  			 P3_1 //led错误io口红灯显示
//#define IO_Error_2  			 P3_5 //led错误io口红灯显示
//#define IO_Error_4  			 P2_6 //led错误io口红灯显示
//#define IO_Error_8  			 P2_7 //led错误io口红灯显示
//#define IO_PACK_VADC  		 P1_6 //该位高电平了才能允许pacl电压采集
//#define IO_POWER_ON 			 P0_5 //MCU控制供电电路自锁
      
/*************20190925************/
#define IO_PACK_VADC  		 P0_3 //该位高电平了才能允许pacl电压采集,以及可以控制电机输出
#define IO_DSG_MOS  			 P0_4 //mcu单独控制mos是否放电，默认状态不放电，电平为高
#define IO_SCP_RST  			 P0_0 //清除短路保护
#define IO_POWER_ON 			 P3_2 //MCU控制供电电路自锁
#define IO_Error_1  			 P3_5 //led错误io口红灯显示
#define IO_Error_2  			 P2_6 //led错误io口红灯显示
#define IO_Error_4  			 P2_7 //led错误io口红灯显示
#define IO_Error_8  			 P0_7 //led错误io口红灯显示
#define IO_CHG_MOS 			   P0_6 //mcu单独控制是否充电

//201911011 定义调试宏
#define debug 0
#define tempdebug 0
#define chargedebug 0
#define elecdebug 1
#define discurdebug 1

void  debug_led1(void);
void  debug_led2(void);
void  debug_led4(void);
void  debug_led8(void);
void  AllInitial(void);//所有都复位
//新增功能
//（1）地刷功能
//			*地刷电机控制
#define IO_Ground_CTRL 			P1_4 //默认是1，刚开始是关闭状态
//			*地刷短路检测
//#define IO_Ground_Short   	P1_5 //int40，这边是作为输入口
//（2）heap识别
#define IO_HEAP							P3_1			//扫描识别高低电平，作为输入
//（3）按键调速
//#define IO_Speed_Change     P2_3			//扫描识别高低电平，作为输入
//（4）单线调试，buck供电使能
#define IO_BUCK_EN    		  P1_2	  //暂时不用

/*************************定义外设功能及保护*******************************/
#define PeriphFunc 1
#if PeriphFunc
//1.地刷控制及保护
#define Ground_Conf  0 //暂时先不加
//2.buck供电
#define BUCK_POWER_EN  0//暂时先不加
//3.按键
#define Botton_Func  0
#endif

/**************************定义外部自定义保护功能（这边暂时先不弄）**************************/
#define PeriphProtect 1
#if PeriphProtect
//1.充电器电压采样
#define GetInputVol_Func  0
//2.充电电流检采样
#define GetInputCur_Func  0
//3.放电外部电压采样
#define GetOutputVol_Func  0
//4.放电外部电流采样
#define GetOutputCur_Func  0
//5.放电外部短路检测
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
extern U8 xdata Short_flag;//短路标志
extern U8 xdata CHGING_cnt;
#endif


/************************8*******/
#define UpEepromWithDelay() {ucFlashWrValid = 0x55; ucUpDataLimitTime = 50;}
#define UpEepromNoDelay() {ucFlashWrValid = 0x55; ucUpDataLimitTime = 0;}


//定义寄存器的Bank
#define BANK0_SEL	INSCON = 0x00;
//选择特殊功能寄存器页1
#define BANK1_SEL	INSCON = 0x40; //null BKS0 null null  DIV MUL null DPS 
																 //0      1   0     0    0   0    0   0 



//定义I/O
//#define DSG1PWM     		P2_5

//#define KEY_ON_IO      		P2_7
//#define KEY_LED_IO     		P3_0
//#define IO_BLEPW			P0_3


#endif

