/*
 * RT-Thread SD Card Driver
 * 20100715 Bernard support SDHC card great than 4G.
 * 20110905 JoyChen support to STM32F2xx
 */
#include <rtthread.h>
#include <dfs_fs.h>
#include "sdio_header.h"
#include "board_sdio.h"
#include "board.h"

static uint32_t CardType =  SDIO_STD_CAPACITY_SD_CARD_V1_1;
//static uint32_t CSD_Tab[4], CID_Tab[4], RCA = 0;
//static uint8_t SDSTATUS_Tab[16];
//__IO uint32_t StopCondition = 0;
//__IO SD_Error TransferError = SD_OK;
//__IO uint32_t TransferEnd = 0;
//SD_CardInfo SDCardInfo;

//SDIO_InitTypeDef SDIO_InitStructure;
//SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
//SDIO_DataInitTypeDef SDIO_DataInitStructure;   


/**
  * @brief  DeInitializes the SD/SD communication.
  * @param  None
  * @retval None
  */
void SD_LowLevel_DeInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  SPI_Cmd(SD_SPI, DISABLE); /*!< SD_SPI disable */
  SPI_I2S_DeInit(SD_SPI);   /*!< DeInitializes the SD_SPI */
  
  /*!< SD_SPI Periph clock disable */
  RCC_APB2PeriphClockCmd(SD_SPI_CLK, DISABLE); 
  
  /*!< Configure SD_SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(SD_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_MISO_PIN;
  GPIO_Init(SD_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_MOSI_PIN;
  GPIO_Init(SD_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI_CS_PIN pin: SD Card CS pin */
  GPIO_InitStructure.GPIO_Pin = SD_CS_PIN;
  GPIO_Init(SD_CS_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI_DETECT_PIN pin: SD Card detect pin */
  GPIO_InitStructure.GPIO_Pin = SD_DETECT_PIN;
  GPIO_Init(SD_DETECT_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @brief  Initializes the SD_SPI and CS pins.
  * @param  None
  * @retval None
  */
void SD_LowLevel_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef   SPI_InitStructure;

  /*!< SD_SPI_CS_GPIO, SD_SPI_MOSI_GPIO, SD_SPI_MISO_GPIO, SD_SPI_DETECT_GPIO 
       and SD_SPI_SCK_GPIO Periph clock enable */
  RCC_APB2PeriphClockCmd(SD_CS_GPIO_CLK | SD_SPI_MOSI_GPIO_CLK | SD_SPI_MISO_GPIO_CLK |
                         SD_SPI_SCK_GPIO_CLK | SD_DETECT_GPIO_CLK, ENABLE);

  /*!< SD_SPI Periph clock enable */
  RCC_APB2PeriphClockCmd(SD_SPI_CLK, ENABLE); 

  
  /*!< Configure SD_SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(SD_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_MISO_PIN;
  GPIO_Init(SD_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = SD_SPI_MOSI_PIN;
  GPIO_Init(SD_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI_CS_PIN pin: SD Card CS pin */
  GPIO_InitStructure.GPIO_Pin = SD_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(SD_CS_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure SD_SPI_DETECT_PIN pin: SD Card detect pin */
  GPIO_InitStructure.GPIO_Pin = SD_DETECT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(SD_DETECT_GPIO_PORT, &GPIO_InitStructure);

  /*!< SD_SPI Config */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SD_SPI, &SPI_InitStructure);
  
  SPI_Cmd(SD_SPI, ENABLE); /*!< SD_SPI enable */
}

void SD_SPI_SetSpeed(uint16_t SPI_BaudRatePrescaler)
{
  if((SPI_BaudRatePrescaler & (uint16_t)0x0038) == SPI_BaudRatePrescaler)
  {
    SD_SPI->CR1 = (SD_SPI->CR1 & (uint16_t)0xFFC7) |SPI_BaudRatePrescaler;
 
    SPI_Cmd(SD_SPI, ENABLE); /*!< SD_SPI enable */
  }
}

void SD_SPI_SetSpeedLow(void)
{
    SD_SPI_SetSpeed(SPI_BaudRatePrescaler_256);
}
void SD_SPI_SetSpeedHi(void)
{
	SD_SPI_SetSpeed(SPI_BaudRatePrescaler_2);
}

/* set sector size to 512 */
#define SECTOR_SIZE		512

static struct rt_device sdcard_device;
static SD_CardInfo SDCardInfo;
static struct dfs_partition part;
static struct rt_semaphore sd_lock;
static rt_uint8_t _sdcard_buffer[SECTOR_SIZE];
/* RT-Thread Device Driver Interface */
static rt_err_t rt_sdcard_init(rt_device_t dev)
{

	if (rt_sem_init(&sd_lock, "sdlock", 1, RT_IPC_FLAG_FIFO) != RT_EOK)
	{
		rt_kprintf("init sd lock semaphore failed\n");
	}
	else
		rt_kprintf("SD Card init OK\n");

	return RT_EOK;
}

static rt_err_t rt_sdcard_open(rt_device_t dev, rt_uint16_t oflag)
{
	return RT_EOK;
}

static rt_err_t rt_sdcard_close(rt_device_t dev)
{
	return RT_EOK;
}

static rt_size_t rt_sdcard_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
	SD_Error status;
	rt_uint32_t retry;
	rt_uint32_t factor;

	status = SD_DATA_OTHER_ERROR;

	if (CardType == SDIO_HIGH_CAPACITY_SD_CARD) factor = 1;
	else factor = SECTOR_SIZE;
	//rt_kprintf("sd: read 0x%X, sector 0x%X, 0x%X\n", (uint32_t)buffer ,pos, size);
	rt_sem_take(&sd_lock, RT_WAITING_FOREVER);

    retry = 3;
    while(retry)
    {
        /* read all sectors */
        if (((rt_uint32_t)buffer % 4 != 0) ||
            ((rt_uint32_t)buffer > 0x20080000))
        {
            rt_uint32_t index;

            /* which is not alignment with 4 or chip SRAM */
            for (index = 0; index < size; index ++)
            {
                status = SD_ReadBlock((uint8_t*)_sdcard_buffer,
					(part.offset + index + pos) * factor, SECTOR_SIZE);

                //status = SD_WaitReadOperation();
        		while(SD_GetStatus() != SD_TRANSFER_OK);
				if (status != SD_RESPONSE_NO_ERROR) break;

                /* copy to the buffer */
                rt_memcpy(((rt_uint8_t*)buffer + index * SECTOR_SIZE), _sdcard_buffer, SECTOR_SIZE);
            }
        }
        else
        {
            if (size == 1)
            {
                status = SD_ReadBlock((uint8_t*)buffer, 
                    (part.offset + pos) * factor, SECTOR_SIZE);
            }
            else
            {
                status = SD_ReadMultiBlocks((uint8_t*)buffer, 
                    (part.offset + pos) * factor, SECTOR_SIZE, size);
            }
			//status = SD_WaitReadOperation();
        	while(SD_GetStatus() != SD_TRANSFER_OK);
			/*rt_kprintf("===DUMP SECTOR %d===\n",pos);
			{
				int i, j;
				char* tmp = (char*)buffer;
				for(i =0; i < 32;i++)
				{
					rt_kprintf("%2d: ",i);
					for(j= 0; j < 16;j++)
						rt_kprintf("%02X ",tmp[i*16+j]);
					rt_kprintf("\n");
				}
			} */
        }

        if (status == SD_RESPONSE_NO_ERROR) break;

        retry --;
    }
	rt_sem_release(&sd_lock);
	if (status == SD_RESPONSE_NO_ERROR) return size;

	rt_kprintf("read failed: %d, buffer 0x%08x\n", status, buffer);
	return 0;
}

