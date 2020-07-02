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

#ifndef __CIP_STRUCTS_H__
#define __CIP_STRUCTS_H__

/* CIP Volume 1 (Common Industrial Protocol) - C-2.1.1 / Table C-2.1 Elementary Data Types */
typedef RTA_BOOL CIP_BOOL;
typedef RTA_I8 CIP_SINT;
typedef RTA_I16 CIP_INT;
typedef RTA_I32 CIP_DINT;
typedef RTA_I64 CIP_LINT;
typedef RTA_U8  CIP_USINT;
typedef RTA_U16 CIP_UINT;
typedef RTA_U32 CIP_UDINT;
typedef RTA_U64 CIP_ULINT;
typedef RTA_FLOAT CIP_REAL;
typedef RTA_DOUBLE CIP_LREAL;
typedef RTA_U8  CIP_BYTE;
typedef RTA_U16 CIP_WORD;
typedef RTA_U32 CIP_DWORD;
typedef RTA_U64 CIP_LWORD;

/* Octet is used throughout the spec, but not defined. We treat an octet as 8-bit unsigned */
typedef RTA_U8 CIP_OCTET;


typedef struct
{
    CIP_UINT    Length;
    CIP_OCTET * pData;
}CIP_String_Variable;
typedef CIP_String_Variable CIP_STRING;
typedef CIP_String_Variable CIP_Opaque_String;

typedef struct
{
    CIP_USINT   Length;
    CIP_OCTET   Data[32];
}CIP_String_Short_32;
typedef CIP_String_Short_32 CIP_SHORT_STRING_32;

typedef struct
{
    CIP_USINT   Length;
    CIP_OCTET   Data[64];
}CIP_String_Short_64;
typedef CIP_String_Short_64 CIP_SHORT_STRING_64;

/* CIP Volume 1 (Common Industrial Protocol) - C-5.2.5 Time Encodings */
typedef struct
{
    CIP_UDINT	Time;
    CIP_UINT	Date;
}CIP_DATE_AND_TIME;

/* CIP Volume 1 (Common Industrial Protocol) - 4-4.1 Class Attributes */
#define CIP_InstanceID_Class_Level  0x00    

/* CIP Volume 1 (Common Industrial Protocol) - Table 5A-1.1 Specifications in the CIP Object Library */
/* 0x00 is unused */
#define CIP_Class_Code_Identity                         0x01
#define CIP_Class_Code_Message_Router                   0x02
#define CIP_Class_Code_DeviceNet                        0x03
#define CIP_Class_Code_Assembly                         0x04
#define CIP_Class_Code_Connection                       0x05
#define CIP_Class_Code_Connection_Manager               0x06
#define CIP_Class_Code_Register                         0x07
#define CIP_Class_Code_Discrete_Input_Point             0x08
#define CIP_Class_Code_Discrete_Output_Point            0x09
#define CIP_Class_Code_Analog_Input_Point               0x0A
#define CIP_Class_Code_Analog_Output_Point              0x0B
/* 0x0C - 0x0D are unused */
#define CIP_Class_Code_Presence_Sensing                 0x0E
#define CIP_Class_Code_Parameter                        0x0F
#define CIP_Class_Code_Parameter_Group                  0x10
/* 0x11 is unused */
#define CIP_Class_Code_Group                            0x12
/* 0x13 - 0x1C are unused */
#define CIP_Class_Code_Discrete_Input_Group             0x1D
#define CIP_Class_Code_Discrete_Output_Group            0x1E
#define CIP_Class_Code_Discrete_Group                   0x1F
#define CIP_Class_Code_Analog_Input_Group               0x20
#define CIP_Class_Code_Analog_Output_Group              0x21
#define CIP_Class_Code_Analog_Group                     0x22
#define CIP_Class_Code_Position_Sensor                  0x23
#define CIP_Class_Code_Position_Controller_Supervisor   0x24
#define CIP_Class_Code_Position_Controller              0x25
#define CIP_Class_Code_Block_Sequencer                  0x26
#define CIP_Class_Code_Command_Block                    0x27
#define CIP_Class_Code_Motor_Data                       0x28
#define CIP_Class_Code_Control_Supervisor               0x29
#define CIP_Class_Code_AC_DC_Drive                      0x2A
#define CIP_Class_Code_Acknowledge_Handler              0x2B
#define CIP_Class_Code_Overload                         0x2C
#define CIP_Class_Code_Softstart                        0x2D
#define CIP_Class_Code_Selection                        0x2E
/* 0x2F is unused */
#define CIP_Class_Code_S_Device_Supervisor              0x30
#define CIP_Class_Code_S_Analog_Sensor                  0x31
#define CIP_Class_Code_S_Analog_Actuator                0x32
#define CIP_Class_Code_S_Single_Stage_Controller        0x33
#define CIP_Class_Code_S_Gas_Calibration                0x34
#define CIP_Class_Code_Trip_Point                       0x35
#define CIP_Class_Code_Drive_Data                       0x36    /* reserved */
#define CIP_Class_Code_File                             0x37
#define CIP_Class_Code_S_Partial_Pressure               0x38
#define CIP_Class_Code_Safety_Supervisor                0x39
#define CIP_Class_Code_Safety_Validator                 0x3A
#define CIP_Class_Code_Safety_Discrete_Output_Point     0x3B
#define CIP_Class_Code_Safety_Discrete_Output_Group     0x3C
#define CIP_Class_Code_Safety_Discrete_Input_Point      0x3D
#define CIP_Class_Code_Safety_Discrete_Input_Group      0x3E
#define CIP_Class_Code_Safety_Dual_Channel_Output       0x3F
#define CIP_Class_Code_S_Sensor_Calibration             0x40
#define CIP_Class_Code_Event_Log                        0x41
#define CIP_Class_Code_Motion_Device_Axis               0x42
#define CIP_Class_Code_Time_Sync                        0x43
#define CIP_Class_Code_Modbus                           0x44
#define CIP_Class_Code_Originator_Connection_List       0x45
#define CIP_Class_Code_Modbus_Serial_Link               0x46
#define CIP_Class_Code_Device_Level_Ring                0x47
#define CIP_Class_Code_QoS                              0x48
#define CIP_Class_Code_Safety_Analog_Input_Point        0x49
#define CIP_Class_Code_Safety_Analog_Input_Group        0x4A
#define CIP_Class_Code_Safety_Dual_Channel_Analog_Input 0x4B 
#define CIP_Class_Code_SERCOS_III_Link                  0x4C
#define CIP_Class_Code_Target_Connection_List           0x4D
#define CIP_Class_Code_Base_Energy                      0x4E
#define CIP_Class_Code_Electrical_Energy                0x4F
#define CIP_Class_Code_Non_Electrical_Energy            0x50
#define CIP_Class_Code_Base_Switch                      0x51
#define CIP_Class_Code_SNMP                             0x52
#define CIP_Class_Code_Power_Management                 0x53
#define CIP_Class_Code_RSTP_Bridge                      0x54
#define CIP_Class_Code_RSTP_Port                        0x55
#define CIP_Class_Code_Parallel_Redundancy_Protocol     0x56
#define CIP_Class_Code_PRP_Nodes_Table                  0x57
#define CIP_Class_Code_Safety_Feedback                  0x58
#define CIP_Class_Code_Safety_Dual_Channel_Feedback     0x59
#define CIP_Class_Code_Safety_Stop_Functions            0x5A
#define CIP_Class_Code_Safety_Limit_Functions           0x5B
#define CIP_Class_Code_Power_Curtailment                0x5C
#define CIP_Class_Code_CIP_Security                     0x5D
#define CIP_Class_Code_EtherNet_IP_Security             0x5E
#define CIP_Class_Code_Certificate_Management           0x5F
/* 0x60 - 0x63 are unused */
/* 0x64 - 0xC7 are vendor specific (100 - 199) */
#define CIP_Class_Code_VendorSpecific_FirstValid        0x64
#define CIP_Class_Code_VendorSpecific_LastValid         0xC7

