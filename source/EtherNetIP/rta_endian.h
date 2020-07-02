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

 #ifndef __RTA_ENDIAN_H__
 #define __RTA_ENDIAN_H__

extern RTA_U32 rta_endian_Swap32(RTA_U32 const u32PassedVal);
extern RTA_U16 rta_endian_Swap16(RTA_U16 const u16PassedVal);

extern void rta_endian_PutBigEndian16(RTA_U16 const u16PassedVal, RTA_U8 * const pPos);
extern void rta_endian_PutBigEndian32(RTA_U32 const u32PassedVal, RTA_U8 * const pPos);
extern void rta_endian_PutBigEndian64(RTA_U64 const u64PassedVal, RTA_U8 * const pPos);
extern void rta_endian_PutBigEndianFloat(RTA_FLOAT const fPassedVal, RTA_U8 * const pPos);

extern void rta_endian_PutLitEndian16(RTA_U16 const u16PassedVal, RTA_U8 * const pPos);
extern void rta_endian_PutLitEndian32(RTA_U32 const u32PassedVal, RTA_U8 * const pPos);
extern void rta_endian_PutLitEndian64(RTA_U64 const u64PassedVal, RTA_U8 * const pPos);
extern void rta_endian_PutLitEndianFloat(RTA_FLOAT const fPassedVal, RTA_U8 * const pPos);

extern RTA_U16 rta_endian_GetBigEndian16(RTA_U8 const * const pPos);
extern RTA_U32 rta_endian_GetBigEndian32(RTA_U8 const * const pPos);
extern RTA_U64 rta_endian_GetBigEndian64(RTA_U8 const * const pPos);
extern RTA_FLOAT rta_endian_GetBigEndianFloat(RTA_U8  const * const pPos);

extern RTA_U16 rta_endian_GetLitEndian16(RTA_U8 const * const pPos);
extern RTA_U32 rta_endian_GetLitEndian32(RTA_U8 const * const pPos);
extern RTA_U64 rta_endian_GetLitEndian64(RTA_U8 const * const pPos);
extern RTA_FLOAT rta_endian_GetLitEndianFloat(RTA_U8 const * const pPos);

#endif /* __RTA_ENDIAN_H__ */

/* *********** */
/* END OF FILE */
/* *********** */