static rt_size_t rt_sdcard_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
	SD_Error status;
	rt_uint32_t factor;

	status = SD_DATA_OTHER_ERROR;

	if (CardType == SDIO_HIGH_CAPACITY_SD_CARD) factor = 1;
	else factor = SECTOR_SIZE;

	//rt_kprintf("sd: write 0x%X, sector 0x%X, 0x%X\n", (uint32_t)buffer , pos, size);
	rt_sem_take(&sd_lock, RT_WAITING_FOREVER);
	
	/* read all sectors */
	if (((rt_uint32_t)buffer % 4 != 0) ||
        ((rt_uint32_t)buffer > 0x20080000))
	{
	    rt_uint32_t index;

        /* which is not alignment with 4 or not chip SRAM */
        for (index = 0; index < size; index ++)
        {
            /* copy to the buffer */
            rt_memcpy(_sdcard_buffer, ((rt_uint8_t*)buffer + index * SECTOR_SIZE), SECTOR_SIZE);

            status = SD_WriteBlock((uint8_t*)_sdcard_buffer, 
                (part.offset + index + pos) * factor, SECTOR_SIZE);

			//status = SD_WaitWriteOperation();
        	while(SD_GetStatus() != SD_TRANSFER_OK);

            if (status != SD_RESPONSE_NO_ERROR) break;
        }
	}
	else
	{
        if (size == 1)
        {
            status = SD_WriteBlock((uint8_t*)buffer,
                (part.offset + pos) * factor, SECTOR_SIZE);
        }
        else
        {
            status = SD_WriteMultiBlocks((uint8_t*)buffer, 
				(part.offset + pos) * factor, SECTOR_SIZE, size);
        }

		//status = SD_WaitWriteOperation();
        while(SD_GetStatus() != SD_TRANSFER_OK);
	}
	rt_sem_release(&sd_lock);

	if (status == SD_RESPONSE_NO_ERROR) return size;

	rt_kprintf("write failed: %d, buffer 0x%08x\n", status, buffer);
	return 0;
}

