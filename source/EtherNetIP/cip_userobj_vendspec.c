/*
 *            Copyright (c) 2015, 2016 by Real Time Automation, Inc.
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
#define __RTA_FILE__ "cip_userobj_vendspec.c"

/*lint -save -e788 */ /* enum constant not in switch */

/* ========================== */
/* GLOBAL FUNCTION PROTOTYPES */
/* ========================== */
/*
 * API to provide interface to trigger a manual restart event.  defined in RestartButton.c
 */
extern void RequestManualRestart();

/* ========================== */	
/*   GLOBAL DATA              */
/* ========================== */
VS_ControlSupervisorObject ControlSupervisorObject;
VS_OverloadObject OverloadObject;
VS_FactoryCfgObject FactoryCfgObject;
VS_RealTimeObject RealTimeObject;

const CIP_BOOL Default_BOOL_Min = 0;
const CIP_BOOL Default_BOOL_Max = 1;

const CIP_INT TripFLCSet_INT_Min = 0;
const CIP_INT TripFLCSet_INT_Max = 32767;

#define UINT_MIN (0)
#define UINT_MAX (0x8000)
#define UDINT_MIN (0)
#define UDINT_MAX (0x80000000)

const CIP_UINT MULT_MIN = 1; // Current tranformer ratio MIN is 1
const CIP_UINT MULT_MAX = UINT_MAX;
const CIP_UINT PT_MIN = 1; // Potential tranformer ratio MIN is 1
const CIP_UINT PT_MAX = UINT_MAX;
const CIP_UINT VUB_MIN = UINT_MIN;
const CIP_UINT VUB_MAX = UINT_MAX;
const CIP_UINT TC_MIN = 2; // MIN trip class is 2
const CIP_UINT TC_MAX = UINT_MAX;
const CIP_UINT UCTD_MIN = UINT_MIN;
const CIP_UINT UCTD_MAX = UINT_MAX;
const CIP_UINT CUB_MIN = UINT_MIN;
const CIP_UINT CUB_MAX = UINT_MAX;
const CIP_UINT CUBTD_MIN = UINT_MIN;
const CIP_UINT CUBTD_MAX = UINT_MAX;
const CIP_UINT LIN_MIN = UINT_MIN;
const CIP_UINT LIN_MAX = UINT_MAX;
const CIP_UINT RU_MIN = UINT_MIN;
const CIP_UINT RU_MAX = UINT_MAX;
const CIP_UINT RF_MIN = UINT_MIN;
const CIP_UINT RF_MAX = UINT_MAX;
const CIP_UINT GFTD_MIN = UINT_MIN;
const CIP_UINT GFTD_MAX = UINT_MAX;
const CIP_UINT GFID_MIN = UINT_MIN;
const CIP_UINT GFID_MAX = UINT_MAX;
const CIP_UINT HPRTD_MIN = UINT_MIN;
const CIP_UINT HPRTD_MAX = UINT_MAX;
const CIP_UINT STLP_MIN = UINT_MIN;
const CIP_UINT STLP_MAX = UINT_MAX;
const CIP_UINT STLTD_MIN = UINT_MIN;
const CIP_UINT STLTD_MAX = UINT_MAX;
const CIP_UINT STLID_MIN = UINT_MIN;
const CIP_UINT STLID_MAX = UINT_MAX;
const CIP_UINT MACCTRL_MIN = UINT_MIN;
const CIP_UINT MACCTRL_MAX = UINT_MAX;
const CIP_UINT MACTD_MIN = UINT_MIN;
const CIP_UINT MACTD_MAX = UINT_MAX;
const CIP_UINT CMD_MIN = UINT_MIN;
const CIP_UINT CMD_MAX = UINT_MAX;
const CIP_UINT NAME_MIN = UINT_MIN;
const CIP_UINT NAME_MAX = 20;


const CIP_UDINT LV_MIN = UDINT_MIN;
const CIP_UDINT LV_MAX = UDINT_MAX;
const CIP_UDINT HV_MIN = UDINT_MIN;
const CIP_UDINT HV_MAX = UDINT_MAX;
const CIP_UDINT OC_MIN = 5;
const CIP_UDINT OC_MAX = UDINT_MAX;
const CIP_UDINT UC_MIN = UDINT_MIN;
const CIP_UDINT UC_MAX = UDINT_MAX;
const CIP_UDINT RD0_MIN = UDINT_MIN;
const CIP_UDINT RD0_MAX = UDINT_MAX;
const CIP_UDINT RD1_MIN = UDINT_MIN;
const CIP_UDINT RD1_MAX = UDINT_MAX;
const CIP_UDINT RD2_MIN = UDINT_MIN;
const CIP_UDINT RD2_MAX = UDINT_MAX;
const CIP_UDINT RD3_MIN = UDINT_MIN;
const CIP_UDINT RD3_MAX = UDINT_MAX;
const CIP_UDINT GF_MIN = UDINT_MIN;
const CIP_UDINT GF_MAX = UDINT_MAX;
const CIP_UDINT LKW_MIN = UDINT_MIN;
const CIP_UDINT LKW_MAX = UDINT_MAX;
const CIP_UDINT HKW_MIN = UDINT_MIN;
const CIP_UDINT HKW_MAX = UDINT_MAX;
const CIP_UDINT ENDIS_MIN = UDINT_MIN;
const CIP_UDINT ENDIS_MAX = UDINT_MAX;
const CIP_UDINT CNFG_MIN = UDINT_MIN;
const CIP_UDINT CNFG_MAX = UDINT_MAX;
const CIP_UDINT COMCFG_MIN = UDINT_MIN;
const CIP_UDINT COMCFG_MAX = UDINT_MAX;
const CIP_UDINT CAN_CNFG_MIN = UDINT_MIN;
const CIP_UDINT CAN_CNFG_MAX = UDINT_MAX;
const CIP_UDINT MOD_CNFG_MIN = UDINT_MIN;
const CIP_UDINT MOD_CNFG_MAX = UDINT_MAX;

CIP_SHORT_STRING_32 DummyStringMinMax; /* if min/max equal, we don't check range... we still need a pointer for settable types */

/* ========================= */
/* LOCAL FUNCTION PROTOTYPES */
/* ========================= */
static void local_init_dummy_values (void);
static CIP_UDINT local_get_max_wildcard_instance (CIP_UDINT const ClassID);
static void local_load_wildcard_instance (CIP_UDINT const ClassID, CIP_UDINT const InstID);
static void local_save_wildcard_instance (CIP_UDINT const ClassID, CIP_UDINT const InstID);
static CIP_ReturnCode local_get_attribute_single(EIP_ControlStruct * const p_eip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse, VS_LOOKUP_TABLE_PTRS const * const p_table_entry);
static CIP_ReturnCode local_set_attribute_single(EIP_ControlStruct * const p_eip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse, VS_LOOKUP_TABLE_PTRS const * const p_table_entry);
static RTA_Size_Type local_get_expected_size_in_bytes (VS_LOOKUP_TABLE_PTRS const * const p_table_entry);

/* ========================= */
/*   LOCAL DATA              */
/* ========================= */

