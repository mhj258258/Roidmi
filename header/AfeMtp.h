#ifndef	_SCI_H
#define	_SCI_H

#define AFE_CHIP_ID 		(0x1B<<1)						 //AFE_ID

#define TRY_TIMES     		5
																	//									//AFESCONF1 ：0x40
#define Cleaner_AFE_Set		REG.AFEINTEN=0x6C;REG.AFESCONF1=0x40;REG.AFESCONF2=0x08;REG.AFESCONF3=0xF8;REG.AFESCONF4=0x00;REG.AFESCONF5=0x00;REG.AFESCONF6=0x70;REG.AFESCONF7=0x01;REG.AFESCONF8=0x03;REG.AFESCONF9=0xff;REG.AFESCONF10=0x00;
//													REG.AFEINTEN=0x6C;//- （SC_INT硬件短路使能） OV_INT（硬件过冲使能） CD_INT（充放电状态中断使能） CADC_INT VADC_INT WDT_INT TWI_INT 
//																					
//																						 //0    1        							1         							 0                             1        1        0      0
//													REG.AFESCONF1=0x40;//LTCLR（清除SC,OV,TWI,WDT标志位） CTLD_EN（放电mosfet优先控制管脚）  PD_EN(低功耗状态控制位)   WDT_EN  SC_EN  OV_EN LOAD_EN(负载检测使能)  CHGR_EN（充电器检测位）
//																									0																	1                                 0                           0     0       0      0    							 0
//													REG.AFESCONF2=0x08;//- - - - RESET/PF(外部复位，二级保护)  ALARM_C（ALARM输出选择位，0低电平脉冲，1持续低） DSG_C（放电mos控制位） CHG_C（充电mos控制位）
//																							//0 0 0 0       1                            0                                              0                      0                
//													REG.AFESCONF3=0xF8;//CADC_EN CADC_M(0单次采集，1连续采集) CBIT_C（0 10bit，1 13bit）  VADC_EN  VADC_C（0电压采集 1电压和温度采集）  SCAN_C(VADC转换周期)
//																							//   1        1                           1                            1       1                                    000(50ms一次)
//													REG.AFESCONF4=0x00;//- - - - CB10 CB9 CB8 CB7 CB6    CBn是平衡回路控制位 0关闭 1开启
//													REG.AFESCONF5=0x00;//- - - CB5 CB4 CB3 CB2 CB1
//													REG.AFESCONF6=0x70;//RSNS.1 RSNS.0(CADC采集范围) RST.1 RST.0（复位外部mcu宽度选择）SCV1 SCV0（硬件短路保护电压） SCT1 SCT0（硬件短路延时保护设置）
//																				//				0     1 （200mv）         1     1 （1s）                    0     0  (100mv)    					0			0 （50us）					   
//													
//													REG.AFESCONF7=0x01;//- OVT.2 0VT.1 OVT.0(硬件过冲保护延时选择位) CHS1 CHS2（充放电状态监测阈值选择位） WDTT.1 WDTT.0（看门狗溢出事件选择位）
//																			       // 0   0    0      0（1个转换周期）              0    0(1.4mv)                         0      1  (10s)
//													REG.AFESCONF8=0x03;//- - - - - - OVD.9 OVD.8  硬件过充电保护阈值高两位
//													REG.AFESCONF9=0xff;//OVD.7~OVD.0							阈值低8位   计算方式:寄存器值*5.86
//													REG.AFESCONF10=0x00;//PIN.7~PIN.0进低功耗控制为  如果值是0x33，允许PD_EN使能位写1 关闭LDO





//Define AFE register addr
#define AFE_FLAG1			0x00
#define AFE_FLAG2	    	0x01
#define AFE_BSTATUS	    	0x02
#define AFE_INT_EN		    0x03
#define AFE_SCONF1 			0x04
#define AFE_SCONF2			0x05
#define AFE_SCONF3    		0x06
#define AFE_SCONF4			0x07
#define AFE_SCONF5			0x08
#define AFE_SCONF6			0x09
#define AFE_SCONF7			0x0A
#define AFE_SCONF8			0x0B
#define AFE_SCONF9			0x0C
#define AFE_SCONF10			0x0D

#define AFE_CELL1H			0x0E //电芯1电压寄存器地址
#define AFE_CELL1L			0x0F
#define AFE_CELL2H			0x10
#define AFE_CELL2L			0x11
#define AFE_CELL3H			0x12									    
#define AFE_CELL3L			0x13
#define AFE_CELL4H			0x14
#define AFE_CELL4L			0x15
#define AFE_CELL5H			0x16
#define AFE_CELL5L			0x17
#define AFE_CELL6H			0x18
#define AFE_CELL6L			0x19
#define AFE_CELL7H			0x1A
#define AFE_CELL7L			0x1B
#define AFE_CELL8H			0x1C
#define AFE_CELL8L			0x1D
#define AFE_CELL9H			0x1E
#define AFE_CELL9L			0x1F
#define AFE_CELL10H			0x20
#define AFE_CELL10L			0x21
#define AFE_TS1H			0x22 //ts1温度寄存器（外部温度）
#define AFE_TS1L			0x23
#define AFE_TS2H			0x24
#define AFE_TS2L			0x25
#define AFE_TEMP1H			0x26//芯片内部温度采集
#define AFE_TEMP1L			0x27
#define AFE_TEMP2H			0x28
#define AFE_TEMP2L			0x29
#define AFE_CURH			0x2A //电流寄存器
#define AFE_CURL  			0x2B
//要改
#define OV_DELAY_CNT        (E2ucDelayOV*20) //2*20*单位25 =40*25=1000 ms
#define OVR_DELAY_CNT       (E2ucDelayOVR*20) 
#define UV_DELAY_CNT        (E2ucDelayUV*20)
#define UVR_DELAY_CNT       (E2ucDelayUVR*20)
#define TEMPE_DELAY_CNT     (E2ucDelayTemp*40)//120 3*40*25 3000
#define TEMPER_DELAY_CNT    (E2ucDelayTemp*40)
#define OCC_DELAY_CNT       (E2ucDelayOCC<<3)
#define OCD_DELAY_CNT       (E2ucDelayOCD*20)  //原来:(E2ucDelayOCD<<3) //8*25=200ms    -->1000ms
//---------------------by tomi--------------------------
#define OCD2_DELAY_CNT       (E2ucDelayOCD2<<1) //100ms 
//------------------------------------------------------
#define OCCR_DELAY_CNT      (E2ucDelayOCCR*40)
#define OCDR_DELAY_CNT      (E2ucDelayOCCR*40)
#define BAL_DELAY_CNT				(E2ucBalanceDelay*10)

#define BALANCE_ENABLE  	1
#define BALANCE_START   	2
#define BALANCING       	3
#define BALANCE_DISABLE 	4
#define BALANCE_NULL    	5

extern bit TWIWriteAFE(U8 WrAddr, U8 xdata *WrBuf);
extern bit TWIReadAFE(U8 RdAddr, U8 xdata *RdBuf);
extern bit InitAFE(void);
extern void ClearAFEFlg(void);
extern bit WaitADCConvert(void);
extern void PorOnProtect(void);
extern void IntoPowerDown(void);


#endif