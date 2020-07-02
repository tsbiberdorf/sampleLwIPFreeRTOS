/*
 * CommonComm.h
 *
 *  Created on: Jun 29, 2015
 *      Author: AMcguire
 */

#ifndef SOURCES_COMMUNICATIONS_COMMONCOMM_H_
#define SOURCES_COMMUNICATIONS_COMMONCOMM_H_

#include <stdint.h>

/**
 * Fault / Hold-off Masks
 *
 * @note ARD[2..0]
 * 	0 -> inactive
 * 	1 -> RD0 timer running
 * 	2 -> RD1 timer running
 * 	3 -> RD2 timer running
 * 	4 -> RD3 timer running
 */
typedef enum _FaultHoldoffMasks_e
{
	eFaultmNoFault 	= 0x00000000, /**< no fault */
	eFaultmOCF 		= 0x00000001, /**< Tripped on over current */
	eFaultmUCF 		= 0x00000002, /**< Tripped on under current */
	eFaultmCUBF 	= 0x00000004, /**< Tripped on current unbalance */
	eFaultmCSPF 	= 0x00000008, /**< Tripped on current single-phasing */
	eFaultmCTCF 	= 0x00000010, /**< Tripped on contactor failure */
	eFaultmGFF 		= 0x00000020, /**< Tripped on ground fault */
	eFaultmHPF 		= 0x00000040, /**< Tripped on High Power Fault */
	eFaultmLPF 		= 0x00000080, /**< Tripped on Low Power Fault */
	eFaultmLCVF 	= 0x00000100, /**< Tripped on low control voltage fault */
	eFaultmPTCF 	= 0x00000200, /**< Tripped on PTC fault */
	eFaultmRMTF 	= 0x00000400, /**< Tripped triggered from remote source */
	eFaultmLIN 		= 0x00000800, /**< Tripped on linear over current */
	eFaultmSTALL	= 0x00001000, /**< Tripped on motor stall */
	eFaultmLVH 		= 0x00010000, /**< Hold-off from low voltage */
	eFaultmARD0		= 0x00010000, /**< Active Restart Delay Field Bit 0 */
	eFaultmHVH 		= 0x00020000, /**< Hold-off from high voltage */
	eFaultmARD1		= 0x00020000, /**< Active Restart Delay Field Bit 1 */
	eFaultmVUBH 	= 0x00040000, /**< Hold-off from voltage unbalance voltage */
	eFaultmARD2		= 0x00040000, /**< Active Restart Delay Field Bit 2 */
	eFaultmSEQ  	= 0x00080000, /**< Hold-off from voltage sequence */
	eFaultmUNDEF 	= 0x00100000, /**< undefined Tripped condition */
	eFaultmPTCSHORT	= 0x00200000, /**< Tripped on SHORT PTC fault */
	eFaultmPTCOPEN 	= 0x00400000, /**< Tripped on OPEN PTC fault */
	eFaultmGFALARM 	= 0x00800000, /**< Ground Fault Alarm */
	eFaultmFMEA 	= 0x01000000, /**< MP8000 fault requires replacement */
	eFaultmMANR  	= 0x02000000, /**< Manual restart required */
	eFaultmCURMODE 	= 0x04000000, /**< Enter Current monitor only mode */
	eFaultmNORMODE 	= 0x08000000, /**< Exit Current monitor only mode, return to normal measurement mode*/
}eFaultHoldoffMasks_t;

/**
 * Fault / Hold-off Codes
 */
typedef enum _FaultHoldoffCodes_e
{
	eFaultcNoFault 	= 0, 		/**< no fault */
	eFaultcOCF 		= 1, 		/**< Tripped on over current */
	eFaultcUCF 		= 2, 		/**< Tripped on under current */
	eFaultcCUBF 	= 3, 		/**< Tripped on current unbalance */
	eFaultcCSPF 	= 4, 		/**< Tripped on current single-phasing */
	eFaultcCTCF 	= 5, 		/**< Tripped on contactor failure */
	eFaultcGFF 		= 6, 		/**< Tripped on ground fault */
	eFaultcHPF 		= 7, 		/**< Tripped on High Power Fault */
	eFaultcLPF 		= 8, 		/**< Tripped on Low Power Fault */
	eFaultcLCVF 	= 9, 		/**< Tripped on low control voltage fault */
	eFaultcPTCF 	= 10, 		/**< Tripped on PTC fault */
	eFaultcRMTF 	= 11, 		/**< Tripped triggered from remote source */
	eFaultcLINF 	= 12, 		/**< Tripped on linear over current fault*/
	eFaultcSTALLF 	= 13, 		/**< Tripped on motor stall fault*/
	eFaultcPTCSHORT	= 14, 		/**< Tripped on PTC Short fault */
	eFaultcPTCOPEN 	= 15, 		/**< Tripped on PTC Open fault */
	eFaultcGFFAlarm	= 16, 		/**< Tripped on ground fault alarm */
	eFaultcLVH 		= 100, 		/**< Hold-off from low voltage */
	eFaultcHVH 		= 101, 		/**< Hold-off from high voltage */
	eFaultcVUBH 	= 102, 		/**< Hold-off from voltage unbalance voltage */
	eFaultcSEQ  	= 103, 		/**< Hold-off from voltage sequence */
	eFaultcMANR		= 104,		/**< Forced trip condition requested */
	eFaultcCURMODE	= 105,		/**< Forced trip enable current only mode */
	eFaultcNORMODE	= 106,		/**< Forced trip disable current only mode run in normal mode */
	eFaultcFW  		= 0x1000,	/**< Firmware Update just completed */
	eFaultcFMEA  	= 0x1001,	/**< FMEA fault detected */
	eFaultcEXCP		= 0x1002,	/**< Exception event */
	eFaultcCurrent	= 0x2000,	/**< Used by cntl logic to signal current present for period of time */
	eFaultcNCurrent	= 0x2001,	/**< Used by cntl logic to signal no current present */
	eFaultcUNDEF 	= 0xEEEE,	/**< undefined Tripped condition */
}eFaultHoldoffCodes_t;

/*!
 * Determines if two regions of memory overlap.
 * (so, for example, if a memory map write would touch a certian region of memory)
 * sm - start of first region
 * em - end of first region
 * sb - start of the second region
 * eb - end of the second region
 *
 */
#define IS_OVERLAPPED(sm, em, sb, eb)  (((sb) >= (sm) && (sb) <  (em)) || \
										((eb) >  (sm) && (eb) <= (em)) || \
										((sb) <  (sm) && (eb) >= (em)))

/**
 * @note Factory Default Levels
 */
