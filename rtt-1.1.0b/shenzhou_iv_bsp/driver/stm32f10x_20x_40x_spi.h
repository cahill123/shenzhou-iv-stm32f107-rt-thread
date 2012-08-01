/*
 * File      : stm32f20x_40x_spi.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009 RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 20012-01-01    aozima       first implementation.
 */

#ifndef STM32F10X_20X_40X_SPI_H_INCLUDED
#define STM32F10X_20X_40X_SPI_H_INCLUDED

#include <stdint.h>
#include <rtthread.h>
#include <drivers/spi.h>

//#include "rtconfig.h"

#ifdef RT_SPI_STM32F1XX
#define STM32F1XX
#else
#ifdef RT_SPI_STM32F2XX
#define STM32F2XX
#else //RT_SPI_STM32F4XX
#define STM32F4XX
#endif // STM32F2XX
#endif // STM32F1XX

#ifdef STM32F1XX
#include "stm32f10x_spi.h"
#else
#include "stm32f4xx.h"
#endif //STM32F1XX

//#define SPI_USE_DMA

struct stm32_spi_bus
{
    struct rt_spi_bus parent;
    SPI_TypeDef * SPI;
#ifdef SPI_USE_DMA
#ifdef STM32F1XX
    DMA_Channel_TypeDef * DMA_Channel_TX;
    DMA_Channel_TypeDef * DMA_Channel_RX;
#else
    DMA_Stream_TypeDef * DMA_Stream_TX;
    uint32_t DMA_Channel_TX;

    DMA_Stream_TypeDef * DMA_Stream_RX;
    uint32_t DMA_Channel_RX;
#endif //STM32F1XX
    uint32_t DMA_Channel_TX_FLAG_TC;
#ifdef STM32F1XX
    uint32_t DMA_Channel_TX_FLAG_TE;
#endif //STM32F1XX
    uint32_t DMA_Channel_RX_FLAG_TC;
#ifdef STM32F1XX
    uint32_t DMA_Channel_RX_FLAG_TE;
#endif //STM32F1XX
#endif /* #ifdef SPI_USE_DMA */
};

struct stm32_spi_cs
{
    GPIO_TypeDef * GPIOx;
    uint16_t GPIO_Pin;
};

/* public function */
rt_err_t stm32_spi_register(SPI_TypeDef * SPI,
                            struct stm32_spi_bus * stm32_spi,
                            const char * spi_bus_name);
#ifdef STM32F1XX
extern void rt_stm32f10x_spi_init(void);
#endif //STM32F1XX
#endif // STM32F10X_20X_40X_SPI_H_INCLUDED
