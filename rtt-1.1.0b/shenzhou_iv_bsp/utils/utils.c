#include "utils.h"

//u32 g_ChipUniqueID[3];
//BYTE *g_chipid;
BYTE g_chipid[12];

u8 *get_chipid(void)
{
	u32 ChipUniqueID[3];
	BYTE *chipid;
	u8 size = sizeof(g_chipid);
	
	ChipUniqueID[2] = *(__IO u32 *)(0X1FFFF7F0); // ¸ß×Ö½Ú
	ChipUniqueID[1] = *(__IO u32 *)(0X1FFFF7EC); // 
	ChipUniqueID[0] = *(__IO u32 *)(0X1FFFF7E8); // µÍ×Ö½Ú
	chipid = (BYTE*)&ChipUniqueID;
	{
		BYTE *dst,*src;
		dst = g_chipid;
		src = chipid + 11;
		memset(g_chipid,0,size);
		while (size--)
		{
			*dst++ = *src--;
		}
	}
	return g_chipid;
}

void list_chipinfo()
{
	u8 i,size = sizeof(g_chipid);
	get_chipid();
	rt_kprintf("chip id: ");
//chip id: 05DDFF36 34305432 43254142  
//chip id: 42412543 32543034 36FFDD05
//chip id: 43254142-34305432-05DDFF36
	for (i=0;i<size;i++)
	{
		rt_kprintf("%02X",g_chipid[i]);
	}
	rt_kprintf("\n");
}

#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT_ALIAS(list_chipinfo, list_chipinfo, list chip info. e.g: list_chipinfo())
#endif
