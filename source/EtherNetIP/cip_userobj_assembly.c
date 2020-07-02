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

/* ========================== */
/*   INCLUDE FILES            */
/* ========================== */
#include "cip_system.h"
#include "Memory/CommonComm.h"

/* this must be after the include files */
#undef __RTA_FILE__
#define __RTA_FILE__ "cip_userobj_assembly.c"

/* ========================== */
/* GLOBAL FUNCTION PROTOTYPES */
/* ========================== */

/* ========================== */	
/*   GLOBAL DATA              */
/* ========================== */

/* ========================= */
/* LOCAL FUNCTION PROTOTYPES */
/* ========================= */
static CIP_ReturnCode local_process_CIP_Service_Get_Attribute_Single (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);
static CIP_ReturnCode local_process_CIP_Service_Get_Attribute_Single_ClassLevel (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);
static CIP_ReturnCode local_process_CIP_Service_Get_Attribute_Single_InstanceLevel_T2O (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);
static CIP_ReturnCode local_process_CIP_Service_Get_Attribute_Single_InstanceLevel_O2T (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);
static CIP_ReturnCode local_process_CIP_Service_Get_Attribute_Single_InstanceLevel_Configuration (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);

static CIP_ReturnCode local_process_CIP_Service_Set_Attribute_Single (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);
static CIP_ReturnCode local_process_CIP_Service_Set_Attribute_Single_InstanceLevel_T2O (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);
static CIP_ReturnCode local_process_CIP_Service_Set_Attribute_Single_InstanceLevel_O2T (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);
static CIP_ReturnCode local_process_CIP_Service_Set_Attribute_Single_InstanceLevel_Configuration (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);

static void local_update_assemblies (CIP_UDINT const instance);
static void local_process_configuration_write (CIP_UDINT const instance);

/*
 * API to provide the Fault and Warning codes, defined in CntlLogicTask.c
 */
extern unsigned short AssignFaultCode(unsigned char *PtrFaultCode,unsigned char *ptrWarningCode);

/*
 * API to provide interface to trigger a manual restart event.  defined in RestartButton.c
 */
extern void RequestManualRestart();

/* ========================= */
/*   LOCAL DATA              */
/* ========================= */

/* ========================= */
/*   MISCELLANEOUS           */
/* ========================= */
#define CIP_Class_Code_Assembly_Revision    2

typedef struct
{
    CIP_UDINT   InstanceID;
    CIP_UINT    DataSizeInBytes;   
    CIP_BYTE    * p_Data;
}ASM_INSTANCE_LOOKUP;

#define CIP_Assembly_T2O_Instance_BasicOverload     50
#define CIP_Assembly_T2O_SizeBytes_BasicOverload    1

#define CIP_Assembly_T2O_Instance_ExtendedOverload  51
#define CIP_Assembly_T2O_SizeBytes_ExtendedOverload 1

#define CIP_Assembly_T2O_Instance_RealTime   100
#define CIP_Assembly_T2O_Offset_RealTime     0
#define CIP_Assembly_T2O_SizeBytes_RealTime  100

#define CIP_Assembly_T2O_Instance_Fault      101
#define CIP_Assembly_T2O_Offset_Fault        (CIP_Assembly_T2O_SizeBytes_RealTime)
#define CIP_Assembly_T2O_SizeBytes_Fault     152

#define CIP_Assembly_T2O_Instance_Combined   102
#define CIP_Assembly_T2O_Offset_Combined     0
#define CIP_Assembly_T2O_SizeBytes_Combined  (CIP_Assembly_T2O_SizeBytes_RealTime+CIP_Assembly_T2O_SizeBytes_Fault)

CIP_BYTE CIP_AssemblyData_T2O_BasicOverload[CIP_Assembly_T2O_SizeBytes_BasicOverload];
CIP_BYTE CIP_AssemblyData_T2O_ExtendedOverload[CIP_Assembly_T2O_SizeBytes_ExtendedOverload];
CIP_BYTE CIP_AssemblyData_T2O_LittelfuseCombined[CIP_Assembly_T2O_SizeBytes_Combined];

ASM_INSTANCE_LOOKUP T2O_Entries[] =
{
    /*  ----------                                  ---------------                                 ------ */
    /*  InstanceID                                  DataSizeInBytes                                 p_Data */
    /*  ----------                                  ---------------                                 ------ */
    {   CIP_Assembly_T2O_Instance_BasicOverload,    CIP_Assembly_T2O_SizeBytes_BasicOverload,       CIP_AssemblyData_T2O_BasicOverload  },
    {   CIP_Assembly_T2O_Instance_ExtendedOverload, CIP_Assembly_T2O_SizeBytes_ExtendedOverload,    CIP_AssemblyData_T2O_ExtendedOverload  },
    {   CIP_Assembly_T2O_Instance_RealTime,         CIP_Assembly_T2O_SizeBytes_RealTime,            &CIP_AssemblyData_T2O_LittelfuseCombined[CIP_Assembly_T2O_Offset_RealTime]},
    {   CIP_Assembly_T2O_Instance_Fault,            CIP_Assembly_T2O_SizeBytes_Fault,               &CIP_AssemblyData_T2O_LittelfuseCombined[CIP_Assembly_T2O_Offset_Fault]},
    {   CIP_Assembly_T2O_Instance_Combined,         CIP_Assembly_T2O_SizeBytes_Combined,            &CIP_AssemblyData_T2O_LittelfuseCombined[CIP_Assembly_T2O_Offset_Combined]},
};
#define Num_T2O_Entries (sizeof(T2O_Entries)/sizeof(ASM_INSTANCE_LOOKUP))

