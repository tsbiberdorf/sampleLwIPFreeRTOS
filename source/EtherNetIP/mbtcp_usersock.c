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
 *     Module Name: mbtcp_usersock.c
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
 * This file contains socket definitions.  When the init routine is
 * called, the user must open a listening socket to the port 502.  When 
 * data is received, it is passed on to "mbtcp_rtasys_onTCPData" for
 * processing.
 *
 */

/* ---------------------------- */
/* INCLUDE FILES                */
/* ---------------------------- */
#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/sockets.h"
#include "lwip/src/include/lwip/debug.h"

#include "inc/mbtcp_rta_system.h"
#include "inc/mbtcp_system.h"
#include "Peripheral/RTC/rtcInterface.h"
//#include "source/ProcessorExpert/PDD_MK60D10.h"

/* ---------------------------- */
/* LOCAL STRUCTURE DEFINITIONS  */
/* ---------------------------- */
#define INACTIVITY_TIMEOUT (60)
#define USERSOCK_MAX_BUF_SIZ 300
#define USERSOCK_LISTEN_IX   0
#define USERSOCK_TCPSTART_IX 1
#define USERSOCK_SOCKID_UNUSED 0

typedef struct
{
   struct sockaddr_in    socka;
   MBTCP_USERSYS_SOCKTYPE sockid;
   uint8 tx_in_prog_flag;
   uint32_t timeStamp;
}USERSOCK_SOCKSTRUCT;

/* ---------------------------- */
/* STATIC VARIABLES             */
/* ---------------------------- */
static USERSOCK_SOCKSTRUCT usersock_sockets [MBTCP_USER_MAX_NUM_SOCKETS];

/* ---------------------------- */
/* LOCAL FUNCTIONS              */
/* ---------------------------- */
static void   local_startTcpListen     (USERSOCK_SOCKSTRUCT *user_sock);
static int16  local_findSocketIx       (MBTCP_USERSYS_SOCKTYPE socket_id);
static void   local_freeSocketIx       (MBTCP_USERSYS_SOCKTYPE socket_id);
static int16  local_findUnusedSocketIx (void);

/* ---------------------------- */
/* MISCELLANEOUS                */
/* ---------------------------- */

/**/
/* ******************************************************************** */
/*                      GLOBAL FUNCTIONS                                */
/* ******************************************************************** */
/* ====================================================================
Function:     mbtcp_usersock_init
Parameters:   N/A
Returns:      N/A

This function initialize all user socket variables.
======================================================================= */
void mbtcp_usersock_init (void)
{
   int16 i;

   /* initialize all static variables */
   for(i=0; i<MBTCP_USER_MAX_NUM_SOCKETS; i++)
   {
      usersock_sockets[i].sockid = ((MBTCP_USERSYS_SOCKTYPE)USERSOCK_SOCKID_UNUSED);
      usersock_sockets[i].tx_in_prog_flag = FALSE;
   }

   /* start a listening socket */
   local_startTcpListen (&usersock_sockets[USERSOCK_LISTEN_IX]);
}