#define CFG_MEMMAP_FW_REV_DEFAULT	((MAJOR_VERSION*0x1000000) | (MINOR_VERSION*0x10000) | (VERSION_VERSION * 0x100) | REVISION_VERSION)
#define CFG_MEMMAP_MULT_DEFAULT		(1)
#define CFG_MEMMAP_PC_DEFAULT		(8003)
#define CFG_MEMMAP_PT_DEFAULT		(1)
#define CFG_MEMMAP_LV_DEFAULT		(60000UL)
#define CFG_MEMMAP_HV_DEFAULT		(00000UL)
#define CFG_MEMMAP_VUB_DEFAULT		(500)
#define CFG_MEMMAP_TC_DEFAULT		(5)
#define CFG_MEMMAP_OC_DEFAULT		(1000)
#define CFG_MEMMAP_UC_DEFAULT		(500)
#define CFG_MEMMAP_UCTD_DEFAULT		(5)
#define CFG_MEMMAP_CUB_DEFAULT		(700)
#define CFG_MEMMAP_CUBTD_DEFAULT	(30)
#define CFG_MEMMAP_EIPTO_DEFAULT	(120) // githug issue #52
#define CFG_MEMMAP_LIN_DEFAULT		(0)
#define CFG_MEMMAP_RD0_DEFAULT		(0)
#define CFG_MEMMAP_RD1_DEFAULT		(0)
#define CFG_MEMMAP_RD2_DEFAULT		(300)
#define CFG_MEMMAP_RD3_DEFAULT		(300)
#define CFG_MEMMAP_RU_DEFAULT		(1)
#define CFG_MEMMAP_RF_DEFAULT		(1)
#define CFG_MEMMAP_GF_DEFAULT		(0)
#define CFG_MEMMAP_GFTD_DEFAULT		(50)
#define CFG_MEMMAP_ENDIS_DEFAULT	(0x0000000)
#define CFG_MEMMAP_RPWRNVM_DEFAULT	(0x0000000)
#define CFG_MEMMAP_IPWRNVM_DEFAULT	(0x0000000)
#define CFG_MEMMAP_LKW_DEFAULT		(0UL)
#define CFG_MEMMAP_HKW_DEFAULT		(0UL)
#define CFG_MEMMAP_HPRTD_DEFAULT	(60)
#define CFG_MEMMAP_CNFG_DEFAULT		(0x6001) // default RD0 on power on, RD1 when zero current and GF to motor relay
#define CFG_MEMMAP_STLTD_DEFAULT	(0x5)
#define CFG_MEMMAP_STLID_DEFAULT	(0x0)
#define CFG_MEMMAP_STLP_DEFAULT		(0)
#define CFG_MEMMAP_MACTD_DEFAULT	(0)
#define CFG_MEMMAP_MOD_CNFG_DEFAULT (0xFF0BFFED)
#define CFG_MEMMAP_MACCTRL_DEFAULT	(0)
#define CFG_MEMMAP_IACAL_DEFAULT	(0xde808080)
#define CFG_MEMMAP_IBCAL_DEFAULT	(19765921UL) // LINEI
#define CFG_MEMMAP_ICCAL_DEFAULT	(145)
#define CFG_MEMMAP_V1CAL_DEFAULT	(6588623UL) // PWRI
#define CFG_MEMMAP_V2CAL_DEFAULT	(4055399UL) // PWRV
#define CFG_MEMMAP_V3CAL_DEFAULT	(0x80)
#define CFG_MEMMAP_GFCAL_DEFAULT	(398)
#define CFG_MEMMAP_SIP_DEFAULT		(0xFFFFFFFF) // static IP address of 255.255.255.255 DHCP operation
#define CFG_MEMMAP_SIP_STATIC_IP	(0x6401A8C0) // static IP address of 192.168.1.100
#define CFG_MEMMAP_MRTNVM_DEFAULT	(0)
#define CFG_MEMMAP_MSTNVM_DEFAULT	(0)
#define CFG_MEMMAP_SCNTNVM_DEFAULT  (0)
#define CFG_MEMMAP_CFP_DEFAULT 		(100) // default cooling factor 100% of heating factor
#define CONTACTOR_FAILURE_TRIP_DELAY (2)
#define CURRENT_PHASE_TRIP_DELAY (2)
#define PTC_FAILURE_TRIP_DELAY (2)

#define CFG_MEMMAP_CFP_MAX_VALUE 	(1000) // maximum allowed CFP is 1000% of heating factor
#define CFG_MEMMAP_CFP_MIN_VALUE 	(10) // maximum allowed CFP is 10% of heating factor

/**
 * @note Configuration Register Sizes
 */
#define CFG_MEMMAP_FW_REV_SIZE		(0x0004)
#define CFG_MEMMAP_PC_SIZE			(0x0002)
#define CFG_MEMMAP_MULT_SIZE		(0x0002)
#define CFG_MEMMAP_PT_SIZE			(0x0002)
#define CFG_MEMMAP_LV_SIZE			(0x0004)
#define CFG_MEMMAP_HV_SIZE			(0x0004)
#define CFG_MEMMAP_VUB_SIZE			(0x0002)
#define CFG_MEMMAP_TC_SIZE			(0x0002)
#define CFG_MEMMAP_OC_SIZE			(0x0004)
#define CFG_MEMMAP_UC_SIZE			(0x0004)
#define CFG_MEMMAP_UCTD_SIZE		(0x0002)
#define CFG_MEMMAP_CUB_SIZE			(0x0002)
#define CFG_MEMMAP_CUBTD_SIZE		(0x0002)
#define CFG_MEMMAP_EIPTO_SIZE		(0x0002) // Github issue #52
#define CFG_MEMMAP_LIN_SIZE			(0x0002)
#define CFG_MEMMAP_RD0_SIZE			(0x0004)
#define CFG_MEMMAP_RD1_SIZE			(0x0004)
#define CFG_MEMMAP_RD2_SIZE			(0x0004)
#define CFG_MEMMAP_RD3_SIZE			(0x0004)
#define CFG_MEMMAP_RU_SIZE			(0x0002)
#define CFG_MEMMAP_RF_SIZE			(0x0002)
#define CFG_MEMMAP_GF_SIZE			(0x0004)
#define CFG_MEMMAP_GFTD_SIZE		(0x0002)
#define CFG_MEMMAP_GFID_SIZE		(0x0002)
#define CFG_MEMMAP_LKW_SIZE			(0x0004)
#define CFG_MEMMAP_HKW_SIZE			(0x0004)
#define CFG_MEMMAP_HPRTD_SIZE		(0x0002)
#define CFG_MEMMAP_STLP_SIZE		(0x0002)
#define CFG_MEMMAP_STLTD_SIZE		(0x0002)
#define CFG_MEMMAP_STLID_SIZE		(0x0002)
#define CFG_MEMMAP_MACCTRL_SIZE		(0x0002)
#define CFG_MEMMAP_MACTD_SIZE		(0x0002)
#define CFG_MEMMAP_ENDIS_SIZE		(0x0004)
#define CFG_MEMMAP_RPWR_SIZE		(0x0004)
#define CFG_MEMMAP_CNFG_SIZE		(0x0004)
#define CFG_MEMMAP_GATEIP_SIZE		(0x0004)
#define CFG_MEMMAP_COMCFG_SIZE		(0x0004)
#define CFG_MEMMAP_SUBMASKIP_SIZE   (0x0004)
#define CFG_MEMMAP_CAN_CNFG_SIZE	(0x0004)
#define CFG_MEMMAP_MOD_CNFG_SIZE	(0x0004)
#define CFG_MEMMAP_IPWR_SIZE        (0x0004)
#define CFG_MEMMAP_NAME_SIZE		(0x000C)
#define CFG_MEMMAP_CMD_SIZE			(0x0002)
#define CFG_MEMMAP_V1CAL_SIZE		(0x0004)
#define CFG_MEMMAP_V2CAL_SIZE		(0x0004)
#define CFG_MEMMAP_V3CAL_SIZE		(0x0004)
#define CFG_MEMMAP_IACAL_SIZE		(0x0004)
#define CFG_MEMMAP_IBCAL_SIZE		(0x0004)
#define CFG_MEMMAP_ICCAL_SIZE		(0x0004)
#define CFG_MEMMAP_GFCAL_SIZE		(0x0004)
#define CFG_MEMMAP_PASSKEY_SIZE		(0x0040)
#define CFG_MEMMAP_XXXXXXX_SIZE		(0x000C)
#define CFG_MEMMAP_SIP_SIZE			(0x0004)
#define CFG_MEMMAP_MSTNVM_SIZE		(0x0004)
#define CFG_MEMMAP_MRTNVM_SIZE		(0x0004)
#define CFG_MEMMAP_SCNTNVM_SIZE		(0x0002)
#define CFG_MEMMAP_CFP_SIZE			(0x0002)
#define CFG_MEMMAP_TCRP_SIZE		(0x0002)
#define CFG_MEMMAP_RPWRNVM_SIZE        (0x0004)
#define CFG_MEMMAP_IPWRNVM_SIZE        (0x0004)

/**
 * @note RealTime Register Sizes
 */
#define CFG_MEMMAP_MST_SIZE			(0x0004)
#define CFG_MEMMAP_MRT_SIZE			(0x0004)
#define CFG_MEMMAP_SCNT_SIZE		(0x0002)
#define CFG_MEMMAP_FAULT_STAT_SIZE 	(0x0004)
#define CFG_MEMMAP_WARN_STAT_SIZE	(0x0004)
#define CFG_MEMMAP_FAULT_CODE_SIZE	(0x0002)
#define CFG_MEMMAP_RTDT_SIZE 		(0x0004)
#define CFG_MEMMAP_RDR_SIZE			(0x0004)
#define CFG_MEMMAP_TCR_SIZE			(0x0002)
#define CFG_MEMMAP_MLF_SIZE			(0x0002)
#define CFG_MEMMAP_SEQ_SIZE			(0x0002)
#define CFG_MEMMAP_VUBM_SIZE		(0x0002)
#define CFG_MEMMAP_CUBM_SIZE		(0x0002)
#define CFG_MEMMAP_V1_SIZE			(0x0004)
#define CFG_MEMMAP_V2_SIZE			(0x0004)
#define CFG_MEMMAP_V3_SIZE			(0x0004)
#define CFG_MEMMAP_I1_SIZE			(0x0004)
#define CFG_MEMMAP_I2_SIZE			(0x0004)
#define CFG_MEMMAP_I3_SIZE			(0x0004)
#define CFG_MEMMAP_P1_SIZE			(0x0004)
#define CFG_MEMMAP_P2_SIZE			(0x0004)
#define CFG_MEMMAP_P3_SIZE			(0x0004)
#define CFG_MEMMAP_PF1_SIZE			(0x0002)
#define CFG_MEMMAP_PF2_SIZE			(0x0002)
#define CFG_MEMMAP_PF3_SIZE			(0x0002)
#define CFG_MEMMAP_IGF_SIZE			(0x0004)
#define CFG_MEMMAP_PTC_SIZE			(0x0002)
#define CFG_MEMMAP_MRELAY_SIZE		(0x0002)
#define CFG_MEMMAP_ARELAY_SIZE		(0x0002)
#define CFG_MEMMAP_TOTPWR_SIZE		(0x0004)
#define CFG_MEMMAP_REACPWR_SIZE		(0x0004)
#define CFG_MEMMAP_INPUT_SIZE		(0x0002)
#define CFG_MEMMAP_RPWR_SIZE        (0x0004)
#define CFG_MEMMAP_IPWR_SIZE        (0x0004)