#define CIP_Assembly_O2T_Instance_BasicOverload     2
#define CIP_Assembly_O2T_SizeBytes_BasicOverload    1

CIP_BYTE CIP_AssemblyData_O2T_BasicOverload[CIP_Assembly_O2T_SizeBytes_BasicOverload];
ASM_INSTANCE_LOOKUP O2T_Entries[] =
{
    /*  ----------                                  ---------------                             ------ */
    /*  InstanceID                                  DataSizeInBytes                             p_Data */
    /*  ----------                                  ---------------                             ------ */
    {   CIP_Assembly_O2T_Instance_BasicOverload,    CIP_Assembly_O2T_SizeBytes_BasicOverload,   CIP_AssemblyData_O2T_BasicOverload  },
};
#define Num_O2T_Entries (sizeof(O2T_Entries)/sizeof(ASM_INSTANCE_LOOKUP))

typedef struct
{
    CIP_UDINT   InstanceID;
    CIP_UINT    DataSizeInBytes;   
    CIP_UINT    DataSizeInWords;   
    CIP_BYTE    * p_Data;
}ASM_CONFIG_INSTANCE_LOOKUP;

#define CIP_Assembly_Config_Instance_Basic      150 // subset of the configuration variables
#define CIP_Assembly_Config_SizeBytes_Basic     52 // size in bytes where the basic configuration is being written
#define CIP_Assembly_Config_Offset_Basic        0

/* advanced included basic config */
#define CIP_Assembly_Config_Instance_All   151 // all configuration variables
#define CIP_Assembly_Config_SizeBytes_Advanced  80 // size in bytes where the full configuration array is being written.
#define CIP_Assembly_Config_Offset_Advanced     (0)
CIP_BYTE CIP_Assembly_Config_ByteData[CIP_Assembly_Config_SizeBytes_Advanced];

ASM_CONFIG_INSTANCE_LOOKUP Configuration_Entries[] =
{
    /*  ----------                              ---------------                         ---------------                             ------ */
    /*  InstanceID                              DataSizeInBytes                         DataSizeInWords                             p_Data */
    /*  ----------                              ---------------                         ---------------                             ------ */
    {   CIP_Assembly_Config_Instance_Basic,     CIP_Assembly_Config_SizeBytes_Basic,    (CIP_Assembly_Config_SizeBytes_Basic/2),    &CIP_Assembly_Config_ByteData[CIP_Assembly_Config_Offset_Basic]  },
    {   CIP_Assembly_Config_Instance_All,  CIP_Assembly_Config_SizeBytes_Advanced, (CIP_Assembly_Config_SizeBytes_Advanced/2), &CIP_Assembly_Config_ByteData[CIP_Assembly_Config_Offset_Advanced]  }
};
#define Num_Configuration_Entries (sizeof(Configuration_Entries)/sizeof(ASM_CONFIG_INSTANCE_LOOKUP))

#define CIP_AssemblyInstance_InputOnly_Heartbeat    199

/* Class Level Attributes */
#define CIP_AssemblyClass_AttributeId_Revision  1

/* Instance Level Attributes */
#define CIP_AssemblyInstance_AttributeId_Data   3

