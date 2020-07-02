/**
 * @file IpConnectHandler.c
 *
 * @details full implementation of functions involving direct connecting and disconnecting from the wired network.
 * @see IpConnectHandler.h to see public API
 * @todo test point to point connection
 * @todo handle using IP Address stored within external flash memory configuration
 * @todo handle using differently stored mac address than the currently hardcoded value
 */

#include <FreeRTOS.h>
#include <task.h>

#include "lwip/netifapi.h"
#include "lwip/dhcp.h"
#include "prot/dhcp.h"
#include "tcpip.h"
#include "board.h"
#include "enet_ethernetif.h"
#include "netif.h"
#include "ip4_addr.h"
#include "fsl_phy.h"
#include "NGRMRelay.h"
#include "linkwatch/linkwatch.h"
#include "IpConnectHandler.h"

/*******************************************************************************
 * Definitions ================================================================
 ******************************************************************************/

#define IP_ADDR_SUCCESS (0)
#define IP_ADDR_FAILURE (1)

#define PTP_IP_OCTECT0 (10)
#define PTP_IP_OCTECT1 (106)
#define DHCP_CYCLES 60

#define PHY_ADDRESS BOARD_ENET0_PHY_ADDRESS
#define PHY_CLOCK kCLOCK_CoreSysClk

/*******************************************************************************
 * Prototypes/externs =========================================================
 ******************************************************************************/
extern uint32_t DebugTaskWrite(const char *WriteData, size_t Size); /**< external call to write items to the CLI*/

/*******************************************************************************
 * Variables ==================================================================
 ******************************************************************************/
static uint8_t tl_NetworkStartedFlag = 0; // flag to signal if the network has been started
static struct netif tl_fsl_netif0; //@todo replace this with NVM storage
static ip4_addr_t tl_fsl_netif0_ipaddr;
static ip4_addr_t tl_fsl_netif0_netmask ;//= {0xffffff};
static ip4_addr_t tl_fsl_netif0_gw;

//static TaskHandle_t  tl_netConnTaskId;
static uint8_t gPtPOctect2 = 0;
static uint8_t gPtPOctect3 = 0;

/******************************************************************************
 * Private Methods ===========================================================
 *****************************************************************************/
/*!
 * @brief Prints DHCP status of the interface when it has changed from last status.
 *
 * @param netif network interface structure
 * @return -1 if no change was made in status, return dhcp state otherwise
 */
