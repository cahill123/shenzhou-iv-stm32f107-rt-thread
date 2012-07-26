/*
 * File      : startup.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2010-01-25     Bernard      first version
 */

#include <rthw.h>
#include <rtthread.h>

#include "board.h"
#ifdef RT_USING_FINSH
#include "finsh.h"
extern void finsh_system_init(void);
#endif

/**
 * @addtogroup NUC100
 */

/*@{*/
#if defined(__CC_ARM)
extern int Image$$RW_IRAM1$$ZI$$Limit;
#elif defined(__GNUC__)
extern unsigned char __bss_start;
extern unsigned char __bss_end;
#endif

extern int  rt_application_init(void);

/**
 * This function will startup RT-Thread RTOS.
 */
void rtthread_startup(void)
{
	/* init kernel object */
	rt_system_object_init();

	/* init board */
	rt_hw_board_init();
	rt_show_version();

	/* init tick */
	rt_system_tick_init();

	/* init timer system */
	rt_system_timer_init();

#ifdef RT_USING_HEAP
#ifdef __CC_ARM
	rt_system_heap_init((void*)&Image$$RW_IRAM1$$ZI$$Limit, (void*)0x20004000);
#elif __ICCARM__
    rt_system_heap_init(__segment_end("HEAP"), (void*)0x20004000);
#else
	rt_system_heap_init((void*)&__bss_end, (void*)0x20004000);
#endif
#endif

	/* init scheduler system */
	rt_system_scheduler_init();

#ifdef RT_USING_HOOK /* if the hook is used */
	/* set idle thread hook */
	rt_thread_idle_sethook(rt_hw_led_flash);
#endif

#ifdef RT_USING_DEVICE
	/* init all device */
	rt_device_init_all();
#endif

	/* init application */
	rt_application_init();

#ifdef RT_USING_FINSH
	/* init finsh */
	finsh_system_init();
	finsh_set_device("uart1");
#endif

	/* init idle thread */
	rt_thread_idle_init();

	/* start scheduler */
	rt_system_scheduler_start();

	/* never reach here */
	return ;
}

int main (void)
{
	rt_uint32_t UNUSED level;

	/* disable interrupt first */
	level = rt_hw_interrupt_disable();

	/* invoke rtthread_startup */
	rtthread_startup();

	return 0;
}

/*@}*/
