/*
 * File      : spi_flash_w25qxx.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2011, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2011-12-16     aozima       the first version
 * 2012-05-06     aozima       can page write.
 */

#include <stdint.h>
#include "spi_flash_w25qxx.h"

#define FLASH_DEBUG

#ifdef FLASH_DEBUG
#define FLASH_TRACE         rt_kprintf
#else
#define FLASH_TRACE(...)
#endif /* #ifdef FLASH_DEBUG */

/* JEDEC Manufacturer��s ID */
#define MF_ID           (0xEF)
/* JEDEC Device ID: Memory type and Capacity */
#define MTC_W25Q16_BV_CL_CV   (0x4015) /* W25Q16BV W25Q16CL W25Q16CV  */
#define MTC_W25Q16_DW         (0x6015) /* W25Q16DW  */
#define MTC_W25Q32_BV         (0x4016) /* W25Q32BV */
#define MTC_W25Q32_DW         (0x6016) /* W25Q32DW */
#define MTC_W25Q64_BV_CV      (0x4017) /* W25Q64BV W25Q64CV */
#define MTC_W25Q64_DW         (0x4017) /* W25Q64DW */
#define MTC_W25Q128_BV        (0x4018) /* W25Q128BV */
#define MTC_W25Q256_FV        (TBD)    /* W25Q256FV */
#define MTC_W25X16	          (0x3015) /* W25X16 */
#define MTC_W25X32	          (0x3016) /* W25X32 */
#define MTC_W25X64	          (0x3017) /* W25X64 */

/* command list */
#define CMD_RDSR                    (0x05)  /* ��״̬�Ĵ���     */
#define CMD_WRSR                    (0x01)  /* д״̬�Ĵ���     */
#define CMD_EWSR                    (0x50)  /* ʹ��д״̬�Ĵ��� */
#define CMD_WRDI                    (0x04)  /* �ر�дʹ��       */
#define CMD_WREN                    (0x06)  /* ��дʹ��       */
#define CMD_READ                    (0x03)  /* ������           */
#define CMD_FAST_READ               (0x0B)  /* ���ٶ�           */
#define CMD_BP                      (0x02)  /* �ֽڱ��         */
#define CMD_ERASE_4K                (0x20)  /* ��������:4K      */
#define CMD_ERASE_32K               (0x52)  /* ��������:32K     */
#define CMD_ERASE_64K               (0xD8)  /* ��������:64K     */
#define CMD_ERASE_full              (0xC7)  /* ȫƬ����         */
#define CMD_JEDEC_ID                (0x9F)  /* �� JEDEC_ID      */


#define DUMMY                       (0xFF)

static struct spi_flash_device  spi_flash_device;

static uint8_t w25qxx_read_status(void)
{
    return rt_spi_sendrecv8(spi_flash_device.rt_spi_device, CMD_RDSR);
}

static void w25qxx_wait_busy(void)
{
    while( w25qxx_read_status() & (0x01));
}

/** \brief read [size] byte from [offset] to [buffer]
 *
 * \param offset uint32_t unit : byte
 * \param buffer uint8_t*
 * \param size uint32_t   unit : byte
 * \return uint32_t byte for read
 *
 */
static uint32_t w25qxx_read(uint32_t offset, uint8_t * buffer, uint32_t size)
{
    uint8_t send_buffer[4];

    send_buffer[0] = CMD_WRDI;
    rt_spi_send(spi_flash_device.rt_spi_device, send_buffer, 1);

    send_buffer[0] = CMD_READ;
    send_buffer[1] = (uint8_t)(offset>>16);
    send_buffer[2] = (uint8_t)(offset>>8);
    send_buffer[3] = (uint8_t)(offset);

    rt_spi_send_then_recv(spi_flash_device.rt_spi_device,
                          send_buffer, 4,
                          buffer, size);

    return size;
}

/** \brief write N page on [page]
 *
 * \param page uint32_t unit : byte (4096 * N,1 page = 4096byte)
 * \param buffer const uint8_t*
 * \param size uint32_t unit : byte ( 4096*N )
 * \return uint32_t
 *
 */
