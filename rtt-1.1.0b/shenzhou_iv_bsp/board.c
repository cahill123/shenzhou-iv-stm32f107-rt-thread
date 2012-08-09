/*
 * File      : board.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009 RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      first implementation
 */

#include <rthw.h>
#include <rtthread.h>

#include "stm32f10x.h"
#include "board.h"
#include "led.h"
#include "rt_rtc.h"

#ifdef RT_USING_SPI
#include "stm32f10x_spi.h"
#include "rt_stm32f10x_spi.h"
#endif

/**
 * @addtogroup STM32
 */

/*@{*/

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM
	/* Set the Vector Table base location at 0x20000000 */
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
	/* Set the Vector Table base location at 0x08000000 */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif
}

/*******************************************************************************
 * Function Name  : SysTick_Configuration
 * Description    : Configures the SysTick for OS tick.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void  SysTick_Configuration(void)
{
	RCC_ClocksTypeDef  rcc_clocks;
	rt_uint32_t         cnts;

	RCC_GetClocksFreq(&rcc_clocks);

	cnts = (rt_uint32_t)rcc_clocks.HCLK_Frequency / RT_TICK_PER_SECOND;

	SysTick_Config(cnts);
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
}

/**
 * This is the timer interrupt service routine.
 *
 */
void rt_hw_timer_handler(void)
{
	/* enter interrupt */
	rt_interrupt_enter();

	rt_tick_increase();

	/* leave interrupt */
	rt_interrupt_leave();
}

/**
 * This function will initial STM32 board.
 */
void rt_hw_board_init()
{
	/* NVIC Configuration */
	NVIC_Configuration();

	/* Configure the SysTick */
	SysTick_Configuration();

	rt_hw_usart_init();
	rt_hw_led_init();
	rt_hw_rtc_init();
	
	rt_console_set_device(CONSOLE_DEVICE);
#ifdef RT_USING_SPI
    rt_stm32f10x_spi_init();
#   ifdef USING_SPI1
    /* attach spi10 : CS PA4 */
    {
		//rt_kprintf("USING_SPI1 \r\n");
        static struct rt_spi_device spi_device;
        static struct stm32_spi_cs  spi_cs;
        GPIO_InitTypeDef GPIO_InitStructure;

        GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

        spi_cs.GPIOx = GPIOB;
        spi_cs.GPIO_Pin = GPIO_Pin_9;

        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

		GPIO_SetBits(GPIOB, GPIO_Pin_9);
        GPIO_Init(GPIOB, &GPIO_InitStructure);


        rt_spi_bus_attach_device(&spi_device, "spi10", "spi1", (void*)&spi_cs);

    }
#   endif
#endif
	//rt_console_set_device(CONSOLE_DEVICE);
}

void rt_hw_spi1_baud_rate(uint16_t SPI_BaudRatePrescaler)
{
	SPI1->CR1 &= ~SPI_BaudRatePrescaler_256;
	SPI1->CR1 |= SPI_BaudRatePrescaler;
}

/*@}*/
