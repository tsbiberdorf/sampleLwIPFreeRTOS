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

/* ---------------------------- */
/* INCLUDE FILES                */
/* ---------------------------- */
#include <ctype.h>
#include "rta_system.h"

/* this must be after the include files */
#undef __RTA_FILE__
#define __RTA_FILE__ "rta_utils.c"

/* ---------------------------- */
/* INCLUDE FILES                */
/* ---------------------------- */

/* ---------------------------- */
/* FUNCTION PROTOTYPES          */
/* ---------------------------- */

/* ---------------------------- */
/* VARIABLES                    */
/* ---------------------------- */

/* ---------------------------- */
/* MISCELLANEOUS                */
/* ---------------------------- */

/************************************************************************/
/*  GLOBAL FUNCTIONS (all must be re-entrant)                           */
/************************************************************************/
void rta_convert_ipv4_to_ipv4_string(RTA_U32 const ip, char buf[16])
{
    rta_snprintf_dbg(__RTA_FILE__, __LINE__, buf, 16,"%u.%u.%u.%u",(RTA_U8)(ip>>24),(RTA_U8)((ip>>16)&0xFF),(RTA_U8)((ip>>8)&0xFF),(RTA_U8)(ip&0xFF));
}

RTA_I16 rta_strcmp_nocase (char const * const str1, char const * const str2)
{
	size_t i;
    size_t str_len;

    if(!str1 || !str2)
        return(-1);

    if(strlen(str1) != strlen(str2))
        return(-2);

    str_len=strlen(str1);
    for (i = 0; i < str_len; i++)
    {
        const char c1 = str1[i];
        const char c2 = str2[i];
        if(toupper(c1) != toupper(c2))
            return(-3);
    }

    return (0);
}

RTA_I16 rta_strncmp_nocase(char const * const str1, char const * const str2, RTA_U32 const len)
{
	RTA_U32 i;

	if (!str1 || !str2)
		return(-1);

	for (i = 0; i < len; i++)
	{
		const char c1 = str1[i];
		const char c2 = str2[i];
		if (toupper(c1) != toupper(c2))
			return(-3);
	}

	return (0);
}

/* ==================================================== */
/*         LOCAL FUNCTIONS                              */
/* ==================================================== */

/* *********** */
/* END OF FILE */
/* *********** */