uint32_t w25qxx_page_write(uint32_t page, const uint8_t * buffer, uint32_t size)
{
    uint32_t index;
    uint8_t send_buffer[4];

    RT_ASSERT((page&0xFF) == 0);

    send_buffer[0] = CMD_WREN;
    rt_spi_send(spi_flash_device.rt_spi_device, send_buffer, 1);

    send_buffer[0] = CMD_ERASE_4K;
    send_buffer[1] = (page >> 16);
    send_buffer[2] = (page >> 8);
    send_buffer[3] = (page);
    rt_spi_send(spi_flash_device.rt_spi_device, send_buffer, 4);

    w25qxx_wait_busy(); // wait erase done.

    for(index=0; index < (size/256); index++)
    {
        send_buffer[0] = CMD_WREN;
        rt_spi_send(spi_flash_device.rt_spi_device, send_buffer, 1);

        send_buffer[0] = CMD_BP;
        send_buffer[1] = (uint8_t)(page >> 16);
        send_buffer[2] = (uint8_t)(page >> 8);
        send_buffer[3] = (uint8_t)(page);

        rt_spi_send_then_send(spi_flash_device.rt_spi_device,
                              send_buffer, 4,
                              buffer, 256);

        buffer += 256;
        page += 256;
        w25qxx_wait_busy();
    }

    send_buffer[0] = CMD_WRDI;
    rt_spi_send(spi_flash_device.rt_spi_device, send_buffer, 1);

    return size;
}