/* ====================================================================
Function:     mbtcp_usersock_process
Parameters:   N/A
Returns:      N/A

The sockets process code is the task or function call which processes 
Modbus TCP/IP Server events. These events include TCP data received, 
connection open events, close events, data packets received and send 
complete events.
======================================================================= */
void mbtcp_usersock_process (void)
{
   int16 sock_ix;
   int16 buffer_size;
   char buffer[USERSOCK_MAX_BUF_SIZ];
   MBTCP_USERSYS_SOCKTYPE temp_sockid;
   struct netbuf *pNetBuf;
   portCHAR *pcRxString;
   unsigned portSHORT usLength;
   uint32_t timeStamp = 0;
   /* ******************************************** */
   /* Accept logic to receive a new TCP connection */
   /* ******************************************** */
   /* look for unused socket */
   sock_ix = local_findUnusedSocketIx();
   if(sock_ix < 0)
   {
       /* since we don't have a free socket, just close the new socket attempt */
       if (netconn_accept(usersock_sockets[USERSOCK_LISTEN_IX].sockid, &temp_sockid) == ERR_OK)
       {
           netconn_delete(temp_sockid);
       }

       timeStamp = RTC_TSR;
       if( (timeStamp - usersock_sockets[1].timeStamp) > INACTIVITY_TIMEOUT)
       {
    	   usersock_sockets[1].timeStamp = 0;
    	   mbtcp_usersock_tcpClose(usersock_sockets[1].sockid);
       }
       if( (timeStamp - usersock_sockets[2].timeStamp) > INACTIVITY_TIMEOUT)
       {
    	   usersock_sockets[2].timeStamp = 0;
    	   mbtcp_usersock_tcpClose(usersock_sockets[2].sockid);
       }
   }
   else
   {
	    if (netconn_accept(usersock_sockets[USERSOCK_LISTEN_IX].sockid, &usersock_sockets[sock_ix].sockid) == ERR_OK)
	    {
	        if (usersock_sockets[sock_ix].sockid != NULL)
	        {

	        	usersock_sockets[sock_ix].timeStamp = RTC_TSR;
	            // netconn_recv timeout, in ms, needs LWIP_SO_RCVTIMEO = 1 in lwipopts.h
	            netconn_set_recvtimeout(usersock_sockets[sock_ix].sockid, 1);

	            mbtcp_rtasys_onTCPAccept(usersock_sockets[sock_ix].sockid);
	        }
	    }
	    else
	    {
//        	usersock_sockets[sock_ix].timeStamp = RTC_TSR;

	    	timeStamp = RTC_TSR;
            if(usersock_sockets[1].timeStamp)
            {
            	if( (timeStamp - usersock_sockets[1].timeStamp) > INACTIVITY_TIMEOUT)
            	{
            		usersock_sockets[1].timeStamp = 0;
            		mbtcp_usersock_tcpClose(usersock_sockets[1].sockid);
            	}
            }

            if(usersock_sockets[2].timeStamp)
            {
            	if( (timeStamp - usersock_sockets[2].timeStamp) > INACTIVITY_TIMEOUT)
            	{
            		usersock_sockets[2].timeStamp = 0;
            		mbtcp_usersock_tcpClose(usersock_sockets[2].sockid);
            	}
            }
	    }
   }

   /* ******************************************** */
   /* check for new TCP data or a remote TCP close */
   /* ******************************************** */

   /* TCP Data Received or Send Complete */
   for(sock_ix=USERSOCK_TCPSTART_IX; sock_ix<MBTCP_USER_MAX_NUM_SOCKETS; sock_ix++)
   {
      /* only check used sockets */
      if (usersock_sockets[sock_ix].sockid != USERSOCK_SOCKID_UNUSED)
      {

    	    // Thread is blocked here (netconn_recv)...
    	    err_t err = netconn_recv(usersock_sockets[sock_ix].sockid, &pNetBuf);
    	    if (err == ERR_OK)
    	    {
    	        netbuf_data(pNetBuf, (void *)&pcRxString, &usLength);

    	        if (pcRxString && usLength > 0 && usLength <= USERSOCK_MAX_BUF_SIZ)
    	        {

    	        	usersock_sockets[sock_ix].timeStamp = RTC_TSR;

    	        	buffer_size = (uint16)usLength;
    	            memcpy((char *)buffer, pcRxString, usLength);

                    mbtcp_rtasys_onTCPData (usersock_sockets[sock_ix].sockid, (uint8 *)buffer, (uint16) buffer_size);
    	        }

    	        netbuf_delete(pNetBuf);
    	    }
    	    else if (err == ERR_TIMEOUT)
    	    {
    	        // do nothing
    	    }
    	    else
    	    {
                mbtcp_usersock_tcpClose(usersock_sockets[sock_ix].sockid);
                usersock_sockets[sock_ix].timeStamp = 0;
    	    }
      }

   }

} // END-> function

/* ====================================================================
Function:     mbtcp_usersock_sendData
Parameters:   socket id
              pointer to socket data
              socket data size
Returns:      N/A

This function sends the passed data to the network client.
======================================================================= */
void mbtcp_usersock_sendData (MBTCP_USERSYS_SOCKTYPE socket_id, uint8 * socket_data, uint16 socket_data_size)
{
   int16 ix;

   /* successful send (keep track to inform the user on success */
   ix = local_findSocketIx(socket_id);
   if(ix)
   {
	   if (netconn_write(usersock_sockets[ix].sockid, socket_data, socket_data_size, NETCONN_COPY) != ERR_OK)
	   {
		   mbtcp_usersock_tcpClose(socket_id);
		   return;
	   }

      usersock_sockets[ix].tx_in_prog_flag = TRUE;
      
      #ifdef MBTCP_PRINTALL
         mbtcp_user_dbprint2("Sent %d bytes [Socket %d]\n", socket_data_size, socket_id);
      #endif
   }
}

/* ====================================================================
Function:     mbtcp_usersock_tcpClose
Parameters:   socket id
Returns:      N/A

This function closes the passed socket.
======================================================================= */
void mbtcp_usersock_tcpClose (MBTCP_USERSYS_SOCKTYPE socket_id)
{
//   int16 return_code;

   netconn_delete(socket_id);

   /* try to close the socket */
//   return_code = ((int16)close (socket_id));

   /* close was successful, inform RTA source */
//   if(return_code == 0)
   {
      mbtcp_rtasys_onTCPClose(socket_id);
   }
  
   /* Free the socket to be used */
   local_freeSocketIx(socket_id);
}