/* ==================================================== */
/*         GLOBAL FUNCTIONS                             */
/* ==================================================== */
CIP_ReturnCode cip_userobj_assembly_init (CIP_InitType const init_type)
{
    CIP_UINT asm_ix;

    CIP_LOG_FUNCTION_ENTER;

    switch(init_type)
    {
        case CIP_INITTYPE_NORMAL:
        case CIP_INITTYPE_OUTOFBOX:
        case CIP_INITTYPE_OUTOFBOX_NOCOMMS:
        case CIP_INITTYPE_MFR:
        default:
            break;
    }

    for(asm_ix=0; asm_ix<Num_T2O_Entries; asm_ix++)
    {
        if(!T2O_Entries[asm_ix].p_Data || T2O_Entries[asm_ix].DataSizeInBytes == 0)
            CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_NECESSARY_PTR_NOT_PROVIDED);
            
        memset(T2O_Entries[asm_ix].p_Data, 0, T2O_Entries[asm_ix].DataSizeInBytes);
    }

    for(asm_ix=0; asm_ix<Num_O2T_Entries; asm_ix++)
    {
        if(!O2T_Entries[asm_ix].p_Data || O2T_Entries[asm_ix].DataSizeInBytes == 0)
            CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_NECESSARY_PTR_NOT_PROVIDED);
            
        memset(O2T_Entries[asm_ix].p_Data, 0, O2T_Entries[asm_ix].DataSizeInBytes);
    }

    for(asm_ix=0; asm_ix<Num_Configuration_Entries; asm_ix++)
    {
        if(!Configuration_Entries[asm_ix].p_Data || Configuration_Entries[asm_ix].DataSizeInBytes == 0)
            CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_NECESSARY_PTR_NOT_PROVIDED);
            
        memset(Configuration_Entries[asm_ix].p_Data, 0, Configuration_Entries[asm_ix].DataSizeInBytes);
    }

    //populateCIPFaultValues(CIP_AssemblyData_T2O_LittelfuseCombined,1);
    //populateCIPFaultValues(CIP_AssemblyData_T2O_LittelfuseCombined,0);

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_userobj_assembly_process (RTA_U32 const ms_since_last_call)
{
    CIP_UINT asm_ix;
    CIP_UINT min_asm_size_bytes;

    RTA_UNUSED_PARAM(ms_since_last_call);

    /* on new data, loop back and update the sequence numbers */
    for(asm_ix=0; asm_ix<Num_T2O_Entries && asm_ix<Num_O2T_Entries; asm_ix++)
    {
        min_asm_size_bytes = RTA_MIN(T2O_Entries[asm_ix].DataSizeInBytes, O2T_Entries[asm_ix].DataSizeInBytes);

        if(memcmp(T2O_Entries[asm_ix].p_Data, O2T_Entries[asm_ix].p_Data, min_asm_size_bytes) != 0)
        {
            memcpy(T2O_Entries[asm_ix].p_Data, O2T_Entries[asm_ix].p_Data, min_asm_size_bytes);
            cip_obj_cnxnmgr_implicit_new_T2O_data (T2O_Entries[asm_ix].InstanceID);
        }
    }

    return(CIP_RETURNCODE_SUCCESS);
}

CIP_BOOL cip_userobj_assembly_is_instance_supported (CIP_CIAM_Path const * const p_path)
{
    CIP_UINT asm_ix;

    if(!p_path)
        return(RTA_FALSE);

    if(!p_path->instance_id_found)
        return(RTA_FALSE);

    if(p_path->instance_id == CIP_InstanceID_Class_Level)
        return(RTA_TRUE);   

    for(asm_ix=0; asm_ix<Num_T2O_Entries; asm_ix++)
        if(p_path->instance_id == T2O_Entries[asm_ix].InstanceID) 
            return(RTA_TRUE);    

    for(asm_ix=0; asm_ix<Num_O2T_Entries; asm_ix++)
        if(p_path->instance_id == O2T_Entries[asm_ix].InstanceID) 
            return(RTA_TRUE);    

    for(asm_ix=0; asm_ix<Num_Configuration_Entries; asm_ix++)
        if(p_path->instance_id == Configuration_Entries[asm_ix].InstanceID) 
            return(RTA_TRUE);    

    return(RTA_FALSE);
}

/* Since this is called from a function pointer, we can't restrict p_eip_ctrl to be 'const * const' as some functions need to modify p_eip_ctrl's contents */
/*lint -efunc(818, cip_userobj_assembly_process_service) */ /* 818 - Pointer parameter 'p_eip_ctrl' could be declared ptr to const */
CIP_ReturnCode cip_userobj_assembly_process_service (EIP_ControlStruct * const p_eip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_ReturnCode rc;
    CIP_ControlStruct const * p_cip_ctrl;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_eip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    p_cip_ctrl = &p_eip_ctrl->cip_ctrl;

    /* path was pre-validated for class_id and instance_id, so feel free to use it */

    switch(p_MessageRouterRequest->Service)
    {
        case CIP_Service_Get_Attribute_Single:
            if(!p_MessageRouterRequest->CIAM_Path.attribute_id_found)
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Path_destination_unknown));

            rc = local_process_CIP_Service_Get_Attribute_Single (p_cip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        case CIP_Service_Set_Attribute_Single:
            if(!p_MessageRouterRequest->CIAM_Path.attribute_id_found)
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Path_destination_unknown));

            rc = local_process_CIP_Service_Set_Attribute_Single (p_cip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break;

        default:
            CIP_LOG_ERROR_AND_RETURN (CIP_RETURNCODE_ERROR_INTERNAL);
    }    

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_BOOL cip_userobj_assembly_is_service_supported_for_instance (CIP_USINT const service, CIP_UDINT const instance)
{
    CIP_UINT asm_ix;

    if(instance == CIP_InstanceID_Class_Level)
    {
        if(service == CIP_Service_Get_Attribute_Single)
            return(RTA_TRUE);    
        else
            return(RTA_FALSE);
    }

    for(asm_ix=0; asm_ix<Num_T2O_Entries; asm_ix++)
    {
        if(instance == T2O_Entries[asm_ix].InstanceID) 
        {
            if(service == CIP_Service_Get_Attribute_Single)
                return(RTA_TRUE);    
            else
                return(RTA_FALSE);
        }
    }

    for(asm_ix=0; asm_ix<Num_O2T_Entries; asm_ix++)
    {
        if(instance == O2T_Entries[asm_ix].InstanceID) 
        {
            if(service == CIP_Service_Get_Attribute_Single)
                return(RTA_TRUE);    
            else if(service == CIP_Service_Set_Attribute_Single)
                return(RTA_TRUE);    
            else
                return(RTA_FALSE);
        }
    }

    for(asm_ix=0; asm_ix<Num_Configuration_Entries; asm_ix++)
    {
        if(instance == Configuration_Entries[asm_ix].InstanceID) 
        {
            if(service == CIP_Service_Get_Attribute_Single)
                return(RTA_TRUE);    
            else if(service == CIP_Service_Set_Attribute_Single)
                return(RTA_TRUE);    
            else
                return(RTA_FALSE);
        }
    }

    return(RTA_FALSE);
}