/* RT-Thread device interface */
static rt_err_t w25qxx_flash_init(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t w25qxx_flash_open(rt_device_t dev, rt_uint16_t oflag)
{
    uint8_t send_buffer[3];

    send_buffer[0] = CMD_WREN;
    rt_spi_send(spi_flash_device.rt_spi_device, send_buffer, 1);

    send_buffer[0] = CMD_WRSR;
    send_buffer[1] = 0;
    send_buffer[2] = 0;
    rt_spi_send(spi_flash_device.rt_spi_device, send_buffer, 3);

    return RT_EOK;
}

static rt_err_t w25qxx_flash_close(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t w25qxx_flash_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    RT_ASSERT(dev != RT_NULL);

    if (cmd == RT_DEVICE_CTRL_BLK_GETGEOME)
    {
        struct rt_device_blk_geometry *geometry;

        geometry = (struct rt_device_blk_geometry *)args;
        if (geometry == RT_NULL) return -RT_ERROR;

        geometry->bytes_per_sector = spi_flash_device.geometry.bytes_per_sector;
        geometry->sector_count = spi_flash_device.geometry.sector_count;
        geometry->block_size = spi_flash_device.geometry.block_size;
    }

    return RT_EOK;
}

static rt_size_t w25qxx_flash_read(rt_device_t dev,
                                   rt_off_t pos,
                                   void* buffer,
                                   rt_size_t size)
{
    w25qxx_read(pos*spi_flash_device.geometry.bytes_per_sector,
                buffer,
                size*spi_flash_device.geometry.bytes_per_sector);
    return size;
}

static rt_size_t w25qxx_flash_write(rt_device_t dev,
                                    rt_off_t pos,
                                    const void* buffer,
                                    rt_size_t size)
{
    w25qxx_page_write(pos*spi_flash_device.geometry.bytes_per_sector,
                      buffer,
                      size*spi_flash_device.geometry.bytes_per_sector);
    return size;
}

rt_err_t w25qxx_init(const char * flash_device_name, const char * spi_device_name)
{
    struct rt_spi_device * rt_spi_device;

    rt_spi_device = (struct rt_spi_device *)rt_device_find(spi_device_name);
    if(rt_spi_device == RT_NULL)
    {
        FLASH_TRACE("spi device %s not found!\r\n", spi_device_name);
        return -RT_ENOSYS;
    }
    spi_flash_device.rt_spi_device = rt_spi_device;

    /* config spi */
    {
        struct rt_spi_configuration cfg;
        cfg.data_width = 8;
        cfg.mode = RT_SPI_MODE_0 | RT_SPI_MSB; /* SPI Compatible: Mode 0 and Mode 3 */
        cfg.max_hz = 1000000; /* 50M */
        rt_spi_configure(spi_flash_device.rt_spi_device, &cfg);
    }

    /* init flash */
    {
        rt_uint8_t cmd;
        rt_uint8_t id_recv[3];
        uint16_t memory_type_capacity;

        cmd = CMD_WRDI;
        rt_spi_send(spi_flash_device.rt_spi_device, &cmd, 1);

        /* read flash id */
        cmd = CMD_JEDEC_ID;
        rt_spi_send_then_recv(spi_flash_device.rt_spi_device, &cmd, 1, id_recv, 3);

        if(id_recv[0] != MF_ID)
        {
            FLASH_TRACE("Manufacturer��s ID error!\r\n");
            FLASH_TRACE("JEDEC Read-ID Data : %02X %02X %02X\r\n", id_recv[0], id_recv[1], id_recv[2]);
            return -RT_ENOSYS;
        }

        spi_flash_device.geometry.bytes_per_sector = 4096;
        spi_flash_device.geometry.block_size = 4096; /* block erase: 4k */

        /* get memory type and capacity */
        memory_type_capacity = id_recv[1];
        memory_type_capacity = (memory_type_capacity << 8) | id_recv[2];

        if(memory_type_capacity == MTC_W25Q128_BV)
        {
            FLASH_TRACE("W25Q128BV detection\r\n");
            spi_flash_device.geometry.sector_count = 4096;
        }
        else if(memory_type_capacity == MTC_W25Q64_BV_CV)
        {
            FLASH_TRACE("W25Q64BV or W25Q64CV detection\r\n");
            spi_flash_device.geometry.sector_count = 2048;
        }
        else if(memory_type_capacity == MTC_W25Q64_DW)
        {
            FLASH_TRACE("W25Q64DW detection\r\n");
            spi_flash_device.geometry.sector_count = 2048;
        }
        else if(memory_type_capacity == MTC_W25Q32_BV)
        {
            FLASH_TRACE("W25Q32BV detection\r\n");
            spi_flash_device.geometry.sector_count = 1024;
        }
        else if(memory_type_capacity == MTC_W25Q32_DW)
        {
            FLASH_TRACE("W25Q32DW detection\r\n");
            spi_flash_device.geometry.sector_count = 1024;
        }
        else if(memory_type_capacity == MTC_W25Q16_BV_CL_CV)
        {
            FLASH_TRACE("W25Q16BV or W25Q16CL or W25Q16CV detection\r\n");
            spi_flash_device.geometry.sector_count = 512;
        }
        else if(memory_type_capacity == MTC_W25Q16_DW)
        {
            FLASH_TRACE("W25Q16DW detection\r\n");
            spi_flash_device.geometry.sector_count = 512;
        }
        else if(memory_type_capacity == MTC_W25X64)
        {
            FLASH_TRACE("W25X64 detection\r\n");
            spi_flash_device.geometry.sector_count = 2048;
        }
        else if(memory_type_capacity == MTC_W25X32)
        {
            FLASH_TRACE("W25X32 detection\r\n");
            spi_flash_device.geometry.sector_count = 1024;
        }
        else if(memory_type_capacity == MTC_W25X16)
        {
            FLASH_TRACE("W25X16 detection\r\n");
            spi_flash_device.geometry.sector_count = 512;
        }
        else
        {
            FLASH_TRACE("Memory Capacity error!\r\n");
            return -RT_ENOSYS;
        }
    }

    /* register device */
    spi_flash_device.flash_device.type    = RT_Device_Class_Block;
    spi_flash_device.flash_device.init    = w25qxx_flash_init;
    spi_flash_device.flash_device.open    = w25qxx_flash_open;
    spi_flash_device.flash_device.close   = w25qxx_flash_close;
    spi_flash_device.flash_device.read 	  = w25qxx_flash_read;
    spi_flash_device.flash_device.write   = w25qxx_flash_write;
    spi_flash_device.flash_device.control = w25qxx_flash_control;
    /* no private */
    spi_flash_device.flash_device.user_data = RT_NULL;

    rt_device_register(&spi_flash_device.flash_device, flash_device_name,
                       RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE);

    return RT_EOK;
}
