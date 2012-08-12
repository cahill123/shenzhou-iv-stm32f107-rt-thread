/*
 * File      : stm32f20x_40x_spi.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009 RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2012-01-01    aozima       first implementation.
 * 2012-07-30    darcy.g      add stm32f10x.
 */

#include "stm32f10x_20x_40x_spi.h"

/* private rt-thread spi ops function */
static rt_err_t configure(struct rt_spi_device* device, struct rt_spi_configuration* configuration);
static rt_uint32_t xfer(struct rt_spi_device* device, struct rt_spi_message* message);

static struct rt_spi_ops stm32_spi_ops =
{
    configure,
    xfer
};

#ifdef SPI_USE_DMA
static uint8_t dummy = 0xFF;
static void DMA_RxConfiguration(struct stm32_spi_bus * stm32_spi_bus,
                                const void * send_addr,
                                void * recv_addr,
                                rt_size_t size)
{
    DMA_InitTypeDef DMA_InitStructure;

    /* Reset DMA Stream registers (for debug purpose) */
#ifndef STM32F1XX
    DMA_DeInit(stm32_spi_bus->DMA_Stream_RX);
    DMA_DeInit(stm32_spi_bus->DMA_Stream_TX);

    /* Check if the DMA Stream is disabled before enabling it.
       Note that this step is useful when the same Stream is used multiple times:
       enabled, then disabled then re-enabled... In this case, the DMA Stream disable
       will be effective only at the end of the ongoing data transfer and it will
       not be possible to re-configure it before making sure that the Enable bit
       has been cleared by hardware. If the Stream is used only once, this step might
       be bypassed. */
    while (DMA_GetCmdStatus(stm32_spi_bus->DMA_Stream_RX) != DISABLE);
    while (DMA_GetCmdStatus(stm32_spi_bus->DMA_Stream_TX) != DISABLE);
#else
    DMA_ClearFlag(stm32_spi_bus->DMA_Channel_RX_FLAG_TC
              | stm32_spi_bus->DMA_Channel_RX_FLAG_TE
              | stm32_spi_bus->DMA_Channel_TX_FLAG_TC
              | stm32_spi_bus->DMA_Channel_TX_FLAG_TE);
#endif //STM32F1XX

    /* Configure DMA_RX Stream or Channel */
#ifndef STM32F1XX
    DMA_Cmd(stm32_spi_bus->DMA_Stream_RX, DISABLE);
    
    DMA_InitStructure.DMA_Channel = stm32_spi_bus->DMA_Channel_RX;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(stm32_spi_bus->SPI->DR));
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = (uint32_t)size;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
#else
    DMA_Cmd(stm32_spi_bus->DMA_Channel_RX, DISABLE);
    
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(stm32_spi_bus->SPI->DR));
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    
    DMA_InitStructure.DMA_BufferSize = (uint32_t)size;
#endif //STM32F1XX
	
    if(recv_addr != RT_NULL)
    {
#ifndef STM32F1XX
  		  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) recv_addr;
#else
  		  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) recv_addr;			
#endif //STM32F1XX
  			DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;

    }
    else
    {
#ifndef STM32F1XX
        DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) (&dummy);
#else
        DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) (&dummy);
#endif //STM32F1XX
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    }
#ifndef STM32F1XX
    DMA_Init(stm32_spi_bus->DMA_Stream_RX, &DMA_InitStructure);

    DMA_Cmd(stm32_spi_bus->DMA_Stream_RX, ENABLE);
#else
    DMA_Init(stm32_spi_bus->DMA_Channel_RX, &DMA_InitStructure);

    DMA_Cmd(stm32_spi_bus->DMA_Channel_RX, ENABLE);
#endif //STM32F1XX

    /* Configure DMA_TX Stream or Channel */
#ifndef STM32F1XX
    DMA_Cmd(stm32_spi_bus->DMA_Stream_TX, DISABLE);

    DMA_InitStructure.DMA_Channel = stm32_spi_bus->DMA_Channel_TX;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(stm32_spi_bus->SPI->DR));
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize = (uint32_t)size;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
#else
    DMA_Cmd(stm32_spi_bus->DMA_Channel_TX, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(stm32_spi_bus->SPI->DR));
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    DMA_InitStructure.DMA_BufferSize = (uint32_t)size;
#endif //STM32F1XX

    if(send_addr != RT_NULL)
    {
#ifndef STM32F1XX
        DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)send_addr;