CIP_UINT cip_userobj_assembly_get_size_for_instance (CIP_UDINT const instance)
{
    CIP_UINT asm_ix;

    if(instance == CIP_AssemblyInstance_InputOnly_Heartbeat)
        return (0); /* all heartbeats are 0 length */

    for(asm_ix=0; asm_ix<Num_T2O_Entries; asm_ix++)
        if(instance == T2O_Entries[asm_ix].InstanceID) 
            return(T2O_Entries[asm_ix].DataSizeInBytes);

    for(asm_ix=0; asm_ix<Num_O2T_Entries; asm_ix++)
        if(instance == O2T_Entries[asm_ix].InstanceID) 
            return(O2T_Entries[asm_ix].DataSizeInBytes);

    for(asm_ix=0; asm_ix<Num_Configuration_Entries; asm_ix++)
        if(instance == Configuration_Entries[asm_ix].InstanceID) 
            return(Configuration_Entries[asm_ix].DataSizeInBytes);

    return(0);
}

/**
 *   This function is called whenever an assembly is read explicitly, or if an I/O packet needs to be transmitted.
 *   You can immediately update your I/O data values prior to copying to the passed string buffer.
*/
CIP_ReturnCode cip_userobj_assembly_read_data_for_instance (CIP_UDINT const instance, CIP_Opaque_String const * const p_Opaque_String)
{
    CIP_UINT expected_size;
    CIP_UINT asm_ix;

    if(!p_Opaque_String)
        return(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    expected_size = cip_userobj_assembly_get_size_for_instance(instance);
    if(p_Opaque_String->Length != expected_size)
        return(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    if(!expected_size) /* zero sizes are OK, for heartbeats */
        return(CIP_RETURNCODE_SUCCESS);

    if(!p_Opaque_String->pData)
        return(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    local_update_assemblies (instance);

    for(asm_ix=0; asm_ix<Num_T2O_Entries; asm_ix++)
    {
        if(instance == T2O_Entries[asm_ix].InstanceID) 
        {
            memcpy(p_Opaque_String->pData, T2O_Entries[asm_ix].p_Data, expected_size);
            return(CIP_RETURNCODE_SUCCESS);
        }
    }

    for(asm_ix=0; asm_ix<Num_O2T_Entries; asm_ix++)
    {
        if(instance == O2T_Entries[asm_ix].InstanceID) 
        {
            memcpy(p_Opaque_String->pData, O2T_Entries[asm_ix].p_Data, expected_size);
            return(CIP_RETURNCODE_SUCCESS);
        }
    }

    for(asm_ix=0; asm_ix<Num_Configuration_Entries; asm_ix++)
    {
        if(instance == Configuration_Entries[asm_ix].InstanceID) 
        {
            memcpy(p_Opaque_String->pData, Configuration_Entries[asm_ix].p_Data, expected_size);
            return(CIP_RETURNCODE_SUCCESS);
        }
    }

    return(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);
}

/*  This function is called whenever an assembly is written explicitly, or if an I/O packet was received.
    You can use the new data from the passed string buffer to update your internal values.
*/
CIP_ReturnCode cip_userobj_assembly_write_data_for_instance (CIP_UDINT const instance, CIP_Opaque_String const * const p_Opaque_String)
{
    CIP_UINT expected_size;
    CIP_UINT asm_ix;

    if(!p_Opaque_String)
        return(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    expected_size = cip_userobj_assembly_get_size_for_instance(instance);
    if(p_Opaque_String->Length != expected_size)
        return(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    if(!expected_size) /* zero sizes are OK, for heartbeats */
        return(CIP_RETURNCODE_SUCCESS);

    if(!p_Opaque_String->pData)
        return(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    for(asm_ix=0; asm_ix<Num_O2T_Entries; asm_ix++)
    {
        if(instance == O2T_Entries[asm_ix].InstanceID) 
        {
            if( ((O2T_Entries[asm_ix].p_Data[0] & 0x04) != (p_Opaque_String->pData[0] & 0x04)) &&
                (p_Opaque_String->pData[0] & 0x04))
            {
                /* clear fault */
//            	RequestManualRestart(); @todo remove this if not needed
            }

            memcpy(O2T_Entries[asm_ix].p_Data, p_Opaque_String->pData, expected_size);

            return(CIP_RETURNCODE_SUCCESS);
        }
    }

    for(asm_ix=0; asm_ix<Num_Configuration_Entries; asm_ix++)
    {
        if(instance == Configuration_Entries[asm_ix].InstanceID) 
        {
            memcpy(Configuration_Entries[asm_ix].p_Data, p_Opaque_String->pData, expected_size);

            return(CIP_RETURNCODE_SUCCESS);
        }
    }

    return(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);
}

/*  This function is continually called if the PLC/Scanner is in an idle state. 
    This generally happens with the PLC is in program mode.
    Output data is not valid when the scanner is in an idle state.
    Generally devices stay in their current process while this occurs (i.e. keep doing what you were doing). 
    Some devices (like drives) may stop their motion while in an idle state.
    Idle conditions don't generally trigger fault logic, but often invoke safe state logic.
    This state is usually temporary. Communications are present while in idle, so you know the PLC/Scanner is still there.
*/
void cip_userobj_assembly_idle_received_for_o2t_instance(CIP_UDINT const instance)
{
    RTA_UNUSED_PARAM(instance);
}

/*  This function is called once when the I/O connection times out.
    This generally happens when the PLC/Scanner faults or there is a cable break.
    Generally adapter devices go to a fault state when this occurs.
    This usually means you stop what you are doing and report a fault to your application.
    This is recoverable if a new connection is established, so you don't generally enter hard fault state.
*/
void cip_userobj_assembly_timeout_received_for_o2t_instance(CIP_UDINT const instance)
{
    RTA_UNUSED_PARAM(instance);
}

/*  This function is called when the PLC/Scanner explicitly closes the I/O connection.
    Generally, you just stop what you are doing and go back to your power up (stopped) state.
    This is not usually a fault since the PLC decided to talk to you and now they aren't.      
*/
void cip_userobj_assembly_close_received_for_o2t_instance(CIP_UDINT const instance)
{
    RTA_UNUSED_PARAM(instance);
}

CIP_BOOL cip_userobj_assembly_is_an_o2t_instance (CIP_UDINT const instance)
{
    CIP_UINT asm_ix;
    if(instance == CIP_AssemblyInstance_InputOnly_Heartbeat)
        return(RTA_TRUE);

    for(asm_ix=0; asm_ix<Num_O2T_Entries; asm_ix++)
        if(instance == O2T_Entries[asm_ix].InstanceID) 
            return(RTA_TRUE);

    return(RTA_FALSE);
}

CIP_BOOL cip_userobj_assembly_is_an_o2t_heartbeat (CIP_UDINT const instance)
{
    RTA_UNUSED_PARAM(instance);
    if(instance == CIP_AssemblyInstance_InputOnly_Heartbeat)
        return(RTA_TRUE);

    return(RTA_FALSE);
}

CIP_BOOL cip_userobj_assembly_is_an_t2o_instance (CIP_UDINT const instance)
{
    CIP_UINT asm_ix;


    for(asm_ix=0; asm_ix<Num_T2O_Entries; asm_ix++)
        if(instance == T2O_Entries[asm_ix].InstanceID) 
            return(RTA_TRUE);

    return(RTA_FALSE);
}

CIP_BOOL cip_userobj_assembly_is_an_t2o_heartbeat (CIP_UDINT const instance)
{
    return(RTA_FALSE);
}

CIP_ReturnCode cip_userobj_assembly_validate_ConfigurationData(CIP_UDINT const instance, CIP_DataSegment_SimpleDataSegment * const p_DataSegment_SimpleDataSegment, CIP_OBJ_ForwardOpen_ErrorStruct const * const p_ErrorStruct)
{
    CIP_USINT NumberofStatuses = 0;
    CIP_UINT Statuses[2];
    RTA_U16 word_ix, byte_ix;
    CIP_UINT asm_ix;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_DataSegment_SimpleDataSegment || !p_ErrorStruct)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    for(asm_ix=0; asm_ix<Num_Configuration_Entries; asm_ix++)
    {
        if(instance == Configuration_Entries[asm_ix].InstanceID) 
        {
            if(p_DataSegment_SimpleDataSegment->Data_Size_In_Words != Configuration_Entries[asm_ix].DataSizeInWords)
            {
                Statuses[NumberofStatuses++] = CIP_Connection_failure_ExtStatus_INVALID_CONFIGURATION_SIZE;
                Statuses[NumberofStatuses++] = Configuration_Entries[asm_ix].DataSizeInWords;
                return(cip_obj_cnxnmgr_build_forwardopen_error_multiple_extended_status (p_ErrorStruct, NumberofStatuses, Statuses));
            }
            
            /* convert the words to the byte array */
            for(word_ix=0, byte_ix=0; word_ix<Configuration_Entries[asm_ix].DataSizeInWords; word_ix++, byte_ix+=2)
                rta_endian_PutLitEndian16(p_DataSegment_SimpleDataSegment->p_WordData[word_ix], &Configuration_Entries[asm_ix].p_Data[byte_ix]);

            local_process_configuration_write(instance);
            CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
        }
    }

    Statuses[NumberofStatuses++] = CIP_Connection_failure_ExtStatus_INVALID_CONFIGURATION_APPLICATION_PATH;
    return(cip_obj_cnxnmgr_build_forwardopen_error_multiple_extended_status (p_ErrorStruct, NumberofStatuses, Statuses));
}

/* ==================================================== */
/*         LOCAL FUNCTIONS                              */
/* ==================================================== */
static CIP_ReturnCode local_process_CIP_Service_Get_Attribute_Single (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_ReturnCode rc;
    CIP_UINT asm_ix;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */
    
    if(p_MessageRouterRequest->CIAM_Path.instance_id == CIP_InstanceID_Class_Level)
    {
        rc = local_process_CIP_Service_Get_Attribute_Single_ClassLevel(p_cip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);
        if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
        CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
    }

    for(asm_ix=0; asm_ix<Num_T2O_Entries; asm_ix++)
    {
        if(p_MessageRouterRequest->CIAM_Path.instance_id == T2O_Entries[asm_ix].InstanceID) 
        {
            rc = local_process_CIP_Service_Get_Attribute_Single_InstanceLevel_T2O (p_cip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
        }
    }

    for(asm_ix=0; asm_ix<Num_O2T_Entries; asm_ix++)
    {
        if(p_MessageRouterRequest->CIAM_Path.instance_id == O2T_Entries[asm_ix].InstanceID) 
        {
            rc = local_process_CIP_Service_Get_Attribute_Single_InstanceLevel_O2T (p_cip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
        }
    }

    for(asm_ix=0; asm_ix<Num_Configuration_Entries; asm_ix++)
    {
        if(p_MessageRouterRequest->CIAM_Path.instance_id == Configuration_Entries[asm_ix].InstanceID) 
        {
            rc = local_process_CIP_Service_Get_Attribute_Single_InstanceLevel_Configuration (p_cip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
        }
    }

    return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Object_does_not_exist));               
}

static CIP_ReturnCode local_process_CIP_Service_Get_Attribute_Single_ClassLevel (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_ReturnCode rc;
    CIP_UINT const revision = CIP_Class_Code_Assembly_Revision;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */

    switch(p_MessageRouterRequest->CIAM_Path.attribute_id)
    {  
        case CIP_AssemblyClass_AttributeId_Revision:
            rc = cip_process_write_response_data_with_CIP_UINT(p_MessageRouterResponse, &revision);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            break; 

        default:
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Attribute_not_supported));        
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

static CIP_ReturnCode local_process_CIP_Service_Get_Attribute_Single_InstanceLevel_T2O (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_ReturnCode rc;
    CIP_Opaque_String Opaque_String;
    CIP_UINT asm_ix;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */

    switch(p_MessageRouterRequest->CIAM_Path.attribute_id)
    {  
        case CIP_AssemblyInstance_AttributeId_Data:
            for(asm_ix=0; asm_ix<Num_T2O_Entries; asm_ix++)
            {
                if(p_MessageRouterRequest->CIAM_Path.instance_id == T2O_Entries[asm_ix].InstanceID) 
                {
                    Opaque_String.Length = T2O_Entries[asm_ix].DataSizeInBytes;
                    Opaque_String.pData = T2O_Entries[asm_ix].p_Data;

                    rc = cip_userobj_assembly_read_data_for_instance(p_MessageRouterRequest->CIAM_Path.instance_id, &Opaque_String); 
                    if(rc != CIP_RETURNCODE_SUCCESS) 
                        CIP_LOG_ERROR_AND_RETURN (rc);

                    rc = cip_process_write_response_data_with_CIP_Opaque_String(p_cip_ctrl, p_MessageRouterResponse, &Opaque_String);
                    if(rc != CIP_RETURNCODE_SUCCESS) 
                        CIP_LOG_ERROR_AND_RETURN (rc);

                    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
                }
            }
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Object_does_not_exist));               

        default:
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Attribute_not_supported));        
    }
}

static CIP_ReturnCode local_process_CIP_Service_Get_Attribute_Single_InstanceLevel_O2T (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_ReturnCode rc;
    CIP_Opaque_String Opaque_String;
    CIP_UINT asm_ix;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */

    switch(p_MessageRouterRequest->CIAM_Path.attribute_id)
    {  
        case CIP_AssemblyInstance_AttributeId_Data:
            for(asm_ix=0; asm_ix<Num_O2T_Entries; asm_ix++)
            {
                if(p_MessageRouterRequest->CIAM_Path.instance_id == O2T_Entries[asm_ix].InstanceID) 
                {
                    Opaque_String.Length = O2T_Entries[asm_ix].DataSizeInBytes;
                    Opaque_String.pData = O2T_Entries[asm_ix].p_Data;

                    rc = cip_userobj_assembly_read_data_for_instance(p_MessageRouterRequest->CIAM_Path.instance_id, &Opaque_String); 
                    if(rc != CIP_RETURNCODE_SUCCESS) 
                        CIP_LOG_ERROR_AND_RETURN (rc);

                    rc = cip_process_write_response_data_with_CIP_Opaque_String(p_cip_ctrl, p_MessageRouterResponse, &Opaque_String);
                    if(rc != CIP_RETURNCODE_SUCCESS) 
                        CIP_LOG_ERROR_AND_RETURN (rc);

                    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
                }
            }
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Object_does_not_exist));               

        default:
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Attribute_not_supported));        
    }
}