/**
 * @note Fault Retrieval Register Sizes
 */
#define CFG_MEMMAP_FRI_SIZE			(0x0004)
#define CFG_MEMMAP_FRO_SIZE 		(0x0004)
#define CFG_MEMMAP_FCODE_SIZE		(0x0004)
#define CFG_MEMMAP_FDT_SIZE			(0x0004)
#define CFG_MEMMAP_V1F_SIZE			(0x0004)
#define CFG_MEMMAP_V2F_SIZE			(0x0004)
#define CFG_MEMMAP_V3F_SIZE			(0x0004)
#define CFG_MEMMAP_I1F_SIZE			(0x0004)
#define CFG_MEMMAP_I2F_SIZE			(0x0004)
#define CFG_MEMMAP_I3F_SIZE			(0x0004)
#define CFG_MEMMAP_MRTF_SIZE		(0x0004)
#define CFG_MEMMAP_TCRF_SIZE		(0x0004)
#define CFG_MEMMAP_P1F_SIZE			(0x0004)
#define CFG_MEMMAP_P2F_SIZE			(0x0004)
#define CFG_MEMMAP_P3F_SIZE			(0x0004)
#define CFG_MEMMAP_PF1F_SIZE		(0x0002)
#define CFG_MEMMAP_PF2F_SIZE		(0x0002)
#define CFG_MEMMAP_PF3F_SIZE		(0x0002)
#define CFG_MEMMAP_IGFF_SIZE		(0x0004)
#define CFG_MEMMAP_VUBF_SIZE		(0x0002)
#define CFG_MEMMAP_CUBF_SIZE		(0x0002)
#define CFG_MEMMAP_FAF_SIZE			(0x0002)
#define CFG_MEMMAP_SEQF_SIZE		(0x0002)
#define CFG_MEMMAP_FSDF_SIZE		(0x000A)

/**
 * @note Set Static IP Register Sizes
 */
#define CFG_MEMMAP_SETIP_SIZE 		(0x0004)
#define CFG_MEMMAP_SETSUBNET_SIZE 	(0x0004)
#define CFG_MEMMAP_SETGATEIP_SIZE	(0x0004)

/**
 * @note Set RTC Time stampe Register Sizes
 */
#define CFG_MEMMAP_RTCTIME_SIZE		(0x0004)


/**
 * @note Manufaturer test Registers
 * @todo update these with actual values when POC verified
 */
#define MFT_MEMMAP_CMD_SIZE (0x0004)
#define MFT_MEMMAP_1_SIZE   (0x0004)
#define MFT_MEMMAP_SIZE     (0x0100)

#define MFT_MEMMAP_START_ADDR  (0x00600)
#define MFT_MEMMAP_CMD_ADDR    (MFT_MEMMAP_START_ADDR)
#define MFT_MEMMAP_1_ADDR      (MFT_MEMMAP_CMD_ADDR+MFT_MEMMAP_CMD_SIZE)
#define MFT_MEMMAP_END_ADDR    (MFT_MEMMAP_START_ADDR+MFT_MEMMAP_SIZE) //@TODO make this relative value based on size and starting address




#define CFG_MEMMAP_START_ADDRESS	(0x0000)

