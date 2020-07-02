/*
 *            Copyright (c) 2015-2017 by Real Time Automation, Inc.
 *
 *  This software is copyrighted by and is the sole property of
 *  Real Time Automation, Inc. (RTA).  All rights, title, ownership,
 *  or other interests in the software remain the property of RTA.
 *  This software may only be used in accordance with the corresponding
 *  license agreement.  Any unauthorized use, duplication, transmission,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MAY NOT be removed or modified without prior
 *  written consent of RTA.
 *
 *  RTA reserves the right to modify this software without notice.
 *
 *  www.rtaautomation.com
 */

#ifndef __EIP_STRUCTS_H__
#define __EIP_STRUCTS_H__

#define EIP_ENCAP_HEADER_ENCODED_SIZE 24

typedef struct
{
    CIP_UINT    Command;
    CIP_UINT    Length;
    CIP_UDINT   SessionHandle;
    CIP_UDINT   Status;
    CIP_OCTET   SenderContext[8];
    CIP_UDINT   Options;    
}EIP_Encap_Header;

typedef struct
{
    CIP_INT     sin_family;
    CIP_UINT    sin_port;
    CIP_UDINT   sin_addr;
    CIP_USINT   sin_zero[8];
}EIP_SocketAddress; /* Encoded as Big Endian */

/* CIP Volume 2 (EtherNet/IP) - 2-6.2.1 */
#define EIP_ItemID_NullAddress 0x0000   /* no data */
#define EIP_ItemLength_Max_NullAddress  0
#define EIP_Item_NullAddress void

/* CIP Volume 2 (EtherNet/IP) - 2-6.2.2 */
#define EIP_ItemID_ConnectedAddress 0x00A1
#define EIP_ItemLength_Max_ConnectedAddress  4
typedef struct
{
    CIP_UDINT ConnectionIdentifier;
}EIP_Item_ConnectedAddress;

/* CIP Volume 2 (EtherNet/IP) - 2-6.2.3 */
#define EIP_ItemID_SequencedAddress 0x8002
#define EIP_ItemLength_Expected_SequencedAddress 8
typedef struct
{
    CIP_UDINT ConnectionIdentifier;
    CIP_UDINT EncapsulationSequenceNumber;
}EIP_Item_SequencedAddress;

/* CIP Volume 2 (EtherNet/IP) - 2-6.3.1 */
#define EIP_ItemID_UnconnectedData 0x00B2
#define EIP_ItemLength_Max_UnconnectedData  504

typedef struct
{
    RTA_BOOL    is_request;
    union
    {
        CIP_MessageRouterRequest    Request;
        CIP_MessageRouterResponse   Response;
    }MessageRouter;
}EIP_Item_UnconnectedData;

/* CIP Volume 2 (EtherNet/IP) - 2-6.3.2 */
#define EIP_ItemID_ConnectedData 0x00B1
#define EIP_ItemLength_Max_ConnectedData  504
typedef struct
{
    RTA_BOOL    is_request;
    CIP_UINT    sequence_number;
    union
    {
        CIP_MessageRouterRequest    Request;
        CIP_MessageRouterResponse   Response;
    }MessageRouter;
}EIP_Item_ConnectedData;

/* CIP Volume 2 (EtherNet/IP) - 2-6.3.3 (Big Endian Encoding) */
#define EIP_ItemID_Sockaddr_Info_O2T 0x8000
#define EIP_ItemID_Sockaddr_Info_T2O 0x8001
typedef struct
{
    EIP_SocketAddress SocketAddress;
}EIP_Item_Sockaddr_Info;

#define EIP_ItemID_CIP_Identity 0x000C
typedef struct
{
    CIP_UINT            EncapProtocolVer;
    EIP_SocketAddress   SocketAddress;      
    CIP_IdentityObject  IdentityObject;
}EIP_Item_CIP_Identity;

#define EIP_ItemID_ListService 0x0100

