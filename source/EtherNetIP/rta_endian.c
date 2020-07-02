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

/*  --------------------------  */
/*  INCLUDE FILES               */
/*  --------------------------  */
#include "rta_system.h"

/* this must be after the include files */
#undef __RTA_FILE__
#define __RTA_FILE__ "rta_endian.c"

/*  --------------------------  */
/*  LOCAL FUNCTION PROTOTYPES   */
/*  --------------------------  */

/*  --------------------------  */
/*  GLOBAL FUNCTION PROTOTYPES  */
/*  --------------------------  */

/*  --------------------------  */
/*  LOCAL VARIABLES (STATIC)    */
/*  --------------------------  */

/*  --------------------------  */
/*  GLOBAL VARIABLES            */
/*  --------------------------  */

/*  --------------------------  */
/*  EXTERN VARIABLES            */
/*  --------------------------  */

/*  --------------------------  */
/*  MISCELLANEOUS               */
/*  --------------------------  */

/* #define RTA_SWAP_ENDIAN 1 */ /* only define if the endian is backwards */

/**/
/* ******************************************************************** */
/*                      GLOBAL FUNCTIONS                                */
/* ******************************************************************** */
RTA_U32 rta_endian_Swap32(RTA_U32 const u32PassedVal)
{
    RTA_U32 u32ReturnVal;

    u32ReturnVal  = (u32PassedVal & 0xFF000000)>>24;
    u32ReturnVal |= (u32PassedVal & 0x00FF0000)>>8;
    u32ReturnVal |= (u32PassedVal & 0x0000FF00)<<8;
    u32ReturnVal |= (u32PassedVal & 0x000000FF)<<24;

    return(u32ReturnVal);
}

RTA_U16 rta_endian_Swap16(RTA_U16 const u16PassedVal)
{
    RTA_U16 u16ReturnVal;

    u16ReturnVal  = (u16PassedVal & 0xFF00)>>8;
    u16ReturnVal |= (u16PassedVal & 0x00FF)<<8;

    return(u16ReturnVal);
}

void rta_endian_PutBigEndian16(RTA_U16 const u16PassedVal, RTA_U8 * const pPos)
{
#ifdef RTA_SWAP_ENDIAN
    pPos[0] = (RTA_U8)((u16PassedVal) & 0x00FF);
    pPos[1] = (RTA_U8)((u16PassedVal>>8) & 0x00FF);
#else
    pPos[0] = (RTA_U8)((u16PassedVal>>8) & 0x00FF);
    pPos[1] = (RTA_U8)((u16PassedVal) & 0x00FF);
#endif
}

void rta_endian_PutBigEndian32(RTA_U32 const u32PassedVal, RTA_U8 * const pPos)
{
#ifdef RTA_SWAP_ENDIAN
    pPos[0] = (RTA_U8)((u32PassedVal) & 0x000000FFL);
    pPos[1] = (RTA_U8)((u32PassedVal>> 8) & 0x000000FFL);
    pPos[2] = (RTA_U8)((u32PassedVal>>16) & 0x000000FFL);
    pPos[3] = (RTA_U8)((u32PassedVal>>24) & 0x000000FFL);
#else
    pPos[3] = (RTA_U8)((u32PassedVal) & 0x000000FFL);
    pPos[2] = (RTA_U8)((u32PassedVal>> 8) & 0x000000FFL);
    pPos[1] = (RTA_U8)((u32PassedVal>>16) & 0x000000FFL);
    pPos[0] = (RTA_U8)((u32PassedVal>>24) & 0x000000FFL);
#endif
}

void rta_endian_PutBigEndian64(RTA_U64 const u64PassedVal, RTA_U8 * const pPos)
{
#ifdef RTA_SWAP_ENDIAN
    pPos[0] = (RTA_U8)((u64PassedVal) & 0x000000FFL);
    pPos[1] = (RTA_U8)((u64PassedVal>> 8) & 0x000000FFL);
    pPos[2] = (RTA_U8)((u64PassedVal>>16) & 0x000000FFL);
    pPos[3] = (RTA_U8)((u64PassedVal>>24) & 0x000000FFL);
    pPos[4] = (RTA_U8)((u64PassedVal>>32) & 0x000000FFL);
    pPos[5] = (RTA_U8)((u64PassedVal>>40) & 0x000000FFL);
    pPos[6] = (RTA_U8)((u64PassedVal>>48) & 0x000000FFL);
    pPos[7] = (RTA_U8)((u64PassedVal>>56) & 0x000000FFL);
#else
    pPos[7] = (RTA_U8)((u64PassedVal) & 0x000000FFL);
    pPos[6] = (RTA_U8)((u64PassedVal>> 8) & 0x000000FFL);
    pPos[5] = (RTA_U8)((u64PassedVal>>16) & 0x000000FFL);
    pPos[4] = (RTA_U8)((u64PassedVal>>24) & 0x000000FFL);
    pPos[3] = (RTA_U8)((u64PassedVal>>32) & 0x000000FFL);
    pPos[2] = (RTA_U8)((u64PassedVal>>40) & 0x000000FFL);
    pPos[1] = (RTA_U8)((u64PassedVal>>48) & 0x000000FFL);
    pPos[0] = (RTA_U8)((u64PassedVal>>56) & 0x000000FFL);
#endif
}