/* ==== Configuration Registers ==== */
#define CFG_MEMMAP_FW_REV_ADDR		(CFG_MEMMAP_START_ADDRESS) /**< Software Revision */
#define CFG_MEMMAP_PC_ADDR			(CFG_MEMMAP_FW_REV_ADDR + CFG_MEMMAP_FW_REV_SIZE) /**< Product Code */
#define CFG_MEMMAP_MULT_ADDR		(CFG_MEMMAP_PC_ADDR + CFG_MEMMAP_PC_SIZE) /**< Current Transformer Ratio */
#define CFG_MEMMAP_PT_ADDR			(CFG_MEMMAP_MULT_ADDR + CFG_MEMMAP_MULT_SIZE) /**< Potential Transformer Ratio */
#define CFG_MEMMAP_LV_ADDR			(CFG_MEMMAP_PT_ADDR + CFG_MEMMAP_PT_SIZE) /**< Low Voltage Holdoff Condition */
#define CFG_MEMMAP_HV_ADDR			(CFG_MEMMAP_LV_ADDR + CFG_MEMMAP_LV_SIZE) /**< High Voltage Holdoff Condition */
#define CFG_MEMMAP_VUB_ADDR			(CFG_MEMMAP_HV_ADDR + CFG_MEMMAP_HV_SIZE) /**< Voltage Unbalance Holdoff Percentage */
#define CFG_MEMMAP_TC_ADDR			(CFG_MEMMAP_VUB_ADDR + CFG_MEMMAP_VUB_SIZE) /**< NEMA Trip Class */
#define CFG_MEMMAP_OC_ADDR			(CFG_MEMMAP_TC_ADDR + CFG_MEMMAP_TC_SIZE) /**< Overcurrent Threshold (FLA of motor) */
#define CFG_MEMMAP_UC_ADDR			(CFG_MEMMAP_OC_ADDR + CFG_MEMMAP_OC_SIZE) /**< Undercurrent Threshold */
#define CFG_MEMMAP_UCTD_ADDR		(CFG_MEMMAP_UC_ADDR + CFG_MEMMAP_UC_SIZE) /**< Undercurrent Trip Delay */
#define CFG_MEMMAP_CUB_ADDR			(CFG_MEMMAP_UCTD_ADDR + CFG_MEMMAP_UCTD_SIZE) /**< Current Unbalance Threshold */
#define CFG_MEMMAP_CUBTD_ADDR		(CFG_MEMMAP_CUB_ADDR + CFG_MEMMAP_CUB_SIZE) /**< Current Unbalance Trip Delay */
#define CFG_MEMMAP_EIPTO_ADDR		(CFG_MEMMAP_CUB_ADDR + CFG_MEMMAP_CUB_SIZE) /**< Ethernet/IP TCP Timeout github issue #52 */
#define CFG_MEMMAP_LIN_ADDR			(CFG_MEMMAP_CUBTD_ADDR + CFG_MEMMAP_CUBTD_SIZE) /**< Linear Overcurrent Trip Delay */
#define CFG_MEMMAP_RD0_ADDR			(CFG_MEMMAP_LIN_ADDR + CFG_MEMMAP_LIN_SIZE) /**< Restart Delay 0 (power on delay) */
#define CFG_MEMMAP_RD1_ADDR			(CFG_MEMMAP_RD0_ADDR + CFG_MEMMAP_RD0_SIZE) /**< Restart Delay 1 (quick start delay) */
#define CFG_MEMMAP_RD2_ADDR			(CFG_MEMMAP_RD1_ADDR + CFG_MEMMAP_RD1_SIZE) /**< Restart Delay 2 (motor cooldown delay) */
#define CFG_MEMMAP_RD3_ADDR			(CFG_MEMMAP_RD2_ADDR + CFG_MEMMAP_RD2_SIZE) /**< Restart Delay 3 (dry well delay) */
#define CFG_MEMMAP_RU_ADDR			(CFG_MEMMAP_RD3_ADDR + CFG_MEMMAP_RD3_SIZE) /**< Restart Attempts for Undercurrent Trips */
#define CFG_MEMMAP_RF_ADDR			(CFG_MEMMAP_RU_ADDR + CFG_MEMMAP_RU_SIZE) /**< Restart Attempts for all other faults */
#define CFG_MEMMAP_GF_ADDR			(CFG_MEMMAP_RF_ADDR + CFG_MEMMAP_RF_SIZE) /**< Ground Fault Current Threshold */
#define CFG_MEMMAP_GFTD_ADDR		(CFG_MEMMAP_GF_ADDR + CFG_MEMMAP_GF_SIZE) /**< Ground Fault Trip Delay */
#define CFG_MEMMAP_GFID_ADDR		(CFG_MEMMAP_GFTD_ADDR + CFG_MEMMAP_GFTD_SIZE) /**< Ground Fault Inhibit Delay */
#define CFG_MEMMAP_LKW_ADDR			(CFG_MEMMAP_GFID_ADDR + CFG_MEMMAP_GFID_SIZE) /**< Low Power Trip Threshold */
#define CFG_MEMMAP_HKW_ADDR			(CFG_MEMMAP_LKW_ADDR + CFG_MEMMAP_LKW_SIZE) /**< High Power Trip Threshold */
#define CFG_MEMMAP_HPRTD_ADDR		(CFG_MEMMAP_HKW_ADDR + CFG_MEMMAP_HKW_SIZE) /**< High Power Trip Delay */
#define CFG_MEMMAP_STLP_ADDR		(CFG_MEMMAP_HPRTD_ADDR + CFG_MEMMAP_HPRTD_SIZE) /**< Stall Percentage (of OC) */
#define CFG_MEMMAP_STLTD_ADDR		(CFG_MEMMAP_STLP_ADDR + CFG_MEMMAP_STLP_SIZE) /**< Stall Trip Delay */
#define CFG_MEMMAP_STLID_ADDR		(CFG_MEMMAP_STLTD_ADDR + CFG_MEMMAP_STLTD_SIZE) /**< Stall Inhibit Delay */
#define CFG_MEMMAP_MACCTRL_ADDR		(CFG_MEMMAP_STLID_ADDR + CFG_MEMMAP_STLID_SIZE) /**< Motor Acceleration Control Bits */
#define CFG_MEMMAP_MACTD_ADDR		(CFG_MEMMAP_MACCTRL_ADDR + CFG_MEMMAP_MACCTRL_SIZE) /**< Motor Acceleration Time Delay */
#define CFG_MEMMAP_ENDIS_ADDR		(CFG_MEMMAP_MACTD_ADDR + CFG_MEMMAP_MACTD_SIZE) /**< Feature Enable/Disable Mask */
#define CFG_MEMMAP_RPWRNVM_ADDR     (CFG_MEMMAP_MACTD_ADDR + CFG_MEMMAP_MACTD_SIZE) /**< Real Power storage location issue #130 */
#define CFG_MEMMAP_CNFG_ADDR		(CFG_MEMMAP_RPWRNVM_ADDR + CFG_MEMMAP_RPWR_SIZE) /**< Hardware Configuration Fields */
#define CFG_MEMMAP_COMCFG_ADDR		(CFG_MEMMAP_CNFG_ADDR + CFG_MEMMAP_CNFG_SIZE) /**< Communication Configuration */
#define CFG_MEMMAP_GATEIP_ADDR		(CFG_MEMMAP_CNFG_ADDR + CFG_MEMMAP_CNFG_SIZE) /**< Gateway IP Address */
#define CFG_MEMMAP_CAN_CNFG_ADDR	(CFG_MEMMAP_COMCFG_ADDR + CFG_MEMMAP_COMCFG_SIZE) /**< CAN BUS Configuration */
#define CFG_MEMMAP_SUBMASKIP_ADDR	(CFG_MEMMAP_COMCFG_ADDR + CFG_MEMMAP_COMCFG_SIZE) /**< SUB MASK IP address  */
#define CFG_MEMMAP_MOD_CNFG_ADDR	(CFG_MEMMAP_CAN_CNFG_ADDR + CFG_MEMMAP_SUBMASKIP_SIZE) /**< MODBus Configuration */
#define CFG_MEMMAP_IPWRNVM_ADDR	    (CFG_MEMMAP_SUBMASKIP_ADDR + CFG_MEMMAP_SUBMASKIP_SIZE) /**< Reactive Power (Imaginary Power) Issue #130 */
#define CFG_MEMMAP_NAME_ADDR		(CFG_MEMMAP_IPWRNVM_ADDR + CFG_MEMMAP_IPWR_SIZE) /**< Friendly Device Name */
#define CFG_MEMMAP_CMD_ADDR			(CFG_MEMMAP_NAME_ADDR + CFG_MEMMAP_NAME_SIZE) /**< Command Interface */
#define CFG_MEMMAP_V1CAL_ADDR		(CFG_MEMMAP_CMD_ADDR + CFG_MEMMAP_CMD_SIZE) /**< PwrLibCurrentCal*/
#define CFG_MEMMAP_V2CAL_ADDR		(CFG_MEMMAP_V1CAL_ADDR + CFG_MEMMAP_V1CAL_SIZE) /**< PwrLibVoltCal*/
#define CFG_MEMMAP_V3CAL_ADDR		(CFG_MEMMAP_V2CAL_ADDR + CFG_MEMMAP_V2CAL_SIZE) /**< V1 Calibration Data */
#define CFG_MEMMAP_IACAL_ADDR		(CFG_MEMMAP_V3CAL_ADDR + CFG_MEMMAP_V3CAL_SIZE) /**< IA Calibration Data */
#define CFG_MEMMAP_IBCAL_ADDR		(CFG_MEMMAP_IACAL_ADDR + CFG_MEMMAP_IACAL_SIZE) /**< IB Calibration Data */
#define CFG_MEMMAP_ICCAL_ADDR		(CFG_MEMMAP_IBCAL_ADDR + CFG_MEMMAP_IBCAL_SIZE) /**< IC Calibration Data */
#define CFG_MEMMAP_GFCAL_ADDR		(CFG_MEMMAP_ICCAL_ADDR + CFG_MEMMAP_ICCAL_SIZE) /**< GF Calibration Data */
#define CFG_MEMMAP_PASSKEY_ADDR		(CFG_MEMMAP_GFCAL_ADDR + CFG_MEMMAP_GFCAL_SIZE) /**< Password Data */
#define CFG_MEMMAP_XXXXXXX_ADDR		(CFG_MEMMAP_PASSKEY_ADDR + CFG_MEMMAP_PASSKEY_SIZE) /**< Password Data */
#define CFG_MEMMAP_SIP_ADDR			(CFG_MEMMAP_XXXXXXX_ADDR + CFG_MEMMAP_XXXXXXX_SIZE) /**< Static IP address to use if set */
#define CFG_MEMMAP_MSTNVM_ADDR		(CFG_MEMMAP_SIP_ADDR + CFG_MEMMAP_SIP_SIZE) /**< NVM store of Motor Service Time: Time since motor last serviced */
#define CFG_MEMMAP_MRTNVM_ADDR		(CFG_MEMMAP_MSTNVM_ADDR + CFG_MEMMAP_MST_SIZE) /**< NVM store of Motor Run Time: Total time motor has run */
#define CFG_MEMMAP_SCNTNVM_ADDR		(CFG_MEMMAP_MRTNVM_ADDR + CFG_MEMMAP_MRT_SIZE) /**< NVM store of Motor Start Count */
#define CFG_MEMMAP_CFP_ADDR    		(CFG_MEMMAP_SCNTNVM_ADDR+CFG_MEMMAP_SCNTNVM_SIZE) /**< cooling factor */
#define CFG_MEMMAP_TCRP_ADDR    	(CFG_MEMMAP_CFP_ADDR+CFG_MEMMAP_CFP_SIZE) /**< minimum thermal capacity restart level */


// Add 4 for the CRC; add one for block status
#define COMM_MEMMAP_SIZE (CFG_MEMMAP_TCRP_ADDR+CFG_MEMMAP_TCRP_SIZE+4+1)