#else
        DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)send_addr;
#endif //STM32F1XX
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    }
    else
    {
        dummy = 0xFF;
#ifndef STM32F1XX
        DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)(&dummy);;
#else
        DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(&dummy);;
#endif //STM32F1XX
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    }
#ifndef STM32F1XX
    DMA_Init(stm32_spi_bus->DMA_Stream_TX, &DMA_InitStructure);

    DMA_Cmd(stm32_spi_bus->DMA_Stream_TX, ENABLE);
#else
	DMA_Init(stm32_spi_bus->DMA_Channel_TX, &DMA_InitStructure);

    DMA_Cmd(stm32_spi_bus->DMA_Channel_TX, ENABLE);
#endif //STM32F1XX

}
#endif

static rt_err_t configure(struct rt_spi_device* device,
                          struct rt_spi_configuration* configuration)
{
    struct stm32_spi_bus * stm32_spi_bus = (struct stm32_spi_bus *)device->bus;
    SPI_InitTypeDef SPI_InitStructure;
	
		SPI_StructInit(&SPI_InitStructure);
    /* data_width */
    if(configuration->data_width <= 8)
    {
        SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    }
    else if(configuration->data_width <= 16)
    {
        SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
    }
    else
    {
        return RT_EIO;
    }

    /* baudrate */
    {
        uint32_t SPI_APB_CLOCK;
        uint32_t stm32_spi_max_clock;
        uint32_t max_hz;

        stm32_spi_max_clock = 18000000;
        max_hz = configuration->max_hz;
#ifdef STM32F4XX
        stm32_spi_max_clock = 37500000;
#elif STM32F2XX
        stm32_spi_max_clock = 30000000;
#endif

        if(max_hz > stm32_spi_max_clock)
        {
            max_hz = stm32_spi_max_clock;
        }

        SPI_APB_CLOCK = SystemCoreClock / 4;


#ifdef STM32F1XX
        /* STM32F1xx SPI MAX 18Mhz */
#define STM32_PI_APB_CLOCK_MAX 18000000
#else
        /* STM32F2xx SPI MAX 30Mhz */
        /* STM32F4xx SPI MAX 37.5Mhz */
#define STM32_PI_APB_CLOCK_MAX 30000000
#endif //STM32F1XX
			
        if(max_hz >= SPI_APB_CLOCK/2 && SPI_APB_CLOCK/2 <= STM32_PI_APB_CLOCK_MAX)
        {
            SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
        }
        else if(max_hz >= SPI_APB_CLOCK/4)
        {
            SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
        }
        else if(max_hz >= SPI_APB_CLOCK/8)
        {
            SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
        }
        else if(max_hz >= SPI_APB_CLOCK/16)
        {
            SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
        }
        else if(max_hz >= SPI_APB_CLOCK/32)
        {
            SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
        }
        else if(max_hz >= SPI_APB_CLOCK/64)
        {
            SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
        }
        else if(max_hz >= SPI_APB_CLOCK/128)
        {
            SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
        }
        else
        {
            /*  min prescaler 256 */
            SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
        }
    } /* baudrate */

    /* CPOL */
    if(configuration->mode & RT_SPI_CPOL)
    {
        SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    }
    else
    {
        SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    }
    /* CPHA */
    if(configuration->mode & RT_SPI_CPHA)
    {
        SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    }
    else
    {
        SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    }
    /* MSB or LSB */
    if(configuration->mode & RT_SPI_MSB)
    {
        SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    }
    else
    {
        SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
    }
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_NSS  = SPI_NSS_Soft;

    /* init SPI */
    SPI_I2S_DeInit(stm32_spi_bus->SPI);
    SPI_Init(stm32_spi_bus->SPI, &SPI_InitStructure);
    /* Enable SPI_MASTER */
    SPI_Cmd(stm32_spi_bus->SPI, ENABLE);
    SPI_CalculateCRC(stm32_spi_bus->SPI, DISABLE);