static CIP_ReturnCode local_process_CIP_Service_Get_Attribute_Single_InstanceLevel_Configuration (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_ReturnCode rc;
    CIP_Opaque_String Opaque_String;
    CIP_UINT asm_ix;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */

    switch(p_MessageRouterRequest->CIAM_Path.attribute_id)
    {  
        case CIP_AssemblyInstance_AttributeId_Data:
            for(asm_ix=0; asm_ix<Num_Configuration_Entries; asm_ix++)
            {
                if(p_MessageRouterRequest->CIAM_Path.instance_id == Configuration_Entries[asm_ix].InstanceID) 
                {
                    Opaque_String.Length = Configuration_Entries[asm_ix].DataSizeInBytes;
                    Opaque_String.pData = Configuration_Entries[asm_ix].p_Data;

                    rc = cip_userobj_assembly_read_data_for_instance(p_MessageRouterRequest->CIAM_Path.instance_id, &Opaque_String); 
                    if(rc != CIP_RETURNCODE_SUCCESS) 
                        CIP_LOG_ERROR_AND_RETURN (rc);

                    rc = cip_process_write_response_data_with_CIP_Opaque_String(p_cip_ctrl, p_MessageRouterResponse, &Opaque_String);
                    if(rc != CIP_RETURNCODE_SUCCESS) 
                        CIP_LOG_ERROR_AND_RETURN (rc);

                    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
                }
            }
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Object_does_not_exist));               

        default:
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Attribute_not_supported));        
    }
}