/* ==== Realtime Status Registers ==== */
#define CFG_MEMMAP_RTREG_ADDR		(0x00000200) /**< start address of where the Real Time registers will be stored */
#define CFG_MEMMAP_MST_ADDR			(CFG_MEMMAP_RTREG_ADDR) /**< Time since motor last serviced (can only be written to zero) */
#define CFG_MEMMAP_MRT_ADDR			(CFG_MEMMAP_MST_ADDR + CFG_MEMMAP_MST_SIZE) /**< Time since motor started */
#define CFG_MEMMAP_SCNT_ADDR		(CFG_MEMMAP_MRT_ADDR + CFG_MEMMAP_MRT_SIZE) /**< Start Count -- Number of Motor starts (since last cleared) */
#define CFG_MEMMAP_FAULT_STAT_ADDR	(CFG_MEMMAP_SCNT_ADDR + CFG_MEMMAP_SCNT_SIZE) /**< Current Fault Status Mask */
#define CFG_MEMMAP_WARN_STAT_ADDR	(CFG_MEMMAP_FAULT_STAT_ADDR + CFG_MEMMAP_FAULT_STAT_SIZE) /**< Current Warning Status Mask */
#define CFG_MEMMAP_FAULT_CODE_ADDR	(CFG_MEMMAP_WARN_STAT_ADDR + CFG_MEMMAP_WARN_STAT_SIZE) /**< Indicates the reason we have tripped or are in holdoff. */
#define CFG_MEMMAP_RTDT_ADDR		(CFG_MEMMAP_FAULT_CODE_ADDR + CFG_MEMMAP_FAULT_CODE_SIZE) /**< Remaining Trip Delay Time */
#define CFG_MEMMAP_RDR_ADDR			(CFG_MEMMAP_RTDT_ADDR + CFG_MEMMAP_RTDT_SIZE) /**< Restart time remaining */
#define CFG_MEMMAP_TCR_ADDR			(CFG_MEMMAP_RDR_ADDR + CFG_MEMMAP_RDR_SIZE) /**< Thermal Capacity Remaining */
#define CFG_MEMMAP_MLF_ADDR			(CFG_MEMMAP_TCR_ADDR + CFG_MEMMAP_TCR_SIZE) /**< Measured Line Frequency */
#define CFG_MEMMAP_SEQ_ADDR			(CFG_MEMMAP_MLF_ADDR + CFG_MEMMAP_MLF_SIZE) /**< Measured Line Frequency */
#define CFG_MEMMAP_VUBM_ADDR		(CFG_MEMMAP_SEQ_ADDR + CFG_MEMMAP_SEQ_SIZE) /**< Measured Voltage Unbalance */
#define CFG_MEMMAP_CUBM_ADDR		(CFG_MEMMAP_VUBM_ADDR + CFG_MEMMAP_VUBM_SIZE) /**< Measured Current Unbalance */
#define CFG_MEMMAP_V1_ADDR			(CFG_MEMMAP_CUBM_ADDR + CFG_MEMMAP_CUBM_SIZE) /**< Measured line 1 Voltage RMS */
#define CFG_MEMMAP_V2_ADDR			(CFG_MEMMAP_V1_ADDR + CFG_MEMMAP_V1_SIZE) /**< Measured line 2 Voltage RMS */
#define CFG_MEMMAP_V3_ADDR			(CFG_MEMMAP_V2_ADDR + CFG_MEMMAP_V2_SIZE) /**< Measured line 3 Voltage RMS */
#define CFG_MEMMAP_I1_ADDR			(CFG_MEMMAP_V3_ADDR + CFG_MEMMAP_V3_SIZE) /**< Measured line 1 Current RMS */
#define CFG_MEMMAP_I2_ADDR			(CFG_MEMMAP_I1_ADDR + CFG_MEMMAP_I1_SIZE) /**< Measured line 2 Current RMS */
#define CFG_MEMMAP_I3_ADDR			(CFG_MEMMAP_I2_ADDR + CFG_MEMMAP_I2_SIZE) /**< Measured line 3 Current RMS */
#define CFG_MEMMAP_P1_ADDR			(CFG_MEMMAP_I3_ADDR + CFG_MEMMAP_I3_SIZE) /**< Measured line 1 Power RMS */
#define CFG_MEMMAP_P2_ADDR			(CFG_MEMMAP_P1_ADDR + CFG_MEMMAP_P1_SIZE) /**< Measured line 2 Power RMS */
#define CFG_MEMMAP_P3_ADDR			(CFG_MEMMAP_P2_ADDR + CFG_MEMMAP_P2_SIZE) /**< Measured line 3 Power RMS */
#define CFG_MEMMAP_PF1_ADDR			(CFG_MEMMAP_P3_ADDR + CFG_MEMMAP_P3_SIZE) /**< Measured line 1 Power Factor */
#define CFG_MEMMAP_PF2_ADDR			(CFG_MEMMAP_PF1_ADDR + CFG_MEMMAP_PF1_SIZE) /**< Measured line 2 Power Factor */
#define CFG_MEMMAP_PF3_ADDR			(CFG_MEMMAP_PF2_ADDR + CFG_MEMMAP_PF2_SIZE) /**< Measured line 3 Power Factor */
#define CFG_MEMMAP_IGF_ADDR			(CFG_MEMMAP_PF3_ADDR + CFG_MEMMAP_PF3_SIZE) /**< Ground Fault Current */
#define CFG_MEMMAP_PTC_ADDR			(CFG_MEMMAP_IGF_ADDR + CFG_MEMMAP_IGF_SIZE) /**< PTC level*/
#define CFG_MEMMAP_MRELAY_ADDR		(CFG_MEMMAP_PTC_ADDR + CFG_MEMMAP_PTC_SIZE) /**< Motor Relay State */
#define CFG_MEMMAP_ARELAY_ADDR		(CFG_MEMMAP_MRELAY_ADDR + CFG_MEMMAP_MRELAY_SIZE) /**< Aux Relay State*/
#define CFG_MEMMAP_TOTPWR_ADDR		(CFG_MEMMAP_ARELAY_ADDR + CFG_MEMMAP_ARELAY_SIZE) /**< current total power */
#define CFG_MEMMAP_REACPWR_ADDR		(CFG_MEMMAP_TOTPWR_ADDR + CFG_MEMMAP_TOTPWR_SIZE) /**< current total power */
#define CFG_MEMMAP_INPUT_ADDR		(CFG_MEMMAP_REACPWR_ADDR + CFG_MEMMAP_REACPWR_SIZE ) /**< Input pin state */
#define CFG_MEMMAP_RPWR_ADDR		(CFG_MEMMAP_INPUT_ADDR + CFG_MEMMAP_INPUT_SIZE) /**< Real Measured Power */
#define CFG_MEMMAP_IPWR_ADDR		(CFG_MEMMAP_RPWR_ADDR + CFG_MEMMAP_RPWR_SIZE) /**< Reactive Measured Power */

#define CFG_MEMMAP_RTREG_END		(CFG_MEMMAP_IPWR_ADDR + CFG_MEMMAP_IPWR_SIZE)

/* ==== Fault Retrieval Registers ==== */
#define CFG_MEMMAP_FAULRREG_ADDR	(0x00000300) /**< start address of where the Fault Retrieval registers will be stored */