    return RT_EOK;
};

static rt_uint32_t xfer(struct rt_spi_device* device, struct rt_spi_message* message)
{
    struct stm32_spi_bus * stm32_spi_bus = (struct stm32_spi_bus *)device->bus;
    struct rt_spi_configuration * config = &device->config;
    SPI_TypeDef * SPI = stm32_spi_bus->SPI;
    struct stm32_spi_cs * stm32_spi_cs = device->parent.user_data;
    rt_uint32_t size = message->length;

    /* take CS */
    if(message->cs_take)
    {
        GPIO_ResetBits(stm32_spi_cs->GPIOx, stm32_spi_cs->GPIO_Pin);
    }

#ifdef SPI_USE_DMA
    if(message->length > 32)
    {
        if(config->data_width <= 8)
        {
            DMA_RxConfiguration(stm32_spi_bus, message->send_buf, message->recv_buf, message->length);
//            SPI_I2S_ClearFlag(SPI, SPI_I2S_FLAG_RXNE);
            SPI_I2S_DMACmd(SPI, SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx, ENABLE);
#ifndef STM32F1XX
            while (DMA_GetFlagStatus(stm32_spi_bus->DMA_Stream_RX, stm32_spi_bus->DMA_Channel_RX_FLAG_TC) == RESET
                    || DMA_GetFlagStatus(stm32_spi_bus->DMA_Stream_TX, stm32_spi_bus->DMA_Channel_TX_FLAG_TC) == RESET);
#else
            while (DMA_GetFlagStatus(stm32_spi_bus->DMA_Channel_RX_FLAG_TC) == RESET
            		|| DMA_GetFlagStatus(stm32_spi_bus->DMA_Channel_TX_FLAG_TC) == RESET);	
#endif //STM32F1XX
            SPI_I2S_DMACmd(SPI, SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx, DISABLE);
        }
    }
    else
#endif
    {
        if(config->data_width <= 8)
        {
            const rt_uint8_t * send_ptr = message->send_buf;
            rt_uint8_t * recv_ptr = message->recv_buf;

            while(size--)
            {
                rt_uint8_t data = 0xFF;

                if(send_ptr != RT_NULL)
                {
                    data = *send_ptr++;
                }

                //Wait until the transmit buffer is empty
                while (SPI_I2S_GetFlagStatus(SPI, SPI_I2S_FLAG_TXE) == RESET);
                // Send the byte
                SPI_I2S_SendData(SPI, data);

                //Wait until a data is received
                while (SPI_I2S_GetFlagStatus(SPI, SPI_I2S_FLAG_RXNE) == RESET);
                // Get the received data
                data = SPI_I2S_ReceiveData(SPI);

                if(recv_ptr != RT_NULL)
                {
                    *recv_ptr++ = data;
                }
            }
        }
        else if(config->data_width <= 16)
        {
            const rt_uint16_t * send_ptr = message->send_buf;
            rt_uint16_t * recv_ptr = message->recv_buf;

            while(size--)
            {
                rt_uint16_t data = 0xFF;

                if(send_ptr != RT_NULL)
                {
                    data = *send_ptr++;
                }

                //Wait until the transmit buffer is empty
                while (SPI_I2S_GetFlagStatus(SPI, SPI_I2S_FLAG_TXE) == RESET);
                // Send the byte
                SPI_I2S_SendData(SPI, data);

                //Wait until a data is received
                while (SPI_I2S_GetFlagStatus(SPI, SPI_I2S_FLAG_RXNE) == RESET);
                // Get the received data
                data = SPI_I2S_ReceiveData(SPI);

                if(recv_ptr != RT_NULL)
                {
                    *recv_ptr++ = data;
                }
            }
        }
    }

    /* release CS */
    if(message->cs_release)
    {
        GPIO_SetBits(stm32_spi_cs->GPIOx, stm32_spi_cs->GPIO_Pin);
    }

    return message->length;
};

