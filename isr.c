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


//以下为定时器中断服务函数
void TIM2_IRQHandler(void)
{
	if(TIM2->SR&1)
	{
		
		//此处编写中断代码
		TIM2->SR &= ~1; 
	}
}

void TIM3_IRQHandler(void)
{
	if(TIM3->SR&1)
	{
		//此处编写中断代码
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
		//此处编写中断代码
		TIM4->SR &= ~1; 
	}
}


//以下为串口中断服务函数
void USART1_IRQHandler(void)
{
	if (USART1->SR&0x20)
	{
		//此处编写中断代码

		USART1->SR &= ~0x20;   //清除标志位
	}
}


void USART2_IRQHandler(void)
{
	if (USART2->SR&0x20)
	{
		//此处编写中断代码

		USART2->SR &= ~0x20;   //清除标志位
	}
}

void USART3_IRQHandler(void)
{
	if (USART3->SR&0x20)
	{
		//此处编写中断代码

		USART3->SR &= ~0x20;   //清除标志位
	}
}


//以下为外部中断服务函数
void EXTI0_IRQHandler(void) // PA0/PB0/PC0
{
	if(EXTI->PR&(1<<0))
	{
		//此处编写中断代码
		
		EXTI->PR = 1<<0; //清除标志位
	}
}

void EXTI1_IRQHandler(void) // PA1/PB1/PC1
{
	if(EXTI->PR&(1<<1))
	{
		//此处编写中断代码
		
		EXTI->PR = 1<<1; //清除标志位
	}
}
void EXTI2_IRQHandler(void) // PA2/PB2/PC2
{
	if(EXTI->PR&(1<<2))
	{
		//此处编写中断代码
		if(gpio_get(GPIO_A, Pin_3))
			Encoder_count1 --;
		else
			Encoder_count1 ++;
		
		EXTI->PR = 1<<2; //清除标志位
	}
}
void EXTI3_IRQHandler(void) // PA3/PB3/PC3
{
	if(EXTI->PR&(1<<3))
	{
		//此处编写中断代码
		
		EXTI->PR = 1<<3; //清除标志位
	}
}
void EXTI4_IRQHandler(void) // PA4/PB4/PC4
{
	if(EXTI->PR&(1<<4))
	{
		//此处编写中断代码
		if(gpio_get(GPIO_A, Pin_5))
			Encoder_count2 ++;
		else
			Encoder_count2 --;
		EXTI->PR = 1<<4; //清除标志位
	}
}

void EXTI9_5_IRQHandler(void)
{
	if(EXTI->PR&(1<<5))   //EXTI5  PA5/PB5/PC5
	{
		//此处编写中断代码

		EXTI->PR = 1<<5; //清除标志位
	}
	
	if(EXTI->PR&(1<<6))   //EXTI6  PA6/PB6/PC6
	{
		//此处编写中断代码
		
		EXTI->PR = 1<<6; //清除标志位
	}
	
	if(EXTI->PR&(1<<7))   //EXTI7  PA7/PB7/PC7
	{
		//此处编写中断代码
		
		// 获取原始数据
		MPU6050_GetData();
		HMC5883L_GetData();
		
		// 陀螺仪角度
		roll_gyro += (float)gx / 16.4 * 0.005;
		pitch_gyro += (float)gy / 16.4 * 0.005;
		yaw_gyro += (float)gz / 16.4 * 0.005;
		
		// 加速度计角度
		roll_acc = atan((float)ay/az) * 57.296;
		pitch_acc = - atan((float)ax/az) * 57.296;	
		yaw_acc = atan((float)ay/ax) * 57.296;
		
		// 磁力计角度
		yaw_hmc = atan2((float)hmc_x, (float)hmc_y) * 57.296;
		
		// 卡尔曼滤波融合角度
		roll_Kalman = Kalman_Filter(&KF_Roll, roll_acc, (float)gx / 16.4);
		pitch_Kalman = Kalman_Filter(&KF_Pitch, pitch_acc, (float)gy / 16.4);
		yaw_Kalman = Kalman_Filter(&KF_Yaw, yaw_hmc, (float)gz / 16.4);
		
		EXTI->PR = 1<<7; //清除标志位
	}
	
	if(EXTI->PR&(1<<8))   //EXTI8
	{
		//此处编写中断代码
		
		EXTI->PR = 1<<8; //清除标志位
	}
	
	if(EXTI->PR&(1<<9))   //EXTI9
	{
		//此处编写中断代码
		
		EXTI->PR = 1<<9; //清除标志位
	}
}