#define CFG_MEMMAP_FRI_ADDR			(CFG_MEMMAP_FAULRREG_ADDR) /**< Fault Request Index */
#define CFG_MEMMAP_FRO_ADDR			(CFG_MEMMAP_FRI_ADDR + CFG_MEMMAP_FRI_SIZE) /**< Fault Request Offset */
#define CFG_MEMMAP_FCODE_ADDR		(CFG_MEMMAP_FRO_ADDR + CFG_MEMMAP_FRO_SIZE) /**< Fault Code */
#define CFG_MEMMAP_FDT_ADDR			(CFG_MEMMAP_FCODE_ADDR + CFG_MEMMAP_FCODE_SIZE) /**< Date/Time Fault */
#define CFG_MEMMAP_V1F_ADDR			(CFG_MEMMAP_FDT_ADDR + CFG_MEMMAP_FDT_SIZE) /**< Phase 1 RMS Voltage at Fault */
#define CFG_MEMMAP_V2F_ADDR			(CFG_MEMMAP_V1F_ADDR + CFG_MEMMAP_V1F_SIZE) /**< Phase 2 RMS Voltage at Fault */
#define CFG_MEMMAP_V3F_ADDR			(CFG_MEMMAP_V2F_ADDR + CFG_MEMMAP_V2F_SIZE) /**< Phase 3 RMS Voltage at Fault */
#define CFG_MEMMAP_I1F_ADDR			(CFG_MEMMAP_V3F_ADDR + CFG_MEMMAP_V3F_SIZE) /**< Phase 1 RMS Current at Fault */
#define CFG_MEMMAP_I2F_ADDR			(CFG_MEMMAP_I1F_ADDR + CFG_MEMMAP_I1F_SIZE) /**< Phase 2 RMS Current at Fault */
#define CFG_MEMMAP_I3F_ADDR			(CFG_MEMMAP_I2F_ADDR + CFG_MEMMAP_I2F_SIZE) /**< Phase 3 RMS Current at Fault */
#define CFG_MEMMAP_MRTF_ADDR		(CFG_MEMMAP_I3F_ADDR + CFG_MEMMAP_I3F_SIZE) /**< Time since motor started */
#define CFG_MEMMAP_TCRF_ADDR		(CFG_MEMMAP_MRTF_ADDR + CFG_MEMMAP_MRTF_SIZE) /**< Thermal Capacity Remaining */
#define CFG_MEMMAP_P1F_ADDR			(CFG_MEMMAP_TCRF_ADDR + CFG_MEMMAP_TCRF_SIZE) /**< Phase 1 Power at Fault */
#define CFG_MEMMAP_P2F_ADDR			(CFG_MEMMAP_P1F_ADDR + CFG_MEMMAP_P1F_SIZE) /**< Phase 2 Power at Fault */
#define CFG_MEMMAP_P3F_ADDR			(CFG_MEMMAP_P2F_ADDR + CFG_MEMMAP_P2F_SIZE) /**< Phase 3 Power at Fault */
#define CFG_MEMMAP_PF1F_ADDR		(CFG_MEMMAP_P3F_ADDR + CFG_MEMMAP_P3F_SIZE) /**< Phase 1 Power Factor at Fault */
#define CFG_MEMMAP_PF2F_ADDR		(CFG_MEMMAP_PF1F_ADDR + CFG_MEMMAP_PF1F_SIZE) /**< Phase 2 Power Factor at Fault */
#define CFG_MEMMAP_PF3F_ADDR		(CFG_MEMMAP_PF2F_ADDR + CFG_MEMMAP_PF2F_SIZE) /**< Phase 3 Power Factor at Fault */
#define CFG_MEMMAP_IGFF_ADDR		(CFG_MEMMAP_PF3F_ADDR + CFG_MEMMAP_PF3F_SIZE) /**< Ground Fault current */
#define CFG_MEMMAP_VUBF_ADDR		(CFG_MEMMAP_IGFF_ADDR + CFG_MEMMAP_IGFF_SIZE) /**< Voltage Unbalanced at fault */
#define CFG_MEMMAP_CUBF_ADDR		(CFG_MEMMAP_VUBF_ADDR + CFG_MEMMAP_VUBF_SIZE) /**< Current Unbalanced at fault */
#define CFG_MEMMAP_FAF_ADDR			(CFG_MEMMAP_CUBF_ADDR + CFG_MEMMAP_CUBF_SIZE) /**< Frequency measured from phase A */
#define CFG_MEMMAP_SEQF_ADDR		(CFG_MEMMAP_FAF_ADDR + CFG_MEMMAP_FAF_SIZE) /**< Measured phase sequence */
#define CFG_MEMMAP_FSDF_ADDR		(CFG_MEMMAP_SEQF_ADDR + CFG_MEMMAP_SEQF_SIZE) /**< fault specific data */

#define CFG_MEMMAP_FAULRREG_END 	(CFG_MEMMAP_FSDF_ADDR + CFG_MEMMAP_FSDF_SIZE) /**< start address of where the Fault Retrieval registers will be stored */

#define COMM_RAMMAP_SIZE (CFG_MEMMAP_FSDF_ADDR+CFG_MEMMAP_FSDF_SIZE)

#define CFG_MEMMAP_SETIP_SIZE 		(0x0004)
#define CFG_MEMMAP_SETSUBNET_SIZE 	(0x0004)
#define CFG_MEMMAP_SETGATEIP_SIZE	(0x0004)

/* ==== Set Static IP Registers ==== */
#define CFG_MEMMAP_SETIP_ADDR		(0x00000400) /**< start address of setting static IP registers get stored */
#define CFG_MEMMAP_SETSUBNET_ADDR	(CFG_MEMMAP_SETIP_ADDR + CFG_MEMMAP_SETIP_SIZE) /**< subnet mask */
#define CFG_MEMMAP_SETGATEIP_ADDR	(CFG_MEMMAP_SETSUBNET_ADDR + CFG_MEMMAP_SETSUBNET_SIZE) /**< subnet mask */
#define CFG_MEMMAP_SETIP_END 		(CFG_MEMMAP_SETGATEIP_ADDR + CFG_MEMMAP_SETGATEIP_SIZE) /**< start address of setting static IP registers get stored */


/* ==== RTC Timestamp Registers ==== */
#define CFG_MEMMAP_RTCTIME_ADDR		(0x00000500) /**< start address of setting RTC Time stamp */
#define CFG_MEMMAP_RTCTIME_END 		(CFG_MEMMAP_RTCTIME_ADDR + CFG_MEMMAP_RTCTIME_SIZE) /**< end address of setting RTC Time stamp */


// Common flags for the fault control block flags field
#define FCB_FLAGS_MASK_FAULTENABLE 		(0x0001)	///< Fault is enabled
#define FCB_FLAGS_MASK_AUTORESTART 		(0x0002)	///< Infinite restart attempts for the fault
#define FCB_FLAGS_MASK_ALARMENABLE 		(0x0004)	///< fault condition sets the alarm relay

//#define FCB_FLAGS_MASK_FAULTENABLE 		(0x0001)

#define MP8K_RESET_MODE_CHECK 			(-1)
#define MP8K_RESET_MODE_NORMAL 			(0)
#define MP8K_RESET_MODE_BLE_PASSKEY		(10)

/*!
 * Structure type to contain the information needed to handle any fault condition.
 */
typedef struct FaultControlBlock_s
{
	uint32_t InhibitDelay;
	uint32_t TripDelay;
	uint32_t RestartDelay;
	uint16_t RestartAttempts;
	uint16_t Flags;
}sFaultControlParam_t;

/*!
 * Structure containing data needed from memory map for control logic.
 */
typedef struct ControlParam_s
{
	uint16_t MULT;					// Number of times Current wires are wrapped
	uint16_t PT;					// Potential Transformer Ratio
	uint32_t LV;					// Low Voltage Holdoff Threshold
	uint32_t HV;					// High Voltage Holdoff Threshold
	uint16_t VUB;					// Voltage Unbalance Maximum
	sFaultControlParam_t VUBfcb;	// Fault control block for the Voltage Unbalance faults
	uint16_t TC;					// Trip Class
	uint32_t OC;					// Overcurrent.  Max RMS Current allowed
	uint32_t RD0;					// start delay time
	uint32_t RD1;					// Zero-Current Relay open timer
	sFaultControlParam_t Thermfcb;	// Fault control block for the thermal capacity faults
	uint32_t UC;					// Undercurrent.  Minimum RMS Current allowed
	sFaultControlParam_t UCfcb;		// Fault control block for the undercurrent faults
	uint16_t CUB;					// Current Unbalance Maximum
	sFaultControlParam_t CUBfcb;	// Fault control block for the Current Unbalance faults
	//uint16_t LIN;					// Linear Overcurrent Trip Delay
	sFaultControlParam_t LINfcb;	// Fault control block for the Linear TD  faults
	uint32_t GF;					// Ground Fault Trip Current
	sFaultControlParam_t GFfcb;		// Fault control block for the Ground Fault faults
	uint32_t LKW;					// Low Kilowatt Trip Limit
	uint32_t LKWcorrected;			// Low Kilowatt Trip Limit corrected for the control logic
	sFaultControlParam_t LKWfcb;	// Fault control block for the Low Power faults
	uint32_t HKW;					// High Kilowatt Trip Limit
	uint32_t HKWcorrected;			// High Kilowatt Trip Limit corrected for the control logic
	sFaultControlParam_t HKWfcb;	// Fault control block for the High Powerfaults
	uint16_t STLP;					// Stall Percentage Threshold
	sFaultControlParam_t STLfcb;	// Fault control block for the Motor Stall faults
	sFaultControlParam_t CSPfcb;	// Fault control block for the Current Single Phase
	sFaultControlParam_t Contactfcb;// Fault control block for the Contractor failure
	sFaultControlParam_t ZCfcb;		// Zero Current Fault control block
	sFaultControlParam_t PTCfcb;	// Fault control block for the PTC
	uint32_t CNFG;					// Configuration/Control bit field
//	uint32_t ENDIS;					// Enable AUX relay control register
	uint16_t ManualReset;			// Manual Reset
	uint32_t Ph1CurrentCal;			// Phase 1 Current calibration factor
	uint32_t Ph2CurrentCal;			// Phase 2 Current calibration factor
	uint32_t Ph3CurrentCal;			// Phase 3 Current calibration factor
	uint32_t VoltageCal;			// Voltage calibration factor
	uint32_t PwrLibCurrentCal;		// Power Library Current Calibrate
	uint32_t PwrLibVoltCal;			// Power Library Voltage Calibrate
	uint32_t GFCal;					// Ground Fault Calibrate
	uint32_t MRTNVM;				// MRT stored in NVM
	uint32_t MSTNVM;				// MST stored in NVM
	uint16_t SCNTNVM;				// SCNT stored in NVM
	uint16_t CFP;					// cooling factor rate
	uint32_t RPWRNVM;				// Real Power measured stored in NVM
	uint32_t IPWRNVM;				// Reactive Power measured stored in NVM
}sControlParam_t;