/** \brief init and register stm32 spi bus.
 *
 * \param SPI: STM32 SPI, e.g: SPI1,SPI2,SPI3.
 * \param stm32_spi: stm32 spi bus struct.
 * \param spi_bus_name: spi bus name, e.g: "spi1"
 * \return
 *
 */
rt_err_t stm32_spi_register(SPI_TypeDef * SPI,
                            struct stm32_spi_bus * stm32_spi,
                            const char * spi_bus_name)
{
    if(SPI == SPI1)
    {
    	stm32_spi->SPI = SPI1;
#ifndef STM32F1XX
#ifdef SPI_USE_DMA
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
        /* DMA2_Stream0 DMA_Channel_3 : SPI1_RX */
        /* DMA2_Stream2 DMA_Channel_3 : SPI1_RX */
        stm32_spi->DMA_Stream_RX = DMA2_Stream0;
        stm32_spi->DMA_Channel_RX = DMA_Channel_3;
        stm32_spi->DMA_Channel_RX_FLAG_TC = DMA_FLAG_TCIF0;
        /* DMA2_Stream3 DMA_Channel_3 : SPI1_TX */
        /* DMA2_Stream5 DMA_Channel_3 : SPI1_TX */
        stm32_spi->DMA_Stream_TX = DMA2_Stream3;
        stm32_spi->DMA_Channel_TX = DMA_Channel_3;
        stm32_spi->DMA_Channel_TX_FLAG_TC = DMA_FLAG_TCIF3;
#endif /* #ifdef SPI_USE_DMA */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
#else
#ifdef SPI_USE_DMA
        stm32_spi->DMA_Channel_RX = DMA1_Channel2;
        stm32_spi->DMA_Channel_TX = DMA1_Channel3;
        stm32_spi->DMA_Channel_RX_FLAG_TC = DMA1_FLAG_TC2;
        stm32_spi->DMA_Channel_RX_FLAG_TE = DMA1_FLAG_TE2;
        stm32_spi->DMA_Channel_TX_FLAG_TC = DMA1_FLAG_TC3;
        stm32_spi->DMA_Channel_TX_FLAG_TE = DMA1_FLAG_TE3;	
#endif /* #ifdef SPI_USE_DMA */
#endif //STM32F1XX
    }
    else if(SPI == SPI2)
    {
        stm32_spi->SPI = SPI2;
#ifndef STM32F1XX
#ifdef SPI_USE_DMA
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
        /* DMA1_Stream3 DMA_Channel_0 : SPI2_RX */
        stm32_spi->DMA_Stream_RX = DMA1_Stream3;
        stm32_spi->DMA_Channel_RX = DMA_Channel_0;
        stm32_spi->DMA_Channel_RX_FLAG_TC = DMA_FLAG_TCIF3;
        /* DMA1_Stream4 DMA_Channel_0 : SPI2_TX */
        stm32_spi->DMA_Stream_TX = DMA1_Stream4;
        stm32_spi->DMA_Channel_TX = DMA_Channel_0;
        stm32_spi->DMA_Channel_TX_FLAG_TC = DMA_FLAG_TCIF4;
#endif //SPI_USE_DMA
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
#else
#ifdef SPI_USE_DMA
        stm32_spi->DMA_Channel_RX = DMA1_Channel4;
        stm32_spi->DMA_Channel_TX = DMA1_Channel5;
        stm32_spi->DMA_Channel_RX_FLAG_TC = DMA1_FLAG_TC4;
        stm32_spi->DMA_Channel_RX_FLAG_TE = DMA1_FLAG_TE4;
        stm32_spi->DMA_Channel_TX_FLAG_TC = DMA1_FLAG_TC5;
        stm32_spi->DMA_Channel_TX_FLAG_TE = DMA1_FLAG_TE5;
#endif /* #ifdef SPI_USE_DMA */	
#endif //STM32F1XX
    }
    else if(SPI == SPI3)
    {
    	stm32_spi->SPI = SPI3;
#ifndef STM32F1XX
#ifdef SPI_USE_DMA
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
        /* DMA1_Stream2 DMA_Channel_0 : SPI3_RX */
        stm32_spi->DMA_Stream_RX = DMA1_Stream2;
        stm32_spi->DMA_Channel_RX = DMA_Channel_0;
        stm32_spi->DMA_Channel_RX_FLAG_TC = DMA_FLAG_TCIF2;
        /* DMA1_Stream5 DMA_Channel_0 : SPI3_TX */
        stm32_spi->DMA_Stream_TX = DMA1_Stream5;
        stm32_spi->DMA_Channel_TX = DMA_Channel_0;
        stm32_spi->DMA_Channel_TX_FLAG_TC = DMA_FLAG_TCIF5;
#endif
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
#else
#ifdef SPI_USE_DMA
        stm32_spi->DMA_Channel_RX = DMA2_Channel1;
        stm32_spi->DMA_Channel_TX = DMA2_Channel2;
        stm32_spi->DMA_Channel_RX_FLAG_TC = DMA2_FLAG_TC1;
        stm32_spi->DMA_Channel_RX_FLAG_TE = DMA2_FLAG_TE1;
        stm32_spi->DMA_Channel_TX_FLAG_TC = DMA2_FLAG_TC2;
        stm32_spi->DMA_Channel_TX_FLAG_TE = DMA2_FLAG_TE2;
#endif /* #ifdef SPI_USE_DMA */
#endif //
    }
    else
    {
        return RT_ENOSYS;
    }

    return rt_spi_bus_register(&stm32_spi->parent, spi_bus_name, &stm32_spi_ops);
}

