/*
	HTTP CLIENT FOR RAW LWIP
	(c) 2008-2009 Noyens Kenneth
	PUBLIC VERSION V0.2 16/05/2009
 
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE Version 2.1 as published by
	the Free Software Foundation.
 
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.
 
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the
	Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 
*/
 
#ifndef __WEBCLIENT_H
#define __WEBCLIENT_H

#include "rtconfig.h"
#include "rtthread.h"
#include "lwip/opt.h"

#include "lwip/mem.h"
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/timers.h"
#include "lwip/inet_chksum.h"
#include "lwip/tcp.h"

//#ifdef PING_USE_SOCKETS
#include "lwip/sockets.h"
#include "lwip/inet.h"
//#endif /* PING_USE_SOCKETS */

//#include "utils/lwiplib.h"
 
//#include "inc/hw_memmap.h"
//#include "inc/hw_types.h"
//#include "ethernet.h"
#include "url_parse.h"
 
typedef struct tcp_pcb tcp_pcb_t;
 
// You can replace this enum for saving MEMORY (replace with define's)
typedef enum
{
	OK,
	OUT_MEM,
	TIMEOUT,
	NOT_FOUND,
	GEN_ERROR
} hc_errormsg;
 
struct hc_state {
  u8_t Num;
  char *Page;
  char *PostVars;
  char *RecvData;
  u16_t Len;
  u8_t ConnectionTimeout;
  void (* ReturnPage)(u8_t num, hc_errormsg, char *data, u16_t len);
};
 
// Public function
int hc_open(struct ip_addr, char *, char *, void (*)(u8_t, hc_errormsg, char *, u16_t));
 
#endif //  __WEBCLIENT_H
