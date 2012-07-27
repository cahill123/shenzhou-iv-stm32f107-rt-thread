#ifndef _BOARD_LED_H_
#define _BOARD_LED_H_

#define RCC_GPIO_LED                                 RCC_APB2Periph_GPIOD
#define GPIO_LED_PORT                                GPIOD    
#define GPIO_LED1                                    GPIO_Pin_2    
#define GPIO_LED2                                    GPIO_Pin_3    
#define GPIO_LED3                                    GPIO_Pin_4    
#define GPIO_LED4                                    GPIO_Pin_7
#define GPIO_LED_ALL                                 GPIO_LED1 |GPIO_LED2 |GPIO_LED3 |GPIO_LED4 

void led1_on(void);
void led2_on(void);
void led3_on(void);
void led4_on(void);
void led1_off(void);
void led2_off(void);
void led3_off(void);
void led4_off(void);
void led_all_on(void);
void led_all_off(void);
void led_on_id(unsigned char id);
void led_off_id(unsigned char id);
void rt_hw_led_init(void);

#endif //_BOARD_LED_H_