int print_dhcp_state(struct netif *netif)
{
    static u8_t dhcp_last_state = DHCP_STATE_OFF;
    struct dhcp *dhcp = (struct dhcp *)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);
	int returnStatus = -1;
    if (dhcp_last_state != dhcp->state)
    {
    	returnStatus = dhcp->state;
    	//returnStatus = 0;
        dhcp_last_state = dhcp->state;

        PRINTF(" DHCP state       : ");
        DebugTaskWrite(" DHCP state       : ", 20);
        switch (dhcp_last_state)
        {
            case DHCP_STATE_OFF:
                PRINTF("OFF");
                DebugTaskWrite("OFF", 3);
                break;
            case DHCP_STATE_REQUESTING:
                PRINTF("REQUESTING");
                DebugTaskWrite("REQUESTING", 10);
                break;
            case DHCP_STATE_INIT:
                PRINTF("INIT");
                DebugTaskWrite("INIT", 4);
                break;
            case DHCP_STATE_REBOOTING:
                PRINTF("REBOOTING");
                DebugTaskWrite("REBOOTING", 9);
                break;
            case DHCP_STATE_REBINDING:
                PRINTF("REBINDING");
                DebugTaskWrite("REBINDING", 9);
                break;
            case DHCP_STATE_RENEWING:
                PRINTF("RENEWING");
                DebugTaskWrite("RENEWING", 8);
                break;
            case DHCP_STATE_SELECTING:
                PRINTF("SELECTING");
                DebugTaskWrite("SELECTING", 9);
                break;
            case DHCP_STATE_INFORMING:
                PRINTF("INFORMING");
                DebugTaskWrite("INFORMING", 9);
                break;
            case DHCP_STATE_CHECKING:
                PRINTF("CHECKING");
                DebugTaskWrite("CHECKING", 8);
                break;
            case DHCP_STATE_BOUND:
                PRINTF("BOUND");
                DebugTaskWrite("BOUND", 5);
                break;
            case DHCP_STATE_BACKING_OFF:
                PRINTF("BACKING_OFF");
                DebugTaskWrite("BACKING_OFF", 11);
                break;
            default:
                PRINTF("%u", dhcp_last_state);
                assert(0);
                break;
        }
        PRINTF("\r\n");
        DebugTaskWrite("\r\n",sizeof("\n"));

        if (dhcp_last_state == DHCP_STATE_BOUND)
        {

            tl_NetworkStartedFlag = 1;

//        	char ip_addr[64], netMask_addr[64], gateway_addr[64];
            PRINTF("\r\n IPv4 Address     : %u.%u.%u.%u\r\n", ((u8_t *)&netif->ip_addr.addr)[0],
                   ((u8_t *)&netif->ip_addr.addr)[1], ((u8_t *)&netif->ip_addr.addr)[2],
                   ((u8_t *)&netif->ip_addr.addr)[3]);
            PRINTF(" IPv4 Subnet mask : %u.%u.%u.%u\r\n", ((u8_t *)&netif->netmask.addr)[0],
                   ((u8_t *)&netif->netmask.addr)[1], ((u8_t *)&netif->netmask.addr)[2],
                   ((u8_t *)&netif->netmask.addr)[3]);
            PRINTF(" IPv4 Gateway     : %u.%u.%u.%u\r\n\r\n", ((u8_t *)&netif->gw.addr)[0],
                   ((u8_t *)&netif->gw.addr)[1], ((u8_t *)&netif->gw.addr)[2], ((u8_t *)&netif->gw.addr)[3]);
            printIpValues();
//            linkwatch_init();
        }
    }
    return returnStatus;
}

/**
 * @brief attempt to make a network connection with point to point addressing
 * @details Return the IP address to the factory default value CFG_MEMMAP_SIP_STATIC_IP
 */
static int32_t ConnectWithPointToPointAddress(uint32_t IPAddress)
{
    int32_t retCode = IP_ADDR_FAILURE;
    struct netif *netif = &tl_fsl_netif0;
    if (tl_fsl_netif0.flags & NETIF_FLAG_ETHARP)
    {
        if (tl_fsl_netif0.flags & NETIF_FLAG_UP)
        {
            // Release the DHCP lease (it already calls netif_set_down() function)
            dhcp_release(&tl_fsl_netif0);

            // Stop the dhcp service
            dhcp_stop(&tl_fsl_netif0);

            netif_set_down(&tl_fsl_netif0);
        }

        // Set the address to its default hard-coded IP
        tl_fsl_netif0_ipaddr.addr = IPAddress;

        // Set the new ip address
		tl_fsl_netif0_gw.addr = (u32_t) IP_ADDR_ANY;//fsl_netif0_ipaddr.addr | 0xFFFF0000; // class B network

		tl_fsl_netif0_netmask.addr = 0xFFFF; // class B network subnet mask
        netif_set_addr(&tl_fsl_netif0, &tl_fsl_netif0_ipaddr, &tl_fsl_netif0_netmask, &tl_fsl_netif0_gw);

        // Bring the interface up again
        netif_set_up(&tl_fsl_netif0);

       PRINTF("\r\n New IP \r\n");
       PRINTF("\r\n IPv4 Address     : %u.%u.%u.%u\r\n", ((u8_t *)&netif->ip_addr.addr)[0],
               ((u8_t *)&netif->ip_addr.addr)[1], ((u8_t *)&netif->ip_addr.addr)[2],
               ((u8_t *)&netif->ip_addr.addr)[3]);
       PRINTF(" IPv4 Subnet mask : %u.%u.%u.%u\r\n", ((u8_t *)&netif->netmask.addr)[0],
               ((u8_t *)&netif->netmask.addr)[1], ((u8_t *)&netif->netmask.addr)[2],
               ((u8_t *)&netif->netmask.addr)[3]);
       PRINTF(" IPv4 Gateway     : %u.%u.%u.%u\r\n\r\n", ((u8_t *)&netif->gw.addr)[0],
               ((u8_t *)&netif->gw.addr)[1], ((u8_t *)&netif->gw.addr)[2], ((u8_t *)&netif->gw.addr)[3]);
        //dynamicip = 0;
        tl_NetworkStartedFlag = 1;
       printIpValues();
        retCode = IP_ADDR_SUCCESS;
    }
    else
    {
    	PRINTF("Bad Attempt at Default IP\r\n");
    }

    return retCode;
}