/**/
/* ******************************************************************** */
/*                      LOCAL FUNCTIONS                                 */
/* ******************************************************************** */
/* ====================================================================
Function:     local_startTcpListen
Parameters:   listen socket structure pointer
Returns:      N/A

This function attemps to open and bind a TCP socket based on the
parameters stored in the socket structure.  This socket is used to
listen for other socket connect requests.
======================================================================= */
static void local_startTcpListen (USERSOCK_SOCKSTRUCT *user_sock)
{
    user_sock->sockid = netconn_new(NETCONN_TCP);

    if (user_sock->sockid == NULL)
    {
        mbtcp_usersys_fatalError("local_startTcpListen", 0);
        return;
    }

    netconn_set_recvtimeout(user_sock->sockid, 1);

    if (netconn_bind(user_sock->sockid, NULL, MBTCP_PORT) != ERR_OK)
    {
        mbtcp_usersys_fatalError("local_startTcpListen", 1);
        return;
    }
    if (netconn_listen(user_sock->sockid) != ERR_OK)
    {
        mbtcp_usersys_fatalError("local_startTcpListen", 2);
        return;
    }

    return;

#if 0
    int16 return_code;




    /* initialize listen socket */
    memset(&user_sock->socka, 0, sizeof(user_sock->socka));
    user_sock->socka.sin_family = AF_INET;
    user_sock->socka.sin_port = htons(MBTCP_PORT);
    memset(&user_sock->socka.sin_zero, 0, sizeof(user_sock->socka.sin_zero));

    /* get socket */
    user_sock->sockid = socket(AF_INET, SOCK_STREAM, 0);

    /* get failed */
    if(user_sock->sockid < 0)
    {
        mbtcp_usersys_fatalError("local_startTcpListen", 0);
        return;
    }

    /* bind socket */
    return_code = (int16) bind(user_sock->sockid, (struct sockaddr *)&user_sock->socka, sizeof(struct sockaddr));

    /* bind failed */
    if(return_code < 0)
    {
        mbtcp_usersys_fatalError("local_startTcpListen", 1);
        return;
    }

    /* listen for connection */
    return_code = (int16) listen(user_sock->sockid, 1);

    /* listen failed */
    if(return_code < 0)
    {
        mbtcp_usersys_fatalError("local_startTcpListen", 2);
        return;
    }
#endif
#if 0
   int16 return_code;

   /* initialize listen socket */
   memset(&user_sock->socka, 0, sizeof(user_sock->socka));
   user_sock->socka.sin_family = AF_INET;
   user_sock->socka.sin_port = htons(MBTCP_PORT);
   memset(&user_sock->socka.sin_zero, 0, sizeof(user_sock->socka.sin_zero));

   /* get socket */
   user_sock->sockid = socket(AF_INET, SOCK_STREAM, 0);

   /* get failed */
   if(user_sock->sockid < 0)
   {
      mbtcp_usersys_fatalError("local_startTcpListen", 0);
      return;
   }

   /* bind socket */
   return_code = ((int16)bind(user_sock->sockid, (struct sockaddr *)&user_sock->socka, sizeof(struct sockaddr)));

   /* bind failed */
   if(return_code < 0)
   {
      mbtcp_usersys_fatalError("local_startTcpListen", 1);
      return;
   }

   /* listen for connection */
   return_code = ((int16)listen(user_sock->sockid, 1));

   /* listen failed */
   if(return_code < 0)
   {
      mbtcp_usersys_fatalError("local_startTcpListen", 2);
      return;
   }
#endif
}

/* ====================================================================
Function:     local_findSocketIx
Parameters:   socket id
Returns:      ix (-1 on error)

This function searches through the array of socket ids and returns the
index for the matching element.
======================================================================= */
static int16 local_findSocketIx (MBTCP_USERSYS_SOCKTYPE socket_id)
{
   int16 i;

   /* search through the list of sockets for the passed socket id */
   for(i=0; i<MBTCP_USER_MAX_NUM_SOCKETS; i++)
      if (usersock_sockets[i].sockid == socket_id)
         return(i);
   /* no match was found */
   return(-1);
}

/* ====================================================================
Function:     local_freeSocketIx
Parameters:   socket id
Returns:      N/A

This function searches through the array of socket ids for the passed
socket id and frees the socket ix.
======================================================================= */
static void local_freeSocketIx (MBTCP_USERSYS_SOCKTYPE socket_id)
{
   int16 i;

   /* search through the list of sockets for the passed socket id */
   for(i=0; i<MBTCP_USER_MAX_NUM_SOCKETS; i++)
   {
      if (usersock_sockets[i].sockid == socket_id)
      {
         usersock_sockets[i].sockid = ((MBTCP_USERSYS_SOCKTYPE)USERSOCK_SOCKID_UNUSED);
         memset(&usersock_sockets[i].socka,0,sizeof(struct sockaddr_in));
         usersock_sockets[i].tx_in_prog_flag = FALSE;
         return;
      }
   }
}

/* ====================================================================
Function:     local_findUnusedSocketIx
Parameters:   N/A
Returns:      ix (-1 on error)

This function returns the first free index or -1 if all sockets are
used.
======================================================================= */
static int16 local_findUnusedSocketIx (void)
{
   int16 i;

   /* search through the list of sockets for a free socket */
   for(i=USERSOCK_TCPSTART_IX; i<MBTCP_USER_MAX_NUM_SOCKETS; i++)
      if (usersock_sockets[i].sockid == USERSOCK_SOCKID_UNUSED)
         return(i);
   /* no free sockets */
   return(-1);
}

/* *********** */
/* END OF FILE */
/* *********** */