/* 0xC8 - 0xEF are unused */
#define CIP_Class_Code_ControlNet                       0xF0
#define CIP_Class_Code_ControlNet_Keeper                0xF1
#define CIP_Class_Code_ControlNet_Scheduling            0xF2
#define CIP_Class_Code_Connection_Configuration         0xF3
#define CIP_Class_Code_Port                             0xF4
#define CIP_Class_Code_TCP_IP_Interface                 0xF5
#define CIP_Class_Code_Ethernet_Link                    0xF6
#define CIP_Class_Code_CompoNet_Link                    0xF7
#define CIP_Class_Code_CompoNet_Repeater                0xF8
/* 0xF9 - 0xFF are unused */

/* CIP Volume 1 (Common Industrial Protocol) - Table A-3.1 CIP Service Codes and Names */
/* 0x00 is reserved */
#define CIP_Service_Get_Attributes_All          0x01
#define CIP_Service_Set_Attributes_All          0x02
#define CIP_Service_Get_Attribute_List          0x03
#define CIP_Service_Set_Attribute_List          0x04
#define CIP_Service_Reset                       0x05
#define CIP_Service_Start                       0x06
#define CIP_Service_Stop                        0x07
#define CIP_Service_Create                      0x08
#define CIP_Service_Delete                      0x09
#define CIP_Service_Multiple_Service_Packet     0x0A
/* 0x0b - 0x0C are reserved */
#define CIP_Service_Apply_Attributes            0x0D
#define CIP_Service_Get_Attribute_Single        0x0E
/* 0x0F is reserved */
#define CIP_Service_Set_Attribute_Single        0x10
#define CIP_Service_Find_Next_Object_Instance   0x11
/* 0x12 - 0x13 are reserved */
#define CIP_Service_Error_Response              0x14    /* DeviceNet Only */  
#define CIP_Service_Restore                     0x15
#define CIP_Service_Save                        0x16
#define CIP_Service_No Operation                0x17  
#define CIP_Service_Get_Member                  0x18
#define CIP_Service_Set_Member                  0x19
#define CIP_Service_Insert_Member               0x1A
#define CIP_Service_Remove_Member               0x1B
#define CIP_Service_GroupSync                   0x1C
/* 0x1D - 0x31 are reserved */

/* CIP Volume 1 (Common Industrial Protocol) - Table 4-9.6 Service Code Ranges */
/* 0x32 - 0x4A are vendor specific */
/* 0x4B - 0x63 are object class specific */
/* 0x64 - 0x7F are reserved */
/* 0x80 - 0xFF are used as Reply Service Code (0x80 + service code) */

