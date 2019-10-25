#ifndef __PERIPH_H_
#define __PERIPH_H_


typedef enum{
		Error1 ,	 
		Error2 ,		  
		Error4 ,	  
	  Error8 ,	 
}LED_select;

//ϵͳ�ļ���״̬
typedef enum{
	LED_STATE_POWERDOWN,
	LED_STATE_IDLE,
	LED_STATE_CHARGING,
	LED_STATE_DISCHARGE,
	LED_STATE_ERROR,
}LED_DISPLAY_STATE;

//�Ƶļ���״̬,����
typedef struct{
	U8 ontime;//�ƿ�����״̬��ʱ��
	U8 offtime;//��Ϩ���״̬��ʱ��
	U8 repeatCount;//�����0xff��ô����һֱ,�Ͳ����㣬�������
	U8 AlawaysOn;
}LED_state_cfg;
void LED_Proc(void);
void SetLEDState( LED_DISPLAY_STATE state);
//typedef enum{
//		Error1 ,	 
//		Error2 ,		  
//		Error4 ,	  
//	  Error8 ,	 

//}LED_select;



#endif









