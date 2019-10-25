#ifndef __PWM_H_
#define __PWM_H_



#define PWM0Percent_value  0
#define PWM30Percent_value 30
#define PWM60Percent_value 60
#define PWM100Percent_value 100
#define PWMCON_PIN     		P2_5
typedef enum{
	  PWM0Percent,
		PWM30Percent ,	 
		PWM60Percent ,		  
		PWM100Percent ,	  	 
}PWM_Select;


void PWM_Output(PWM_Select PWM_t);



#endif









