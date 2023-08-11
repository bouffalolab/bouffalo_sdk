
/**
 ****************************************************************************************
 *
 * @file dhcp_server.h
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */


#ifndef DHCPV4_SERVER_H__
#define DHCPV4_SERVER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* start, limit is range of dhcpd ip pool, -1 mean use default value */
void dhcpd_start(struct netif *netif, int start, int limit);

#ifdef __cplusplus
}
#endif

#endif

