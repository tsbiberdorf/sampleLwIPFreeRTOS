/*
 *            Copyright (c) 2002-2009 by Real Time Automation, Inc.
 *
 *  This software is copyrighted by and is the sole property of
 *  Real Time Automation Inc. (RTA).  All rights, title, ownership, 
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
 *  Real Time Automation
 *  150 S. Sunny Slope Road             USA 262.439.4999
 *  Suite 130                           http://www.rtaautomation.com
 *  Brookfield, WI 53005                software@rtaautomation.com
 *
 *************************************************************************
 *
 *     Module Name: rta_proto.h
 *         Version: 1.03
 *    Version Date: 05/05/2009
 *          Author: Jamin D. Wendorf (jwendorf@rtaautomation.com)
 *        Language: Ansi C
 * Compile Options: N/A
 * Compile defines: N/A
 *       Libraries: N/A
 *    Link Options: N/A
 *
 * Description.
 * =======================================================================
 * This file contains function prototypes used by all RTA applications.
 *
 */

#ifndef __RTA_PROTO_H__
#define __RTA_PROTO_H__

/* --------------------------------------------------------------- */
/*      UTILITY FUNCTION PROTOTYPES                                */
/* --------------------------------------------------------------- */
void   rta_PutBigEndian16 (uint16 value, uint8 *pos);
void   rta_PutBigEndian32 (uint32 value, uint8 *pos);
void   rta_PutLitEndian16 (uint16 value, uint8 *pos);
void   rta_PutLitEndian32 (uint32 value, uint8 *pos);
uint16 rta_GetBigEndian16 (uint8 *pos);
uint32 rta_GetBigEndian32 (uint8 *pos);
uint16 rta_GetLitEndian16 (uint8 *pos);
uint32 rta_GetLitEndian32 (uint8 *pos);
void   rta_ByteMove       (uint8 *dst, uint8 *src, uint16 len);

#endif /* __RTA_PROTO_H__ */