#ifdef STM32F1XX
void rt_stm32f10x_spi_init(void)
{
#ifdef USING_SPI1
    /* SPI1 config */
    {
		static struct stm32_spi_bus stm32_spi_1;
        GPIO_InitTypeDef GPIO_InitStructure;

        /* Enable SPI1 Periph clock */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA
        | RCC_APB2Periph_AFIO | RCC_APB2Periph_SPI1,
        ENABLE);

#ifdef SPI_USE_DMA
        /* Enable the DMA1 Clock */
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
#endif /* #ifdef SPI_USE_DMA */

        /* Configure SPI1 pins: PA5-SCK, PA6-MISO and PA7-MOSI */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

		stm32_spi_register(SPI1, &stm32_spi_1, "spi1");
    } /* SPI1 config */
#endif //SPI1

#ifdef USING_SPI2
    /* SPI config */
    {
		static struct stm32_spi_bus stm32_spi_2;
        GPIO_InitTypeDef GPIO_InitStructure;

        /* Enable SPI2 Periph clock */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
        /*!< Enable the SPI and GPIO clock */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

#ifdef SPI_USE_DMA
        /* Enable the DMA1 Clock */
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
#endif
        /* Configure SPI2 pins: PB13-SCK, PB14-MISO and PB15-MOSI */
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

        /*!< SPI SCK pin configuration */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; /* PB13 SPI2_SCK */
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        /*!< SPI MISO pin configuration */
        GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14; /* PB14 SPI2_MISO */
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        /*!< SPI MOSI pin configuration */
        GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15; /* PB15 SPI2_MOSI */
        GPIO_Init(GPIOB, &GPIO_InitStructure);

		stm32_spi_register(SPI2, &stm32_spi_2, "spi2");
    } /* SPI config */
#endif //SPI2

#ifdef USING_SPI3
    /* SPI config */
    {
		static struct stm32_spi_bus stm32_spi_3;
        GPIO_InitTypeDef GPIO_InitStructure;

        /* Enable SPI2 Periph clock */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
        /*!< Enable the SPI and GPIO clock */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

#ifdef SPI_USE_DMA
        /* Enable the DMA2 Clock */
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
#endif
        /* Configure SPI2 pins: PB13-SCK, PB14-MISO and PB15-MOSI */
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

        /*!< SPI SCK pin configuration */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; /* PB13 SPI2_SCK */
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        /*!< SPI MISO pin configuration */
        GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14; /* PB14 SPI2_MISO */
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        /*!< SPI MOSI pin configuration */
        GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15; /* PB15 SPI2_MOSI */
        GPIO_Init(GPIOB, &GPIO_InitStructure);

		stm32_spi_register(SPI3, &stm32_spi_3, "spi3");
    } /* SPI config */
#endif //SPI3

}
#endif //STM32F1XX