void rta_endian_PutBigEndianFloat(RTA_FLOAT const fPassedVal, RTA_U8 * const pPos)
{
    RTA_U32 u32Temp;

    memcpy(&u32Temp, &fPassedVal, 4);
    rta_endian_PutBigEndian32(u32Temp, pPos);
}

void rta_endian_PutLitEndian16(RTA_U16 const u16PassedVal, RTA_U8 * const pPos)
{
#ifdef RTA_SWAP_ENDIAN
    pPos[1] = (RTA_U8)((u16PassedVal)& 0x00FF);
    pPos[0] = (RTA_U8)((u16PassedVal >> 8) & 0x00FF);
#else
    pPos[0] = (RTA_U8)((u16PassedVal)& 0x00FF);
    pPos[1] = (RTA_U8)((u16PassedVal >> 8) & 0x00FF);
#endif
}

void rta_endian_PutLitEndian32(RTA_U32 const u32PassedVal, RTA_U8 * const pPos)
{
#ifdef RTA_SWAP_ENDIAN
    pPos[3] = (RTA_U8)((u32PassedVal) & 0x000000FFL);
    pPos[2] = (RTA_U8)((u32PassedVal>> 8) & 0x000000FFL);
    pPos[1] = (RTA_U8)((u32PassedVal>>16) & 0x000000FFL);
    pPos[0] = (RTA_U8)((u32PassedVal>>24) & 0x000000FFL);
#else
    pPos[0] = (RTA_U8)((u32PassedVal) & 0x000000FFL);
    pPos[1] = (RTA_U8)((u32PassedVal>> 8) & 0x000000FFL);
    pPos[2] = (RTA_U8)((u32PassedVal>>16) & 0x000000FFL);
    pPos[3] = (RTA_U8)((u32PassedVal>>24) & 0x000000FFL);
#endif
}

void rta_endian_PutLitEndian64(RTA_U64 const u64PassedVal, RTA_U8 * const pPos)
{
#ifdef RTA_SWAP_ENDIAN
    pPos[7] = (RTA_U8)((u64PassedVal) & 0x000000FFL);
    pPos[6] = (RTA_U8)((u64PassedVal>> 8) & 0x000000FFL);
    pPos[5] = (RTA_U8)((u64PassedVal>>16) & 0x000000FFL);
    pPos[4] = (RTA_U8)((u64PassedVal>>24) & 0x000000FFL);
    pPos[3] = (RTA_U8)((u64PassedVal>>32) & 0x000000FFL);
    pPos[2] = (RTA_U8)((u64PassedVal>>40) & 0x000000FFL);
    pPos[1] = (RTA_U8)((u64PassedVal>>48) & 0x000000FFL);
    pPos[0] = (RTA_U8)((u64PassedVal>>56) & 0x000000FFL);
#else
    pPos[0] = (RTA_U8)((u64PassedVal) & 0x000000FFL);
    pPos[1] = (RTA_U8)((u64PassedVal>> 8) & 0x000000FFL);
    pPos[2] = (RTA_U8)((u64PassedVal>>16) & 0x000000FFL);
    pPos[3] = (RTA_U8)((u64PassedVal>>24) & 0x000000FFL);
    pPos[4] = (RTA_U8)((u64PassedVal>>32) & 0x000000FFL);
    pPos[5] = (RTA_U8)((u64PassedVal>>40) & 0x000000FFL);
    pPos[6] = (RTA_U8)((u64PassedVal>>48) & 0x000000FFL);
    pPos[7] = (RTA_U8)((u64PassedVal>>56) & 0x000000FFL);
#endif
}

void rta_endian_PutLitEndianFloat(RTA_FLOAT const fPassedVal, RTA_U8 * const pPos)
{
    RTA_U32 u32Temp;

    memcpy(&u32Temp, &fPassedVal, 4);
    rta_endian_PutLitEndian32(u32Temp, pPos);
}

