#ifndef __PERIPH_H_
#define __PERIPH_H_


typedef enum{
		Error1 ,	 
		Error2 ,		  
		Error4 ,	  
	  Error8 ,	 
}LED_select;

//系统的几种状态
typedef enum{
	LED_STATE_POWERDOWN,
	LED_STATE_IDLE,
	LED_STATE_CHARGING,
	LED_STATE_DISCHARGE,
	LED_STATE_ERROR,
}LED_DISPLAY_STATE;

//灯的几种状态,配置
typedef struct{
	U8 ontime;//灯开启的状态的时间
	U8 offtime;//灯熄灭的状态的时间
	U8 repeatCount;//如果是0xff那么就是一直,就不计算，量灭次数
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









