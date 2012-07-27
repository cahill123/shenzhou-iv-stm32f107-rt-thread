#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "led.h"
#include "finsh.h"

void led1_on(void)
{
   GPIO_ResetBits(GPIO_LED_PORT, GPIO_LED1);
}
FINSH_FUNCTION_EXPORT(led1_on, led 1 turn on);

void led1_off(void)
{
   GPIO_SetBits(GPIO_LED_PORT, GPIO_LED1);
}
FINSH_FUNCTION_EXPORT(led1_off, led 1 turn off);

void led2_on(void)
{
   GPIO_ResetBits(GPIO_LED_PORT, GPIO_LED2);
}
FINSH_FUNCTION_EXPORT(led2_on, led 2 turn on);

void led2_off(void)
{
   GPIO_SetBits(GPIO_LED_PORT, GPIO_LED2);
}
FINSH_FUNCTION_EXPORT(led2_off, led 2 turn off);

void led3_on(void)
{
   GPIO_ResetBits(GPIO_LED_PORT, GPIO_LED3);
}
FINSH_FUNCTION_EXPORT(led3_on, led 3 turn on);

void led3_off(void)
{
   GPIO_SetBits(GPIO_LED_PORT, GPIO_LED3);
}
FINSH_FUNCTION_EXPORT(led3_off, led 3 turn off);

void led4_on(void)
{
   GPIO_ResetBits(GPIO_LED_PORT, GPIO_LED4);
}
FINSH_FUNCTION_EXPORT(led4_on, led 4 turn on);

void led4_off(void)
{
   GPIO_SetBits(GPIO_LED_PORT, GPIO_LED4);
}
FINSH_FUNCTION_EXPORT(led4_off, led 4 turn off);

void led_all_on(void)
{
   GPIO_ResetBits(GPIO_LED_PORT, GPIO_LED_ALL);
}
FINSH_FUNCTION_EXPORT(led_all_on, all led turn on);

void led_all_off(void)
{
   GPIO_SetBits(GPIO_LED_PORT, GPIO_LED_ALL);
}
FINSH_FUNCTION_EXPORT(led_all_off, all led turn off);

void led_on_id(unsigned char id)
{
	switch (id)
	{
		case 1: led1_on();break;
		case 2: led2_on();break;
		case 3: led3_on();break;
		case 4: led4_on();break;
		default: led_all_off();break;
	}
}
FINSH_FUNCTION_EXPORT_ALIAS(led_on_id, led_on, led id turn on);

void led_off_id(unsigned char id)
{
	switch (id)
	{
		case 1: led1_off();break;
		case 2: led2_off();break;
		case 3: led3_off();break;
		case 4: led4_off();break;
		default: led_all_off();break;
	}
}
FINSH_FUNCTION_EXPORT_ALIAS(led_off_id, led_off, led id turn off);

void rt_hw_led_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Enable GPIOB, GPIOC and AFIO clock */
  RCC_APB2PeriphClockCmd(RCC_GPIO_LED | RCC_APB2Periph_AFIO , ENABLE);  //RCC_APB2Periph_AFIO
  
  /* LEDs pins configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_LED_ALL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIO_LED_PORT, &GPIO_InitStructure);

  led_all_off();
}
