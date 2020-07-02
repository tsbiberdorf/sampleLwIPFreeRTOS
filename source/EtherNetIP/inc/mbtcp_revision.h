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
 *     Module Name: mbtcp_revision.h
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
 * This file contains the Modbus TCP Server revision history.
 *
 */

#define MBTCP_VERSION_STRING "1.03"

/* 
===========================================================================
05/05/2009  REVISION 01.03 (Author: Jamin D. Wendorf)
===========================================================================
1.  Removed compiler warnings.
2.  Added function codes 15 (write multiple coils) and 23 (read/write registers)
3.  Change code to solicit data pointers from user at run time. This way
    we don't waste RAM and they can have non-consecutive registers.
4.  Changed this file to an h file.
5.  Changed to new copyright info including our new address.

===========================================================================
12/08/2003  REVISION 01.02 (Author: Jamin D. Wendorf)
===========================================================================
1.  Visual C++ Sample Version

===========================================================================
03/26/2002  REVISION 01.01 (Author: Jamin D. Wendorf)
===========================================================================
1.  Initial Version

*/

