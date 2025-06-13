#include "headfile.h"

int mode=2,count_black=0,count_flag=0,count_shanbi=0,key=0;
extern float distance;
extern int bb_flag;
int key_val=0,key_up;

void count_proc()
{
	if(D1==0&&D2==0&&D3==0&&D4==0&&D5==0&&D6==0&&D7==0&&D8==0&&count_flag==0)
	{count_black++;count_flag=1;}
	else
		count_flag=0;
}


void out_init()
{
	gpio_init(GPIO_B, Pin_6, IU);//°´¼ü
	gpio_init(GPIO_A,Pin_12,OUT_PP);//µÆ
	gpio_init(GPIO_B,Pin_8,OUT_PP);//·äÃùÆ÷
	gpio_init(GPIO_B,Pin_10,OUT_PP);//Ê¹ÄÜ
	
}

void key_scan()
{
	if(gpio_get(GPIO_B, Pin_6) == 0) key_val=1;
	
}

void out_device()
{
//	if(gpio_get(GPIO_B, Pin_6) == 0) key = 1;
	if(key==1) gpio_set(GPIO_B,Pin_10,1);
	if(bb_flag==1) 
		{
		  gpio_set(GPIO_A,Pin_12,1);
		  gpio_set(GPIO_B,Pin_8,0);
		}
}

int main(void)
{
	OLED_Init();
	
	motor_init();
	encoder_init();

	uart_init(UART_1,9600,0x02);
	sy_hcsr04_init();
	out_init();
	
	
	pid_init(&motorA, DELTA_PID, 35, 10, 7);
	pid_init(&motorB, DELTA_PID, 35, 10, 7);
//pid_init(&angle, POSITION_PID, 2, 0, 0.5);
	
	gray_init();          
	
//	I2C_Init();
//	MPU6050_Init();
//	HMC5883L_Init();
	exti_init(EXTI_PB7,RISING,0);
	
	
	tim_interrupt_ms_init(TIM_3,10,0);
	tim_interrupt_ms_init(TIM_4,30,1);
	gpio_set(GPIO_B,Pin_8,1);
	while (1)
	{
//			out_device();
		  
		  printf("bb is %d\r\n",bb_flag);
	} 
}