static rt_err_t rt_sdcard_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    RT_ASSERT(dev != RT_NULL);

    if (cmd == RT_DEVICE_CTRL_BLK_GETGEOME)
    {
        struct rt_device_blk_geometry *geometry;

        geometry = (struct rt_device_blk_geometry *)args;
        if (geometry == RT_NULL) return -RT_ERROR;

        geometry->bytes_per_sector = 512;
        geometry->block_size = SDCardInfo.CardBlockSize;
		if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)
			geometry->sector_count = (SDCardInfo.SD_csd.DeviceSize + 1)  * 1024;
		else
        	geometry->sector_count = SDCardInfo.CardCapacity/SDCardInfo.CardBlockSize;
    }

	return RT_EOK;
}

void rt_hw_sdcard_init()
{
    //NVIC_InitTypeDef NVIC_InitStructure;
	//rt_kprintf("sdcard init 0\n");
	if (SD_Init() == SD_RESPONSE_NO_ERROR)
	{
		SD_Error status;
		rt_uint8_t *sector;

		//rt_kprintf("sdcard init 1\n");
		/*status = SD_GetCardInfo(&SDCardInfo);
		if (status != SD_OK) goto __return;

		status = SD_SelectDeselect((u32) (SDCardInfo.RCA << 16));
		if (status != SD_OK) goto __return;

		SD_EnableWideBusOperation(SDIO_BusWide_4b);
		SD_SetDeviceMode(SD_DMA_MODE); */

		/* get the first sector to read partition table */
		sector = (rt_uint8_t*) rt_malloc (512);
		if (sector == RT_NULL)
		{
			rt_kprintf("allocate partition sector buffer failed\n");
			return;
		}
		status = SD_ReadBlock((uint8_t*)sector, 0, 512);
		//status = SD_WaitReadOperation();
        while(SD_GetStatus() != SD_TRANSFER_OK);

		if (status == SD_RESPONSE_NO_ERROR)
		{
			/* get the first partition */
			if (dfs_filesystem_get_partition(&part, sector, 0) != 0)
            {
                /* there is no partition */
                part.offset = 0;
                part.size   = 0;
            }
		}
		else
		{
			/* there is no partition table */
			part.offset = 0;
			part.size   = 0;
		}

		/* release sector buffer */
		rt_free(sector);

		/* register sdcard device */
		sdcard_device.type  = RT_Device_Class_Block;
		sdcard_device.init 	= rt_sdcard_init;
		sdcard_device.open 	= rt_sdcard_open;
		sdcard_device.close = rt_sdcard_close;
		sdcard_device.read 	= rt_sdcard_read;
		sdcard_device.write = rt_sdcard_write;
		sdcard_device.control = rt_sdcard_control;

		/* no private */
		sdcard_device.user_data = &SDCardInfo;

		//rt_kprintf("sdcard init xxxx\n");

		rt_device_register(&sdcard_device, "sd0",
			RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_REMOVABLE | RT_DEVICE_FLAG_STANDALONE);
		
		return;
	}else
		rt_kprintf("sdcard init failed\n");
//__return:
//	rt_kprintf("sdcard init failed\n");
}