static CIP_ReturnCode local_process_CIP_Service_Set_Attribute_Single (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_ReturnCode rc;
    CIP_UINT asm_ix;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */

    if(p_MessageRouterRequest->CIAM_Path.instance_id == CIP_InstanceID_Class_Level)
    {
        return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Service_not_supported));               
    }

    for(asm_ix=0; asm_ix<Num_T2O_Entries; asm_ix++)
    {
        if(p_MessageRouterRequest->CIAM_Path.instance_id == T2O_Entries[asm_ix].InstanceID) 
        {
            rc = local_process_CIP_Service_Set_Attribute_Single_InstanceLevel_T2O (p_cip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
        }
    }

    for(asm_ix=0; asm_ix<Num_O2T_Entries; asm_ix++)
    {
        if(p_MessageRouterRequest->CIAM_Path.instance_id == O2T_Entries[asm_ix].InstanceID) 
        {
            rc = local_process_CIP_Service_Set_Attribute_Single_InstanceLevel_O2T (p_cip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
        }
    }

    for(asm_ix=0; asm_ix<Num_Configuration_Entries; asm_ix++)
    {
        if(p_MessageRouterRequest->CIAM_Path.instance_id == Configuration_Entries[asm_ix].InstanceID) 
        {
            rc = local_process_CIP_Service_Set_Attribute_Single_InstanceLevel_Configuration (p_cip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse);
            if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
            CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
        }
    }

    return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Object_does_not_exist));               
}

