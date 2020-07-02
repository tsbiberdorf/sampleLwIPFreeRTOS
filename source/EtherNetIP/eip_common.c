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
#include "eip_system.h"

/* this must be after the include files */
#undef __RTA_FILE__
#define __RTA_FILE__ "eip_common.c"

/* ========================== */
/* GLOBAL FUNCTION PROTOTYPES */
/* ========================== */

/* ========================== */	
/*   GLOBAL DATA              */
/* ========================== */

/* ========================= */
/* LOCAL FUNCTION PROTOTYPES */
/* ========================= */

/* ========================= */
/*   LOCAL DATA              */
/* ========================= */

/* ========================= */
/*   MISCELLANEOUS           */
/* ========================= */

/* ==================================================== */
/*         GLOBAL FUNCTIONS                             */
/* ==================================================== */
CIP_ReturnCode eip_startup (CIP_InitType const InitType)
{
    CIP_ReturnCode rc;

    EIP_LOG_FUNCTION_ENTER;

    /* TODO: Move out of EIP */
    if(rta_memory_init(RTA_INIT_NORMAL) != RTA_ERR_SUCCESS)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_MALLOC_FAILED);

    /* TODO: Move out of EIP */
    if(rta_usersock_init() != RTA_USERSOCK_RETURNCODE_SUCCESS)
        EIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED);

    rc = eip_sockets_init(InitType);
    if(rc!=CIP_RETURNCODE_SUCCESS){EIP_LOG_ERROR_AND_RETURN(rc);}

    rc = eip_encap_init(InitType);
    if(rc!=CIP_RETURNCODE_SUCCESS){EIP_LOG_ERROR_AND_RETURN(rc);}

    rc = cip_userobj_assembly_init(InitType);
    if(rc!=CIP_RETURNCODE_SUCCESS){EIP_LOG_ERROR_AND_RETURN(rc);}

    rc = eip_userobj_ethernet_init(InitType);
    if(rc!=CIP_RETURNCODE_SUCCESS){EIP_LOG_ERROR_AND_RETURN(rc);}

    rc = eip_userobj_tcp_init(InitType);
    if(rc!=CIP_RETURNCODE_SUCCESS){EIP_LOG_ERROR_AND_RETURN(rc);}

#if EIP_USER_OPTION_QOS_ENABLED
    rc = eip_obj_qos_init(InitType);
    if(rc!=CIP_RETURNCODE_SUCCESS){EIP_LOG_ERROR_AND_RETURN(rc);}
#endif /* #if EIP_USER_OPTION_QOS_ENABLED */

#if EIP_USER_OPTION_DLR_ENABLED
    rc = eip_obj_dlr_init(InitType);
    if(rc!=CIP_RETURNCODE_SUCCESS){EIP_LOG_ERROR_AND_RETURN(rc);}
#endif /* EIP_USER_OPTION_DLR_ENABLED */

    rc = cip_obj_cnxnmgr_init(InitType);
    if(rc!=CIP_RETURNCODE_SUCCESS){EIP_LOG_ERROR_AND_RETURN(rc);}

    rc = cip_userobj_vendspec_init(InitType);
    if(rc!=CIP_RETURNCODE_SUCCESS){EIP_LOG_ERROR_AND_RETURN(rc);}

    rc = cip_userobj_identity_init(InitType);
    if(rc!=CIP_RETURNCODE_SUCCESS){EIP_LOG_ERROR_AND_RETURN(rc);}

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_shutdown (void)
{
    EIP_LOG_FUNCTION_ENTER;

    eip_sockets_shutdown();

    EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

/* Don't profile this function since the prints would be too much. */
CIP_ReturnCode eip_process (RTA_U32 const ms_since_last_call)
{
    CIP_ReturnCode rc;

    if(rta_usersock_process(ms_since_last_call) != RTA_USERSOCK_RETURNCODE_SUCCESS) 
    { 
        EIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED)
    };

    rc = eip_sockets_process(ms_since_last_call);
    if(rc!=CIP_RETURNCODE_SUCCESS){EIP_LOG_ERROR_AND_RETURN(rc);}

    rc = eip_encap_process(ms_since_last_call);
    if(rc!=CIP_RETURNCODE_SUCCESS){EIP_LOG_ERROR_AND_RETURN(rc);}

    rc = cip_userobj_assembly_process(ms_since_last_call);
    if(rc!=CIP_RETURNCODE_SUCCESS){EIP_LOG_ERROR_AND_RETURN(rc);}

    rc = eip_userobj_ethernet_process(ms_since_last_call);
    if(rc!=CIP_RETURNCODE_SUCCESS){EIP_LOG_ERROR_AND_RETURN(rc);}

    rc = cip_userobj_identity_process(ms_since_last_call);
    if(rc!=CIP_RETURNCODE_SUCCESS){EIP_LOG_ERROR_AND_RETURN(rc);}

    rc = eip_userobj_tcp_process(ms_since_last_call);
    if(rc!=CIP_RETURNCODE_SUCCESS){EIP_LOG_ERROR_AND_RETURN(rc);}

#if EIP_USER_OPTION_QOS_ENABLED
    rc = eip_obj_qos_process(ms_since_last_call);
    if(rc!=CIP_RETURNCODE_SUCCESS){EIP_LOG_ERROR_AND_RETURN(rc);}
#endif

#if EIP_USER_OPTION_DLR_ENABLED
    rc = eip_obj_dlr_process(ms_since_last_call);
    if(rc!=CIP_RETURNCODE_SUCCESS){EIP_LOG_ERROR_AND_RETURN(rc);}
#endif /* #if EIP_USER_OPTION_DLR_ENABLED */

    rc = cip_obj_cnxnmgr_process(ms_since_last_call);
    if(rc!=CIP_RETURNCODE_SUCCESS){EIP_LOG_ERROR_AND_RETURN(rc);}

    rc = cip_userobj_vendspec_process(ms_since_last_call);
    if(rc!=CIP_RETURNCODE_SUCCESS){EIP_LOG_ERROR_AND_RETURN(rc);}

    return(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode eip_enable_logging (CIP_LogLevel const loglevel)
{
    return (cip_enable_logging(loglevel));
}

CIP_ReturnCode eip_disable_logging (void)
{
    return (cip_disable_logging());
}

CIP_ReturnCode eip_register_user_log_function_ptr (void (* const fptr) (char const * const p_data))
{
    return (cip_register_user_log_function_ptr(fptr));
}

/* ==================================================== */
/*         LOCAL FUNCTIONS                              */
/* ==================================================== */

/* *********** */
/* END OF FILE */
/* *********** */