/* CIP Volume 1 (Common Industrial Protocol) - C-1.4 Segment Type */
#define CIP_SegmentType_Port                    0
#define CIP_SegmentType_Logical                 1
#define CIP_SegmentType_Network                 2
#define CIP_SegmentType_Symbolic                3
#define CIP_SegmentType_Data                    4
#define CIP_SegmentType_DataType_Constructed    5
#define CIP_SegmentType_DataType_Elementary     6
#define CIP_SegmentType_Reserved                7
#define CIP_SegmentType_get_type_from_typeformat(a) ((a >> 5) & 0x07)
#define CIP_SegmentType_Unknown                 0xFF    /* we need a way to decode paths we don't understand, but catch it on process */

/* CIP Volume 1 (Common Industrial Protocol) - C-1.4.2 Logical Segment */
#define CIP_LogicalSegment_LogicalType_ClassID         0
#define CIP_LogicalSegment_LogicalType_InstanceID      1
#define CIP_LogicalSegment_LogicalType_MemberID        2
#define CIP_LogicalSegment_LogicalType_ConnectionPoint 3
#define CIP_LogicalSegment_LogicalType_AttributeID     4
#define CIP_LogicalSegment_LogicalType_Special         5
#define CIP_LogicalSegment_LogicalType_ServiceID       6
#define CIP_LogicalSegment_LogicalType_ExtendedLogical 7
#define CIP_LogicalSegment_get_LogicalType_from_typeformat(a) ((a >> 2) & 0x07)

#define CIP_LogicalSegment_LogicalFormat_8Bit      0
#define CIP_LogicalSegment_LogicalFormat_16Bit     1
#define CIP_LogicalSegment_LogicalFormat_32Bit     2
#define CIP_LogicalSegment_LogicalFormat_Reserved  3
#define CIP_LogicalSegment_get_LogicalFormat_from_typeformat(a) (a & 0x03)

#define CIP_LogicalSegment_SpecialFormat_ElectronicKey    0
#define CIP_LogicalSegment_get_SpecialFormat_from_typeformat(a) (a & 0x03)

/* CIP Volume 1 (Common Industrial Protocol) - Table C-1.4 Electronic Key Segment Format */
#define CIP_LogicalSegment_SpecialFormat_KeyFormat_ElectronicKey 4

typedef struct
{
    CIP_USINT   LogicalFormat;
    CIP_UDINT   Value; 
}CIP_LogicalSegment_Normal;
typedef CIP_LogicalSegment_Normal CIP_LogicalSegment_ClassID;
typedef CIP_LogicalSegment_Normal CIP_LogicalSegment_InstanceID;
typedef CIP_LogicalSegment_Normal CIP_LogicalSegment_MemberID;
typedef CIP_LogicalSegment_Normal CIP_LogicalSegment_ConnectionPoint;
typedef CIP_LogicalSegment_Normal CIP_LogicalSegment_AttributeID;

/* CIP Volume 1 (Common Industrial Protocol) - Table C-1.5 Electronic Key Segment Format */
typedef struct
{
    CIP_UINT    VendorID;
    CIP_UINT    DeviceType; 
    CIP_UINT    ProductCode;
    CIP_BOOL    Compatibility;
    CIP_BYTE    MajorRevision;
    CIP_USINT   MinorRevision;
}CIP_ElectronicKey;

/* CIP Volume 1 (Common Industrial Protocol) - Table C-1.5 Key Format Table */
typedef struct
{
    CIP_USINT   SpecialFormat;
    CIP_USINT   KeyFormat;

    union
    {
        CIP_ElectronicKey ElectronicKey;
    }Value;
}CIP_LogicalSegment_Special;

#define CIP_LogicalSegment_ServiceID void
#define CIP_LogicalSegment_ExtendedLogical void

typedef struct
{
    CIP_USINT   LogicalType;
    union
    {
        CIP_LogicalSegment_ClassID ClassID;
        CIP_LogicalSegment_InstanceID InstanceID;
        CIP_LogicalSegment_MemberID MemberID;
        CIP_LogicalSegment_ConnectionPoint ConnectionPoint;
        CIP_LogicalSegment_AttributeID AttributeID;
        CIP_LogicalSegment_Special * p_Special;
        CIP_LogicalSegment_ServiceID * p_ServiceID;
        CIP_LogicalSegment_ExtendedLogical * p_ExtendedLogical;
    }Value;
}CIP_LogicalSegment;

/* CIP Volume 1 (Common Industrial Protocol) - C-1.4.5.1 Simple Data Segment */
#define CIP_PortSegment_PortType_SimpleDataSegment              0
#define CIP_PortSegment_PortType_ANSI_ExtendedSymbolSegment     17
#define CIP_PortSegment_get_PortType_from_typeformat(a) (a & 0x1F)
typedef struct
{
    CIP_USINT   Data_Size_In_Words;
    CIP_WORD    *p_WordData;
}CIP_DataSegment_SimpleDataSegment;
typedef struct
{
    CIP_USINT   DataType;
    union
    {
        CIP_DataSegment_SimpleDataSegment SimpleDataSegment;
    }Value;
}CIP_DataSegment;
typedef struct
{
    CIP_USINT   SegmentType;
    union
    {
        CIP_LogicalSegment  * p_LogicalSegment;
        CIP_DataSegment     * p_DataSegment;
    }Segment;
}CIP_Path_Element;

