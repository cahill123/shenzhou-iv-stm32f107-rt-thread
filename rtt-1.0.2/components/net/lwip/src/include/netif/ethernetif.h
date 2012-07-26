#ifndef __NETIF_ETHERNETIF_H__
#define __NETIF_ETHERNETIF_H__

#include "lwip/netif.h"
#include <rtthread.h>

#define NIOCTL_GADDR		0x01
#define ETHERNET_MTU		1500


struct eth_device
{
	/* inherit from rt_device */
	struct rt_device parent;

	struct netif *netif;

	/* eth device interface */
	struct pbuf* (*eth_rx)(rt_device_t dev);

	rt_err_t (*eth_tx)(rt_device_t dev, struct pbuf* p);
};

void lwip_enetif_init(void);
err_t dhcp_start(struct netif *netif);

rt_err_t eth_rx_ready(struct eth_device *dev);
rt_err_t eth_device_ready(struct eth_device* dev);
int eth_device_init(struct eth_device * dev, char *name);

#endif /* __NETIF_ETHERNETIF_H__ */
