/**
  ******************************************************************************
  * @file    stm32_eval_spi_sd.h
  * @author  MCD Application Team
  * @version V4.5.0
  * @date    07-March-2011
  * @brief   This file contains all the functions prototypes for the stm32_eval_spi_sd
  *          firmware driver.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************  
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_SD_H
#define __SPI_SD_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "board_sdio.h"

/** @addtogroup STM3210B_EVAL_SD_SPI
  * @{
  */
/**
  * @brief  SD SPI Interface pins
  */
#define SD_SPI                           SPI1
#define SD_SPI_CLK                       RCC_APB2Periph_SPI1
#define SD_SPI_SCK_PIN                   GPIO_Pin_5                  /* PA.05 */
#define SD_SPI_SCK_GPIO_PORT             GPIOA                       /* GPIOA */
#define SD_SPI_SCK_GPIO_CLK              RCC_APB2Periph_GPIOA
#define SD_SPI_MISO_PIN                  GPIO_Pin_6                  /* PA.06 */
#define SD_SPI_MISO_GPIO_PORT            GPIOA                       /* GPIOA */
#define SD_SPI_MISO_GPIO_CLK             RCC_APB2Periph_GPIOA
#define SD_SPI_MOSI_PIN                  GPIO_Pin_7                  /* PA.07 */
#define SD_SPI_MOSI_GPIO_PORT            GPIOA                       /* GPIOA */
#define SD_SPI_MOSI_GPIO_CLK             RCC_APB2Periph_GPIOA
#define SD_CS_PIN                        GPIO_Pin_11                 /* PD.11 */
#define SD_CS_GPIO_PORT                  GPIOD                       /* GPIOD */
#define SD_CS_GPIO_CLK                   RCC_APB2Periph_GPIOD
#define SD_DETECT_PIN                    GPIO_Pin_14                 /* PE.14 */
#define SD_DETECT_GPIO_PORT              GPIOB                       /* GPIOB */
#define SD_DETECT_GPIO_CLK               RCC_APB2Periph_GPIOB
#if 0
#define SD_SPI                           SPI3
#define SD_SPI_CLK                       RCC_APB1Periph_SPI3
#define SD_SPI_SCK_PIN                   GPIO_Pin_10                 /* PC.10 */
#define SD_SPI_SCK_GPIO_PORT             GPIOC                       /* GPIOC */
#define SD_SPI_SCK_GPIO_CLK              RCC_APB2Periph_GPIOC
#define SD_SPI_MISO_PIN                  GPIO_Pin_11                 /* PC.11 */
#define SD_SPI_MISO_GPIO_PORT            GPIOC                       /* GPIOC */
#define SD_SPI_MISO_GPIO_CLK             RCC_APB2Periph_GPIOC
#define SD_SPI_MOSI_PIN                  GPIO_Pin_12                 /* PC.12 */
#define SD_SPI_MOSI_GPIO_PORT            GPIOC                       /* GPIOC */
#define SD_SPI_MOSI_GPIO_CLK             RCC_APB2Periph_GPIOC
#define SD_CS_PIN                        GPIO_Pin_4                  /* PA.04 */
#define SD_CS_GPIO_PORT                  GPIOA                       /* GPIOA */
#define SD_CS_GPIO_CLK                   RCC_APB2Periph_GPIOA
#define SD_DETECT_PIN                    GPIO_Pin_0                  /* PE.00 */
#define SD_DETECT_GPIO_PORT              GPIOE                       /* GPIOE */
#define SD_DETECT_GPIO_CLK               RCC_APB2Periph_GPIOE
#endif


/* Includes ------------------------------------------------------------------*/
//#include "stm32_eval.h"
#include "stm32f10x.h"

/** @addtogroup Utilities
  * @{
  */
  
/** @addtogroup STM32_EVAL
  * @{
  */ 

/** @addtogroup Common
  * @{
  */
  
/** @addtogroup STM32_EVAL_SPI_SD
  * @{
  */  

/**
  * @}
  */
  
/** @defgroup STM32_EVAL_SPI_SD_Exported_Constants
  * @{
  */ 
    
/**
  * @brief  Block Size
  */
#define SD_BLOCK_SIZE    0x200

/**
  * @brief  Dummy byte
  */
#define SD_DUMMY_BYTE   0xFF

/**
  * @brief  Start Data tokens:
  *         Tokens (necessary because at nop/idle (and CS active) only 0xff is 
  *         on the data/command line)  
  */
#define SD_START_DATA_SINGLE_BLOCK_READ    0xFE  /*!< Data token start byte, Start Single Block Read */
#define SD_START_DATA_MULTIPLE_BLOCK_READ  0xFE  /*!< Data token start byte, Start Multiple Block Read */
#define SD_START_DATA_SINGLE_BLOCK_WRITE   0xFE  /*!< Data token start byte, Start Single Block Write */
#define SD_START_DATA_MULTIPLE_BLOCK_WRITE 0xFD  /*!< Data token start byte, Start Multiple Block Write */
#define SD_STOP_DATA_MULTIPLE_BLOCK_WRITE  0xFD  /*!< Data toke stop byte, Stop Multiple Block Write */

/**
  * @brief  SD detection on its memory slot
  */
#define SD_PRESENT        ((uint8_t)0x01)
#define SD_NOT_PRESENT    ((uint8_t)0x00)

/**
  * @}
  */ 
  
/** @defgroup STM32_EVAL_SPI_SD_Exported_Macros
  * @{
  */
/** 
  * @brief  Select SD Card: ChipSelect pin low   
  */  
#define SD_CS_LOW()     GPIO_ResetBits(SD_CS_GPIO_PORT, SD_CS_PIN)
/** 
  * @brief  Deselect SD Card: ChipSelect pin high   
  */ 
#define SD_CS_HIGH()    GPIO_SetBits(SD_CS_GPIO_PORT, SD_CS_PIN)
/**
  * @}
  */ 

/** @defgroup STM32_EVAL_SPI_SD_Exported_Functions
  * @{
  */ 


#ifdef __cplusplus
}
#endif

#endif /* __SPI_SD_H */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */ 

/**
  * @}
  */    

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