RTA_U16 rta_endian_GetBigEndian16(RTA_U8 const * const pPos)
{
    RTA_U16 u16ReturnVal;

#ifdef RTA_SWAP_ENDIAN
    u16ReturnVal = pPos[1];
    u16ReturnVal = (RTA_U16)((u16ReturnVal << 8) | pPos[0]);
#else
    u16ReturnVal = pPos[0];
    u16ReturnVal = (RTA_U16)((u16ReturnVal << 8) | pPos[1]);
#endif

    return(u16ReturnVal);
}

RTA_U32 rta_endian_GetBigEndian32(RTA_U8 const * const pPos)
{
    RTA_U32 u32ReturnVal;

#ifdef RTA_SWAP_ENDIAN
    u32ReturnVal = pPos[3];
    u32ReturnVal = (u32ReturnVal << 8) | pPos[2];
    u32ReturnVal = (u32ReturnVal << 8) | pPos[1];
    u32ReturnVal = (u32ReturnVal << 8) | pPos[0];
#else
    u32ReturnVal = pPos[0];
    u32ReturnVal = (u32ReturnVal << 8) | pPos[1];
    u32ReturnVal = (u32ReturnVal << 8) | pPos[2];
    u32ReturnVal = (u32ReturnVal << 8) | pPos[3];
#endif

    return(u32ReturnVal);
}

RTA_U64 rta_endian_GetBigEndian64(RTA_U8 const * const pPos)
{
    RTA_U64 u64ReturnVal;

#ifdef RTA_SWAP_ENDIAN
    u64ReturnVal = pPos[7];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[6];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[5];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[4];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[3];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[2];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[1];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[0];
#else
    u64ReturnVal = pPos[0];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[1];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[2];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[3];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[4];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[5];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[6];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[7];
#endif

    return(u64ReturnVal);
}

RTA_FLOAT rta_endian_GetBigEndianFloat(RTA_U8 const * const pPos)
{
    RTA_U32 u32Temp;
    RTA_FLOAT fReturnVal;

    u32Temp = rta_endian_GetBigEndian32(pPos);
    memcpy(((RTA_U8 *)&fReturnVal), ((RTA_U8 *)&u32Temp), 4);

    return(fReturnVal);
}

RTA_U16 rta_endian_GetLitEndian16(RTA_U8 const * const pPos)
{
    RTA_U16 u16ReturnVal;

#ifdef RTA_SWAP_ENDIAN
    u16ReturnVal = pPos[0];
    u16ReturnVal = (RTA_U16)((u16ReturnVal << 8) | pPos[1]);
#else
    u16ReturnVal = pPos[1];
    u16ReturnVal = (RTA_U16)((u16ReturnVal << 8) | pPos[0]);
#endif

    return(u16ReturnVal);
}

RTA_U32 rta_endian_GetLitEndian32(RTA_U8 const * const pPos)
{
    RTA_U32 u32ReturnVal;

#ifdef RTA_SWAP_ENDIAN
    u32ReturnVal = pPos[0];
    u32ReturnVal = (u32ReturnVal << 8) | pPos[1];
    u32ReturnVal = (u32ReturnVal << 8) | pPos[2];
    u32ReturnVal = (u32ReturnVal << 8) | pPos[3];
#else
    u32ReturnVal = pPos[3];
    u32ReturnVal = (u32ReturnVal << 8) | pPos[2];
    u32ReturnVal = (u32ReturnVal << 8) | pPos[1];
    u32ReturnVal = (u32ReturnVal << 8) | pPos[0];
#endif

    return(u32ReturnVal);
}

RTA_U64 rta_endian_GetLitEndian64(RTA_U8 const * const pPos)
{
    RTA_U64 u64ReturnVal;

#ifdef RTA_SWAP_ENDIAN
    u64ReturnVal = pPos[0];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[1];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[2];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[3];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[4];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[5];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[6];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[7];
#else
    u64ReturnVal = pPos[7];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[6];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[5];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[4];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[3];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[2];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[1];
    u64ReturnVal = (u64ReturnVal << 8) | pPos[0];
#endif

    return(u64ReturnVal);
}

RTA_FLOAT rta_endian_GetLitEndianFloat(RTA_U8 const * const pPos)
{
    RTA_U32		u32Temp;
    RTA_FLOAT	fReturnVal;

    u32Temp = rta_endian_GetLitEndian32(pPos);
    memcpy(((RTA_U8 *)&fReturnVal), ((RTA_U8 *)&u32Temp), 4);

    return(fReturnVal);
}

/* ==================================================== */
/*         LOCAL FUNCTIONS                              */
/* ==================================================== */

/* *********** */
/* END OF FILE */
/* *********** */
