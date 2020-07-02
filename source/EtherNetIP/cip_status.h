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

#ifndef __CIP_STATUS_H__
#define __CIP_STATUS_H__

/* CIP Volume 1 (Common Industrial Protocol) - Table B-1.1 CIP General Status Codes */
#define CIP_Status_Success                              0x00    /* Service was successfully performed by the object specified. */
#define CIP_Status_Connection_failure                   0x01    /* A connection related service failed along the connection path. */
#define CIP_Status_Resource_unavailable                 0x02    /* Resources needed for the object to perform the requested service were unavailable */
#define CIP_Status_Invalid_parameter_value              0x03    /* See Status Code 0x20, which is the preferred value to use for this condition. */
#define CIP_Status_Path_segment_error                   0x04    /* The path segment identifier or the segment syntax was not understood by the processing node. 
                                                                    Path processing shall stop when a path segment error is encountered. */
#define CIP_Status_Path_destination_unknown             0x05    /* The path is referencing an object class, instance or structure element that is not known or 
                                                                    is not contained in the processing node. Path processing shall stop when a path destination 
                                                                    unknown error is encountered. */
#define CIP_Status_Partial_transfer                     0x06    /* Only part of the expected data was transferred. */
#define CIP_Status_Connection_lost                      0x07    /* The messaging connection was lost. */
#define CIP_Status_Service_not_supported                0x08    /* The requested service was not implemented or was not defined for this Object Class/Instance. */
#define CIP_Status_Invalid_attribute_value              0x09    /* Invalid attribute data detected */
#define CIP_Status_Attribute_list_error                 0x0A    /* An attribute in the Get_Attribute_List or Set_Attribute_List response has a non-zero status. */
#define CIP_Status_Already_in_requested_mode_state      0x0B    /* The object is already in the mode/state being requested by the service */
#define CIP_Status_Object_state_conflict                0x0C    /* The object cannot perform the requested service in its current mode/state */
#define CIP_Status_Object_already_exists                0x0D    /* The requested instance of object to be created already exists. */
#define CIP_Status_Attribute_not_settable               0x0E    /* A request to modify a nonmodifiable attribute was received. */
#define CIP_Status_Privilege_violation                  0x0F    /* A permission/privilege check failed */
#define CIP_Status_Device_state_conflict                0x10    /* The device’s current mode/state prohibits the execution of the requested service. */
#define CIP_Status_Reply_data_too_large                 0x11    /* The data to be transmitted in the response buffer is larger than the allocated response buffer */
#define CIP_Status_Frag_of_a_primitive_value            0x12    /* The service specified an operation that is going to fragment a primitive data value, i.e. half a REAL data type. */
#define CIP_Status_Not_enough_data                      0x13    /* The service did not supply enough data to perform the specified operation. */
#define CIP_Status_Attribute_not_supported              0x14    /* The attribute specified in the request is not supported */
#define CIP_Status_Too_much_data                        0x15    /* The service supplied more data than was expected */
#define CIP_Status_Object_does_not_exist                0x16    /* The object specified does not exist in the device. */
#define CIP_Status_Service_frag_seq_not_in_progress     0x17    /* The fragmentation sequence for this service is not currently active for this data. */
#define CIP_Status_No_stored_attribute_data             0x18    /* The attribute data of this object was not saved prior to the requested service. */
#define CIP_Status_Store_operation_failure              0x19    /* The attribute data of this object was not saved due to a failure during the attempt. */
#define CIP_Status_Routing_fail_req_packet_too_big      0x1A    /* The service request packet was too large for transmission on a network in the path to the destination. 
                                                                The routing device was forced to abort the service. */
#define CIP_Status_Routing_fail_resp_packet_too_big     0x1B    /* The service response packet was too large for transmission on a network in the path from the destination. 
                                                                The routing device was forced to abort the service. */
#define CIP_Status_Missing_attribute_list_entry_data    0x1C    /* The service did not supply an attribute in a list of attributes that was needed by the service to perform the requested behavior. */
#define CIP_Status_Invalid_attribute_value_list         0x1D    /* The service is returning the list of attributes supplied with status information for those attributes that were invalid. */
#define CIP_Status_Embedded_service_error               0x1E    /* An embedded service resulted in an error. */
#define CIP_Status_Vendor_specific_error                0x1F    /* A vendor specific error has been encountered. The Additional Code Field of the Error Response defines the particular error encountered. 
                                                                    Use of this General Error Code should only be performed when none of the Error Codes presented in this table or within an Object Class definition accurately reflect the error. */