/**
 * @details Determine if the DHCP state is DHCP_STATE_BOUND
 * @todo try and combine this function with DoWeHaveDHCPConnection for space saving
 * @return true/false if dhcp->state == DHCP_STATE_BOUND
 */
static int dhcp_state_is_bound(struct netif *netif)
{
    struct dhcp *dhcp = (struct dhcp *)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);
    /*
     * check to see if DHCP_STATE_BOUND
     */
    return (dhcp->state == DHCP_STATE_BOUND);
}
/**
 * @details to return if a DHCP connection was made
 */
bool DoWeHaveDHCPConnection()
{
	return 1 == dhcp_state_is_bound(&tl_fsl_netif0);
}
/**
 * @details change network address to use a DHCP assigned IP address
 * @return IP_ADDR_SUCCESS on success
 */
static int32_t AttemptDHCPIpAddress()
{
    uint32_t counter;
    int32_t retCode = IP_ADDR_FAILURE;
	union param
	{
		uint8_t u8a[4];
		uint32_t u32;
		uint16_t u16;
	}ipAddress;

    if (tl_fsl_netif0.flags & NETIF_FLAG_ETHARP)
    {
        if (tl_fsl_netif0.flags & NETIF_FLAG_UP)
        {
            // Bring the current interface down
            netif_set_down(&tl_fsl_netif0);
        }

        // Set the address to 0.0.0.0 in case the previous static IP
        // was out of the network class of the DHCP server
        netif_set_addr(&tl_fsl_netif0, IP_ADDR_ANY, IP_ADDR_ANY, IP_ADDR_ANY);

        netif_set_up(&tl_fsl_netif0);

        if( dhcp_start(&tl_fsl_netif0) != ERR_OK )
        {
            PRINTF("Failed to start DHCP\r\n");
        }
        else
        {
			// Try to establish connection for 15 seconds
        	print_dhcp_state(&tl_fsl_netif0);
			vTaskDelay( 250 );
			counter = 1;
			while( !DoWeHaveDHCPConnection() && (counter < DHCP_CYCLES) )
			{
				print_dhcp_state(&tl_fsl_netif0);
				vTaskDelay( 250 );
				counter += 1;
			}

			if( counter < DHCP_CYCLES ) // DHCP Success
			{
				// Print Dynamic IP
				print_dhcp_state(&tl_fsl_netif0);
			}
			else // DHCP Failed
			{
				PRINTF("Bad DHCP attempt, try PTP connection");
				/**
				 * @note change to the PPP IP of 10.106.X.Y
				 */
				ipAddress.u8a[0] = PTP_IP_OCTECT0;
				ipAddress.u8a[1] = PTP_IP_OCTECT1;
				ipAddress.u8a[2] = gPtPOctect2;
				ipAddress.u8a[3] = gPtPOctect3;
				ConnectWithPointToPointAddress(ipAddress.u32 );
			}
        }
        retCode = IP_ADDR_SUCCESS;
    }
    else
    {
    	PRINTF("Bad Attempt at Dynamic IP\r\n");
    }

    return retCode;
}

/**
 * @brief return pointer to the hardware mac address
 * @todo may need to move the mac address handling after decisions are made on where to store it
 */