#define EIP_ListService_Capability_Encap_CIP    0x0020  /* Bit 5 - If the device supports EtherNet/IP encapsulation of CIP this bit shall be set (=1); otherwise, it shall be clear (=0) */
#define EIP_ListService_Capability_UDP_IO       0x0100  /* Bit 8 - Supports CIP transport class 0 or 1 UDP-based connections */
#define EIP_ListService_CapabilityFlags         (EIP_ListService_Capability_Encap_CIP | EIP_ListService_Capability_UDP_IO)
typedef struct
{
    CIP_UINT            EncapProtocolVer;
    CIP_UINT            CapabilityFlags;
    CIP_USINT           NameOfService[16];  /* The Name field shall allow up to a 16-byte, NULL-terminated ASCII string for descriptive purposes only. The 16-byte limit shall include the NULL character.*/
}EIP_Item_ListService;

typedef struct
{
    CIP_UINT            ItemID;
    CIP_UINT            ItemLength; 

    /* we know which message to use from the ItemID */
    union
    {
        CIP_Opaque_String           * p_Item_Unknown;
        EIP_Item_NullAddress        * p_Item_NullAddress;
        EIP_Item_ConnectedAddress   * p_Item_ConnectedAddress;
        EIP_Item_SequencedAddress   * p_Item_SequencedAddress;
        EIP_Item_UnconnectedData    * p_Item_UnconnectedData;
        EIP_Item_ConnectedData      * p_Item_ConnectedData;
        EIP_Item_Sockaddr_Info      * p_Item_Sockaddr_Info;
        EIP_Item_CIP_Identity       * p_Item_CIP_Identity;
        EIP_Item_ListService        * p_Item_ListService; 
    }ItemData;
}EIP_Item_Explicit;

#define EIP_Max_Num_EIP_Item_Array_Elements 4
typedef struct
{
    CIP_UINT    ItemCount;
    EIP_Item_Explicit    ItemData[EIP_Max_Num_EIP_Item_Array_Elements];
}EIP_Item_Array_Explicit;

typedef struct
{
    CIP_UINT            ItemID;
    CIP_UINT            ItemLength; 

    /* we know which message to use from the ItemID */
    union
    {        
        CIP_Opaque_String           * p_Item_Unknown;
        EIP_Item_SequencedAddress   * p_Item_SequencedAddress;
        CIP_Opaque_String           * p_Item_ConnectedData_Opaque_String;
    }ItemData;
}EIP_Item_Implicit;

#define EIP_Max_Num_EIP_Item_Array_Implicit_Elements 2
typedef struct
{
    CIP_UINT            ItemCount;
    EIP_Item_Implicit   ItemData[EIP_Max_Num_EIP_Item_Array_Implicit_Elements];
}EIP_Item_Array_Implicit;

/* ************************************** */
/* CIP Volume 2 (EtherNet/IP) - 2-4.1 NOP */
/* ************************************** */
#define EIP_Command_NOP 0x0000  /* no request data; no response sent */

/* *********************************************** */
/* CIP Volume 2 (EtherNet/IP) - 2-4.6 ListServices */
/* *********************************************** */
#define EIP_Command_ListServices 0x0004

/* no data is required... keep this for symmetry */
#define EIP_Encap_ListServices_Request void

typedef struct
{
    EIP_Item_Array_Explicit Item_Array;
}EIP_Encap_ListServices_Response;

/* *********************************************** */
/* CIP Volume 2 (EtherNet/IP) - 2-4.2 ListIdentity */
/* *********************************************** */
#define EIP_Command_ListIdentity 0x0063

/* no data is required... keep this for symmetry */
#define EIP_Encap_ListIdentity_Request void

typedef struct
{
    EIP_Item_Array_Explicit Item_Array;
}EIP_Encap_ListIdentity_Response;