#define CIP_Status_Invalid_parameter                    0x20    /* A parameter associated with the request was invalid. This code is used when a parameter does not 
                                                                    meet the requirements of this specification and/or the requirements defined in an Application Object Specification. */
#define CIP_Status_Write_once_value_already_written     0x21    /* An attempt was made to write to a write-once medium (e.g. WORM drive, PROM) that has already been written, 
                                                                    or to modify a value that cannot be changed once established. */
#define CIP_Status_Invalid_Reply_Received               0x22    /* An invalid reply is received (e.g. reply service code does not match the request service code, or reply message 
                                                                    is shorter than the minimum expected reply size). This status code can serve for other causes of invalid replies. */
#define CIP_Status_Buffer_Overflow                      0x23    /* The message received is larger than the receiving buffer can handle. The entire message was discarded. */
#define CIP_Status_Message_Format_Error                 0x24    /* The format of the received message is not supported by the server. */
#define CIP_Status_Key_Failure_in_path                  0x25    /* The Key Segment that was included as the first segment in the path does not match the destination module. 
                                                                    The object specific status shall indicate which part of the key check failed. */
#define CIP_Status_Path_Size_Invalid                    0x26    /* The size of the path which was sent with the Service Request is either not large enough to allow the 
                                                                    Request to be routed to an object or too much routing data was included. */
#define CIP_Status_Unexpected_attribute_in_list         0x27    /* An attempt was made to set an attribute that is not able to be set at this time. */
#define CIP_Status_Invalid_Member_ID                    0x28    /* The Member ID specified in the request does not exist in the specified Class/Instance/Attribute */
#define CIP_Status_Member_not_settable                  0x29    /* A request to modify a non-modifiable member was received */
#define CIP_Status_Group_2_only_server_general_failure  0x2A    /* This error code may only be reported by DeviceNet Group 2 Only servers with 4K or less code space 
                                                                    and only in place of Service not supported, Attribute not supported and Attribute not settable. */
#define CIP_Status_Unknown_Modbus_Error                 0x2B    /* A CIP to Modbus translator received an unknown Modbus Exception Code. */
#define CIP_Status_Attribute_not_gettable               0x2C    /* A request to read a non-readable attribute was received */
#define CIP_Status_Instance_Not_Deletable               0x2D    /* The requested object instance cannot be deleted */
#define CIP_Status_Service_Not_Supported_for_Path_1     0x2E    /* The object supports the service, but not for the designated application path (e.g. attribute).           
                                                                    NOTE: Not to be used for any set service (use General Status Code 0x0E or 0x29 instead) */
/* 2F - CF  Reserved by CIP for future extensions       */
/* D0 - FF  Reserved for Object Class specific errors   */