unsigned char* getMacAddr()
{
	//this function is actually public
	return tl_fsl_netif0.hwaddr;
}


/******************************************************************************
 * public functions ===========================================================
 *****************************************************************************/
/**
 * @brief init tasks related to connecting via ip address through network cable
 * @details this should only be called once in the programs lifetime, therefore
 * it needs to be referenced as an external.
 */
void initIpAddrTasks()
{
    tcpip_init(NULL, NULL);
	linkwatch_init(); //initialize watching the network connection
}


/**
 * @brief initalize the base network phi connection
 * @details this needs to be called before initalizing the network cable watch to avoid hard faulting.
 */
void initPhyNetIf()
{
    ip4_addr_t netif_ipaddr, netif_netmask, netif_gw;
    ethernetif_config_t enet_config = {
            .phyAddress = PHY_ADDRESS,
            .clockName  = PHY_CLOCK,
            .macAddress = configMAC_ADDR,
    #if defined(FSL_FEATURE_SOC_LPC_ENET_COUNT) && (FSL_FEATURE_SOC_LPC_ENET_COUNT > 0)
            .non_dma_memory = non_dma_memory,
    #endif /* FSL_FEATURE_SOC_LPC_ENET_COUNT */
        };
//    netifapi_netif_add(&netif, &netif_ipaddr, &netif_netmask, &netif_gw, &enet_config, EXAMPLE_NETIF_INIT_FN,
//                       tcpip_input);
//    netifapi_netif_set_default(&netif);
//    netifapi_netif_set_up(&netif);
	netifapi_netif_add(&tl_fsl_netif0, &tl_fsl_netif0_ipaddr, &tl_fsl_netif0_netmask, &tl_fsl_netif0_gw, &enet_config, ethernetif0_init,
			tcpip_input);
	netifapi_netif_set_default(&tl_fsl_netif0);
	netifapi_netif_set_up(&tl_fsl_netif0);
}

/**
 * @brief react to the network cable being plugged or unplugged from the unit.
 *
 * @param linked bool if network cable is currently linked
 */
void IpAddrHandlerLinkChange( bool linked)
{
    bool requestDHCP = (tl_fsl_netif0_ipaddr.addr == IPADDR_NONE || tl_fsl_netif0_ipaddr.addr == IPADDR_ANY);
	if(linked)
	{
		//start the network communication back up with the currently held IP address
	    netif_set_default(&tl_fsl_netif0);
		netif_set_up(&tl_fsl_netif0);

		//if the network connection is dhcp try to do the dhcp or point to point connections based on progress
		if(requestDHCP)
		{
			dhcp_start(&tl_fsl_netif0);
			AttemptDHCPIpAddress();
		}


		tl_NetworkStartedFlag = 1;
	}
	else
	{
		//network wire was disconnected from unit stop hosting on a port
		if(requestDHCP)
		{
			//if connection was dhcp stop the associated dhcp processes
			dhcp_release(&tl_fsl_netif0);
			dhcp_stop(&tl_fsl_netif0);
		}
		netif_set_down(&tl_fsl_netif0);
		tl_NetworkStartedFlag = 0;
	}
}

/**
 * @brief returns pointer to current ip address
 * @details it returns the ip used in network communications if the network
 * communication has been started, otherwise it returns the local value that
 * will be used when network communication has been initialized.
 *
 * @return ptr to list of values for ip address
 */
uint32_t getCurrentIpv4Address()
{
	if(tl_NetworkStartedFlag)
	{
		return tl_fsl_netif0.ip_addr.addr;
	}
	else
	{
		return tl_fsl_netif0_ipaddr.addr;
	}
}


/**
 * @brief returns pointer to current subnet mask
 * @details it returns the subnet mask used in network communications if the network
 * communication has been started, otherwise it returns the local value that
 * will be used when network communication has been initialized.
 *
 * @return ptr to list of values for subnet mask
 */
uint32_t getCurrentIpv4Netmask()
{
	if(tl_NetworkStartedFlag)
	{
		return tl_fsl_netif0.netmask.addr;
	}
	else
	{
		return tl_fsl_netif0_netmask.addr;
	}
}

