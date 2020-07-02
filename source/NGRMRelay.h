/**
 * @file NGRRelay.h
 */

#ifndef NGRMRELAY_H_
#define NGRMRELAY_H_


#define MAJOR_VERSION (0)
#define MINOR_VERSION (1)
#define VERSION_VERSION (1)
#define REVISION_VERSION (18)


#define VERSION_DATA {MAJOR_VERSION, MINOR_VERSION, VERSION_VERSION, REVISION_VERSION}


// @todo should make list of more detailed errors somewhere
#define NGR_SUCCESS (0)
#define NGR_GENERAL_FAILURE (1)

#define MP8000_HW (1)
#define TWR_K60_HW (2)
#define NGR_RELAY_HW (3)
#define BOARD_HW (NGR_RELAY_HW)

//#define MAC_ADDR_LOCATION (0x7FFF0)  /*@TODO if comm not working verify this is the address location of the stored mac */
/* MAC address configuration. */
#define configMAC_ADDR                     \
    {                                      \
        0x02, 0x12, 0x13, 0x10, 0x15, 0x11 \
    }
#endif /* NGRMRELAY_H_ */