/* TODO: consider a linked list / pre-calculate # elements (2 pass) 
            If this is changed to a pointer, deploy a deep copy where used */
#define CIP_MAX_PATH_ELEMENTS 16 /* can't exceed 255 */
typedef struct
{
    CIP_USINT           Path_Element_Count;
    CIP_Path_Element    Path_Elements[CIP_MAX_PATH_ELEMENTS];
}CIP_Path_Array;

/* CIP Volume 1 (Common Industrial Protocol) - 2-4.1 */
typedef struct
{
    CIP_USINT   NumberofStatuses;
    CIP_WORD    *p_Status;
}CIP_AdditionalStatus;

/* CIP Volume 1 (Common Industrial Protocol) - 3-5.4.1.7 Connection Triad */
typedef struct
{
    CIP_UINT    ConnectionSerialNumber;
    CIP_UINT    OriginatorVendorID;
    CIP_UDINT   OriginatorSerialNumber;
}CIP_Connection_Triad;

/* CIP Volume 1 (Common Industrial Protocol) - Table 3-5.17 Forward_Open Request */
typedef struct
{
    CIP_UDINT   NetworkConnectionID;
    CIP_UDINT   RPI_in_microseconds;
    CIP_WORD    NetworkConnectionParameters;
}CIP_ForwardOpen_Request_PerDirection;

typedef struct
{
    CIP_BYTE    Priority_TimeTick;
    CIP_USINT   Timeout_Ticks;
    CIP_Connection_Triad        Connection_Triad;
    CIP_USINT   ConnectionTimeoutMultiplier;
    CIP_BYTE    TransportClass_and_Trigger;
    CIP_ForwardOpen_Request_PerDirection   O2T;
    CIP_ForwardOpen_Request_PerDirection   T2O;
    CIP_Path_Array  Connection_Path;
}CIP_ForwardOpen_Request;

/* CIP Volume 1 (Common Industrial Protocol) - Table 3-5.19 Successful Forward_Open */
typedef struct
{
    CIP_UDINT   NetworkConnectionID;
    CIP_UDINT   API_in_microseconds;
}CIP_ForwardOpen_Response_PerDirection;

typedef struct
{
    CIP_USINT   Reply_Size_In_Words;
    CIP_BYTE    *p_Reply_Bytes;
}CIP_Application_Reply;
typedef CIP_Application_Reply CIP_ForwardOpen_Application_Reply;
typedef CIP_Application_Reply CIP_ForwardClose_Application_Reply;

typedef struct
{
    CIP_ForwardOpen_Response_PerDirection    O2T;
    CIP_ForwardOpen_Response_PerDirection    T2O;
    CIP_Connection_Triad                    Connection_Triad;
    CIP_ForwardOpen_Application_Reply       Application;
}CIP_ForwardOpen_Response_Success;

typedef struct
{
    CIP_Connection_Triad    Connection_Triad;
    /* Remaining Path Size is only present if error is detected by a router, which we aren't */
}CIP_ForwardOpen_Response_Unsuccessful;

typedef struct
{
    CIP_BYTE    Priority_TimeTick;
    CIP_USINT   Timeout_Ticks;
    CIP_Connection_Triad        Connection_Triad;
    CIP_Path_Array  Connection_Path;
}CIP_ForwardClose_Request;

typedef struct
{
    CIP_Connection_Triad                    Connection_Triad;
    CIP_ForwardOpen_Application_Reply       Application;
}CIP_ForwardClose_Response_Success;

typedef struct
{
    CIP_Connection_Triad    Connection_Triad;
    /* Remaining Path Size is only present if error is detected by a router, which we aren't */
}CIP_ForwardClose_Response_Unsuccessful;

/* ************************************************************ */
/* CIP Volume 1 - 5A-2.2 Instance Attributes (Identity Object ) */
/* ************************************************************ */
typedef struct
{
    CIP_USINT   Major;
    CIP_USINT   Minor;
}CIP_IdentityObject_Revision;

typedef struct
{
    CIP_UINT                    VendorID;
    CIP_UINT                    DeviceType;
    CIP_UINT                    ProductCode;
    CIP_IdentityObject_Revision Revision;
    CIP_WORD                    Status;
    CIP_UDINT                   SerialNumber;
    CIP_SHORT_STRING_32         ProductName;
    CIP_USINT                   State;
}CIP_IdentityObject;

/* ********************************************************* */
/* CIP Volume 2 - 5-4.3 Instance Attributes (TCP/IP Object ) */
/* ********************************************************* */

/* CIP Volume 2 - Table 5-4.4 Status Attribute */
typedef enum
{
    InterfaceConfigurationStatus_NotConfigured,
    InterfaceConfigurationStatus_BootpDhcpNvram,
    InterfaceConfigurationStatus_Hardware,
    InterfaceConfigurationStatus_Unknown
}EIP_TCPObject_Status_InterfaceConfigurationStatus;

