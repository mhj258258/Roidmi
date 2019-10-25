#include "System.h"
#include "AfeMtp.h"



//这个算法其实最多只能8个按键
/*************************************************************************************************
* 函数名: button_key0_read
* 参  数: 按键功能选择
* 返回值: 无
* 作  用: 
*************************************************************************************************/
static U8 button_key0_read(void)
{
	if(IO_KEY_ON == 0)
	{
//		IO_Error_4 = 1;
		return 0xfe;//这边的算法是按位来处理的，这样的话可以有8个按键
	}
	else
	{
//		IO_Error_4 = 0;
		return 0xff;
	}
	//return  (IO_KEY_ON&&0x01);//最低位就代表了IO_KEY_ON
}

/*************************************************************************************************
* 函数名: Key_Read
* 参  数: 
* 返回值: 无
* 作  用: 每20ms读取一次
*************************************************************************************************/
U8 xdata Trg = 0;
U8 xdata Cont = 0;
static void Key_Read(void)
{
	U8 xdata ReadData ;
	ReadData = button_key0_read()^0xff;//读取i/o口的值
  Trg = ReadData&(ReadData^Cont);// 用来计算触发变量的，代表触发
	Cont = ReadData;//用来计算连续变量
#if debug
		printf("\n key ============ReadData =%bx,Trg=%bx,Cont=%bx \n",ReadData,Trg,Cont);    
#endif
}
//三种情况 
//异或0xff是取反
//1 没有按键的时候button_key0_read= 0xff 默认按键是高电平0xff ;ReadData = 0;											Trg = 0;                     	Cont = 0;
//2 第一个按下键		button_key0_read= 0xfe 					    			   ReadData = 0xfe^0x0ff = 0x01;      Trg = 0x01&(0x01^0) =0x01 ;	 	Cont = 0x01
//3 按着不松手 																	 								 ReadData = 0x01 ;				      		Trg = 0x01&(0x01^0x01) = 0; 	Cont = 0x01;
//只要按键不松开，Trg这个值就是0，Cont =0xff；

/*************************************************************************************************
* 函数名: Key_Proc
* 参  数: 
* 返回值: 无
* 作  用: //按键长按多少秒
*************************************************************************************************/
U8 xdata cnt_shake = 0;
U8 xdata KEY_State = 0;
U8 xdata Once_key_flag = 0;
static void Key_Proc(void)
{
	
	//如果输出mos是0的话，那么就不执行pwm的切换了
	//if((bDSGMOS != 0)&&(IO_DSG_MOS == 0))//1是不放电,0是允许放电
	{
		//0x01是8个按钮最后一位
		if(Trg&0x01)//key0按了一下和Trg与就相当于按了一次，如果没有松开的话和Cont与就是长按判断
		{
#if debug
		printf("\n !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  \n");    
#endif
			//切换档位	在这里面定义标志位就行了
			switch(KEY_State)
			{
				case 0:
#if debug
		printf("\n key ======================第1档位  \n");    
#endif
					//		PWM_Output(PWM30Percent);
					break;
				case 1:
#if debug
		printf("\n key ======================第2档位  \n");    
#endif
					//		PWM_Output(PWM60Percent);
					break;
				case 2:
#if debug
		printf("\n key ======================第3档位  \n");    
#endif
				//			PWM_Output(PWM100Percent);
					break;
				case 3:
#if debug
		printf("\n key ======================第0档位1  \n");    
#endif
				//			PWM_Output(PWM0Percent);
					break;
				default:
#if debug
		printf("\n key ======================第0档位2  \n");    
#endif
				//			PWM_Output(PWM0Percent);
					break;

			}
			if(++KEY_State>3)
			{
				KEY_State = 0;
			}
		}
	}
//	else
//	{
//#if debug
//		printf("\n key =======================第0档位3  \n");    
//#endif
//		//PWM_Output(PWM0Percent);
//	}
	//可能还要加入保存KEY_State的程序，先不动
	
	
	
	if((!Trg)&Cont&0x01)
	{
		cnt_shake++;//开始计时
	}
	else
	{
		cnt_shake = 0;
	}
		
	if(cnt_shake >= 3)//80)//25m一次循环的话，2s就是
	{
#if debug
		printf("\n key===================按键长按了2s了====================== \n");    
#endif
		//按键按两秒以上	，进入pd模式
		MCU_Short_Clean();
	//	bPDFlg = 1;//进入掉电模式
	}


}
/*************************************************************************************************
* 函数名: Scan_key
* 参  数: 按键功能选择
* 返回值: 无
* 作  用: 
*************************************************************************************************/
void Scan_key(void)
{
	Key_Read();
	Key_Proc();
}















#if 0
switch (Key_State)
        {
            //"First capture key" state
            case 0:

                if(Key_press != 0)
                {
                	os_printf("Key_State = %x\n",Key_State);
                	Key_Prev = Key_press;
                    Key_State = 1;
                }
    
                break;
                
                //"Capture valid key" status
            case 1:
                if(Key_press == Key_Prev)
                {
                	os_printf("Key_State = %x\n",Key_State);
                	Key_State = 2;
                    Key_return= Key_Prev | KEY_DOWN;//??????????????
                }
                else
                {
                    //Button lift, jitter, no response button
                    Key_State = 0;
                }
                break;
                
                //"Capture long press" status
            case 2:
            	//?????????????????????????????????????????turn ???0???
                if(Key_press != Key_Prev)
                {
                	os_printf("Key_press != Key_Prev  Key_State = %x\n",Key_State);
                	Key_State = 0;
                    Key_LongCheck = 0;
                    Key_return = Key_Prev | KEY_UP; //?????
                    return Key_return;
                }
    
                if(Key_press == Key_Prev)
                {
                	os_printf("Key_press == Key_Prev  Key_State = %x\n",Key_State);
                	Key_LongCheck++;
                    if(Key_LongCheck >= (PRESS_LONG_TIME / DEBOUNCE_TIME))    //??????????????????3S (??????????????????30MS * 100)
                    {
                        Key_LongCheck = 0;
                        Key_State = 3;
                        Key_return= Key_press |  KEY_LONG;
                        return Key_return;
                    }
                }
                break;
                
                //"Restore the initial" state
            case 3:
                if(Key_press != Key_Prev)
                {
                	os_printf("Key_press != Key_Prev  Key_State = %x\n",Key_State);
                	Key_State = 0;
                }
                break;
        }


#endif