/* CIP Volume 1 (Common Industrial Protocol) - Table 3-5.33 Connection Manager Service Error Codes */
/* These errors are used when the General Status is CIP_Status_Connection_failure */
/* 0x0000 - 0x00FF are obsolete */
#define CIP_Connection_failure_ExtStatus_CONNECTION_IN_USE_OR_DUPLICATE_FORWARD_OPEN            0x0100
/* 0x0101 - 0x0102 are reserved by CIP */
#define CIP_Connection_failure_ExtStatus_TRANSPORT_CLASS_AND_TRIGGER_COMBINATION_NOT_SUPPORTED  0x0103
/* 0x0104 - 0x0105 are reserved by CIP / CIP Safety */
#define CIP_Connection_failure_ExtStatus_OWNERSHIP_CONFLICT                             0x0106
#define CIP_Connection_failure_ExtStatus_TARGET_CONNECTION_NOT_FOUND                    0x0107
#define CIP_Connection_failure_ExtStatus_INVALID_NETWORK_CONNECTION_PARAMETER           0x0108
#define CIP_Connection_failure_ExtStatus_INVALID_CONNECTION_SIZE                        0x0109
/* 0x010A - 0x010F are reserved by CIP */
#define CIP_Connection_failure_ExtStatus_TARGET_FOR_CONNECTION_NOT_CONFIGURED           0x0110
#define CIP_Connection_failure_ExtStatus_RPI_NOTSUPPORTED                               0x0111  
#define CIP_Connection_failure_ExtStatus_RPI_VALUES_NOT_ACCEPTABLE                      0x0112
#define CIP_Connection_failure_ExtStatus_OUT_OF_CONNECTIONS                             0x0113
#define CIP_Connection_failure_ExtStatus_VENDOR_ID_OR_PRODUCT_CODE_MISMATCH             0x0114
#define CIP_Connection_failure_ExtStatus_DEVICE_TYPE_MISMATCH                           0x0115
#define CIP_Connection_failure_ExtStatus_REVISION_MISMATCH                              0x0116
/* 0x0117 - 0x0118 are deprecated */
#define CIP_Connection_failure_ExtStatus_NON_LISTEN_ONLY_CONNECTION_NOT_OPENED          0x0119
#define CIP_Connection_failure_ExtStatus_TARGET_OBJECT_OUT_OF_CONNECTIONS               0x011A
#define CIP_Connection_failure_ExtStatus_THE_PRODUCTION_INHIBIT_TIME_IS_GREATER_THAN_THE_RPI 0x011B
#define CIP_Connection_failure_ExtStatus_TRANSPORT_CLASS_NOT_SUPPORTED                  0x011C
#define CIP_Connection_failure_ExtStatus_T2O_PRODUCTION_TRIGGER_NOT_SUPPORTED           0x011D
#define CIP_Connection_failure_ExtStatus_DIRECTION_NOT_SUPPORTED                        0x011E
#define CIP_Connection_failure_ExtStatus_INVALID_O2T_NETWORK_CONNECTION_FIXVAR          0x011F
#define CIP_Connection_failure_ExtStatus_INVALID_T2O_NETWORK_CONNECTION_FIXVAR          0x0120
#define CIP_Connection_failure_ExtStatus_INVALID_O2T_NETWORK_CONNECTION_PRIORITY        0x0121
#define CIP_Connection_failure_ExtStatus_INVALID_T2O_NETWORK_CONNECTION_PRIORITY        0x0122
#define CIP_Connection_failure_ExtStatus_INVALID_O2T_NETWORK_CONNECTION_TYPE            0x0123
#define CIP_Connection_failure_ExtStatus_INVALID_T2O_NETWORK_CONNECTION_TYPE            0x0124
#define CIP_Connection_failure_ExtStatus_INVALID_O2T_NETWORK_CONNECTION_REDUNDANT_OWNER 0x0125
#define CIP_Connection_failure_ExtStatus_INVALID_CONFIGURATION_SIZE                     0x0126
#define CIP_Connection_failure_ExtStatus_INVALID_O2T_NETWORK_CONNECTION_SIZE            0x0127
#define CIP_Connection_failure_ExtStatus_INVALID_T2O_NETWORK_CONNECTION_SIZE            0x0128
#define CIP_Connection_failure_ExtStatus_INVALID_CONFIGURATION_APPLICATION_PATH         0x0129
#define CIP_Connection_failure_ExtStatus_INVALID_CONSUMING_APPLICATION_PATH             0x012A
#define CIP_Connection_failure_ExtStatus_INVALID_PRODUCING_APPLICATION_PATH             0x012B
#define CIP_Connection_failure_ExtStatus_CONFIGURATION_SYMBOL_DOES_NOT_EXIST            0x012C
#define CIP_Connection_failure_ExtStatus_CONSUMING_SYMBOL_DOES_NOT_EXIST                0x012D
#define CIP_Connection_failure_ExtStatus_PRODUCING_SYMBOL_DOES_NOT_EXIST                0x012E
#define CIP_Connection_failure_ExtStatus_INCONSISTENT_APPLICATION_PATH_COMBINATION      0x012F
#define CIP_Connection_failure_ExtStatus_INCONSISTENT_CONSUME_DATA_FORMAT               0x0130
#define CIP_Connection_failure_ExtStatus_INCONSISTENT_PRODUCE_DATA_FORMAT               0x0131
#define CIP_Connection_failure_ExtStatus_NULL_FORWARD_OPEN_FUNCTION_NOT_SUPPORTED       0x0132
#define CIP_Connection_failure_ExtStatus_CONNECTION_TIMEOUT_MULTIPLIER_NOT_ACCEPTABLE   0x0133
#define CIP_Connection_failure_ExtStatus_MISMATCHED_T2O_NETWORK_CONNECTION_SIZE         0x0134
#define CIP_Connection_failure_ExtStatus_MISMATCHED_T2O_NETWORK_CONNECTION_FIXVAR       0x0135
#define CIP_Connection_failure_ExtStatus_MISMATCHED_T2O_NETWORK_CONNECTION_PRIORITY     0x0136
#define CIP_Connection_failure_ExtStatus_MISMATCHED_TRANSPORT_CLASS                     0x0137
#define CIP_Connection_failure_ExtStatus_MISMATCHED_T2O_PRODUCTION_TRIGGER              0x0128
#define CIP_Connection_failure_ExtStatus_MISMATCHED_T2O_PRODUCTION_INHIBIT_TIME_SEGMENT 0x0139
/* 0x013A - 0x0202 are reserved by CIP */
#define CIP_Connection_failure_ExtStatus_CONNECTION_TIMED_OUT                           0x0203
#define CIP_Connection_failure_ExtStatus_UNCONNECTED_REQUEST_TIMED_OUT                  0x0204
#define CIP_Connection_failure_ExtStatus_PARAMETER_ERROR_IN_UNCONNECTED_REQUEST_SERVICE 0x0205
#define CIP_Connection_failure_ExtStatus_MESSAGE_TOO_LARGE_FOR_UNCONNECTED_SEND_SERVICE 0x0206
#define CIP_Connection_failure_ExtStatus_UNCONNECTED_ACKNOWLEDGE_WITHOUT_REPLY          0x0207
/* 0x0208 - 0x0300 are reserved by CIP */
#define CIP_Connection_failure_ExtStatus_NO_BUFFER_MEMORY_AVAILABLE                     0x0301
#define CIP_Connection_failure_ExtStatus_NETWORK_BANDWIDTH_NOT_AVAILABLE_FOR_DATA       0x0302
#define CIP_Connection_failure_ExtStatus_NO_CONSUMED_CONNECTION_ID_FILTER_AVAILABLE     0x0303
#define CIP_Connection_failure_ExtStatus_NOT_CONFIGURED_TO_SEND_SCHEDULED_PRIORITY_DATA 0x0304
#define CIP_Connection_failure_ExtStatus_SCHEDULE_SIGNATURE_MISMATCH                    0x0305
#define CIP_Connection_failure_ExtStatus_SCHEDULE_SIGNATURE_VALIDATION_NOT_POSSIBLE     0x0306
/* 0x0307 - 0x0310 are reserved by CIP */
#define CIP_Connection_failure_ExtStatus_PORT_NOT_AVAILABLE     0x0311
#define CIP_Connection_failure_ExtStatus_LINK_ADDRESS_NOT_VALID 0x0312
/* 0x0313 - 0x0314 are reserved by CIP */
#define CIP_Connection_failure_ExtStatus_INVALID_SEGMENT_IN_CONNECTION_PATH             0x0315
#define CIP_Connection_failure_ExtStatus_FORWARD_CLOSE_SERVICE_CONNECTION_PATH_MISMATCH 0x0316
#define CIP_Connection_failure_ExtStatus_SCHEDULING_NOT_SPECIFIED                       0x0317
#define CIP_Connection_failure_ExtStatus_LINK_ADDRESS_TO_SELF_INVALID                   0x0318
#define CIP_Connection_failure_ExtStatus_SECONDARY_RESOURCES_UNAVAILABLE                0x0319
#define CIP_Connection_failure_ExtStatus_RACK_CONNECTION_ALREADY_ESTABLISHED            0x031A
#define CIP_Connection_failure_ExtStatus_MODULE_CONNECTION_ALREADY_ESTABLISHED          0x031B
#define CIP_Connection_failure_ExtStatus_MISCELLANEOUS                                  0x031C
#define CIP_Connection_failure_ExtStatus_REDUNDANT_CONNECTION_MISMATCH                  0x031D
#define CIP_Connection_failure_ExtStatus_NO_MORE_USER_CONFIGURABLE_LINK_CONSUMER_RESOURCES_AVAILABLE_IN_THE_PRODUCING_MODULE    0x031E
#define CIP_Connection_failure_ExtStatus_NO_USER_CONFIGURABLE_LINK_CONSUMER_RESOURCES_CONFIGURED_IN_THE_PRODUCING_MODULE        0x031F
/* 0x0320 - 0x7FF are vendor specific */
#define CIP_Connection_failure_ExtStatus_NETWORK_LINK_OFFLINE   0x0800
/* 0x0801 - 0x080F are reserved for CIP Safety */
#define CIP_Connection_failure_ExtStatus_NO_TARGET_APPLICATION_DATA_AVAILABLE                       0x0810
#define CIP_Connection_failure_ExtStatus_NO_ORIGINATOR_APPLICATION_DATA_AVAILABLE                   0x0811
#define CIP_Connection_failure_ExtStatus_NODE_ADDRESS_HAS_CHANGED_SINCE_THE_NETWORK_WAS_SCHEDULED   0x0812
#define CIP_Connection_failure_ExtStatus_NOT_CONFIGURED_FOR_OFF_SUBNET_MULTICAST                    0x0813
#define CIP_Connection_failure_ExtStatus_INVALID_PRODUCE_CONSUME_DATA_FORMAT                        0x0814
/* 0x0815 - 0xFFF are reserved by CIP / Do not use */

#endif /* __CIP_STATUS_H__ */