/* ========================= */
/*   MISCELLANEOUS           */
/* ========================= */
const VS_LOOKUP_TABLE_PTRS VSAttrTable[] =
{
/*  Settable    ClassID                         InstID  AttrID  DataType                    Min Ptr                     Max Ptr                     Data pointer    */
	{RTA_FALSE, VS_CLASSID_CONTROL_SUPERVISOR,	0,      1,      CIP_DATA_TYPE_CIP_UINT,     (void*)NULL,                (void*)NULL,                (void*)&ControlSupervisorObject.ClassLevel.Revision},
	{RTA_TRUE,  VS_CLASSID_CONTROL_SUPERVISOR,  1,      3,      CIP_DATA_TYPE_CIP_BOOL,     (void*)&Default_BOOL_Min,   (void*)&Default_BOOL_Max,   (void*)&ControlSupervisorObject.InstanceLevel[0].Run1},
	{RTA_FALSE, VS_CLASSID_CONTROL_SUPERVISOR,  1,      7,      CIP_DATA_TYPE_CIP_BOOL,     (void*)NULL,                (void*)NULL,                (void*)&ControlSupervisorObject.InstanceLevel[0].Running1},
	{RTA_FALSE, VS_CLASSID_CONTROL_SUPERVISOR,  1,      10,     CIP_DATA_TYPE_CIP_BOOL,     (void*)NULL,                (void*)NULL,                (void*)&ControlSupervisorObject.InstanceLevel[0].Faulted},
	{RTA_FALSE, VS_CLASSID_CONTROL_SUPERVISOR,  1,      11,     CIP_DATA_TYPE_CIP_BOOL,     (void*)NULL,                (void*)NULL,                (void*)&ControlSupervisorObject.InstanceLevel[0].Warning},
	{RTA_TRUE,  VS_CLASSID_CONTROL_SUPERVISOR,  1,      12,     CIP_DATA_TYPE_CIP_BOOL,     (void*)&Default_BOOL_Min,   (void*)&Default_BOOL_Max,   (void*)&ControlSupervisorObject.InstanceLevel[0].FaultRst},
	{RTA_FALSE, VS_CLASSID_CONTROL_SUPERVISOR,  1,      15,     CIP_DATA_TYPE_CIP_BOOL,     (void*)NULL,                (void*)NULL,                (void*)&ControlSupervisorObject.InstanceLevel[0].CtrlFromNet},
	{RTA_TRUE,  VS_CLASSID_CONTROL_SUPERVISOR,  1,      17,     CIP_DATA_TYPE_CIP_BOOL,     (void*)&Default_BOOL_Min,   (void*)&Default_BOOL_Max,   (void*)&ControlSupervisorObject.InstanceLevel[0].ForceFault},

/*  Settable    ClassID                 InstID  AttrID  DataType                    Min Ptr                     Max Ptr                     Data pointer    */
	{RTA_FALSE, VS_CLASSID_OVERLOAD,    0,      1,      CIP_DATA_TYPE_CIP_UINT,     (void*)NULL,                (void*)NULL,                (void*)&ControlSupervisorObject.ClassLevel.Revision},
	{RTA_TRUE,  VS_CLASSID_OVERLOAD,    1,      3,      CIP_DATA_TYPE_CIP_INT,      (void*)&TripFLCSet_INT_Min, (void*)&TripFLCSet_INT_Max, (void*)&OverloadObject.InstanceLevel[0].TripFLCSet},

/*  Settable    ClassID                     InstID   AttrID  DataType                            Min Ptr                 Max Ptr                 Data pointer    */
	{RTA_FALSE, VS_CLASSID_FACTORY_CFG,	    0,       1,      CIP_DATA_TYPE_CIP_UINT,             (void*)NULL,            (void*)NULL,            (void*)&FactoryCfgObject.ClassLevel.Revision},
	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       1,      CIP_DATA_TYPE_CIP_UINT,            (void*)&MULT_MIN,            (void*)&MULT_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].MULT},
	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       2,      CIP_DATA_TYPE_CIP_UINT,             (void*)&PT_MIN,            (void*)&PT_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].PT},
	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       3,      CIP_DATA_TYPE_CIP_UDINT,            (void*)&LV_MIN,            (void*)&LV_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].LV},
	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       4,      CIP_DATA_TYPE_CIP_UDINT,            (void*)&HV_MIN,            (void*)&HV_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].HV},
	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       5,      CIP_DATA_TYPE_CIP_UINT,             (void*)&VUB_MIN,            (void*)&VUB_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].VUB},
	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       6,      CIP_DATA_TYPE_CIP_UINT,             (void*)&TC_MIN,            (void*)&TC_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].TC},
	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       7,      CIP_DATA_TYPE_CIP_UDINT,            (void*)&OC_MIN,            (void*)&OC_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].OC},
	{RTA_TRUE,  VS_CLASSID_FACTORY_CFG,	    1,       8,      CIP_DATA_TYPE_CIP_UDINT,            (void*)&UC_MIN,            (void*)&UC_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].UC},
	{RTA_TRUE,  VS_CLASSID_FACTORY_CFG,	    1,       9,      CIP_DATA_TYPE_CIP_UINT,             (void*)&UCTD_MIN,            (void*)&UCTD_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].UCTD},
	{RTA_TRUE,  VS_CLASSID_FACTORY_CFG,	    1,       10,     CIP_DATA_TYPE_CIP_UINT,             (void*)&CUB_MIN,            (void*)&CUB_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].CUB},
//		{RTA_TRUE,  VS_CLASSID_FACTORY_CFG,	    1,      11,     CIP_DATA_TYPE_CIP_UINT,	  	  (void*)&CUBTD_MIN,    (void*)&CUBTD_MAX,	    (void*)&tl_ConfigRegsObj.InstanceLevel[0].CUBTD},
	{RTA_TRUE,  VS_CLASSID_FACTORY_CFG,	    1,       12,     CIP_DATA_TYPE_CIP_UINT,             (void*)&LIN_MIN,            (void*)&LIN_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].LIN},
	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       13,      CIP_DATA_TYPE_CIP_UDINT,            (void*)&RD0_MIN,            (void*)&RD0_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].RD0},
	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       14,      CIP_DATA_TYPE_CIP_UDINT,            (void*)&RD1_MIN,            (void*)&RD1_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].RD1},
	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       15,     CIP_DATA_TYPE_CIP_UDINT,            (void*)&RD2_MIN,            (void*)&RD2_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].RD2},
	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       16,     CIP_DATA_TYPE_CIP_UDINT,            (void*)&RD3_MIN,            (void*)&RD3_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].RD3},
	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       17,     CIP_DATA_TYPE_CIP_UINT,             (void*)&RU_MIN,            (void*)&RU_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].RU},
	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       18,     CIP_DATA_TYPE_CIP_UINT,             (void*)&RF_MIN,            (void*)&RF_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].RF},
	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       19,     CIP_DATA_TYPE_CIP_UDINT,            (void*)&GF_MIN,            (void*)&GF_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].GF},
	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       20,     CIP_DATA_TYPE_CIP_UINT,            (void*)&GFTD_MIN,            (void*)&GFTD_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].GFTD},
//	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       21,     CIP_DATA_TYPE_CIP_UDINT,            (void*)&GFID_MIN,            (void*)&GFID_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].GFID},
	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       22,     CIP_DATA_TYPE_CIP_UDINT,            (void*)&LKW_MIN,            (void*)&LKW_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].LKW},
	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       23,     CIP_DATA_TYPE_CIP_UDINT,            (void*)&HKW_MIN,            (void*)&HKW_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].HKW},
	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       24,     CIP_DATA_TYPE_CIP_UINT,             (void*)&HPRTD_MIN,            (void*)&HPRTD_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].HPRTD},
	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       25,     CIP_DATA_TYPE_CIP_UINT,             (void*)&STLP_MIN,            (void*)&STLP_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].STLP},
	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       26,     CIP_DATA_TYPE_CIP_UINT,             (void*)&STLTD_MIN,            (void*)&STLTD_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].STLTD},
	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       27,     CIP_DATA_TYPE_CIP_UINT,             (void*)&STLID_MIN,            (void*)&STLID_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].STLID},
//	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       28,     CIP_DATA_TYPE_CIP_UINT,             (void*)&MACCTRL_MIN,            (void*)&MACCTRL_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].MACCTRL},
//	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       29,     CIP_DATA_TYPE_CIP_UINT,             (void*)&MACTD_MIN,            (void*)&MACTD_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].MACTD},
	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       30,     CIP_DATA_TYPE_CIP_UDINT,            (void*)&ENDIS_MIN,            (void*)&ENDIS_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].ENDIS},
	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       31,     CIP_DATA_TYPE_CIP_UDINT,            (void*)&CNFG_MIN,            (void*)&CNFG_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].CNFG},