/* CIP Volume 2 - 5-4.3.2.1 Status – Attribute 1 */
typedef struct
{
    EIP_TCPObject_Status_InterfaceConfigurationStatus InterfaceConfigurationStatus;
    CIP_BOOL McastChangePending;
    CIP_BOOL InterfaceConfigurationChangePending;
    CIP_BOOL AcdStatus_ConflictDetected;
    CIP_BOOL AcdFault_ConflictDetected;
    CIP_BOOL IANA_Port_Admin_Change_Pending;
    CIP_BOOL IANA_Protocol_Admin_Change_Pending;
}EIP_TCPObject_Status;

/* CIP Volume 2 - 5-4.3.2.2 Configuration Capability – Attribute 2 */
/* CIP Volume 2 - Table 5-4.5 Configuration Capability Attribute */
typedef struct
{
    CIP_BOOL BOOTP_Client;
    CIP_BOOL DNS_Client;
    CIP_BOOL DHCP_Client;
    CIP_BOOL DHCP_DNS_Update;
    CIP_BOOL ConfigurationSettable;
    CIP_BOOL HardwareConfigurable;
    CIP_BOOL InterfaceConfigurationChangeRequiresReset;
    CIP_BOOL ACDCapable;
}EIP_TCPObject_ConfigurationCapability;

/* CIP Volume 2 - Table 5-4.6 Configuration Control Attribute */
typedef enum
{
    ConfigurationControl_ConfigurationMethod_Static,
    ConfigurationControl_ConfigurationMethod_BOOTP,
    ConfigurationControl_ConfigurationMethod_DHCP,
    ConfigurationControl_ConfigurationMethod_Unknown    /* since this is settable, we need to decode unknown options */
}EIP_TCPObject_ConfigurationControl_ConfigurationMethod;

/* CIP Volume 2 - 5-4.3.2.3 Configuration Control – Attribute 3 */
typedef struct
{
    EIP_TCPObject_ConfigurationControl_ConfigurationMethod ConfigurationMethod;
    CIP_BOOL DNS_Enable;
}EIP_TCPObject_ConfigurationControl;

/* CIP Volume 2 - 5-4.3.2.4 Physical Link Object – Attribute 4 */
typedef struct
{
    CIP_UDINT   class_id;
    CIP_UDINT   instance_id;
}EIP_TCPObject_PhysicalLinkObject;

/* CIP Volume 2 - 5-4.3.2.5 Interface Configuration – Attribute 5 */
/* CIP Volume 2 - 5-4.3.2.3.1 Configuration Control Structure */
typedef struct
{
    CIP_UDINT   IP_Address;
    CIP_UDINT   Network_Mask;
    CIP_UDINT   Gateway_Address;
    CIP_UDINT   Name_Server;
    CIP_UDINT   Name_Server2;
    CIP_STRING  Domain_Name;    /* Maximum length is 48 characters. Shall be padded to an even number of characters (pad not included in length).*/
}EIP_TCPObject_InterfaceConfiguration;

/* unused, but needed for GetAttributeAll (See CIP Safety Specfication, Volume 5, Chapter 3) */
typedef struct
{
    CIP_OCTET SafetyNetworkNumber[6];
}EIP_TCPObject_SafetyNetworkNumber;

/* CIP Volume 2 - Table 5-4.9 Alloc Control */
typedef enum
{
    McastConfig_AllocControl_DefaultAlgorithm,
    McastConfig_AllocControl_Use_McastConfig
}EIP_TCPObject_McastConfig_AllocControl;

/* CIP Volume 2 - 5-4.3.2.8 Mcast Config – Attribute 9 */
typedef struct
{
    EIP_TCPObject_McastConfig_AllocControl AllocControl;
    CIP_UINT    NumMcast;
    CIP_UDINT   McastStartAddr;
}EIP_TCPObject_McastConfig;

/* CIP Volume 2 - Table 5-4.10 AcdActivity */
typedef enum
{
    AcdActivity_NoConflictDetected,
    AcdActivity_ProbeIpv4Address,
    AcdActivity_OngoingDetection,
    AcdActivity_SemiActiveProbe
}EIP_TCPObject_LastConflictDetected_AcdActivity;

/* CIP Volume 2 - 5-4.3.2.10 LastConflictDetected – Attribute 11 */
typedef struct
{
    EIP_TCPObject_LastConflictDetected_AcdActivity AcdActivity;
    CIP_USINT   RemoteMAC[6];    
    CIP_USINT   ArpPdu[28];    
}EIP_TCPObject_LastConflictDetected;

typedef struct
{
    EIP_TCPObject_Status Status;
    EIP_TCPObject_ConfigurationCapability ConfigurationCapability;
    EIP_TCPObject_ConfigurationControl ConfigurationControl;
    EIP_TCPObject_PhysicalLinkObject PhysicalLinkObject;
    EIP_TCPObject_InterfaceConfiguration InterfaceConfiguration;
    CIP_STRING  HostName;
    EIP_TCPObject_SafetyNetworkNumber SafetyNetworkNumber;
    CIP_USINT   TTLValue;
    EIP_TCPObject_McastConfig McastConfig;
    CIP_BOOL    SelectAcd;
    EIP_TCPObject_LastConflictDetected LastConflictDetected;
    CIP_BOOL    EtherNetIP_QuickConnect_Enabled;
    CIP_UINT    EncapsulationInactivityTimeout;
}EIP_TCPObject;