#ifdef PTC
#undef PTC
#endif
/*!
 * Structure containing data needed from memory map for Real-time Status Registers.
 */
typedef struct RealTimeRegisters_s
{
	uint32_t MST;			/**< Time since motor last serviced */
	uint32_t MRT;			/**< Time since motor started */
	uint16_t SCNT;			/**< Start Count, number of motor starts */
	uint32_t FaultStatus;	/**< Current Fault Status Mask */
	uint32_t WarningStatus;	/**< Current Warning Status Mask */
	uint16_t FaultCode;		/**< The reason we have trip or in a hold-off */
	uint32_t RTDT;			/**< Remaining Trip Delay Time */
	uint32_t RDR;			/**< Restart time remaining */
	uint16_t TCR;			/**< Thermal Capacity Remaining */
	uint16_t MLF;			/**< Measured Line Frequency */
	uint16_t SEQ;			/**< phase sequence */
	uint16_t VUBM;			/**< voltage Unbalance */
	uint16_t CUBM;			/**< current unbalance */
	uint32_t V1;			/**< line 1 voltage RMS */
	uint32_t V2;			/**< line 2 voltage RMS */
	uint32_t V3;			/**< line 3 voltage RMS */
	uint32_t I1;			/**< line 1 current RMS */
	uint32_t I2;			/**< line 2 current RMS */
	uint32_t I3;			/**< line 3 current RMS */
	uint32_t P1;			/**< line 1 power */
	uint32_t P2;			/**< line 2 power */
	uint32_t P3;			/**< line 3 power */
	uint16_t PF1;			/**< line 1 power factor */
	uint16_t PF2;			/**< line 2 power factor */
	uint16_t PF3;			/**< line 3 power factor */
	uint32_t GFC;			/**< ground fault current */
	uint16_t PTC;			/**< PTC level */
	uint16_t MotorRelayState;/**< Motor Relay State */
	uint16_t AuxRelayState;	/**< Aux Relay State */
	uint32_t TotalPwr;		/**< total real motor power */
	uint32_t ReactivePwr;	/**< total reactive motor power */
	uint16_t InputPinState;	/**< current input pin state*/
	uint32_t RPWR;		/**< Real Power Measured */
	uint32_t IPWR;		/**< Reactive Power Measured */
	double fRPWR;		/**< floating Real Power Measured */
	double fIPWR;		/**< floating Reactive Power Measured */
}sRealTimeRegisters_t;

/*!
 * Structure containing data needed from memory map for fault Registers.
 */
typedef struct FaultRegisters_s
{
	uint32_t FRI;			/**< Fault Request Index */
	uint32_t FRO;			/**< Fault Request Offset */
	uint16_t FCODE;			/**< Fault code indicating the source of the fault */
	uint32_t FDT;			/**< Date Time of Fault */
	uint32_t V1F;			/**< Line 1 Voltage RMS */
	uint32_t V2F;			/**< Line 2 Voltage RMS */
	uint32_t V3F;			/**< Line 3 Voltage RMS */
	uint32_t I1F;			/**< Line 1 Current RMS */
	uint32_t I2F;			/**< Line 2 Current RMS */
	uint32_t I3F;			/**< Line 3 Current RMS */
	uint32_t MRTF;			/**< Time since motor started */
	uint32_t TCRF;			/**< Thermal Capacity Remaining */
	uint32_t P1F;			/**< Measured line 1 Power */
	uint32_t P2F;			/**< Measured line 2 Power */
	uint32_t P3F;			/**< Measured line 3 Power */
	uint16_t PF1F;			/**< Measured line 1 Power Factor */
	uint16_t PF2F;			/**< Measured line 2 Power Factor */
	uint16_t PF3F;			/**< Measured line 3 Power Factor */
	uint32_t IGFF;			/**< ground fault current */
	uint16_t VUBF;			/**< measured voltage UnBalance */
	uint16_t CUBF;			/**< measured Current UnBalance */
	uint16_t FAF;			/**< measured Frequency from phase 1 */
	uint16_t SEQF;			/**< measured phase sequence */
	uint8_t FSDF[10];		/**< Fault Specific Data*/
}sFaultTimeRegisters_t;

typedef union Mp8kFaultTypeInfo_s
{
	uint8_t raw[14];
}Mp8kFaultTypeInfo_t;

/*!
 * @warning This is the wrong location for this structure.
 */
typedef struct Mp8kFaultEntry_s
{
	// Fault Characteristic 1
	eFaultHoldoffCodes_t Code;		// See table above
	uint32_t DateTime;	// Unix time_t of this fault occurring
	// RMS voltage values at time of this fault
	uint32_t Vab;		// fixed point 2 decimal places. Example 24531 = 245.31 Volts RMS
	uint32_t Vbc;
	uint32_t Vca;

	// Fault Characteristic 2
	// RMS current values at time of this fault
	uint32_t Ia;		// fixed point 2 decimal places. Example 24531 = 245.31 Amps RMS
	uint32_t Ib;
	uint32_t Ic;
	uint32_t RunTime;	// In seconds since last fault
	uint32_t TCR;		// Thermal capacity remaining. Divide this number by 0xFFFFFFFFF = 901943132159 to determine percentage.

	// Fault Characteristic 3
	// Real power (Watts) per phase
	uint32_t Pa;	// fixed point 2 decimal places. Example 24531 = 245.31 Watts
	uint32_t Pb;
	uint32_t Pc;
	uint16_t Seq;	// 0 = A,B,C ; 1 = A,C,B
	/*!
	 * Power Factor calculations
	 * 16 bit fields,
	 * Top two bits (15 & 14) determine quadrant of the power factor.
	 * Quadrant 00 inductive (lagging, i.e. current lags voltage) and positive (i.e. this is a load/motor condition)
	 * Quadrant 10 inductive (lagging, i.e. current lags voltage) and negative (i.e. this is a source/generator condition)
	 * Quadrant 01 capacitive (leading, i.e. current leads voltage) and positive (i.e. this is a load/motor condition)
	 * Quadrant 11 capacitive (leading, i.e. current leads voltage) and negative (i.e. this is a source/generator condition)
	 * The 14 LSB must be divided by 16k = 16,383 = 0x3FFF to produce the actual power factor value.
	 * example 1:
	 * PwrFa = 0x115C
	 * mask off upper two bits;	0x115C & 0x3FFF = 0x115C = 4,444
	 * PF = 4,444/16,383 = +0.27 lagging (inductive, load/motor)
	 * example 2:
	 * PwrFa = 0xD15C
	 * mask off upper two bits;	0xD15C & 0x3FFF = 0x115C = 4,444
	 * PF = 4,444/16,383 = -0.27 leading (capacitive, source/generator)
	 */
	uint16_t PwrFa;	// Phase A Power Factor
	uint16_t PwrFb;
	uint16_t PwrFc;
	// Ground Fault Current
	uint32_t Igf;	// fixed point 2 decimal places. Example 245 = 2.45 Amps

	// Fault Characteristic 4
	// Unbalanced voltage and current - Divide these values by 0xFFFF to determine the percent unbalanced
	uint16_t Vunb;
	uint16_t Iunb;
	uint16_t PhaseAfreq;	// fixed point 1 decimal places. Example 612 = 61.2 Hz
	Mp8kFaultTypeInfo_t Extra;
	uint32_t MRT; /**< Motor Run Time in seconds */
	uint32_t MST; /**< Motor Service Time in seconds */
	uint16_t SCNT; /**< Motor start count */
	uint32_t RPWR; /**< Real Power Measured */
	uint32_t IPWR; /**< Reactive Power Measured */
}Mp8kFaultEntry_t;