/**
 * @brief returns pointer to current gateway address
 * @details it returns the gateway address used in network communications if the network
 * communication has been started, otherwise it returns the local value that
 * will be used when network communication has been initialized.
 *
 * @return ptr to list of values for ip address
 */
uint32_t getCurrentIpv4Gateway()
{
	if(tl_NetworkStartedFlag)
	{
		return tl_fsl_netif0.gw.addr;
	}
	else
	{
		return tl_fsl_netif0_gw.addr;
	}
}

/**
 * @brief update the current IP Address
 * @param newAddress list of integers within the new ip address
 * @return 0 for success
 */
uint32_t setCurrentIpv4Address(uint8_t newAddress[])
{
	//@todo handle what happens if improper amount of args given

    IP4_ADDR(&tl_fsl_netif0_ipaddr, newAddress[0], newAddress[1], newAddress[2], newAddress[3]);
    IP4_ADDR(&tl_fsl_netif0.ip_addr, newAddress[0], newAddress[1], newAddress[2], newAddress[3]);
    return 0;
}

/**
 * @brief update the current subnet mask
 * @param newAddress list of integers within the new subnet mask
 * @return 0 for success
 */
uint32_t setCurrentIpv4Netmask(uint8_t newAddress[])
{
	//@todo handle what happens if improper amount of args given

    IP4_ADDR(&tl_fsl_netif0_netmask, newAddress[0], newAddress[1], newAddress[2], newAddress[3]);
    IP4_ADDR(&tl_fsl_netif0.netmask, newAddress[0], newAddress[1], newAddress[2], newAddress[3]);
    return 0;
}

/**
 * @brief update the current gateway address
 * @param newAddress list of integers within the new gateway address
 * @return 0 for success
 */
uint32_t setCurrentIpv4Gateway(uint8_t newAddress[]){
	{
	    IP4_ADDR(&tl_fsl_netif0_gw, newAddress[0], newAddress[1], newAddress[2], newAddress[3]);
	    IP4_ADDR(&tl_fsl_netif0.gw, newAddress[0], newAddress[1], newAddress[2], newAddress[3]);
	    return 0;
	}
}


/**
 * @brief trigger printing of the current network connection values (ip, subnet mask, gateway) to the CLI
 */
void printIpValues()
{
    uint32_t ipAddr, subnetMask, gateway;
	char ip_addr[64], netMask_addr[64], gateway_addr[64];

	if(!tl_NetworkStartedFlag)
	{
		sprintf(ip_addr,"\r\n LwIP start not started yet");
		DebugTaskWrite(ip_addr,strlen(ip_addr));
	}

	ipAddr = getCurrentIpv4Address();
	sprintf(ip_addr,"\r\n IPv4 Address     : %u.%u.%u.%u\r\n", ((u8_t *)&ipAddr)[0],
	           ((u8_t *)&ipAddr)[1], ((u8_t *)&ipAddr)[2],
	           ((u8_t *)&ipAddr)[3]);
	DebugTaskWrite(ip_addr,strlen(ip_addr));

	subnetMask = getCurrentIpv4Netmask();
	sprintf(netMask_addr," IPv4 Subnet mask : %u.%u.%u.%u\r\n", ((u8_t *)&subnetMask)[0],
	           ((u8_t *)&subnetMask)[1], ((u8_t *)&subnetMask)[2],
	           ((u8_t *)&subnetMask)[3]);
	DebugTaskWrite(netMask_addr,strlen(netMask_addr));

	gateway = getCurrentIpv4Gateway();
	sprintf(gateway_addr," IPv4 Gateway     : %u.%u.%u.%u\r\n", ((u8_t *)&gateway)[0],
	           ((u8_t *)&gateway)[1], ((u8_t *)&gateway)[2],
	           ((u8_t *)&gateway)[3]);
	DebugTaskWrite(gateway_addr,strlen(gateway_addr));
}