/* **************************************************************** */
/* CIP Volume 2 - 5-5.3 Instance Attributes (Ethernet Link Object ) */
/* **************************************************************** */

/* CIP Volume 2 - Table 5-5.4 Interface Flags */
typedef enum
{
    NegotiationStatus_AutoNegotiate_in_progress,
    NegotiationStatus_AutoNegotiate_failed,
    NegotiationStatus_AutoNegotiate_failed_detected_speed,
    NegotiationStatus_AutoNegotiate_successful,
    NegotiationStatus_AutoNegotiate_not_attempted
}EIP_EthernetLinkObject_InterfaceFlags_NegotiationStatus;

typedef struct
{
    CIP_BOOL    Link_Status_Active;
    CIP_BOOL    Link_Full_Duplex;
    EIP_EthernetLinkObject_InterfaceFlags_NegotiationStatus Negotiation_Status;
    CIP_BOOL    ManualSettingRequiresReset;
    CIP_BOOL    Local_Hardware_Fault_Detected;
}EIP_EthernetLinkObject_InterfaceFlags_Internal;

typedef struct
{
    CIP_USINT   PhysicalAddress[6];   
}EIP_EthernetLinkObject_PhysicalAddress;

typedef struct
{
    CIP_UDINT   In_Octets;
    CIP_UDINT   In_Ucast_Packets;
    CIP_UDINT   In_NUcast_Packets;
    CIP_UDINT   In_Discards;
    CIP_UDINT   In_Errors;
    CIP_UDINT   In_Unknown_Protos;
    CIP_UDINT   Out_Octets;
    CIP_UDINT   Out_Ucast_Packets;
    CIP_UDINT   Out_NUcast_Packets;
    CIP_UDINT   Out_Discards;
    CIP_UDINT   Out_Errors;
}EIP_EthernetLinkObject_InterfaceCounters;

typedef struct
{
    CIP_UDINT   Alignment_Errors;
    CIP_UDINT   FCS_Errors;
    CIP_UDINT   Single_Collisions;
    CIP_UDINT   Multiple_Collisions;
    CIP_UDINT   SQE_Test_Errors;
    CIP_UDINT   Deferred_Transmissions;
    CIP_UDINT   Late_Collisions;
    CIP_UDINT   Excessive_Collisions;
    CIP_UDINT   MAC_Transmit;
    CIP_UDINT   Carrier_Sense_Errors;
    CIP_UDINT   Frame_Too_Long;
    CIP_UDINT   MAC_Receive_Errors;
}EIP_EthernetLinkObject_MediaCounters;

/* CIP Volume 2 - 5-5.3.2.6.1 Control Bits */
typedef struct
{
    CIP_BOOL    AutoNegotiate;
    CIP_BOOL    ForcedDupluxMode;
}EIP_EthernetLinkObject_ControlBits_Internal;

typedef struct
{
    EIP_EthernetLinkObject_ControlBits_Internal ControlBits;
    CIP_UINT    ForcedInterfaceSpeed;
}EIP_EthernetLinkObject_InterfaceControl_Internal;

/* CIP Volume 2 - Table 5-5.6 Interface Type */
typedef enum
{
    InterfaceType_Unknown,
    InterfaceType_Internal,
    InterfaceType_TwistedPair,
    InterfaceType_OpticalFiber
}EIP_EthernetLinkObject_InterfaceType_Enum;

/* CIP Volume 2 - 5-5.3.2.8 Interface State */
typedef enum
{
    InterfaceState_Unknown,
    InterfaceState_Enabled,
    InterfaceState_Disabled,
    InterfaceState_Testing
}EIP_EthernetLinkObject_InterfaceState_Enum;

/* CIP Volume 2 - 5-5.3.2.9 Admin State */
typedef enum
{
    AdminState_Enabled,
    AdminState_Disabled
}EIP_EthernetLinkObject_AdminState_Enum;

/* CIP Volume 2 - 5-5.3.2.11 Interface Capability */
typedef struct
{
    CIP_BOOL    ManualSettingRequiresReset;
    CIP_BOOL    AutoNegotiate_Supported;
    CIP_BOOL    AutoMDIX_Supported;
    CIP_BOOL    ManualSettings_Supported;
}EIP_EthernetLinkObject_InterfaceCapability_Bits_Internal;

typedef enum
{
    InterfaceDuplexMode_Half,
    InterfaceDuplexMode_Full
}EIP_EthernetLinkObject_InterfaceCapability_InterfaceDuplexModeMode_Enum;

typedef struct
{
    CIP_UINT    InterfaceSpeed;
    EIP_EthernetLinkObject_InterfaceCapability_InterfaceDuplexModeMode_Enum InterfaceDuplexMode;
}EIP_EthernetLinkObject_InterfaceCapability_SpeedDuplex;

typedef struct
{
    CIP_USINT   count;
    EIP_EthernetLinkObject_InterfaceCapability_SpeedDuplex  * p_value;
}EIP_EthernetLinkObject_InterfaceCapability_SpeedDuplex_Array;

typedef struct
{
    EIP_EthernetLinkObject_InterfaceCapability_Bits_Internal InterfaceCapability_Bits;
    EIP_EthernetLinkObject_InterfaceCapability_SpeedDuplex_Array SpeedDuplex_Array;
}EIP_EthernetLinkObject_InterfaceCapability;