//	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       32,     CIP_DATA_TYPE_CIP_UDINT,            (void*)&COMCFG_MIN,            (void*)&COMCFG_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].COMCFG},
//	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       33,     CIP_DATA_TYPE_CIP_UDINT,            (void*)&CAN_CNFG_MIN,            (void*)&CAN_CNFG_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].CAN_CNFG},
//	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       34,     CIP_DATA_TYPE_CIP_UDINT,            (void*)&MOD_CNFG_MIN,            (void*)&MOD_CNFG_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].MOD_CNFG},
	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       35,     CIP_DATA_TYPE_CIP_SHORT_STRING_32,  (void*)&NAME_MIN,            (void*)&NAME_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].NAME},
	{RTA_TRUE, VS_CLASSID_FACTORY_CFG,	    1,       36,     CIP_DATA_TYPE_CIP_UINT,             (void*)&CMD_MIN,            (void*)&CMD_MAX,            (void*)&FactoryCfgObject.InstanceLevel[0].CMD},

	/*  Settable    ClassID                 InstID   AttrID   DataType                           Min Ptr                 Max Ptr                 Data pointer    */
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    0,       1,	      CIP_DATA_TYPE_CIP_UINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.ClassLevel.Revision},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 1,       CIP_DATA_TYPE_CIP_UDINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].MRT},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 2,       CIP_DATA_TYPE_CIP_UDINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].MST},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 3,       CIP_DATA_TYPE_CIP_UINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].SCNT},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 4,       CIP_DATA_TYPE_CIP_UDINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].FAULT_STAT},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 5,       CIP_DATA_TYPE_CIP_UDINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].WARN_STAT},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 6,       CIP_DATA_TYPE_CIP_UINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].FAULT_CODE},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 7,       CIP_DATA_TYPE_CIP_UDINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].RTDT},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 8,       CIP_DATA_TYPE_CIP_UDINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].RDR},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 9,       CIP_DATA_TYPE_CIP_UINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].TCR},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 10,      CIP_DATA_TYPE_CIP_UINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].MLF},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 11,      CIP_DATA_TYPE_CIP_UINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].SEQ},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 12,      CIP_DATA_TYPE_CIP_UINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].VUBM},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 13,      CIP_DATA_TYPE_CIP_UINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].CUBM},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 14,      CIP_DATA_TYPE_CIP_UDINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].V1},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 15,      CIP_DATA_TYPE_CIP_UDINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].V2},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 16,      CIP_DATA_TYPE_CIP_UDINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].V3},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 17,      CIP_DATA_TYPE_CIP_UDINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].I1},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 18,      CIP_DATA_TYPE_CIP_UDINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].I2},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 19,      CIP_DATA_TYPE_CIP_UDINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].I3},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 20,      CIP_DATA_TYPE_CIP_UDINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].P1},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 21,      CIP_DATA_TYPE_CIP_UDINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].P2},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 22,      CIP_DATA_TYPE_CIP_UDINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].P3},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 23,      CIP_DATA_TYPE_CIP_UINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].PF1},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 24,      CIP_DATA_TYPE_CIP_UINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].PF2},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 25,      CIP_DATA_TYPE_CIP_UINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].PF3},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 26,      CIP_DATA_TYPE_CIP_UDINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].IGF},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 27,      CIP_DATA_TYPE_CIP_UINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].PTC},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 28,      CIP_DATA_TYPE_CIP_UINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].MRELAY},
	{RTA_FALSE, VS_CLASSID_REAL_TIME,	    1,		 29,      CIP_DATA_TYPE_CIP_UINT,		     (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.InstanceLevel[0].ARELAY},


/*  Settable    ClassID                 InstID              AttrID  DataType                            Min Ptr                 Max Ptr                 Data pointer    */
	{RTA_FALSE, VS_CLASSID_REAL_TIME,   0,                  1,      CIP_DATA_TYPE_CIP_UINT,             (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.ClassLevel.Revision},
//	{RTA_FALSE, VS_CLASSID_REAL_TIME,	VS_INST_WILDCARD,   1,      CIP_DATA_TYPE_CIP_UINT,             (void*)NULL,            (void*)NULL,            (void*)&RealTimeObject.WildcardInstanceLevel.Number},
};

#define VSAttrTableLen (sizeof(VSAttrTable) / sizeof(VS_LOOKUP_TABLE_PTRS))




/* ==================================================== */
/*         GLOBAL FUNCTIONS                             */
/* ==================================================== */
CIP_ReturnCode cip_userobj_vendspec_init (CIP_InitType const init_type)
{
    CIP_LOG_FUNCTION_ENTER;

    memset(&DummyStringMinMax, 0, sizeof(DummyStringMinMax));


    switch(init_type)
    {
        case CIP_INITTYPE_NORMAL:
            local_init_dummy_values();
            break;

        case CIP_INITTYPE_OUTOFBOX:
        case CIP_INITTYPE_OUTOFBOX_NOCOMMS:
        case CIP_INITTYPE_MFR:
        default:
            break;
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_userobj_vendspec_process (RTA_U32 const ms_since_last_call)
{
    RTA_UNUSED_PARAM(ms_since_last_call);

    return(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_userobj_vendspec_decode (CIP_ControlStruct const * const p_cip_ctrl, CIP_UINT const expected_length, CIP_MessageRouterRequest * const p_MessageRouterRequest)
{
    CIP_ReturnCode rc;
	RTA_U16 table_ix = VSAttrTableLen;
	RTA_U16 ii;

    CIP_LOG_FUNCTION_ENTER;

    RTA_UNUSED_PARAM(expected_length); /* only used if indeterminent length data is passed */

    if(!p_cip_ctrl || !p_MessageRouterRequest)
        CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

    switch(p_MessageRouterRequest->Service)
    {
        case CIP_Service_Set_Attribute_Single:

        	/* we need the class, instance and attribute */
        	if( (!p_MessageRouterRequest->CIAM_Path.class_id_found) ||
        		(!p_MessageRouterRequest->CIAM_Path.instance_id_found) ||
				(!p_MessageRouterRequest->CIAM_Path.attribute_id_found) )
        	{
        		CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED); /* this is an OK exit, so don't return as ERROR */
        	}

        	for(ii=0; ii<VSAttrTableLen; ii++)
        	{
        		/* validate the object */
        		if(VSAttrTable[ii].ClassID != p_MessageRouterRequest->CIAM_Path.class_id)
        			continue;

        		/* validate the instance (wildcard) */
        		if(VSAttrTable[ii].InstID == VS_INST_WILDCARD)
        		{
        			if((p_MessageRouterRequest->CIAM_Path.instance_id == 0) || (p_MessageRouterRequest->CIAM_Path.instance_id > local_get_max_wildcard_instance(VSAttrTable[ii].ClassID)))
        				continue;
        		}
        		/* validate the instance (exact match */
        		else
        		{
        			if(VSAttrTable[ii].InstID != p_MessageRouterRequest->CIAM_Path.instance_id)
        				continue;
        		}

				/* validate the attribute */
				if(VSAttrTable[ii].AttrID != p_MessageRouterRequest->CIAM_Path.attribute_id)
					continue;

        		/* match found */
        		table_ix = ii;
        		break;
        	}

        	if(table_ix >= VSAttrTableLen)
                CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_ERROR_FUNCTION_FAILED); /* this is an OK exit, so don't return as ERROR */

        	switch(VSAttrTable[table_ix].DataType)
        	{
        		case CIP_DATA_TYPE_CIP_BOOL:
                    rc = cip_decode_CIP_MessageRouter_Data_of_type_CIP_BOOL(p_cip_ctrl, &p_MessageRouterRequest->Request_Data);
                    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
                    break;

        		case CIP_DATA_TYPE_CIP_USINT:
                    rc = cip_decode_CIP_MessageRouter_Data_of_type_CIP_USINT(p_cip_ctrl, &p_MessageRouterRequest->Request_Data);
                    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
                    break;

        		case CIP_DATA_TYPE_CIP_UINT:
                    rc = cip_decode_CIP_MessageRouter_Data_of_type_CIP_UINT(p_cip_ctrl, &p_MessageRouterRequest->Request_Data);
                    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
                    break;

        		case CIP_DATA_TYPE_CIP_UDINT:
                    rc = cip_decode_CIP_MessageRouter_Data_of_type_CIP_UDINT(p_cip_ctrl, &p_MessageRouterRequest->Request_Data);
                    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
                    break;

        		case CIP_DATA_TYPE_CIP_SINT:
                    rc = cip_decode_CIP_MessageRouter_Data_of_type_CIP_SINT(p_cip_ctrl, &p_MessageRouterRequest->Request_Data);
                    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
                    break;

        		case CIP_DATA_TYPE_CIP_INT:
                    rc = cip_decode_CIP_MessageRouter_Data_of_type_CIP_INT(p_cip_ctrl, &p_MessageRouterRequest->Request_Data);
                    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
                    break;

        		case CIP_DATA_TYPE_CIP_DINT:
                    rc = cip_decode_CIP_MessageRouter_Data_of_type_CIP_DINT(p_cip_ctrl, &p_MessageRouterRequest->Request_Data);
                    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
                    break;

        		case CIP_DATA_TYPE_CIP_REAL:
                    rc = cip_decode_CIP_MessageRouter_Data_of_type_CIP_REAL(p_cip_ctrl, &p_MessageRouterRequest->Request_Data);
                    if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
                    break;

                default:
                    rc = cip_decode_CIP_MessageRouter_Data_of_type_CIP_Opaque_String(p_cip_ctrl, expected_length, &p_MessageRouterRequest->Request_Data);
                    if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
                    break;
            }
            break;

        default:
            rc = cip_decode_CIP_MessageRouter_Data_of_type_CIP_Opaque_String(p_cip_ctrl, expected_length, &p_MessageRouterRequest->Request_Data);
            if (rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN(rc);
            break;
    }

    CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);
}

CIP_ReturnCode cip_userobj_vendspec_process_service (EIP_ControlStruct * const p_eip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse)
{
    CIP_ReturnCode cip_rc;
	RTA_U16 table_ix = VSAttrTableLen;
	RTA_U16 ii;

	if(!p_eip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse)
	    CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

	for(ii=0; ii<VSAttrTableLen; ii++)
	{
		/* validate the object */
		if(VSAttrTable[ii].ClassID != p_MessageRouterRequest->CIAM_Path.class_id)
			continue;

        /* validate the instance (wildcard) */
        if(VSAttrTable[ii].InstID == VS_INST_WILDCARD)
        {
        	if((p_MessageRouterRequest->CIAM_Path.instance_id == 0) || (p_MessageRouterRequest->CIAM_Path.instance_id > local_get_max_wildcard_instance(VSAttrTable[ii].ClassID)))
        		continue;
        }
		/* validate the instance (exact match */
		else
		{
			if(VSAttrTable[ii].InstID != p_MessageRouterRequest->CIAM_Path.instance_id)
				continue;
		}

		/* match found */
		table_ix = ii;
		break;
	}

	if(table_ix == VSAttrTableLen)
	{
		p_MessageRouterResponse->GeneralStatus = CIP_Status_Object_does_not_exist;
		return(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
	}

	/* validate the service code */
	switch(p_MessageRouterRequest->Service)
	{
        case CIP_Service_Reset:
            if( (p_MessageRouterRequest->CIAM_Path.class_id != VS_CLASSID_CONTROL_SUPERVISOR) || 
                (p_MessageRouterRequest->CIAM_Path.instance_id != 1) )
            {
			    p_MessageRouterResponse->GeneralStatus = CIP_Status_Service_not_supported;
			    return(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
            }
    
            /*
             * trigger a reset
             */
//            RequestManualRestart(); @todo remove this if not needed since no code exists for it yet

            /* TODO: what should the control supervisor reset do? */
            CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);

		case CIP_Service_Get_Attribute_Single:
		case CIP_Service_Set_Attribute_Single:
			/* find the attribute... start where we left off */
			for(ii=table_ix, table_ix=VSAttrTableLen; ii<VSAttrTableLen; ii++)
			{
				/* validate the object */
				if(VSAttrTable[ii].ClassID != p_MessageRouterRequest->CIAM_Path.class_id)
					continue;

        		/* validate the instance (wildcard) */
        		if(VSAttrTable[ii].InstID == VS_INST_WILDCARD)
        		{
        			if((p_MessageRouterRequest->CIAM_Path.instance_id == 0) || (p_MessageRouterRequest->CIAM_Path.instance_id > local_get_max_wildcard_instance(VSAttrTable[ii].ClassID)))
        				continue;

                    local_load_wildcard_instance(p_MessageRouterRequest->CIAM_Path.class_id, p_MessageRouterRequest->CIAM_Path.instance_id);
        		}
				/* validate the instance (exact match */
				else
				{
					if(VSAttrTable[ii].InstID != p_MessageRouterRequest->CIAM_Path.instance_id)
						continue;
				}

				/* validate the attribute */
				if(VSAttrTable[ii].AttrID != p_MessageRouterRequest->CIAM_Path.attribute_id)
					continue;

				/* match found */
				table_ix = ii;
				break;
			}

			if(table_ix == VSAttrTableLen)
			{
				p_MessageRouterResponse->GeneralStatus = CIP_Status_Attribute_not_supported;
				return(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
			}

			/* process the get/set */
			if(p_MessageRouterRequest->Service == CIP_Service_Get_Attribute_Single)
				return(local_get_attribute_single(p_eip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse, &VSAttrTable[table_ix]));
			else
            {
                cip_rc = local_set_attribute_single(p_eip_ctrl, p_MessageRouterRequest, p_MessageRouterResponse, &VSAttrTable[table_ix]);
                if(cip_rc == CIP_RETURNCODE_SUCCESS)
                    local_save_wildcard_instance(p_MessageRouterRequest->CIAM_Path.class_id, p_MessageRouterRequest->CIAM_Path.instance_id);

				return(cip_rc);
            }

		default:
			p_MessageRouterResponse->GeneralStatus = CIP_Status_Service_not_supported;
			return(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
	}
}

/* ==================================================== */
/*         LOCAL FUNCTIONS                              */
/* ==================================================== */

static void GetRealTimeRegisters()
{
//@todo decide if we even need to keep this function and if so update it with values for NGR instead of MP8000
	//    CIP_UINT inst_ix;
//
//    for(inst_ix=0; inst_ix<VS_NUMINST_REAL_TIME; inst_ix++)
//    {
//    	MemMapRead(CFG_MEMMAP_MST_ADDR,CFG_MEMMAP_MST_SIZE,&RealTimeObject.InstanceLevel[inst_ix].MST);
//    	MemMapRead(CFG_MEMMAP_MRT_ADDR,CFG_MEMMAP_MRT_SIZE,&RealTimeObject.InstanceLevel[inst_ix].MRT);
//    	MemMapRead(CFG_MEMMAP_SCNT_ADDR,CFG_MEMMAP_SCNT_SIZE,&RealTimeObject.InstanceLevel[inst_ix].SCNT);
//    	MemMapRead(CFG_MEMMAP_FAULT_STAT_ADDR,CFG_MEMMAP_FAULT_STAT_SIZE,&RealTimeObject.InstanceLevel[inst_ix].FAULT_STAT);
//    	MemMapRead(CFG_MEMMAP_WARN_STAT_ADDR,CFG_MEMMAP_WARN_STAT_SIZE,&RealTimeObject.InstanceLevel[inst_ix].WARN_STAT);
//    	MemMapRead(CFG_MEMMAP_FAULT_CODE_ADDR,CFG_MEMMAP_FAULT_CODE_SIZE,&RealTimeObject.InstanceLevel[inst_ix].FAULT_CODE);
//    	MemMapRead(CFG_MEMMAP_RTDT_ADDR,CFG_MEMMAP_RTDT_SIZE,&RealTimeObject.InstanceLevel[inst_ix].RTDT);
//    	MemMapRead(CFG_MEMMAP_RDR_ADDR,CFG_MEMMAP_RDR_SIZE,&RealTimeObject.InstanceLevel[inst_ix].RDR);
//    	MemMapRead(CFG_MEMMAP_TCR_ADDR,CFG_MEMMAP_TCR_SIZE,&RealTimeObject.InstanceLevel[inst_ix].TCR);
//    	MemMapRead(CFG_MEMMAP_MLF_ADDR,CFG_MEMMAP_MLF_SIZE,&RealTimeObject.InstanceLevel[inst_ix].MLF);
//    	MemMapRead(CFG_MEMMAP_SEQ_ADDR,CFG_MEMMAP_SEQ_SIZE,&RealTimeObject.InstanceLevel[inst_ix].SEQ);
//    	MemMapRead(CFG_MEMMAP_VUBM_ADDR,CFG_MEMMAP_VUBM_SIZE,&RealTimeObject.InstanceLevel[inst_ix].VUBM);
//    	MemMapRead(CFG_MEMMAP_CUBM_ADDR,CFG_MEMMAP_CUBM_SIZE,&RealTimeObject.InstanceLevel[inst_ix].CUBM);
//    	MemMapRead(CFG_MEMMAP_V1_ADDR,CFG_MEMMAP_V1_SIZE,&RealTimeObject.InstanceLevel[inst_ix].V1);
//    	MemMapRead(CFG_MEMMAP_V2_ADDR,CFG_MEMMAP_V2_SIZE,&RealTimeObject.InstanceLevel[inst_ix].V2);
//    	MemMapRead(CFG_MEMMAP_V3_ADDR,CFG_MEMMAP_V3_SIZE,&RealTimeObject.InstanceLevel[inst_ix].V3);
//    	MemMapRead(CFG_MEMMAP_I1_ADDR,CFG_MEMMAP_I1_SIZE,&RealTimeObject.InstanceLevel[inst_ix].I1);
//    	MemMapRead(CFG_MEMMAP_I2_ADDR,CFG_MEMMAP_I2_SIZE,&RealTimeObject.InstanceLevel[inst_ix].I2);
//    	MemMapRead(CFG_MEMMAP_I3_ADDR,CFG_MEMMAP_I3_SIZE,&RealTimeObject.InstanceLevel[inst_ix].I3);
//    	MemMapRead(CFG_MEMMAP_P1_ADDR,CFG_MEMMAP_P1_SIZE,&RealTimeObject.InstanceLevel[inst_ix].P1);
//    	MemMapRead(CFG_MEMMAP_P2_ADDR,CFG_MEMMAP_P2_SIZE,&RealTimeObject.InstanceLevel[inst_ix].P2);
//    	MemMapRead(CFG_MEMMAP_P3_ADDR,CFG_MEMMAP_P3_SIZE,&RealTimeObject.InstanceLevel[inst_ix].P3);
//    	MemMapRead(CFG_MEMMAP_PF1_ADDR,CFG_MEMMAP_PF1_SIZE,&RealTimeObject.InstanceLevel[inst_ix].PF1);
//    	MemMapRead(CFG_MEMMAP_PF2_ADDR,CFG_MEMMAP_PF2_SIZE,&RealTimeObject.InstanceLevel[inst_ix].PF2);
//    	MemMapRead(CFG_MEMMAP_PF3_ADDR,CFG_MEMMAP_PF3_SIZE,&RealTimeObject.InstanceLevel[inst_ix].PF3);
//    	MemMapRead(CFG_MEMMAP_IGF_ADDR,CFG_MEMMAP_IGF_SIZE,&RealTimeObject.InstanceLevel[inst_ix].IGF);
//    	MemMapRead(CFG_MEMMAP_PTC_ADDR,CFG_MEMMAP_PTC_SIZE,&RealTimeObject.InstanceLevel[inst_ix].PTC);
//    	MemMapRead(CFG_MEMMAP_MRELAY_ADDR,CFG_MEMMAP_MRELAY_SIZE,&RealTimeObject.InstanceLevel[inst_ix].MRELAY);
//    	MemMapRead(CFG_MEMMAP_ARELAY_ADDR,CFG_MEMMAP_ARELAY_SIZE,&RealTimeObject.InstanceLevel[inst_ix].ARELAY);
//    }
//
//    for(inst_ix=0; inst_ix<VS_NUMINST_FACTORY_CFG; inst_ix++)
//    {
//        FactoryCfgObject.InstanceLevel[inst_ix].MULT = 1;
//    	MemMapRead(CFG_MEMMAP_MULT_ADDR,CFG_MEMMAP_MULT_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].MULT);
//    	MemMapRead(CFG_MEMMAP_PT_ADDR,CFG_MEMMAP_PT_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].PT);
//    	MemMapRead(CFG_MEMMAP_LV_ADDR,CFG_MEMMAP_LV_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].LV);
//    	MemMapRead(CFG_MEMMAP_HV_ADDR,CFG_MEMMAP_HV_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].HV);
//    	MemMapRead(CFG_MEMMAP_VUB_ADDR,CFG_MEMMAP_VUB_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].VUB);
//    	MemMapRead(CFG_MEMMAP_TC_ADDR,CFG_MEMMAP_TC_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].TC);
//    	MemMapRead(CFG_MEMMAP_OC_ADDR,CFG_MEMMAP_OC_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].OC);
//    	MemMapRead(CFG_MEMMAP_UC_ADDR,CFG_MEMMAP_UC_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].UC);
//    	MemMapRead(CFG_MEMMAP_UCTD_ADDR,CFG_MEMMAP_UCTD_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].UCTD);
//    	MemMapRead(CFG_MEMMAP_CUB_ADDR,CFG_MEMMAP_CUB_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].CUB);
//    	MemMapRead(CFG_MEMMAP_CUBTD_ADDR,CFG_MEMMAP_CUBTD_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].CUBTD);
//    	MemMapRead(CFG_MEMMAP_LIN_ADDR,CFG_MEMMAP_LIN_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].LIN);
//    	MemMapRead(CFG_MEMMAP_RD0_ADDR,CFG_MEMMAP_RD0_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].RD0);
//    	MemMapRead(CFG_MEMMAP_RD1_ADDR,CFG_MEMMAP_RD1_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].RD1);
//    	MemMapRead(CFG_MEMMAP_RD2_ADDR,CFG_MEMMAP_RD2_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].RD2);
//    	MemMapRead(CFG_MEMMAP_RD3_ADDR,CFG_MEMMAP_RD3_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].RD3);
//    	MemMapRead(CFG_MEMMAP_RU_ADDR,CFG_MEMMAP_RU_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].RU);
//    	MemMapRead(CFG_MEMMAP_RF_ADDR,CFG_MEMMAP_RF_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].RF);
//    	MemMapRead(CFG_MEMMAP_GF_ADDR,CFG_MEMMAP_GF_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].GF);
//    	MemMapRead(CFG_MEMMAP_GFTD_ADDR,CFG_MEMMAP_GFTD_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].GFTD);
//    	MemMapRead(CFG_MEMMAP_GFID_ADDR,CFG_MEMMAP_GFID_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].GFID);
//    	MemMapRead(CFG_MEMMAP_LKW_ADDR,CFG_MEMMAP_LKW_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].LKW);
//    	MemMapRead(CFG_MEMMAP_HKW_ADDR,CFG_MEMMAP_HKW_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].HKW);
//    	MemMapRead(CFG_MEMMAP_HPRTD_ADDR,CFG_MEMMAP_HPRTD_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].HPRTD);
//    	MemMapRead(CFG_MEMMAP_STLP_ADDR,CFG_MEMMAP_STLP_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].STLP);
//    	MemMapRead(CFG_MEMMAP_STLTD_ADDR,CFG_MEMMAP_STLTD_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].STLTD);
//    	MemMapRead(CFG_MEMMAP_STLID_ADDR,CFG_MEMMAP_STLID_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].STLID);
//    	MemMapRead(CFG_MEMMAP_MACCTRL_ADDR,CFG_MEMMAP_MACCTRL_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].MACCTRL);
//    	MemMapRead(CFG_MEMMAP_MACTD_ADDR,CFG_MEMMAP_MACTD_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].MACTD);
//    	MemMapRead(CFG_MEMMAP_ENDIS_ADDR,CFG_MEMMAP_ENDIS_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].ENDIS);
//    	MemMapRead(CFG_MEMMAP_CNFG_ADDR,CFG_MEMMAP_CNFG_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].CNFG);
//    	MemMapRead(CFG_MEMMAP_COMCFG_ADDR,CFG_MEMMAP_COMCFG_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].COMCFG);
//    	MemMapRead(CFG_MEMMAP_CAN_CNFG_ADDR,CFG_MEMMAP_CAN_CNFG_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].CAN_CNFG);
//    	MemMapRead(CFG_MEMMAP_MOD_CNFG_ADDR,CFG_MEMMAP_MOD_CNFG_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].MOD_CNFG);
//    	FactoryCfgObject.InstanceLevel[inst_ix].CMD = 0;
//    	MemMapRead(CFG_MEMMAP_NAME_ADDR,CFG_MEMMAP_NAME_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].NAME.Data[0]);
//    	FactoryCfgObject.InstanceLevel[inst_ix].NAME.Length = sizeof(FactoryCfgObject.InstanceLevel[inst_ix].NAME.Data);
//    }

}

static void local_init_dummy_values (void)
{
	//@todo add our own dummy values into here for the NGR relay instead of the MP8000 values
//    CIP_UINT inst_ix;

//
//    memset(&ControlSupervisorObject, 0, sizeof(ControlSupervisorObject));
//    ControlSupervisorObject.ClassLevel.Revision = 1;
//    for(inst_ix=0; inst_ix<VS_NUMINST_CONTROL_SUPERVISOR; inst_ix++)
//    {
//        ControlSupervisorObject.InstanceLevel[inst_ix].CtrlFromNet = 0;
//    }
//
//    memset(&OverloadObject, 0, sizeof(OverloadObject));
//    OverloadObject.ClassLevel.Revision = 1;
//    for(inst_ix=0; inst_ix<VS_NUMINST_CONTROL_SUPERVISOR; inst_ix++)
//    {
//        OverloadObject.InstanceLevel[inst_ix].TripFLCSet = 0;
//    }
//
//    memset(&FactoryCfgObject, 0, sizeof(FactoryCfgObject));
//    FactoryCfgObject.ClassLevel.Revision = 1;
//    for(inst_ix=0; inst_ix<VS_NUMINST_FACTORY_CFG; inst_ix++)
//    {
//        FactoryCfgObject.InstanceLevel[inst_ix].MULT = 1;
//    	MemMapRead(CFG_MEMMAP_MULT_ADDR,CFG_MEMMAP_MULT_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].MULT);
//    	MemMapRead(CFG_MEMMAP_PT_ADDR,CFG_MEMMAP_PT_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].PT);
//    	MemMapRead(CFG_MEMMAP_LV_ADDR,CFG_MEMMAP_LV_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].LV);
//    	MemMapRead(CFG_MEMMAP_HV_ADDR,CFG_MEMMAP_HV_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].HV);
//    	MemMapRead(CFG_MEMMAP_VUB_ADDR,CFG_MEMMAP_VUB_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].VUB);
//    	MemMapRead(CFG_MEMMAP_TC_ADDR,CFG_MEMMAP_TC_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].TC);
//    	MemMapRead(CFG_MEMMAP_OC_ADDR,CFG_MEMMAP_OC_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].OC);
//    	MemMapRead(CFG_MEMMAP_UC_ADDR,CFG_MEMMAP_UC_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].UC);
//    	MemMapRead(CFG_MEMMAP_UCTD_ADDR,CFG_MEMMAP_UCTD_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].UCTD);
//    	MemMapRead(CFG_MEMMAP_CUB_ADDR,CFG_MEMMAP_CUB_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].CUB);
//    	MemMapRead(CFG_MEMMAP_CUBTD_ADDR,CFG_MEMMAP_CUBTD_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].CUBTD);
//    	MemMapRead(CFG_MEMMAP_LIN_ADDR,CFG_MEMMAP_LIN_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].LIN);
//    	MemMapRead(CFG_MEMMAP_RD0_ADDR,CFG_MEMMAP_RD0_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].RD0);
//    	MemMapRead(CFG_MEMMAP_RD1_ADDR,CFG_MEMMAP_RD1_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].RD1);
//    	MemMapRead(CFG_MEMMAP_RD2_ADDR,CFG_MEMMAP_RD2_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].RD2);
//    	MemMapRead(CFG_MEMMAP_RD3_ADDR,CFG_MEMMAP_RD3_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].RD3);
//    	MemMapRead(CFG_MEMMAP_RU_ADDR,CFG_MEMMAP_RU_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].RU);
//    	MemMapRead(CFG_MEMMAP_RF_ADDR,CFG_MEMMAP_RF_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].RF);
//    	MemMapRead(CFG_MEMMAP_GF_ADDR,CFG_MEMMAP_GF_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].GF);
//    	MemMapRead(CFG_MEMMAP_GFTD_ADDR,CFG_MEMMAP_GFTD_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].GFTD);
//    	MemMapRead(CFG_MEMMAP_GFID_ADDR,CFG_MEMMAP_GFID_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].GFID);
//    	MemMapRead(CFG_MEMMAP_LKW_ADDR,CFG_MEMMAP_LKW_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].LKW);
//    	MemMapRead(CFG_MEMMAP_HKW_ADDR,CFG_MEMMAP_HKW_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].HKW);
//    	MemMapRead(CFG_MEMMAP_HPRTD_ADDR,CFG_MEMMAP_HPRTD_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].HPRTD);
//    	MemMapRead(CFG_MEMMAP_STLP_ADDR,CFG_MEMMAP_STLP_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].STLP);
//    	MemMapRead(CFG_MEMMAP_STLTD_ADDR,CFG_MEMMAP_STLTD_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].STLTD);
//    	MemMapRead(CFG_MEMMAP_STLID_ADDR,CFG_MEMMAP_STLID_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].STLID);
//    	MemMapRead(CFG_MEMMAP_MACCTRL_ADDR,CFG_MEMMAP_MACCTRL_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].MACCTRL);
//    	MemMapRead(CFG_MEMMAP_MACTD_ADDR,CFG_MEMMAP_MACTD_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].MACTD);
//    	MemMapRead(CFG_MEMMAP_ENDIS_ADDR,CFG_MEMMAP_ENDIS_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].ENDIS);
//    	MemMapRead(CFG_MEMMAP_CNFG_ADDR,CFG_MEMMAP_CNFG_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].CNFG);
//    	MemMapRead(CFG_MEMMAP_COMCFG_ADDR,CFG_MEMMAP_COMCFG_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].COMCFG);
//    	MemMapRead(CFG_MEMMAP_CAN_CNFG_ADDR,CFG_MEMMAP_CAN_CNFG_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].CAN_CNFG);
//    	MemMapRead(CFG_MEMMAP_MOD_CNFG_ADDR,CFG_MEMMAP_MOD_CNFG_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].MOD_CNFG);
//    	FactoryCfgObject.InstanceLevel[inst_ix].CMD = 0;
//    	MemMapRead(CFG_MEMMAP_NAME_ADDR,CFG_MEMMAP_NAME_SIZE,&FactoryCfgObject.InstanceLevel[inst_ix].NAME.Data[0]);
//    	FactoryCfgObject.InstanceLevel[inst_ix].NAME.Length = sizeof(FactoryCfgObject.InstanceLevel[inst_ix].NAME.Data);
//    }
//
//    memset(&RealTimeObject, 0, sizeof(RealTimeObject));
//    RealTimeObject.ClassLevel.Revision = 1;
//    GetRealTimeRegisters();
}

static CIP_UDINT local_get_max_wildcard_instance (CIP_UDINT const ClassID)
{
    switch(ClassID)
    {

        case VS_CLASSID_CONTROL_SUPERVISOR: return VS_NUMINST_CONTROL_SUPERVISOR;
        case VS_CLASSID_FACTORY_CFG:        return VS_NUMINST_FACTORY_CFG;
        case VS_CLASSID_REAL_TIME:       return VS_NUMINST_REAL_TIME;
        default: return(0);
    }
}

static void local_load_wildcard_instance (CIP_UDINT const ClassID, CIP_UDINT const InstID)
{
    CIP_UDINT zeroBasedInstId;

    if(InstID == 0)
        return;
    if(InstID > local_get_max_wildcard_instance(ClassID))
        return;
    zeroBasedInstId = (CIP_UDINT)(InstID - 1);

    /* TODO TSB: update faults here */

    switch(ClassID)
    {
        case VS_CLASSID_REAL_TIME:
            memcpy(&RealTimeObject.WildcardInstanceLevel, &RealTimeObject.InstanceLevel[zeroBasedInstId], sizeof(RealTimeObject.WildcardInstanceLevel));
            break;

        default: return;
    }
}

static void local_save_wildcard_instance (CIP_UDINT const ClassID, CIP_UDINT const InstID)
{
    CIP_UDINT zeroBasedInstId;
	CIP_UDINT udintTmp;
	CIP_UINT uintTmp;
	CIP_String_Short_32   string32;

    if(InstID == 0)
        return;
    if(InstID > local_get_max_wildcard_instance(ClassID))
        return;
    zeroBasedInstId = (CIP_UDINT)(InstID - 1);

    switch(ClassID)
    {
    case VS_CLASSID_REAL_TIME:
    	memcpy(&RealTimeObject.InstanceLevel[zeroBasedInstId], &RealTimeObject.WildcardInstanceLevel, sizeof(RealTimeObject.InstanceLevel[0]));
    	break;

    case VS_CLASSID_FACTORY_CFG:
		MemMapRead(CFG_MEMMAP_NAME_ADDR,CFG_MEMMAP_NAME_SIZE,&string32.Data);
		string32.Length = CFG_MEMMAP_NAME_SIZE;
		if( strncmp((char *)string32.Data, (char *)FactoryCfgObject.InstanceLevel[0].NAME.Data,FactoryCfgObject.InstanceLevel[0].NAME.Length) != 0)
		{
			MemMapWrite(CFG_MEMMAP_NAME_ADDR,CFG_MEMMAP_NAME_SIZE,&FactoryCfgObject.InstanceLevel[0].NAME.Data[0],1);
		}

		/*
		 * first check all CIP_UDINT types to see if any have changed
		 */
		MemMapRead(CFG_MEMMAP_LV_ADDR,CFG_MEMMAP_LV_SIZE,&udintTmp);
		if(udintTmp != FactoryCfgObject.InstanceLevel[0].LV)
		{
			MemMapWrite(CFG_MEMMAP_LV_ADDR,CFG_MEMMAP_LV_SIZE,&FactoryCfgObject.InstanceLevel[0].LV,1);
		}

		MemMapRead(CFG_MEMMAP_HV_ADDR,CFG_MEMMAP_HV_SIZE,&udintTmp);
		if(udintTmp != FactoryCfgObject.InstanceLevel[0].HV)
		{
			MemMapWrite(CFG_MEMMAP_HV_ADDR,CFG_MEMMAP_HV_SIZE,&FactoryCfgObject.InstanceLevel[0].HV,1);
		}

		MemMapRead(CFG_MEMMAP_OC_ADDR,CFG_MEMMAP_OC_SIZE,&udintTmp);
		if(udintTmp != FactoryCfgObject.InstanceLevel[0].OC)
		{
			MemMapWrite(CFG_MEMMAP_OC_ADDR,CFG_MEMMAP_OC_SIZE,&FactoryCfgObject.InstanceLevel[0].OC,1);
		}

		MemMapRead(CFG_MEMMAP_UC_ADDR,CFG_MEMMAP_UC_SIZE,&udintTmp);
		if(udintTmp != FactoryCfgObject.InstanceLevel[0].UC)
		{
			MemMapWrite(CFG_MEMMAP_UC_ADDR,CFG_MEMMAP_UC_SIZE,&FactoryCfgObject.InstanceLevel[0].UC,1);
		}

		MemMapRead(CFG_MEMMAP_RD0_ADDR,CFG_MEMMAP_RD0_SIZE,&udintTmp);
		if(udintTmp != FactoryCfgObject.InstanceLevel[0].RD0)
		{
			MemMapWrite(CFG_MEMMAP_RD0_ADDR,CFG_MEMMAP_RD0_SIZE,&FactoryCfgObject.InstanceLevel[0].RD0,1);
		}

		MemMapRead(CFG_MEMMAP_RD1_ADDR,CFG_MEMMAP_RD1_SIZE,&udintTmp);
		if(udintTmp != FactoryCfgObject.InstanceLevel[0].RD1)
		{
			MemMapWrite(CFG_MEMMAP_RD1_ADDR,CFG_MEMMAP_RD1_SIZE,&FactoryCfgObject.InstanceLevel[0].RD1,1);
		}

		MemMapRead(CFG_MEMMAP_RD2_ADDR,CFG_MEMMAP_RD2_SIZE,&udintTmp);
		if(udintTmp != FactoryCfgObject.InstanceLevel[0].RD2)
		{
			MemMapWrite(CFG_MEMMAP_RD2_ADDR,CFG_MEMMAP_RD2_SIZE,&FactoryCfgObject.InstanceLevel[0].RD2,1);
		}

		MemMapRead(CFG_MEMMAP_RD3_ADDR,CFG_MEMMAP_RD3_SIZE,&udintTmp);
		if(udintTmp != FactoryCfgObject.InstanceLevel[0].RD3)
		{
			MemMapWrite(CFG_MEMMAP_RD3_ADDR,CFG_MEMMAP_RD3_SIZE,&FactoryCfgObject.InstanceLevel[0].RD3,1);
		}

		MemMapRead(CFG_MEMMAP_GF_ADDR,CFG_MEMMAP_GF_SIZE,&udintTmp);
		if(udintTmp != FactoryCfgObject.InstanceLevel[0].GF)
		{
			MemMapWrite(CFG_MEMMAP_GF_ADDR,CFG_MEMMAP_GF_SIZE,&FactoryCfgObject.InstanceLevel[0].GF,1);
		}

		MemMapRead(CFG_MEMMAP_LKW_ADDR,CFG_MEMMAP_LKW_SIZE,&udintTmp);
		if(udintTmp != FactoryCfgObject.InstanceLevel[0].LKW)
		{
			MemMapWrite(CFG_MEMMAP_LKW_ADDR,CFG_MEMMAP_LKW_SIZE,&FactoryCfgObject.InstanceLevel[0].LKW,1);
		}

		MemMapRead(CFG_MEMMAP_HKW_ADDR,CFG_MEMMAP_HKW_SIZE,&udintTmp);
		if(udintTmp != FactoryCfgObject.InstanceLevel[0].HKW)
		{
			MemMapWrite(CFG_MEMMAP_HKW_ADDR,CFG_MEMMAP_HKW_SIZE,&FactoryCfgObject.InstanceLevel[0].HKW,1);
		}

		MemMapRead(CFG_MEMMAP_CNFG_ADDR,CFG_MEMMAP_CNFG_SIZE,&udintTmp);
		if(udintTmp != FactoryCfgObject.InstanceLevel[0].CNFG)
		{
			MemMapWrite(CFG_MEMMAP_CNFG_ADDR,CFG_MEMMAP_CNFG_SIZE,&FactoryCfgObject.InstanceLevel[0].CNFG,1);
		}


		/*
		 * now check all the CIP_UINT types to see if any have changed
		 */
		MemMapRead(CFG_MEMMAP_MULT_ADDR,CFG_MEMMAP_MULT_SIZE,&uintTmp);
		if(uintTmp != FactoryCfgObject.InstanceLevel[0].MULT)
		{
			MemMapWrite(CFG_MEMMAP_MULT_ADDR,CFG_MEMMAP_MULT_SIZE,&FactoryCfgObject.InstanceLevel[0].MULT,1);
		}

		MemMapRead(CFG_MEMMAP_PT_ADDR,CFG_MEMMAP_PT_SIZE,&uintTmp);
		if(uintTmp != FactoryCfgObject.InstanceLevel[0].PT)
		{
			MemMapWrite(CFG_MEMMAP_PT_ADDR,CFG_MEMMAP_PT_SIZE,&FactoryCfgObject.InstanceLevel[0].PT,1);
		}

		MemMapRead(CFG_MEMMAP_VUB_ADDR,CFG_MEMMAP_VUB_SIZE,&uintTmp);
		if(uintTmp != FactoryCfgObject.InstanceLevel[0].VUB)
		{
			MemMapWrite(CFG_MEMMAP_VUB_ADDR,CFG_MEMMAP_VUB_SIZE,&FactoryCfgObject.InstanceLevel[0].VUB,1);
		}

		MemMapRead(CFG_MEMMAP_TC_ADDR,CFG_MEMMAP_TC_SIZE,&uintTmp);
		if(uintTmp != FactoryCfgObject.InstanceLevel[0].TC)
		{
			MemMapWrite(CFG_MEMMAP_TC_ADDR,CFG_MEMMAP_TC_SIZE,&FactoryCfgObject.InstanceLevel[0].TC,1);
		}

		MemMapRead(CFG_MEMMAP_UCTD_ADDR,CFG_MEMMAP_UCTD_SIZE,&uintTmp);
		if(uintTmp != FactoryCfgObject.InstanceLevel[0].UCTD)
		{
			MemMapWrite(CFG_MEMMAP_UCTD_ADDR,CFG_MEMMAP_UCTD_SIZE,&FactoryCfgObject.InstanceLevel[0].UCTD,1);
		}

		MemMapRead(CFG_MEMMAP_CUB_ADDR,CFG_MEMMAP_CUB_SIZE,&uintTmp);
		if(uintTmp != FactoryCfgObject.InstanceLevel[0].CUB)
		{
			MemMapWrite(CFG_MEMMAP_CUB_ADDR,CFG_MEMMAP_CUB_SIZE,&FactoryCfgObject.InstanceLevel[0].CUB,1);
		}

		MemMapRead(CFG_MEMMAP_LIN_ADDR,CFG_MEMMAP_LIN_SIZE,&uintTmp);
		if(uintTmp != FactoryCfgObject.InstanceLevel[0].LIN)
		{
			MemMapWrite(CFG_MEMMAP_LIN_ADDR,CFG_MEMMAP_LIN_SIZE,&FactoryCfgObject.InstanceLevel[0].LIN,1);
		}

		MemMapRead(CFG_MEMMAP_RU_ADDR,CFG_MEMMAP_RU_SIZE,&uintTmp);
		if(uintTmp != FactoryCfgObject.InstanceLevel[0].RU)
		{
			MemMapWrite(CFG_MEMMAP_RU_ADDR,CFG_MEMMAP_RU_SIZE,&FactoryCfgObject.InstanceLevel[0].RU,1);
		}

		MemMapRead(CFG_MEMMAP_RF_ADDR,CFG_MEMMAP_RF_SIZE,&udintTmp);
		if(udintTmp != FactoryCfgObject.InstanceLevel[0].RF)
		{
			MemMapWrite(CFG_MEMMAP_RF_ADDR,CFG_MEMMAP_RF_SIZE,&FactoryCfgObject.InstanceLevel[0].RF,1);
		}

		MemMapRead(CFG_MEMMAP_GFTD_ADDR,CFG_MEMMAP_GFTD_SIZE,&udintTmp);
		if(udintTmp != FactoryCfgObject.InstanceLevel[0].GFTD)
		{
			MemMapWrite(CFG_MEMMAP_GFTD_ADDR,CFG_MEMMAP_GFTD_SIZE,&FactoryCfgObject.InstanceLevel[0].GFTD,1);
		}

		MemMapRead(CFG_MEMMAP_HPRTD_ADDR,CFG_MEMMAP_HPRTD_SIZE,&udintTmp);
		if(udintTmp != FactoryCfgObject.InstanceLevel[0].HPRTD)
		{
			MemMapWrite(CFG_MEMMAP_HPRTD_ADDR,CFG_MEMMAP_HPRTD_SIZE,&FactoryCfgObject.InstanceLevel[0].HPRTD,1);
		}

		MemMapRead(CFG_MEMMAP_STLP_ADDR,CFG_MEMMAP_STLP_SIZE,&udintTmp);
		if(udintTmp != FactoryCfgObject.InstanceLevel[0].STLP)
		{
			MemMapWrite(CFG_MEMMAP_STLP_ADDR,CFG_MEMMAP_STLP_SIZE,&FactoryCfgObject.InstanceLevel[0].STLP,1);
		}

		MemMapRead(CFG_MEMMAP_STLTD_ADDR,CFG_MEMMAP_STLTD_SIZE,&udintTmp);
		if(udintTmp != FactoryCfgObject.InstanceLevel[0].STLTD)
		{
			MemMapWrite(CFG_MEMMAP_STLTD_ADDR,CFG_MEMMAP_STLTD_SIZE,&FactoryCfgObject.InstanceLevel[0].STLTD,1);
		}

		MemMapRead(CFG_MEMMAP_STLID_ADDR,CFG_MEMMAP_STLID_SIZE,&udintTmp);
		if(udintTmp != FactoryCfgObject.InstanceLevel[0].STLID)
		{
			MemMapWrite(CFG_MEMMAP_STLID_ADDR,CFG_MEMMAP_STLID_SIZE,&FactoryCfgObject.InstanceLevel[0].STLID,1);
		}

		MemMapRead(CFG_MEMMAP_CMD_ADDR,CFG_MEMMAP_CMD_SIZE,&udintTmp);
		if(udintTmp != FactoryCfgObject.InstanceLevel[0].CMD)
		{
			MemMapWrite(CFG_MEMMAP_CMD_ADDR,CFG_MEMMAP_CMD_SIZE,&FactoryCfgObject.InstanceLevel[0].CMD,1);
		}

    	break;
    default:
    	return;
    }
}

static CIP_ReturnCode local_get_attribute_single(EIP_ControlStruct * const p_eip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse, VS_LOOKUP_TABLE_PTRS const * const p_table_entry)
{
    RTA_UNUSED_PARAM(p_eip_ctrl);
    CIP_ReturnCode rc;

	if(!p_eip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse || !p_table_entry)
	    CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

	/* call function pointer */
	if(!p_table_entry->DataPtr)
	{
		p_MessageRouterResponse->GeneralStatus = CIP_Status_Embedded_service_error;
		return(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
	}

	GetRealTimeRegisters();

	switch(p_table_entry->DataType)
	{
		case CIP_DATA_TYPE_CIP_BOOL:  return(cip_process_write_response_data_with_CIP_BOOL (p_MessageRouterResponse, p_table_entry->DataPtr));
		case CIP_DATA_TYPE_CIP_USINT: return(cip_process_write_response_data_with_CIP_USINT(p_MessageRouterResponse, p_table_entry->DataPtr));
		case CIP_DATA_TYPE_CIP_UINT:  return(cip_process_write_response_data_with_CIP_UINT (p_MessageRouterResponse, p_table_entry->DataPtr));
		case CIP_DATA_TYPE_CIP_UDINT: return(cip_process_write_response_data_with_CIP_UDINT(p_MessageRouterResponse, p_table_entry->DataPtr));
		case CIP_DATA_TYPE_CIP_SINT:  return(cip_process_write_response_data_with_CIP_SINT (p_MessageRouterResponse, p_table_entry->DataPtr));
		case CIP_DATA_TYPE_CIP_INT:   return(cip_process_write_response_data_with_CIP_INT  (p_MessageRouterResponse, p_table_entry->DataPtr));
		case CIP_DATA_TYPE_CIP_DINT:  return(cip_process_write_response_data_with_CIP_DINT (p_MessageRouterResponse, p_table_entry->DataPtr));
		case CIP_DATA_TYPE_CIP_REAL:  return(cip_process_write_response_data_with_CIP_REAL (p_MessageRouterResponse, p_table_entry->DataPtr));		
		case CIP_DATA_TYPE_CIP_WORD:  return(cip_process_write_response_data_with_CIP_WORD (p_MessageRouterResponse, p_table_entry->DataPtr));
		case CIP_DATA_TYPE_CIP_DWORD: return(cip_process_write_response_data_with_CIP_DWORD(p_MessageRouterResponse, p_table_entry->DataPtr));
		case CIP_DATA_TYPE_CIP_SHORT_STRING_32:
			rc = cip_process_write_response_data_with_CIP_SHORT_STRING_32(p_MessageRouterResponse, p_table_entry->DataPtr);
	        if(rc != CIP_RETURNCODE_SUCCESS) CIP_LOG_ERROR_AND_RETURN (rc);
	        CIP_LOG_EXIT_AND_RETURN(CIP_RETURNCODE_SUCCESS);

		default:
			p_MessageRouterResponse->GeneralStatus = CIP_Status_Embedded_service_error;
			return(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
	}

	/* we shouldn't get here */
}

static CIP_ReturnCode local_set_attribute_single(EIP_ControlStruct * const p_eip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse, VS_LOOKUP_TABLE_PTRS const * const p_table_entry)
{
    RTA_Size_Type expected_size_in_bytes;

	if(!p_eip_ctrl || !p_MessageRouterRequest || !p_MessageRouterResponse || !p_table_entry)
	    CIP_LOG_ERROR_AND_RETURN(CIP_RETURNCODE_ERROR_INVALID_PARAMETER);

	if(!p_table_entry->Settable)
	{
		p_MessageRouterResponse->GeneralStatus = CIP_Status_Attribute_not_settable;
		return(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
	}

	/* error if no data pointer present */
	if(!p_table_entry->DataPtr)
	{
		p_MessageRouterResponse->GeneralStatus = CIP_Status_Embedded_service_error;
		return(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
	}

	if(p_MessageRouterRequest->Request_Data.Type == CIP_DATA_TYPE_NO_DATA)
	{
		p_MessageRouterResponse->GeneralStatus = CIP_Status_Not_enough_data;
		return(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
	}

	if(p_MessageRouterRequest->Request_Data.Type != p_table_entry->DataType)
	{
        if(p_MessageRouterRequest->Request_Data.Type == CIP_DATA_TYPE_CIP_Opaque_String)
        {
            /* data not decoded properly - if this is expected, change the logic to handle that set */

            expected_size_in_bytes = local_get_expected_size_in_bytes(p_table_entry);

            if(expected_size_in_bytes > p_MessageRouterRequest->Request_Data.Value.value_Opaque_String.Length)
			{
				p_MessageRouterResponse->GeneralStatus = CIP_Status_Not_enough_data;
				return(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
			}

            else if(expected_size_in_bytes < p_MessageRouterRequest->Request_Data.Value.value_Opaque_String.Length)
			{
				p_MessageRouterResponse->GeneralStatus = CIP_Status_Too_much_data;
				return(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
			}
        }
        else
        {
        	p_MessageRouterResponse->GeneralStatus = CIP_Status_Embedded_service_error;
        	return(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
        }
	}

	if((!p_table_entry->MinPtr) || (!p_table_entry->MaxPtr))
	{
		p_MessageRouterResponse->GeneralStatus = CIP_Status_Embedded_service_error;
		return(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
	}

	switch(p_table_entry->DataType)
	{
		case CIP_DATA_TYPE_CIP_BOOL:
			if(*(CIP_BOOL *)p_table_entry->MinPtr != *(CIP_BOOL *)p_table_entry->MaxPtr)
			{
				if( (p_MessageRouterRequest->Request_Data.Value.value_BOOL < *(CIP_BOOL *)p_table_entry->MinPtr) ||
					(p_MessageRouterRequest->Request_Data.Value.value_BOOL > *(CIP_BOOL *)p_table_entry->MaxPtr))
				{
					p_MessageRouterResponse->GeneralStatus = CIP_Status_Invalid_attribute_value;
					return(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
				}
			}
			*(CIP_BOOL *)p_table_entry->DataPtr = p_MessageRouterRequest->Request_Data.Value.value_BOOL;
			break;

		case CIP_DATA_TYPE_CIP_USINT:
			if(*(CIP_USINT *)p_table_entry->MinPtr != *(CIP_USINT *)p_table_entry->MaxPtr)
			{
				if( (p_MessageRouterRequest->Request_Data.Value.value_USINT < *(CIP_USINT *)p_table_entry->MinPtr) ||
					(p_MessageRouterRequest->Request_Data.Value.value_USINT > *(CIP_USINT *)p_table_entry->MaxPtr))
				{
					p_MessageRouterResponse->GeneralStatus = CIP_Status_Invalid_attribute_value;
					return(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
				}
			}
			*(CIP_USINT *)p_table_entry->DataPtr = p_MessageRouterRequest->Request_Data.Value.value_USINT;
			break;

		case CIP_DATA_TYPE_CIP_UINT:
			if(*(CIP_UINT *)p_table_entry->MinPtr != *(CIP_UINT *)p_table_entry->MaxPtr)
			{
				if( (p_MessageRouterRequest->Request_Data.Value.value_UINT < *(CIP_UINT *)p_table_entry->MinPtr) ||
					(p_MessageRouterRequest->Request_Data.Value.value_UINT > *(CIP_UINT *)p_table_entry->MaxPtr))
				{
					p_MessageRouterResponse->GeneralStatus = CIP_Status_Invalid_attribute_value;
					return(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
				}
			}
			*(CIP_UINT *)p_table_entry->DataPtr = p_MessageRouterRequest->Request_Data.Value.value_UINT;
			break;

		case CIP_DATA_TYPE_CIP_UDINT:
			if(*(CIP_UDINT *)p_table_entry->MinPtr != *(CIP_UDINT *)p_table_entry->MaxPtr)
			{
				if( (p_MessageRouterRequest->Request_Data.Value.value_UDINT < *(CIP_UDINT *)p_table_entry->MinPtr) ||
					(p_MessageRouterRequest->Request_Data.Value.value_UDINT > *(CIP_UDINT *)p_table_entry->MaxPtr))
				{
					p_MessageRouterResponse->GeneralStatus = CIP_Status_Invalid_attribute_value;
					return(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
				}
			}
			*(CIP_UDINT *)p_table_entry->DataPtr = p_MessageRouterRequest->Request_Data.Value.value_UDINT;
			break;

		case CIP_DATA_TYPE_CIP_SINT:  
			if(*(CIP_SINT *)p_table_entry->MinPtr != *(CIP_SINT *)p_table_entry->MaxPtr)
			{
				if( (p_MessageRouterRequest->Request_Data.Value.value_SINT < *(CIP_SINT *)p_table_entry->MinPtr) ||
					(p_MessageRouterRequest->Request_Data.Value.value_SINT > *(CIP_SINT *)p_table_entry->MaxPtr))
				{
					p_MessageRouterResponse->GeneralStatus = CIP_Status_Invalid_attribute_value;
					return(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
				}
			}
			*(CIP_SINT *)p_table_entry->DataPtr = p_MessageRouterRequest->Request_Data.Value.value_SINT;
            break;

		case CIP_DATA_TYPE_CIP_INT:   
			if(*(CIP_INT *)p_table_entry->MinPtr != *(CIP_INT *)p_table_entry->MaxPtr)
			{
				if( (p_MessageRouterRequest->Request_Data.Value.value_INT < *(CIP_INT *)p_table_entry->MinPtr) ||
					(p_MessageRouterRequest->Request_Data.Value.value_INT > *(CIP_INT *)p_table_entry->MaxPtr))
				{
					p_MessageRouterResponse->GeneralStatus = CIP_Status_Invalid_attribute_value;
					return(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
				}
			}
			*(CIP_INT *)p_table_entry->DataPtr = p_MessageRouterRequest->Request_Data.Value.value_INT;
            break;

		case CIP_DATA_TYPE_CIP_DINT:  
			if(*(CIP_DINT *)p_table_entry->MinPtr != *(CIP_DINT *)p_table_entry->MaxPtr)
			{
				if( (p_MessageRouterRequest->Request_Data.Value.value_DINT < *(CIP_DINT *)p_table_entry->MinPtr) ||
					(p_MessageRouterRequest->Request_Data.Value.value_DINT > *(CIP_DINT *)p_table_entry->MaxPtr))
				{
					p_MessageRouterResponse->GeneralStatus = CIP_Status_Invalid_attribute_value;
					return(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
				}
			}
			*(CIP_DINT *)p_table_entry->DataPtr = p_MessageRouterRequest->Request_Data.Value.value_DINT;
            break;

		case CIP_DATA_TYPE_CIP_REAL:
            /*lint -e777 *//* test for float == */
			if(*(CIP_REAL *)p_table_entry->MinPtr != *(CIP_REAL *)p_table_entry->MaxPtr) 
			{
				if( (p_MessageRouterRequest->Request_Data.Value.value_REAL < *(CIP_REAL *)p_table_entry->MinPtr) ||
					(p_MessageRouterRequest->Request_Data.Value.value_REAL > *(CIP_REAL *)p_table_entry->MaxPtr))
				{
					p_MessageRouterResponse->GeneralStatus = CIP_Status_Invalid_attribute_value;
					return(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
				}
			}

			*(CIP_REAL *)p_table_entry->DataPtr = p_MessageRouterRequest->Request_Data.Value.value_REAL;
			break;

		case CIP_DATA_TYPE_CIP_WORD:  
			if(*(CIP_WORD *)p_table_entry->MinPtr != *(CIP_WORD *)p_table_entry->MaxPtr)
			{
				if( (p_MessageRouterRequest->Request_Data.Value.value_WORD < *(CIP_WORD *)p_table_entry->MinPtr) ||
					(p_MessageRouterRequest->Request_Data.Value.value_WORD > *(CIP_WORD *)p_table_entry->MaxPtr))
				{
					p_MessageRouterResponse->GeneralStatus = CIP_Status_Invalid_attribute_value;
					return(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
				}
			}
			*(CIP_WORD *)p_table_entry->DataPtr = p_MessageRouterRequest->Request_Data.Value.value_WORD;
            break;

		case CIP_DATA_TYPE_CIP_DWORD: 
			if(*(CIP_DWORD *)p_table_entry->MinPtr != *(CIP_DWORD *)p_table_entry->MaxPtr)
			{
				if( (p_MessageRouterRequest->Request_Data.Value.value_DWORD < *(CIP_DWORD *)p_table_entry->MinPtr) ||
					(p_MessageRouterRequest->Request_Data.Value.value_DWORD > *(CIP_DWORD *)p_table_entry->MaxPtr))
				{
					p_MessageRouterResponse->GeneralStatus = CIP_Status_Invalid_attribute_value;
					return(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
				}
			}
			*(CIP_DWORD *)p_table_entry->DataPtr = p_MessageRouterRequest->Request_Data.Value.value_DWORD;
            break;

		case CIP_DATA_TYPE_CIP_SHORT_STRING_32:
			if( p_MessageRouterRequest->Request_Data.Type == CIP_DATA_TYPE_CIP_Opaque_String)
			{
				CIP_String_Short_32 *dest,*source;
				dest = (CIP_String_Short_32 *)p_table_entry->DataPtr;
				source = (CIP_String_Short_32 *)p_MessageRouterRequest->Request_Data.Value.value_Opaque_String.pData;
				memcpy( dest->Data, source->Data, source->Length);
				dest->Length = strlen(source->Data);
			}
			else
			{
				return(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
			}
			break;

		default:
			p_MessageRouterResponse->GeneralStatus = CIP_Status_Embedded_service_error;
			return(CIP_RETURNCODE_MSGRTR_USE_GENERAL_STATUS);
	}

	return(CIP_RETURNCODE_SUCCESS);
}

static RTA_Size_Type local_get_expected_size_in_bytes (VS_LOOKUP_TABLE_PTRS const * const p_table_entry)
{
    if(!p_table_entry)
        return(0);

	switch(p_table_entry->DataType)
	{
		case CIP_DATA_TYPE_CIP_BOOL:    return(1);
		case CIP_DATA_TYPE_CIP_USINT:   return(1);
		case CIP_DATA_TYPE_CIP_UINT:    return(2);
    	case CIP_DATA_TYPE_CIP_UDINT:   return(4);
		case CIP_DATA_TYPE_CIP_SINT:    return(1);
		case CIP_DATA_TYPE_CIP_INT:     return(2);  
		case CIP_DATA_TYPE_CIP_DINT:    return(4); 
		case CIP_DATA_TYPE_CIP_REAL:    return(4);
		case CIP_DATA_TYPE_CIP_WORD:    return(2);
		case CIP_DATA_TYPE_CIP_DWORD:   return(4);
		case CIP_DATA_TYPE_CIP_SHORT_STRING_32: return (33);
		default:    return(0);
	}
}

/*lint -restore -e788 */ /* enum constant not in switch */

/* *********** */
/* END OF FILE */
/* *********** */
