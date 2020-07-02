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

 #ifndef __CIP_USEROBJ_VENDSPEC_H__
 #define __CIP_USEROBJ_VENDSPEC_H__

CIP_ReturnCode cip_userobj_vendspec_init (CIP_InitType const init_type);
CIP_ReturnCode cip_userobj_vendspec_process (RTA_U32 const ms_since_last_call);
CIP_ReturnCode cip_userobj_vendspec_decode (CIP_ControlStruct const * const p_cip_ctrl, CIP_UINT const expected_length, CIP_MessageRouterRequest * const p_MessageRouterRequest);
CIP_ReturnCode cip_userobj_vendspec_process_service (EIP_ControlStruct * const p_eip_ctrl, CIP_MessageRouterRequest const * const p_MessageRouterRequest, CIP_MessageRouterResponse * const p_MessageRouterResponse);

/* ******************************************************** */
/*                  START OF USER DEFINES                   */
/* ******************************************************** */
#define VS_INST_WILDCARD 0xFFFFFFFF
#define VS_ATTR_UNUSED   0xFFFFFFFF

/* ATTRIBUTE LOOKUP TABLE */
typedef struct
{
	CIP_BOOL        Settable;
	CIP_UDINT       ClassID;
	CIP_UDINT       InstID;
	CIP_UDINT       AttrID;
	CIP_DATA_TYPE   DataType;
	void            *MinPtr;
	void            *MaxPtr;
	void            *DataPtr;
} VS_LOOKUP_TABLE_PTRS;

/* ************************************************************************ */
/*              Class 41 (0x29) Control Supervisor Object                   */
/* ************************************************************************ */
#define VS_CLASSID_CONTROL_SUPERVISOR   0x29
#define VS_NUMINST_CONTROL_SUPERVISOR   1
typedef struct
{
    CIP_UINT    Revision;
}VS_ControlSupervisorObject_ClassLevel;

typedef struct
{
    CIP_BOOL    Run1;   
    CIP_BOOL    Running1;
    CIP_BOOL    Faulted;
    CIP_BOOL    Warning;
    CIP_BOOL    FaultRst;
    CIP_BOOL    CtrlFromNet;
    CIP_BOOL    ForceFault;
}VS_ControlSupervisorObject_InstanceLevel;

typedef struct
{
    VS_ControlSupervisorObject_ClassLevel      ClassLevel;
    VS_ControlSupervisorObject_InstanceLevel   InstanceLevel[VS_NUMINST_CONTROL_SUPERVISOR];
}VS_ControlSupervisorObject;

/* ************************************************************************ */
/*              Class 44 (0x2C) Overload Object                             */
/* ************************************************************************ */
#define VS_CLASSID_OVERLOAD     0x2C
#define VS_NUMINST_OVERLOAD     1
typedef struct
{
    CIP_UINT    Revision;
}VS_OverloadObject_ClassLevel;

typedef struct
{
    CIP_INT     TripFLCSet;
}VS_OverloadObject_InstanceLevel;

typedef struct
{
    VS_OverloadObject_ClassLevel      ClassLevel;
    VS_OverloadObject_InstanceLevel   InstanceLevel[VS_NUMINST_CONTROL_SUPERVISOR];
}VS_OverloadObject;

/* ************************************************************************ */
/*              Class 100 (0x64) Configuration Object                       */
/* ************************************************************************ */
#define VS_CLASSID_FACTORY_CFG  100
#define VS_NUMINST_FACTORY_CFG  1
typedef struct
{
    CIP_UINT    Revision;
}VS_FactoryCfgObject_ClassLevel;

