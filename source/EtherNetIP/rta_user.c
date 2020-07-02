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
#include "rta_system.h"

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
#define ROW_SIZE 16		/* number of characters per row in rta_user_print_memory */
#define MAX_SIZE 512	/* max number of characters for the buffer print in rta_user_print_memory */

/************************************************************************/
/*  GLOBAL FUNCTIONS (all must be re-entrant)                           */
/************************************************************************/
RTA_U32 rta_user_rand_u32(RTA_U32 const seed_if_not_zero)
{
	if (seed_if_not_zero)
		srand(seed_if_not_zero);

	return((RTA_U32)rand());
}

void rta_snprintf_dbg(char const * const filename, int const linenum, char * const str, size_t const size, char const * format, ...)
{
	va_list args;
	va_start (args, format);
	(void)rta_vsnprintf(str, (size-1),  format, args);
	va_end (args);
//    RTA_UNUSED_PARAM(args);

	if (strlen(str) == (size-1))
	{
		printf("%s:%d Buffer size needs to be larger!", filename, linenum);
	}
}

int rta_vsnprintf(char * const str, size_t const size, char const * const format, va_list const ap)
{
    return vsnprintf(str, size, format, ap);
}

void rta_user_print_memory(void const * const ptr, RTA_U32 const length, char * const str)
{
    RTA_U32 adjusted_len = length;
	RTA_U8 const * p = (RTA_U8 const *)ptr;
	RTA_U32 i;
	RTA_U32 j;

	if (p)
	{
		printf("-------------------------------------\r\n");
        printf("MEMORY DUMP - %s (%lu bytes)\r\n", str, length);
        if (adjusted_len > MAX_SIZE)
		{
            printf("TRUNCATED FROM %lu to %d\r\n", adjusted_len, MAX_SIZE);
            adjusted_len = MAX_SIZE;
		}
		printf("-------------------------------------");
        for (i = 0; i<adjusted_len; i += ROW_SIZE)
		{
			RTA_U8 row[ROW_SIZE];

			/* store a row at a time */
			memcpy(row, p, sizeof(row));
			p += sizeof(row);

			for (j = 0; j<ROW_SIZE; j++)
			{
				if (j == 0)
					printf("\r\n%5lu: ", i);
				else if (j == (ROW_SIZE / 2))
					printf("  ");

				if (i + j<adjusted_len)
					printf(" %02x", row[j]);
				else
					printf("   ");
			}

			printf(" ");

			for (j = 0; j<16; j++)
			{
				if (i + j<adjusted_len)
				{
					if (row[j] >= 0x20 && row[j] <= 0x7F)
						printf("%c", row[j]);
					else
						printf(".");
				}
			}
		}
		printf("\r\n-------------------------------------\r\n");
	}
}

void rta_user_log(char const * const file, RTA_U32 const line_num, RTA_U16 const task_id, RTA_U16 const dev_id, RTA_USER_LOGLEVEL_TYPE const log_level, char const *fmt, ...)
{
	va_list ap;

	RTA_UNUSED_PARAM(task_id);
	RTA_UNUSED_PARAM(dev_id);

	switch (log_level)
	{
		case RTA_USER_LOGLEVEL_ALWAYS: break;

		case RTA_USER_LOGLEVEL_ERROR: 
		#ifdef RTA_USER_ENABLE_LOGLEVEL_ERROR
            break;
        #else
            return;
		#endif

		case RTA_USER_LOGLEVEL_STARTUP:
		#ifdef RTA_USER_ENABLE_LOGLEVEL_STARTUP
            break;
        #else
            return;
		#endif

		case RTA_USER_LOGLEVEL_EVENT:
		#ifdef RTA_USER_ENABLE_LOGLEVEL_EVENT
            break;
        #else
            return;
		#endif

		case RTA_USER_LOGLEVEL_PROTOCOL:
		#ifdef RTA_USER_ENABLE_LOGLEVEL_PROTOCOL
            break;
        #else
            return;
		#endif

		case RTA_USER_LOGLEVEL_BSP:
		#ifdef RTA_USER_ENABLE_LOGLEVEL_BSP
            break;
        #else
            return;
		#endif

		/* no printing enabled */
		default:
			return;
	}

	/* Initialize the pointer to the variable length argument list */
	va_start(ap, fmt);

	/* call special sprintf to allow nested variable argument functions */
	printf("%s:%lu=> ", file, line_num);
	vprintf(fmt, ap);

	/* Cleanup the variable length argument list */
	va_end(ap);
//    RTA_UNUSED_PARAM(ap);

	return;
}

/* ==================================================== */
/*         LOCAL FUNCTIONS                              */
/* ==================================================== */

/* *********** */
/* END OF FILE */
/* *********** */