typedef struct
{
    CIP_UDINT   InterfaceSpeed;
    EIP_EthernetLinkObject_InterfaceFlags_Internal  InterfaceFlags;
    EIP_EthernetLinkObject_PhysicalAddress PhysicalAddress;
    EIP_EthernetLinkObject_InterfaceCounters InterfaceCounters;
    EIP_EthernetLinkObject_MediaCounters MediaCounters;
    EIP_EthernetLinkObject_InterfaceControl_Internal InterfaceControl;
    EIP_EthernetLinkObject_InterfaceType_Enum InterfaceType;
    EIP_EthernetLinkObject_InterfaceState_Enum InterfaceState;
    EIP_EthernetLinkObject_AdminState_Enum AdminState;
    CIP_SHORT_STRING_64 InterfaceLabel;
    EIP_EthernetLinkObject_InterfaceCapability InterfaceCapability;
}CIP_EthernetLinkObject;

#if EIP_USER_OPTION_DLR_ENABLED
/* ************************************************************************* */
/* CIP Volume 2 - 5-6.3 Instance Attributes (Device Level Ring (DLR) Object) */
/* ************************************************************************* */

/*  TODO: SINCE THIS CODE IS FROM THE INNOVASIC STACK, WE ARE JUST USING THE BASE
    TYPES, NOT OUR NORMAL INTERNAL REPRESENTATION!!!! IF WE STOP USING INNOVASIC
    WE NEED TO DO THIS DIFFERENTLY */
typedef struct /* although it is called different things, the pair of IP and Ethernet MAC is used throughout this object */
{
    CIP_UDINT   IPAddress;
    CIP_USINT   MACAddress[6];
}CIP_DLRObject_Participant;

typedef struct
{
    CIP_USINT   NetworkTopology;
    CIP_USINT   NetworkStatus;
    CIP_DLRObject_Participant ActiveSupervisorAddress;  
    CIP_DWORD   CapabilityFlags;
}CIP_DLRObject;
#endif /* #if EIP_USER_OPTION_DLR_ENABLED */

/* ************************************************************************** */
/* CIP Volume 2 - 5-7.4 Instance Attributes (Quality of Service (QoS) Object) */
/* ************************************************************************** */
#if EIP_USER_OPTION_QOS_ENABLED
typedef struct
{
    CIP_USINT   DSCP_Urgent;
    CIP_USINT   DSCP_Scheduled;
    CIP_USINT   DSCP_High;
    CIP_USINT   DSCP_Low;
    CIP_USINT   DSCP_Explicit;
}CIP_QoSObject;
#endif /* EIP_USER_OPTION_QOS_ENABLED */

typedef enum 
{
    CIP_DATA_TYPE_NO_DATA,
    CIP_DATA_TYPE_CIP_BOOL,
    CIP_DATA_TYPE_CIP_SINT,
    CIP_DATA_TYPE_CIP_INT,
    CIP_DATA_TYPE_CIP_DINT,
    CIP_DATA_TYPE_CIP_LINT,
    CIP_DATA_TYPE_CIP_USINT,
    CIP_DATA_TYPE_CIP_UINT,
    CIP_DATA_TYPE_CIP_UDINT,
    CIP_DATA_TYPE_CIP_ULINT,
    CIP_DATA_TYPE_CIP_REAL,
    CIP_DATA_TYPE_CIP_LREAL,
    CIP_DATA_TYPE_CIP_BYTE,
    CIP_DATA_TYPE_CIP_WORD,
    CIP_DATA_TYPE_CIP_DWORD,
    CIP_DATA_TYPE_CIP_LWORD,
    CIP_DATA_TYPE_CIP_STRING,
    CIP_DATA_TYPE_CIP_Opaque_String,
    CIP_DATA_TYPE_CIP_SHORT_STRING_32,

    CIP_DATA_TYPE_CIP_DATE_AND_TIME,

    CIP_DATA_TYPE_CIP_IdentityObject_Revision,
    CIP_DATA_TYPE_CIP_IdentityObject,

    CIP_DATA_TYPE_CIP_ForwardOpen_Request,
    CIP_DATA_TYPE_CIP_ForwardOpen_Response_Success,
    CIP_DATA_TYPE_CIP_ForwardOpen_Response_Unsuccessful,

    CIP_DATA_TYPE_CIP_ForwardClose_Request,
    CIP_DATA_TYPE_CIP_ForwardClose_Response_Success,
    CIP_DATA_TYPE_CIP_ForwardClose_Response_Unsuccessful,

    CIP_DATA_TYPE_EIP_TCPObject_Status,
    CIP_DATA_TYPE_EIP_TCPObject_ConfigurationCapability,
    CIP_DATA_TYPE_EIP_TCPObject_ConfigurationControl,
    CIP_DATA_TYPE_EIP_TCPObject_PhysicalLinkObject,
    CIP_DATA_TYPE_EIP_TCPObject_InterfaceConfiguration,
    CIP_DATA_TYPE_CIP_STRING48_16BIT_PADDED,
    CIP_DATA_TYPE_CIP_STRING64_16BIT_PADDED,

    CIP_DATA_TYPE_EIP_EthernetLinkObject_InterfaceFlags_Internal,
    CIP_DATA_TYPE_EIP_EthernetLinkObject_PhysicalAddress,
    CIP_DATA_TYPE_EIP_EthernetLinkObject_InterfaceCounters,
    CIP_DATA_TYPE_EIP_EthernetLinkObject_MediaCounters,
    CIP_DATA_TYPE_EIP_EthernetLinkObject_InterfaceCapability
}CIP_DATA_TYPE;

