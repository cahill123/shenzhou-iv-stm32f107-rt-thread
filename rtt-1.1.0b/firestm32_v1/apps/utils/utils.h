#ifndef __UTILS_COMMON_H__
#define __UTILS_COMMON_H__

#include "rtconfig.h"
#include "rtthread.h"
#include "filesystems/elmfat/integer.h"
#include "stm32f10x.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"


extern u32 g_ChipUniqueID[3];

u8 *get_chipid(void);

#endif //__UTILS_COMMON_H__
