#ifndef __BOARD_SDIO_SD_H
#define __BOARD_SDIO_SD_H

#ifdef __cplusplus
 extern "C" {
#endif
	
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#include "board.h"
#include "stm32f10x.h"
#include "spi_sd.h"

void SD_LowLevel_DeInit(void);
void SD_LowLevel_Init(void); 
void SD_SPI_SetSpeed(uint16_t SPI_BaudRatePrescaler);
void SD_SPI_SetSpeedLow(void);
void SD_SPI_SetSpeedHi(void);

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_SDIO_SD_H */
