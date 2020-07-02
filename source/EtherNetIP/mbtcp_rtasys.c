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
 *     Module Name: mbtcp_rtasys.c
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
 * This file contains the definitions needed for supporting Modbus TCP 
 * servers.  All callback functions called by the user reside in this file.
 *
 */

/* ---------------------------- */
/* INCLUDE FILES                */
/* ---------------------------- */
/* Application definitions */
#include "inc/mbtcp_rta_system.h"
#include "inc/mbtcp_system.h"

/* ---------------------------- */
/* FUNCTION PROTOTYPES          */
/* ---------------------------- */
static int16 local_findSocketIx (MBTCP_USERSYS_SOCKTYPE socket_id);
static void  local_freeSocketIx (MBTCP_USERSYS_SOCKTYPE socket_id);
static int16 local_findFreeSocketIx (void);

/* ---------------------------- */
/* STATIC VARIABLES             */
/* ---------------------------- */
static struct
{
   uint8  buf[300]; // this is fixed by the spec
   uint16 buf_size;
   MBTCP_USERSYS_SOCKTYPE socket_id;
}struct_data[MBTCP_USER_MAX_NUM_SOCKETS];

/* ---------------------------- */
/* MISCELLANEOUS                */
/* ---------------------------- */

/**/
/* ******************************************************************** */
/*               GLOBAL FUNCTIONS                                       */
/* ******************************************************************** */
/* ====================================================================
Function:     mbtcp_rtasys_init
Parameters:   N/A
Returns:      N/A

This function initializes all variables for the Modbus TCP Server stack.
======================================================================= */
void mbtcp_rtasys_init (void)
{
   uint16 i;

   /* init all objects */
   mbtcp_server_init   ();
   mbtcp_userobj_init  ();
   mbtcp_usersock_init ();
   mbtcp_usersys_init  ();

   /* initialize local structures */
   for(i=0; i<MBTCP_USER_MAX_NUM_SOCKETS; i++)
   {
      struct_data[i].socket_id = ((MBTCP_USERSYS_SOCKTYPE)MBTCP_USER_SOCKET_UNUSED);
      struct_data[i].buf_size = 0;
      memset(struct_data[i].buf, 0, sizeof(struct_data[i].buf));
   }
}

/* ====================================================================
Function:     mbtcp_rtasys_onTCPAccept
Parameters:   socket_id
Returns:      N/A

This function is called when the listening socket accepts a new
connection.  We need to set up a structure to match the new socket so 
we can receive data from the socket.
======================================================================= */
void mbtcp_rtasys_onTCPAccept (MBTCP_USERSYS_SOCKTYPE socket_id)
{
   int16 i;

   mbtcp_user_dbprint1("TCP New Connection [Socket %d]\n",socket_id);

   /* see if we can accept the new connection */
   i = local_findFreeSocketIx ();

   /* we can't receive the socket, close it */
   if(i == -1)
      mbtcp_usersock_tcpClose(socket_id);

   /* set up a structure to store messages from the socket */
   else
   {
      struct_data[i].socket_id = socket_id;
      struct_data[i].buf_size = 0;
      memset(struct_data[i].buf, 0, sizeof(struct_data[i].buf));
   }
}

/* ====================================================================
Function:     mbtcp_rtasys_onTCPData
Parameters:   socket_id
              pointer to the data
              size of the data
Returns:      N/A

This function is called when TCP data is received.  This function 
forwards the data to the Modbus message processing.
======================================================================= */
void mbtcp_rtasys_onTCPData (MBTCP_USERSYS_SOCKTYPE socket_id, uint8 * socket_data, uint16 socket_data_size)
{
   MBTCP_SOCKET_STRUCT sock;
   int16 i;
   uint16 protocol_id, msg_length;
   uint16 trans_id __attribute__((unused));
   uint16 j, left_over_data;
   uint8 done_flag;

   /* see if we have this socket */
   i = local_findSocketIx (socket_id);
   if(i == -1)
   {
      mbtcp_user_dbprint1("ON TCP Data Error [Socket %d]\n",socket_id);
      return;
   }

   #ifdef MBTCP_PRINTALL
      mbtcp_user_dbprint2("TCP Data Received %d bytes [Socket %d]\n",socket_data_size,socket_id);
   #endif

   /* add the message to the data buffer */
   memcpy(struct_data[i].buf+struct_data[i].buf_size, socket_data, socket_data_size);
   struct_data[i].buf_size = (uint16)(struct_data[i].buf_size + socket_data_size);

   // be capable of processing more than one message
   done_flag = FALSE;
   while (done_flag == FALSE)
   {
      // validate the Modbus TCP header (6 bytes) 
      if(struct_data[i].buf_size < 6)
         done_flag = TRUE;

      else
      {
         // parse the header
         trans_id    = rta_GetBigEndian16(struct_data[i].buf+MBTCP_OFFSET_TRANSID); 
         protocol_id = rta_GetBigEndian16(struct_data[i].buf+MBTCP_OFFSET_PROTOID);
         msg_length  = rta_GetBigEndian16(struct_data[i].buf+MBTCP_OFFSET_LENGTH);

         // close the socket if the protocol id is invalid or the length is too large
         if((protocol_id != 0) || (msg_length > 256))
         {
            mbtcp_usersock_tcpClose(socket_id);
            done_flag = TRUE;
         }
 
         // validate the whole message is received (msg len + header)
         else if(struct_data[i].buf_size < (msg_length+6))
         {
            done_flag = TRUE;
         }

         // message is received, process
         else
         {
            sock.sock_id = socket_id;
            sock.sock_type = SOCKTYPE_TCP;
            mbtcp_server_process (&sock, struct_data[i].buf, struct_data[i].buf_size);

            // check if more data is present
            left_over_data = (uint16)(struct_data[i].buf_size - (msg_length+6));

            // if more data is present, don't delete it             
            if(left_over_data > 0)
            {
               for(j=0; j<left_over_data; j++)
                  struct_data[i].buf[j] = struct_data[i].buf[(j+msg_length+6)];
            }

            struct_data[i].buf_size = left_over_data;
         }
      }
   }
}

