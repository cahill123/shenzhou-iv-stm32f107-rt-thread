/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */

/**
 * @addtogroup STM32
 */
/*@{*/

#include <board.h>
#include <rtthread.h>

#ifdef RT_USING_DFS
/* dfs init */
#include <dfs_init.h>
/* dfs filesystem:ELM filesystem init */
#include <dfs_elm.h>
/* dfs Filesystem APIs */
#include <dfs_fs.h>
#endif

#ifdef RT_USING_SPI
//#include <spi_flash_at45dbxx.h>
#include <spi_flash_w25qxx.h>
#endif

#ifdef RT_USING_LWIP
#include <lwip/sys.h>
#include <lwip/api.h>
#include <netif/ethernetif.h>
#include "stm32_eth.h"
#endif

void rt_init_thread_entry(void* parameter)
{
/* Filesystem Initialization */
#ifdef RT_USING_DFS
	{
		/* init the device filesystem */
		dfs_init();
#ifdef RT_USING_SPI
	    /* init hardware device */
	    if(w25qxx_init("flash0", "spi10") != RT_EOK)
	    {							 
	    	rt_kprintf("[error] No such spi flash!\r\n");
	    }
#endif

#ifdef RT_USING_DFS_ELMFAT
		/* init the elm chan FatFs filesystam*/
		elm_init();

#ifdef RT_USING_SPI
		if (dfs_mount("flash0", "/", "elm", 0, 0) == 0)
			rt_kprintf("SPI File System initialized!\n");
		else
			rt_kprintf("SPI File System init failed!\n");
#endif

        /* init sdcard driver */
        //rt_hw_sdcard_init();

        /* mount sd card fat partition 1 as root directory */
        //if (dfs_mount("sd0", "/sd0", "elm", 0, 0) == 0)
        //{
        //    rt_kprintf("File System initialized!\n");
        //}
        //else
        //    rt_kprintf("File System initialzation failed!\n");
#endif
	}
#endif

    /* LwIP Initialization */
#ifdef RT_USING_LWIP
    {
        extern void lwip_sys_init(void);
			
        /* register ethernetif device */
        eth_system_device_init();

  		rt_hw_stm32_eth_init();
        /* re-init device driver */

  		rt_device_init_all();

  			/* init lwip system */
        lwip_sys_init();
        rt_kprintf("TCP/IP initialized!\n");
    }
#endif
}

int rt_application_init()
{
    rt_thread_t init_thread;

#if (RT_THREAD_PRIORITY_MAX == 32)
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 8, 20);
#else
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 80, 20);
#endif

    if (init_thread != RT_NULL)
        rt_thread_startup(init_thread);

    return 0;
}

/*@}*/