static CIP_ReturnCode local_process_CIP_Service_Set_Attribute_Single_InstanceLevel_T2O (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */

    switch(p_MessageRouterRequest->CIAM_Path.attribute_id)
    {  
        case CIP_AssemblyInstance_AttributeId_Data:
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Attribute_not_settable));        

        default:
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Attribute_not_supported));        
    }
}

static CIP_ReturnCode local_process_CIP_Service_Set_Attribute_Single_InstanceLevel_O2T (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_ReturnCode rc;
    CIP_Opaque_String const * p_Opaque_String;
    CIP_UINT asm_ix;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */

    switch(p_MessageRouterRequest->CIAM_Path.attribute_id)
    {  
        case CIP_AssemblyInstance_AttributeId_Data:
            if(p_MessageRouterRequest->Request_Data.Type == CIP_DATA_TYPE_NO_DATA)
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Not_enough_data));        

            if(p_MessageRouterRequest->Request_Data.Type != CIP_DATA_TYPE_CIP_Opaque_String)
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Message_Format_Error));        

            p_Opaque_String = &p_MessageRouterRequest->Request_Data.Value.value_Opaque_String;

            for(asm_ix=0; asm_ix<Num_O2T_Entries; asm_ix++)
            {
                if(p_MessageRouterRequest->CIAM_Path.instance_id == O2T_Entries[asm_ix].InstanceID) 
                {
                    if(p_Opaque_String->Length < O2T_Entries[asm_ix].DataSizeInBytes)
                        return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Not_enough_data));        

                    if(p_Opaque_String->Length > O2T_Entries[asm_ix].DataSizeInBytes)
                        return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Too_much_data));        
    
                    /* TODO: add check for ownership if I/O messaging is supported */
                    if( cip_obj_cnxnmgr_implicit_o2t_assembly_owned(p_MessageRouterRequest->CIAM_Path.instance_id) == RTA_TRUE)
                    {
                    	return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Device_state_conflict));
                    }

                    rc = cip_userobj_assembly_write_data_for_instance(p_MessageRouterRequest->CIAM_Path.instance_id, p_Opaque_String); 
                    if (rc != CIP_RETURNCODE_SUCCESS)
                        CIP_LOG_ERROR_AND_RETURN(rc);

                    memcpy(O2T_Entries[asm_ix].p_Data, p_Opaque_String->pData, p_Opaque_String->Length);


                    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
                }
            }

            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Object_does_not_exist));               

        default:
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Attribute_not_supported));        
    }
}