/* since we always need to have enough room for the largest entry, and we can't have more than one data type per message
    we don't need to use pointers, we can allocate the largest entry */
typedef union
{   
    CIP_BOOL    value_BOOL;    
    CIP_SINT    value_SINT;    
    CIP_INT     value_INT;    
    CIP_DINT    value_DINT;    
    CIP_LINT    value_LINT;    
    CIP_USINT   value_USINT;    
    CIP_UINT    value_UINT;
    CIP_UDINT   value_UDINT;    
    CIP_ULINT   value_ULINT;        
    CIP_REAL    value_REAL;    
    CIP_LREAL   value_LREAL;    
    CIP_BYTE    value_BYTE;    
    CIP_WORD    value_WORD;    
    CIP_DWORD   value_DWORD;    
    CIP_LWORD   value_LWORD;    
    CIP_STRING  value_STRING; 

    CIP_Opaque_String   value_Opaque_String;
    CIP_SHORT_STRING_32 value_SHORT_STRING_32;

    CIP_DATE_AND_TIME value_CIP_DATE_AND_TIME;

    CIP_IdentityObject_Revision value_IdentityObject_Revision;
    CIP_IdentityObject value_IdentityObject;
    
    CIP_ForwardOpen_Request value_ForwardOpen_Request;
    CIP_ForwardOpen_Response_Success value_ForwardOpen_Response_Success;
    CIP_ForwardOpen_Response_Unsuccessful value_ForwardOpen_Response_Unsuccessful;

    CIP_ForwardClose_Request value_ForwardClose_Request;
    CIP_ForwardClose_Response_Success value_ForwardClose_Response_Success;
    CIP_ForwardClose_Response_Unsuccessful value_ForwardClose_Response_Unsuccessful;

    EIP_TCPObject_Status value_TCPObject_Status;
    EIP_TCPObject_ConfigurationCapability value_TCPObject_ConfigurationCapability;
    EIP_TCPObject_ConfigurationControl value_TCPObject_ConfigurationControl;
    EIP_TCPObject_PhysicalLinkObject value_TCPObject_PhysicalLinkObject;
    EIP_TCPObject_InterfaceConfiguration value_TCPObject_InterfaceConfiguration;    
    CIP_STRING  value_STRINGn_16BIT_PADDED; /* one data type for all string of variable length that are padded */

    EIP_EthernetLinkObject_InterfaceFlags_Internal value_EthernetLinkObject_InterfaceFlags_Internal;
    EIP_EthernetLinkObject_PhysicalAddress value_EthernetLinkObject_PhysicalAddress;
    EIP_EthernetLinkObject_InterfaceCounters value_EthernetLinkObject_InterfaceCounters;
    EIP_EthernetLinkObject_MediaCounters value_EthernetLinkObject_MediaCounters;
    EIP_EthernetLinkObject_InterfaceCapability value_EthernetLinkObject_InterfaceCapability;
}CIP_MessageRouter_Data_Value;

typedef struct
{
    CIP_DATA_TYPE                   Type;
    CIP_MessageRouter_Data_Value    Value;
}CIP_MessageRouter_Data;

typedef struct
{
    CIP_BOOL    path_valid;
    CIP_UDINT   class_id;
    CIP_BOOL    class_id_found;
    CIP_UDINT   instance_id;
    CIP_BOOL    instance_id_found;
    CIP_UDINT   attribute_id;
    CIP_BOOL    attribute_id_found;
    CIP_UDINT   member_id;
    CIP_BOOL    member_id_found;
}CIP_CIAM_Path;

typedef struct
{
    CIP_USINT               Service;
    CIP_Path_Array          Path;
    CIP_CIAM_Path           CIAM_Path;
    CIP_MessageRouter_Data  Request_Data;
}CIP_MessageRouterRequest;

/* CIP Volume 1 (Common Industrial Protocol) - 2-4.2 */
typedef struct
{
    CIP_USINT               Service;
    CIP_OCTET               Reserved;
    CIP_USINT               GeneralStatus;
    CIP_AdditionalStatus    AdditionalStatus;
    CIP_MessageRouter_Data  Response_Data;
}CIP_MessageRouterResponse;

typedef struct
{
    RTA_MEMORY_HEAP     * p_heap_src;    
    RTA_MEMORY_HEAP     * p_heap_dst;  

    /* we need a hook to do something on "send complete" like Identity Object reset */
    RTA_Usersock_ReturnCode (*p_fptr_call_on_send_complete) (RTA_Usersock_Socket const * const user_sock);
    
    /* we need a way to pass this down the rabbit hole so we can get our IP and other information later... */
    CIP_UDINT   EIP_SessionHandle;
    
    /* if our decode finds a CIP error, we need a way to report it */
    CIP_USINT   decode_GeneralStatus;
}CIP_ControlStruct;
#endif /* __CIP_STRUCTS_H__ */