/* ************************************************** */
/* CIP Volume 2 (EtherNet/IP) - 2-4.4 RegisterSession */
/* ************************************************** */
#define EIP_Command_RegisterSession 0x0065

typedef struct
{
    CIP_UINT    EncapProtocolVer;
    CIP_UINT    OptionsFlags;
}EIP_Encap_RegisterSession_Request;

typedef struct
{
    CIP_UINT    EncapProtocolVer;
    CIP_UINT    OptionsFlags;
}EIP_Encap_RegisterSession_Response;

/* **************************************************** */
/* CIP Volume 2 (EtherNet/IP) - 2-4.5 UnRegisterSession */
/* **************************************************** */
#define EIP_Command_UnRegisterSession 0x0066

/* no data is required... keep this for symmetry */
#define EIP_Encap_UnRegisterSession_Request void
#define EIP_Encap_UnRegisterSession_Response void

/* ********************************************* */
/* CIP Volume 2 (EtherNet/IP) - 2-4.7 SendRRData */
/* ********************************************* */
#define EIP_Command_SendRRData 0x006F

typedef struct
{
    CIP_UDINT       InterfaceHandle;
    CIP_UINT        Timeout;
    EIP_Item_Array_Explicit  EncapsulatedPacket;
}EIP_Encap_SendRRData_Request;

typedef struct
{
    CIP_UDINT       InterfaceHandle;
    CIP_UINT        Timeout;
    EIP_Item_Array_Explicit  EncapsulatedPacket;
}EIP_Encap_SendRRData_Response;

/* *********************************************** */
/* CIP Volume 2 (EtherNet/IP) - 2-4.8 SendUnitData */
/* *********************************************** */
#define EIP_Command_SendUnitData 0x0070

typedef struct
{
    CIP_UDINT       InterfaceHandle;
    CIP_UINT        Timeout;
    EIP_Item_Array_Explicit  EncapsulatedPacket;
}EIP_Encap_SendUnitData_Request;

typedef struct
{
    CIP_UDINT       InterfaceHandle;
    CIP_UINT        Timeout;
    EIP_Item_Array_Explicit  EncapsulatedPacket;
}EIP_Encap_SendUnitData_Response;

/* *************************************************** */
/* CIP Volume 2 (EtherNet/IP) - Encapsulation Messages */
/* *************************************************** */
typedef struct
{
    /* all messages have a header, so no conditional malloc is needed */
    EIP_Encap_Header Header;

    /* we know which message to use from the header */
    union
    {
        EIP_Encap_ListServices_Request * p_ListServices;
        EIP_Encap_ListIdentity_Request * p_ListIdentity;
        EIP_Encap_RegisterSession_Request * p_RegisterSession;
        EIP_Encap_UnRegisterSession_Request * p_UnRegisterSession;
        EIP_Encap_SendRRData_Request * p_SendRRData;
        EIP_Encap_SendUnitData_Request * p_SendUnitData;
    }Request;
    union
    {
        EIP_Encap_ListServices_Response * p_ListServices;
        EIP_Encap_ListIdentity_Response * p_ListIdentity;
        EIP_Encap_RegisterSession_Response * p_RegisterSession;
        EIP_Encap_UnRegisterSession_Response * p_UnRegisterSession;
        EIP_Encap_SendRRData_Response * p_SendRRData;
        EIP_Encap_SendUnitData_Response * p_SendUnitData;
    }Response;
}EIP_Message;

typedef struct
{
    CIP_ControlStruct   cip_ctrl;   
    RTA_Usersock_Socket const * p_socket;
    EIP_Item_Array_Explicit * p_Response_ItemArray;
    EIP_Item_Array_Explicit * p_Request_ItemArray;

    EIP_Encap_Header    * p_Encap_Header;
    RTA_U8 * p_EIP_Encap_Header_Length_EIP_UINT;
    RTA_U8 * p_EIP_Encap_data_start;
}EIP_ControlStruct;

#endif /* __EIP_STRUCTS_H__ */
