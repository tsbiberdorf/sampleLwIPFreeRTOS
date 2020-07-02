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


#ifndef __CIP_SYSTEM_H__
#define __CIP_SYSTEM_H__

#include "eip_user_options.h" /* this must be first since it defines optional stack behaviors */

#include "rta_system.h"

#include "cip_common.h"
#include "cip_structs.h"
#include "eip_structs.h"

#include "cip_utils.h"
#include "cip_status.h"
#include "cip_decode.h"
#include "cip_encode.h"
#include "cip_process.h"

#include "cip_obj_cnxnmgr.h"
#include "cip_obj_cnxnmgr_explicit.h"
#include "cip_obj_cnxnmgr_implicit.h"
#include "cip_userobj_assembly.h"
#include "cip_userobj_identity.h"
#include "eip_userobj_tcp.h"
#include "eip_userobj_ethernet.h"

#if EIP_USER_OPTION_DLR_ENABLED
#include "eip_obj_dlr.h"
#endif /* #if EIP_USER_OPTION_DLR_ENABLED */

#if EIP_USER_OPTION_QOS_ENABLED
#include "eip_obj_qos.h"
#endif /* #if EIP_USER_OPTION_QOS_ENABLED */

#include "cip_userobj_vendspec.h"


/* --------------------------------------------------------------- */
/*      RTA DEFINED COMMON DEFINITIONS                             */
/* --------------------------------------------------------------- */

/* --------------------------------------------------------------- */
/*      RTA DEFINED GLOBAL FUNCTIONS                               */
/* --------------------------------------------------------------- */

#endif /* __CIP_SYSTEM_H__ */