typedef struct
{
	//                     attribute
    CIP_UINT    MULT;	// 1
    CIP_UINT    PT;		// 2
    CIP_UDINT   LV;		// 3
    CIP_UDINT   HV;		// 4
    CIP_UINT    VUB;	// 5
    CIP_UINT    TC;		// 6
    CIP_UDINT   OC;		// 7
	CIP_UDINT	UC; 	// 8
	CIP_UINT	UCTD; 	// 9
	CIP_UINT	CUB; 	// 10
	CIP_UINT	CUBTD; 	// 11
	CIP_UINT	LIN; 	// 12
    CIP_UDINT   RD0; 	// 13
    CIP_UDINT   RD1;	// 14
    CIP_UDINT   RD2;	// 15
    CIP_UDINT   RD3;	// 16
    CIP_UINT    RU;		// 17
    CIP_UINT    RF;		// 18
    CIP_UDINT   GF;		// 19
    CIP_UDINT   GFTD;	// 20
    CIP_UDINT   GFID;	// 21
    CIP_UDINT   LKW;	// 22
    CIP_UDINT   HKW;	// 23
    CIP_UINT    HPRTD;	// 24
    CIP_UINT    STLP;	// 25
    CIP_UINT    STLTD;	// 26
    CIP_UINT    STLID;	// 27
    CIP_UINT    MACCTRL;// 28
    CIP_UINT    MACTD;	// 29
    CIP_UDINT   ENDIS;	// 30
    CIP_UDINT   CNFG;	// 31
    CIP_UDINT   COMCFG;	// 32
    CIP_UDINT   CAN_CNFG;//33
    CIP_UDINT   MOD_CNFG;//34
    CIP_String_Short_32    NAME;//35
    CIP_UINT    CMD;	// 36
}VS_FactoryCfgObject_InstanceLevel;

typedef struct
{
    VS_FactoryCfgObject_ClassLevel      ClassLevel;
    VS_FactoryCfgObject_InstanceLevel   InstanceLevel[VS_NUMINST_FACTORY_CFG];
}VS_FactoryCfgObject;

/* ************************************************************************ */
/*              Class 101 (0x65) Real Time Object                           */
/* ************************************************************************ */
#define VS_CLASSID_REAL_TIME 101
#define VS_NUMINST_REAL_TIME 10
typedef struct
{
    CIP_UINT    Revision;
}VS_RealTimeObject_ClassLevel;


typedef struct
{
    CIP_UDINT         	MST;		// 1
    CIP_UDINT        	MRT;		// 2
    CIP_UINT          	SCNT;		// 3
    CIP_UDINT        	FAULT_STAT;	// 4
	CIP_UDINT        	WARN_STAT;	// 5
	CIP_UINT          	FAULT_CODE;	// 6
	CIP_UDINT        	RTDT;		// 7
	CIP_UDINT        	RDR;		// 8
	CIP_UINT          	TCR;		// 9
	CIP_UINT          	MLF;		// 10
	CIP_UINT          	SEQ;		// 11
	CIP_UINT          	VUBM;		// 12
	CIP_UINT          	CUBM;		// 13
	CIP_UDINT        	V1;			// 14
	CIP_UDINT        	V2;			// 15
	CIP_UDINT        	V3;			// 16
	CIP_UDINT        	I1;			// 17
	CIP_UDINT        	I2;			// 18
	CIP_UDINT        	I3;			// 19
	CIP_UDINT          	P1;			// 20
	CIP_UDINT          	P2;			// 21
	CIP_UDINT          	P3;			// 22
	CIP_UINT        	PF1;		// 23
	CIP_UINT        	PF2;		// 24
	CIP_UINT        	PF3;		// 25
	CIP_UDINT        	IGF;		// 26
	CIP_UINT          	PTC;		// 27
	CIP_UINT          	MRELAY;		// 28
	CIP_UINT          	ARELAY;		// 29

}VS_RealTimeObject_InstanceLevel;

typedef struct
{
    VS_RealTimeObject_ClassLevel    ClassLevel;
    VS_RealTimeObject_InstanceLevel WildcardInstanceLevel;  /* stored instance to simplify lookup table */
    VS_RealTimeObject_InstanceLevel InstanceLevel[VS_NUMINST_REAL_TIME];
}VS_RealTimeObject;

#endif /* __CIP_USEROBJ_VENDSPEC_H__ */

/* *********** */
/* END OF FILE */
/* *********** */
