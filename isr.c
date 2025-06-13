#include "stm32f10x.h"                  // Device header
#include "headfile.h"
float distance=500;
int bizhang=0;
static int bz_t=0;
extern int stop_flag;
int run_state=0;
int stop_time=0;
int run_flag,r_spe1,r_spe2;
#define xunji 0;
#define bizhang1 1;
#define bizhang2 2;
#define bizhang3 3;


//����Ϊ��ʱ���жϷ�����
void TIM2_IRQHandler(void)
{
	if(TIM2->SR&1)
	{
		
		//�˴���д�жϴ���
		TIM2->SR &= ~1; 
	}
}

void TIM3_IRQHandler(void)
{
	if(TIM3->SR&1)
	{
		//�˴���д�жϴ���
		TIM3->SR &= ~1; 
    switch(run_state)
		{
			case 0: 
			run_flag=1;
      r_spe1=0;
			r_spe2=0;
      bz_t=0;
			gpio_set(GPIO_B,Pin_8,1);			
			break;
			
			case 1: 
			run_flag=0;
			r_spe1=285;
			r_spe2=15;
			gpio_set(GPIO_B,Pin_8,0);
			if(++bz_t>=35)
			{run_state=2;bz_t=0;}
			break;
			
			case 2:
			run_flag=0;
			r_spe1=300;
			r_spe2=300;
			if(++bz_t>=73)
			{run_state=3;bz_t=0;}
			break;
					
			case 3:
			run_flag=0;
			r_spe1=23;
			r_spe2=278;
			if(++bz_t>=53)
			{run_state=4;bz_t=0;}
			break;
			
			case 4:
			run_flag=0;
			r_spe1=300;
			r_spe2=300;
			if(D6+D7+D8<=2)
			{run_state=0;bz_t=0;}
//			if(++bz_t>=40)
//			{run_state=0;bz_t=0;}
			break;
		}   
		pid_control(run_flag,r_spe1,r_spe2);
		
		TIM3->SR &= ~1; 
	}
}

void TIM4_IRQHandler(void)
{
	if(TIM4->SR&1)
	{
		TIM4->SR &= ~1;
		stop_time++;
		distance=sy_hcsr04_cm();
		if(distance<25&&run_state==0) run_state=1;
//		printf("distance is %.2f\r\n",distance);
//		printf("run_state is %d\r\n",run_state);
		printf("bz_t is %d\r\n",bz_t);
		printf("time is %d\r\n",stop_time);
//		if(distance<20&&bz_t>300)
//		{
//			bz_t=0;
//			bizhang=1;
//		  pid_control(0,60,10);		
//		 }		
//	  if(bizhang)
//		  {
//		    bz_t++;
//		    if(bz_t>60)
//		    {
//		      pid_control(0,30,30);	      
//	      }
//		    if(bz_t>80) pid_control(0,10,60);
//		    if(D2+D3+D4+D5+D6+D7+D8<=6)
//		    bizhang=0;
//	    }
		//�˴���д�жϴ���
		TIM4->SR &= ~1; 
	}
}


//����Ϊ�����жϷ�����
void USART1_IRQHandler(void)
{
	if (USART1->SR&0x20)
	{
		//�˴���д�жϴ���

		USART1->SR &= ~0x20;   //�����־λ
	}
}


void USART2_IRQHandler(void)
{
	if (USART2->SR&0x20)
	{
		//�˴���д�жϴ���

		USART2->SR &= ~0x20;   //�����־λ
	}
}

void USART3_IRQHandler(void)
{
	if (USART3->SR&0x20)
	{
		//�˴���д�жϴ���

		USART3->SR &= ~0x20;   //�����־λ
	}
}


//����Ϊ�ⲿ�жϷ�����
void EXTI0_IRQHandler(void) // PA0/PB0/PC0
{
	if(EXTI->PR&(1<<0))
	{
		//�˴���д�жϴ���
		
		EXTI->PR = 1<<0; //�����־λ
	}
}

void EXTI1_IRQHandler(void) // PA1/PB1/PC1
{
	if(EXTI->PR&(1<<1))
	{
		//�˴���д�жϴ���
		
		EXTI->PR = 1<<1; //�����־λ
	}
}
void EXTI2_IRQHandler(void) // PA2/PB2/PC2
{
	if(EXTI->PR&(1<<2))
	{
		//�˴���д�жϴ���
		if(gpio_get(GPIO_A, Pin_3))
			Encoder_count1 --;
		else
			Encoder_count1 ++;
		
		EXTI->PR = 1<<2; //�����־λ
	}
}
void EXTI3_IRQHandler(void) // PA3/PB3/PC3
{
	if(EXTI->PR&(1<<3))
	{
		//�˴���д�жϴ���
		
		EXTI->PR = 1<<3; //�����־λ
	}
}
void EXTI4_IRQHandler(void) // PA4/PB4/PC4
{
	if(EXTI->PR&(1<<4))
	{
		//�˴���д�жϴ���
		if(gpio_get(GPIO_A, Pin_5))
			Encoder_count2 ++;
		else
			Encoder_count2 --;
		EXTI->PR = 1<<4; //�����־λ
	}
}

void EXTI9_5_IRQHandler(void)
{
	if(EXTI->PR&(1<<5))   //EXTI5  PA5/PB5/PC5
	{
		//�˴���д�жϴ���

		EXTI->PR = 1<<5; //�����־λ
	}
	
	if(EXTI->PR&(1<<6))   //EXTI6  PA6/PB6/PC6
	{
		//�˴���д�жϴ���
		
		EXTI->PR = 1<<6; //�����־λ
	}
	
	if(EXTI->PR&(1<<7))   //EXTI7  PA7/PB7/PC7
	{
		//�˴���д�жϴ���
		
		// ��ȡԭʼ����
		MPU6050_GetData();
		HMC5883L_GetData();
		
		// �����ǽǶ�
		roll_gyro += (float)gx / 16.4 * 0.005;
		pitch_gyro += (float)gy / 16.4 * 0.005;
		yaw_gyro += (float)gz / 16.4 * 0.005;
		
		// ���ٶȼƽǶ�
		roll_acc = atan((float)ay/az) * 57.296;
		pitch_acc = - atan((float)ax/az) * 57.296;	
		yaw_acc = atan((float)ay/ax) * 57.296;
		
		// �����ƽǶ�
		yaw_hmc = atan2((float)hmc_x, (float)hmc_y) * 57.296;
		
		// �������˲��ںϽǶ�
		roll_Kalman = Kalman_Filter(&KF_Roll, roll_acc, (float)gx / 16.4);
		pitch_Kalman = Kalman_Filter(&KF_Pitch, pitch_acc, (float)gy / 16.4);
		yaw_Kalman = Kalman_Filter(&KF_Yaw, yaw_hmc, (float)gz / 16.4);
		
		EXTI->PR = 1<<7; //�����־λ
	}
	
	if(EXTI->PR&(1<<8))   //EXTI8
	{
		//�˴���д�жϴ���
		
		EXTI->PR = 1<<8; //�����־λ
	}
	
	if(EXTI->PR&(1<<9))   //EXTI9
	{
		//�˴���д�жϴ���
		
		EXTI->PR = 1<<9; //�����־λ
	}
}
