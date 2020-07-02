/**
 * @file IpConnectHandler.h
 * @brief Public API for interacting with cable network connection and ip addressing
 */

#ifndef NETWORKTASKS_IPCONNECTHANDLER_H_
#define NETWORKTASKS_IPCONNECTHANDLER_H_

#include "netif.h"


//if low on space we can try to combine these functions into one return of the struct
uint32_t getCurrentIpv4Address();
uint32_t getCurrentIpv4Netmask();
uint32_t getCurrentIpv4Gateway();

uint32_t setCurrentIpv4Address(uint8_t newAddress[]);
uint32_t setCurrentIpv4Netmask(uint8_t newAddress[]);
uint32_t setCurrentIpv4Gateway(uint8_t newAddress[]);

void printIpValues();
void initIpAddrTasks();
void IpAddrHandlerLinkChange( bool linked);
void initPhyNetIf();

#endif /* NETWORKTASKS_IPCONNECTHANDLER_H_ */
