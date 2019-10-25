#include "System.h"
#include "AfeMtp.h"


/*************************************************************************************************
* ������: PWM_Output
* ��  ��: ѡ��ռ�ձ�
* ����ֵ: ��
* ��  ��: 
*************************************************************************************************/
void PWM_Output(PWM_Select PWM_t)
{

	switch(PWM_t)
	{
		case PWM0Percent:
		PWM2CON &= ~0x01; //����IO��
	  PWMCON_PIN = 0;
		break;
		case PWM30Percent:
		PWM2DL = (U16)(PWM2PL+PWM2PH*256)*PWM30Percent_value/100;
		PWM2DH = ((U16)(PWM2PL+PWM2PH*256)*PWM30Percent_value/100) >> 8;
		PWM2CON |= 0x01;
		break;
		case PWM60Percent:
		PWM2DL = (U16)(PWM2PL+PWM2PH*256)*PWM60Percent_value/100;
		PWM2DH = ((U16)(PWM2PL+PWM2PH*256)*PWM60Percent_value/100) >> 8;
		PWM2CON |= 0x01;
		break;
		case PWM100Percent:
		PWM2CON &= ~0x01; //����IO��
	  PWMCON_PIN = 1;
		break;
		default:
		PWM2CON &= ~0x01; //����IO��
	  PWMCON_PIN = 0;
		break;
	}

}





