static CIP_ReturnCode local_process_CIP_Service_Set_Attribute_Single_InstanceLevel_Configuration (CIP_ControlStruct const * const p_cip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_ReturnCode rc;
    CIP_Opaque_String const * p_Opaque_String;
    CIP_UINT asm_ix;

    CIP_LOG_FUNCTION_ENTER;

    if (!p_cip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    /* path was pre-validated, so feel free to use it */

    switch(p_MessageRouterRequest->CIAM_Path.attribute_id)
    {  
        case CIP_AssemblyInstance_AttributeId_Data:
            if(p_MessageRouterRequest->Request_Data.Type == CIP_DATA_TYPE_NO_DATA)
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Not_enough_data));        

            if(p_MessageRouterRequest->Request_Data.Type != CIP_DATA_TYPE_CIP_Opaque_String)
                return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Message_Format_Error));        

            p_Opaque_String = &p_MessageRouterRequest->Request_Data.Value.value_Opaque_String;

            for(asm_ix=0; asm_ix<Num_Configuration_Entries; asm_ix++)
            {
                if(p_MessageRouterRequest->CIAM_Path.instance_id == Configuration_Entries[asm_ix].InstanceID) 
                {
                    if(p_Opaque_String->Length < Configuration_Entries[asm_ix].DataSizeInBytes)
                        return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Not_enough_data));        

                    if(p_Opaque_String->Length > Configuration_Entries[asm_ix].DataSizeInBytes)
                        return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Too_much_data));        
               
                    memcpy(Configuration_Entries[asm_ix].p_Data, p_Opaque_String->pData, p_Opaque_String->Length);

                    rc = cip_userobj_assembly_write_data_for_instance(p_MessageRouterRequest->CIAM_Path.instance_id, p_Opaque_String); 
                    if (rc != CIP_RETURNCODE_SUCCESS)
                        CIP_LOG_ERROR_AND_RETURN(rc);

                    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
                }
            }

            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Object_does_not_exist));               

        default:
            return(cip_process_build_error(p_MessageRouterResponse, CIP_Status_Attribute_not_supported));        
    }
}

static void local_update_assemblies (CIP_UDINT const instance)
{

    switch(instance)
    {
        case CIP_Assembly_T2O_Instance_BasicOverload:
        case CIP_Assembly_T2O_Instance_ExtendedOverload:
//        	AssignFaultCode(T2O_Entries[0].p_Data,T2O_Entries[1].p_Data);
            break;

        case CIP_Assembly_Config_Instance_Basic:
        case CIP_Assembly_Config_Instance_All:
//        	populateCIPConfigurationValues(CIP_Assembly_Config_ByteData);
        	break;

        case CIP_Assembly_T2O_Instance_RealTime:
//        	populateCIPRealTimeValues(CIP_AssemblyData_T2O_LittelfuseCombined);
            break;
        case CIP_Assembly_T2O_Instance_Fault:
//            populateCIPFaultValues(CIP_AssemblyData_T2O_LittelfuseCombined,1);
//            populateCIPFaultValues(CIP_AssemblyData_T2O_LittelfuseCombined,0);
            break;
        case CIP_Assembly_T2O_Instance_Combined:
//        	populateCIPRealTimeValues(CIP_AssemblyData_T2O_LittelfuseCombined);
//            populateCIPFaultValues(CIP_AssemblyData_T2O_LittelfuseCombined,1);
//            populateCIPFaultValues(CIP_AssemblyData_T2O_LittelfuseCombined,0);
            break;

        default:
            break;
    }
}

static void local_process_configuration_write (CIP_UDINT const instance)
{
    switch(instance)
    {
        case CIP_Assembly_Config_Instance_Basic:
        case CIP_Assembly_Config_Instance_All:
        	/*
        	 * store configuration data
        	 */
//        	saveCIPConfigurationValues(CIP_Assembly_Config_ByteData,(int)instance);

            break;


        default:
            break;
    }
}

/* *********** */
/* END OF FILE */
/* *********** */
