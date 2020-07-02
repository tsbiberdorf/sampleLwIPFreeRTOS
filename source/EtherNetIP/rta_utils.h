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
 
#ifndef __RTA_UTILS_H__
#define __RTA_UTILS_H__

typedef char rta_ipv4_string[16];   /* Maximum IPv4 string length is 16 including null, "xxx.xxx.xxx.xxx" */
void rta_convert_ipv4_to_ipv4_string(RTA_U32 const ip, rta_ipv4_string buf);
RTA_I16 rta_strcmp_nocase(char const * const str1, char const * const str2);
RTA_I16 rta_strncmp_nocase(char const * const str1, char const * const str2, RTA_U32 const len);

void rta_snprintf_nodbg(char * const str, size_t const size, char const * format, ...);
void rta_snprintf_dbg(char const * const filename, int const linenum, char * const str, const size_t size, char const * format, ...);
int  rta_vsnprintf(char * const str, size_t const size, char const * const format, va_list const ap);

#endif /* __RTA_UTILS_H__ */