/*!
 * These are parameters the control logic needs to update and hand off to the NVM.
 *
 * (also, some of these will need to be read at boot)
 */
typedef struct ControlStateNvmParam_s
{
	uint32_t ThermalCapacity;
	uint32_t RestartDelayRemaining;
	//uint16_t OCRestarts;
}sControlStateNvmParam_t;

/**
 * ENDIS REGISTER MAP
 *
 * BIT 0:  Ground Fault Trip Enabled
 * BIT 1:  Voltage UnBalance Holdoff Enabled
 * BIT 2:  Current UnBalance Trip Enabld
 * BIT 3:  Under Current Trip Enabled
 * BIT 4:  Over Current Trip Enabled (Thermal Capacity)
 * BIT 5:  Linear Overcurrent Trip
 * BIT 6:  LowPoweR Trip Enabled
 * BIT 7:  HighPoweR Trip Enabled
 * BIT 8:  Motor Stall Trip Enabled
 * BIT 9:  Current Single Phase Trip Enabled
 * BIT 10: PTC Trip Enabled
 * BIT 11: Ground Fault Alarm Enabled
 * BIT 12: Contactor Trip Enabled
 * BIT 13: Reserved
 * . . .
 * BIT 31: Reserved

 */
#define ENDIS_GF_ENABLE_MASK 			(0x00000001)
#define ENDIS_VUB_ENABLE_MASK 			(0x00000002)
#define ENDIS_CUB_ENABLE_MASK 			(0x00000004)
#define ENDIS_UC_ENABLE_MASK 			(0x00000008)
#define ENDIS_OC_ENABLE_MASK 			(0x00000010)
#define ENDIS_LIN_ENABLE_MASK 			(0x00000020)
#define ENDIS_LKW_ENABLE_MASK 			(0x00000040)
#define ENDIS_HKW_ENABLE_MASK 			(0x00000080)
#define ENDIS_STL_ENABLE_MASK 			(0x00000100)
#define ENDIS_CSP_ENABLE_MASK 			(0x00000200)
#define ENDIS_PTC_ENABLE_MASK 			(0x00000400)
#define ENDIS_GF_ALARM_ENABLE_MASK 		(0x00000800)
#define ENDIS_CONTACT_ENABLE_MASK 		(0x00001000)

/*!
 * MACCTRL REGISTER MAP
 *
 * Bit 0: Reserved
 * Bit 1: Reserved
 * Bit 2: Reserved
 * Bit 3: Motor acceleration trip delay applies to UC/LPR trip
 * Bit 4: Reserved
 * Bit 5: Motor acceleration trip delay applies to GF trip
 * Bit 6: Motor acceleration trip delay applies to CUB trip
 * Bit 7: Reserved
 * Bit 8: Reserved
 * Bit 9: Motor acceleration trip delay applies to HKW trip1
 * Bit 10: Reserved
 * Bit 11: Reserved
 * Bit 12: Reserved
 * Bit 13: Reserved
 * Bit 14: Reserved
 * Bit 15: Reserved
 */
#define MACTRL_UCLPR_ENABLE_MASK 		(0x00000008)
//	 #define MACTRL_GF_ENABLE_MASK 			(0x00000020) /// shouldn't exist according to the current requirements
#define MACTRL_CUB_ENABLE_MASK 			(0x00000040)
#define MACTRL_HKW_ENABLE_MASK 			(0x00000200)

/*!
 * cfgCtl Configuration Control bits
 * Bit 0: GFMT Ground Fault Motor Trip mask
 * Bit 1: Allow AUX relay control
 * Bit 2: Disable the BLE module
 * Bit 3: Run MP8000 in currently only mode
 * Bit 4: Enable AUX relay when fault present mode
 * if bits 6..5 are zero three phase motor measure all phases
 * Bit 5: single phase motor device
 * Bit 6: 3phase motor with only 1 voltage
 * Bit 7: Reserved
 * Bit 8: Disable reverse polarity hold-off
 * Bit 9: Enable PTC readings
 * Bit 10: stall enabled
 * Bit 11: low KW mode
 * Bit 12: CBA Phase Rotation not a fault, change rotation to CBA
 * Bit 13: RD0 on power on
 * Bit 14: RD1 invoked on current loss
 * Bit 15: Enable emergency run
 */
#define CNFG_GFMT_MASK 						(0x0001)
#define CNFG_ALLOW_AUX_RELAY_MASK 			(0x0002)
#define CNFG_DISABLE_BLE_MASK 				(0x0004)
#define CNFG_CURRENT_ONLY_MODE_MASK			(0x0008)
#define CNFG_AUX_FAULT_MASK					(0x0010)
#define CNFG_DISABLE_RP_HOLDOFF_MASK 		(0x0100)
#define CNFG_ENABLE_PTC_MASK 				(0x0200)
#define CNFG_STALL_ENABLE_MASK 				(0x0400)
#define CNFG_LOW_KW_MODE_MASK 				(0x0800)
#define CNFG_CHANGE_ROTATION_TO_CBA_MASK 	(0x1000)
#define CNFG_RD1_ON_CURRENT_LOSS_MASK 		(0x4000)
#define CNFG_EMERGENCY_RUN_MASK 			(0x8000)

/*!
 * CMD Command support calls
 */
#define CMD_MOTOR_RESTART				(0x0009)
#define CMD_RESET_MOTOR_SERVICE_TIME 	(0x0010)
#define CMD_FAULT_LOOKUP				(0x0011)
#define CMD_RESET_FACTOR_DEFAULTS		(0x0012)
#define CMD_RESET_MOTOR_RUN_TIME 		(0x0013)
#define CMD_RESET_MOTOR_SCNT 			(0x0014)
#define CMD_RESET_POWER_MEASUREMENTS	(0x0015)
#define CMD_FORCE_TRIP		 			(0x0020)
#define CMD_AUX_RELAY_OFF				(0x0030)
#define CMD_AUX_RELAY_ON				(0x0031)
#define CMD_SET_BLE_DISABLE_BIT			(0x0040)
#define CMD_CLR_BLE_DISABLE_BIT			(0x0041)
#define CMD_BLE_CHECK_PASS				(0x0052)
#define CMD_BLE_UPDATE_DEVICE_PARAM		(0x0053)
#define CMD_ENABLETFTP					(0x0060)
#define CMD_COMMIT_STATICIOP			(0x0070)
#define CMD_ENABLE_CURRENT_ONLY_MODE	(0x0080)
#define CMD_DISABLE_CURRENT_ONLY_MODE	(0x0081)
#define CMD_ENABLE_AUX_FAULT_MODE		(0x0090)
#define CMD_DISABLE_AUX_FAULT_MODE		(0x0091)

/*!
 * Use fake fault data over BLE instead of hooking nvm code.
 */
#define BLE_USE_FAKE_FAULT_DATA (1)

#ifdef SEMAPHORE_H
extern SemaphoreHandle_t ts_ManualRestartSem; /**< semaphore to signal a manual restart has been requested */
#endif

int MemMapLoadDefaults();
int MemMapRead(uint16_t Address, uint16_t Size, void *Value);
int MemMapWrite(uint16_t Address, uint16_t Size, const void *Value, int Commit);
sControlParam_t *GetControlParameters(int32_t *ptrStatus);
sControlParam_t *GetControlParametersForLogging();
int MemMapInit();
uint32_t SetRTParameters(sRealTimeRegisters_t *RTReg);
uint32_t GetRTParameters(sRealTimeRegisters_t *RTReg);
uint8_t *MemmapAcquire(uint32_t block_time);
void MemmapRelease();

extern int LogFaultLookupByIndex(uint32_t index, Mp8kFaultEntry_t *entrystorage);
extern int LogFaultLookupByDate(uint32_t unix_time, int32_t offset, Mp8kFaultEntry_t *entrystorage);

int GetResetMode();
int SetResetMode(int mode);
void updateCIPFaultValues(Mp8kFaultEntry_t *ptrFault);
void populateCIPFaultValues(uint8_t *ptrCIPAssemblyData,uint32_t Index);
void populateCIPRealTimeValues(uint8_t *ptrCIPAssemblyData);
void populateCIPConfigurationValues(uint8_t *ptrCIPAssemblyConfig);
void saveCIPConfigurationValues(uint8_t *ptrCIPAssemblyConfig,uint32_t Instance);
uint32_t CommitStaticIPSettings();

#endif /* SOURCES_COMMUNICATIONS_COMMONCOMM_H_ */