/* ====================================================================
Function:     mbtcp_rtasys_onTCPSent
Parameters:   socket_id
Returns:      N/A

This function is called once data is successfully sent.  If the state
for the passed socket isn't running, cause the appropriate reset.
======================================================================= */
void mbtcp_rtasys_onTCPSent  (MBTCP_USERSYS_SOCKTYPE socket_id)
{
   MBTCP_UNUSED_PARAM(socket_id);
 
   #ifdef MBTCP_PRINTALL
      mbtcp_user_dbprint1("TCP Send Complete [Socket %d]\n",socket_id);
   #endif
}

/* ====================================================================
Function:     mbtcp_rtasys_onTCPClose
Parameters:   socket_id
Returns:      N/A

This function is called when a socket is closed.  Free all resources
associated with the socket id.
======================================================================= */
void mbtcp_rtasys_onTCPClose (MBTCP_USERSYS_SOCKTYPE socket_id)
{
   mbtcp_user_dbprint1("TCP Closed [Socket %d]\n",socket_id);

   /* free the local socket structure */
   local_freeSocketIx (socket_id);
}

/**/
/* ******************************************************************** */
/*               LOCAL FUNCTIONS                                        */
/* ******************************************************************** */
/* ====================================================================
Function:     local_findSocketIx
Parameters:   Socket ID
Returns:      Index where socket info is stored (-1 on error)

This function finds which index stores the socket information.
======================================================================= */
static int16 local_findSocketIx (MBTCP_USERSYS_SOCKTYPE socket_id)
{
   int16 i;

   /* search for a free element */
   for(i=0; i<MBTCP_USER_MAX_NUM_SOCKETS; i++)
   {
      /* the socket element is free */
      if(struct_data[i].socket_id == socket_id)
      {
         return(i);
      }
   }

   /* socket index not found */
   return (-1);
}

/* ====================================================================
Function:     local_freeSocketIx
Parameters:   Socket ID
Returns:      N/A

This function finds which index stores the passed socket id and frees
the resources.
======================================================================= */
static void local_freeSocketIx (MBTCP_USERSYS_SOCKTYPE socket_id)
{
   int16 i;

   for(i=0; i<MBTCP_USER_MAX_NUM_SOCKETS; i++)
   {
      if(struct_data[i].socket_id == socket_id)
      {
         struct_data[i].socket_id = ((MBTCP_USERSYS_SOCKTYPE)MBTCP_USER_SOCKET_UNUSED);
         struct_data[i].buf_size = 0;
         memset(struct_data[i].buf, 0, sizeof(struct_data[i].buf));
      }
   }
}

/* ====================================================================
Function:     local_findFreeSocketIx
Parameters:   N/A
Returns:      Index to store the socket info (-1 on error)

This function finds the next free index to store the socket info.
======================================================================= */
static int16 local_findFreeSocketIx (void)
{
   int16 i;

   /* search for a free element */
   for(i=0; i<MBTCP_USER_MAX_NUM_SOCKETS; i++)
   {
      /* the socket element is free */
      if(struct_data[i].socket_id == MBTCP_USER_SOCKET_UNUSED)
      {
         return(i);
      }
   }

   /* no free socket indexes */
   return (-1);
}

/* *********** */
/* END OF FILE */
/* *********** */
