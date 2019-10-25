#include "System.h"
#include "AfeMtp.h"



//����㷨��ʵ���ֻ��8������
/*************************************************************************************************
* ������: button_key0_read
* ��  ��: ��������ѡ��
* ����ֵ: ��
* ��  ��: 
*************************************************************************************************/
static U8 button_key0_read(void)
{
	if(IO_KEY_ON == 0)
	{
//		IO_Error_4 = 1;
		return 0xfe;//��ߵ��㷨�ǰ�λ������ģ������Ļ�������8������
	}
	else
	{
//		IO_Error_4 = 0;
		return 0xff;
	}
	//return  (IO_KEY_ON&&0x01);//���λ�ʹ�����IO_KEY_ON
}

/*************************************************************************************************
* ������: Key_Read
* ��  ��: 
* ����ֵ: ��
* ��  ��: ÿ20ms��ȡһ��
*************************************************************************************************/
U8 xdata Trg = 0;
U8 xdata Cont = 0;
static void Key_Read(void)
{
	U8 xdata ReadData ;
	ReadData = button_key0_read()^0xff;//��ȡi/o�ڵ�ֵ
  Trg = ReadData&(ReadData^Cont);// �������㴥�������ģ�������
	Cont = ReadData;//����������������
#if debug
		printf("\n key ============ReadData =%bx,Trg=%bx,Cont=%bx \n",ReadData,Trg,Cont);    
#endif
}
//������� 
//���0xff��ȡ��
//1 û�а�����ʱ��bbutton_key0_read= 0xff Ĭ�ϰ����Ǹߵ�ƽ0xff ;ReadData = 0;											Trg = 0;                     	Cont = 0;
//2 ��һ�����¼�		button_key0_read= 0xfe 					    			   ReadData = 0xfe^0x0ff = 0x01;      Trg = 0x01&(0x01^0) =0x01 ;	 	Cont = 0x01
//3 ���Ų����� 																	 								 ReadData = 0x01 ;				      		Trg = 0x01&(0x01^0x01) = 0; 	Cont = 0x01;
//ֻҪ�������ɿ���Trg���ֵ����0��Cont =0xff��

/*************************************************************************************************
* ������: Key_Proc
* ��  ��: 
* ����ֵ: ��
* ��  ��: //��������������
*************************************************************************************************/
U8 xdata cnt_shake = 0;
U8 xdata KEY_State = 0;
U8 xdata Once_key_flag = 0;
static void Key_Proc(void)
{
	
	//������mos��0�Ļ�����ô�Ͳ�ִ��pwm���л���
	//if((bDSGMOS != 0)&&(IO_DSG_MOS == 0))//1�ǲ��ŵ�,0������ŵ�
	{
		//0x01��8����ť���һλ
		if(Trg&0x01)//key0����һ�º�Trg����൱�ڰ���һ�Σ����û���ɿ��Ļ���Cont����ǳ����ж�
		{
#if debug
		printf("\n !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  \n");    
#endif
			//�л���λ	�������涨���־λ������
			switch(KEY_State)
			{
				case 0:
#if debug
		printf("\n key ======================��1��λ  \n");    
#endif
					//		PWM_Output(PWM30Percent);
					break;
				case 1:
#if debug
		printf("\n key ======================��2��λ  \n");    
#endif
					//		PWM_Output(PWM60Percent);
					break;
				case 2:
#if debug
		printf("\n key ======================��3��λ  \n");    
#endif
				//			PWM_Output(PWM100Percent);
					break;
				case 3:
#if debug
		printf("\n key ======================��0��λ1  \n");    
#endif
				//			PWM_Output(PWM0Percent);
					break;
				default:
#if debug
		printf("\n key ======================��0��λ2  \n");    
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
//		printf("\n key =======================��0��λ3  \n");    
//#endif
//		//PWM_Output(PWM0Percent);
//	}
	//���ܻ�Ҫ���뱣��KEY_State�ĳ����Ȳ���
	
	
	
	if((!Trg)&Cont&0x01)
	{
		cnt_shake++;//��ʼ��ʱ
	}
	else
	{
		cnt_shake = 0;
	}
		
	if(cnt_shake >= 3)//80)//25mһ��ѭ���Ļ���2s����
	{
#if debug
		printf("\n key===================����������2s��====================== \n");    
#endif
		//��������������	������pdģʽ
		MCU_Short_Clean();
	//	bPDFlg = 1;//�������ģʽ
	}


}
/*************************************************************************************************
* ������: Scan_key
* ��  ��: ��������ѡ��
* ����ֵ: ��
* ��  ��: 
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










