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
 *     Module Name: mbtcp_usersys.c
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
 * This file contains system functions that need to be written by the user.
 * This includes the fatal error function.
 *
 */

/* ---------------------------- */
/* INCLUDE FILES                */
/* ---------------------------- */
#include "inc/mbtcp_rta_system.h"
#include "inc/mbtcp_system.h"

/* ---------------------------- */
/* EXTERN FUNCTIONS             */
/* ---------------------------- */

/* ---------------------------- */
/* LOCAL STRUCTURE DEFINITIONS  */
/* ---------------------------- */

/* ---------------------------- */
/* STATIC VARIABLES             */
/* ---------------------------- */

/* ---------------------------- */
/* LOCAL FUNCTIONS              */
/* ---------------------------- */

/* ---------------------------- */
/* MISCELLANEOUS                */
/* ---------------------------- */

/**/
/* ******************************************************************** */
/*               GLOBAL FUNCTIONS CALLED BY RTA                         */
/* ******************************************************************** */
/* ====================================================================
Function:     mbtcp_usersys_init
Parameters:   N/A
Returns:      N/A

This function initialize all user system variables.
======================================================================= */
void mbtcp_usersys_init (void)
{
}

/* ====================================================================
Function:     mbtcp_usersys_fatalError
Parameters:   N/A
Returns:      N/A

This function handles fatal errors.
======================================================================= */
void mbtcp_usersys_fatalError (char *function_name, int16 error_num)
{
//   printf("FATAL ERROR: \"%s\" Error: %d\n",function_name, error_num);
//   exit(1);
   while(1)
   {
	   ; // @TODO  @@@TERRY make breakpoit or something for this failure
   }
}

/**/
/* ******************************************************************** */
/*               LOCAL FUNCTIONS                                        */
/* ******************************************************************** */

/* *********** */
/* END OF FILE */
/* *********** */
