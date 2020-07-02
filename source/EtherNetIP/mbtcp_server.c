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
 *     Module Name: mbtcp_server.c
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
 * This file contains the definitions needed for parsing and processing
 * Modbus TCP Server Messages.  The buffer passed with the request can not
 * be used for the response since an additional request may be in the 
 * passed buffer.
 *
 */

/* ---------------------------- */
/* INCLUDE FILES                */
/* ---------------------------- */
#include "inc/mbtcp_rta_system.h"
#include "inc/mbtcp_system.h"

/* ---------------------------- */
/* LOCAL FUNCTION PROTOTYPES    */
/* ---------------------------- */

#ifdef FC01_RDCOILSTAT /* only include supported functions */
  void local_FC1ReadCoilStatus (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz);
#endif

#ifdef FC02_RDINPUTSTAT /* only include supported functions */
  void local_FC2InputCoils (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz);
#endif

#ifdef FC03_RDHOLDREG /* only include supported functions */
  void local_FC3ReadHoldRegs (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz);
#endif

#ifdef FC04_RDINPUTREG /* only include supported functions */
   void local_FC4ReadInputRegs (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz);
#endif

#ifdef FC05_WRSINGLECOIL /* only include supported functions */
   void local_FC5ForceSingleCoil (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz);
#endif

#ifdef FC06_WRSINGLEREG /* only include supported functions */
   void local_FC6PresetSingleReg (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz);
#endif

#ifdef FC07_RDEXCEPTSTAT /* only include supported functions */
   void local_FC7ReadExceptionStatus (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz);
#endif

#ifdef FC15_WRMULTCOILS /* only include supported functions */
   void local_FC15ForceMultCoils (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz);
#endif

#ifdef FC16_WRMULTREGS /* only include supported functions */
   void local_FC16PresetMultRegs (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz);
#endif

#ifdef FC23_RDWRMULTREGS /* only include supported functions */
   void local_FC23ReadWriteMultRegs (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz);
#endif

// alway able to send an error message
void local_sendErrorMsg (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint8 error_code);

/* ---------------------------- */
/* STATIC VARIABLES             */
/* ---------------------------- */

/* ---------------------------- */
/* MISCELLANEOUS                */
/* ---------------------------- */
#define ADDR_OFFSET (0) // @NOTE not sure why the original code would add +1 to add requested addresses
#define ALLOW_BROADCAST_ADDRESS (0) // @NOTE original MP8000 code allows a Modbus address of 0

MBTCP_CODESPACE uint8 MBCOIL_SHIFT_MASK_RIGHT[8]  = {0xFF,0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x80};
MBTCP_CODESPACE uint8 MBCOIL_SHIFT_MASK_LEFT[8]   = {0x00,0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F};
MBTCP_CODESPACE uint8 MBCOIL_SHIFT_MASK_UNUSED[8] = {0xFF,0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F};

uint16 mbtcps_mb_passthru_registers[100];

/**/
/* ******************************************************************** */
/*                      GLOBAL FUNCTIONS                                */
/* ******************************************************************** */
/* ====================================================================
Function:    mbtcp_server_init
Parameters:  init type
Returns:     N/A

This function initializes all static variables used in this file.
======================================================================= */
void mbtcp_server_init (void)
{
}

/* ====================================================================
Function:     mbtcp_server_process
Parameters:   socket structure pointer
              message pointer
              message size
Returns:      N/A

This function processes the Modbus TCP message. We don't care what the 
Unit Id (slave address) is.
======================================================================= */
void mbtcp_server_process (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz)
{
   // we need at least 2 bytes of data
   if(rta_GetBigEndian16(msg+MBTCP_OFFSET_LENGTH)< 2)
      return;   

   // make sure this isn't a response message
   if((msg[MBTCP_OFFSET_FUNCCODE] & MBTCP_FCERRBIT) == MBTCP_FCERRBIT)
      return;

   // if the Unit Identifier matters, verify it
   switch(msg[MBTCP_OFFSET_FUNCCODE])
   {
      #ifdef FC01_RDCOILSTAT /* only include supported functions */
      case 1:
         local_FC1ReadCoilStatus (sock, msg, msg_siz);
         break;
      #endif

      #ifdef FC02_RDINPUTSTAT /* only include supported functions */
      case 2:
         local_FC2InputCoils (sock, msg, msg_siz);
         break;
      #endif

      #ifdef FC03_RDHOLDREG /* only include supported functions */
      case 3:
         local_FC3ReadHoldRegs (sock, msg, msg_siz);
         break;
      #endif

      #ifdef FC04_RDINPUTREG /* only include supported functions */     
      case 4:
         local_FC4ReadInputRegs (sock, msg, msg_siz);
         break;
      #endif

      #ifdef FC05_WRSINGLECOIL /* only include supported functions */
      case 5:
         local_FC5ForceSingleCoil (sock, msg, msg_siz);
         break;
      #endif

      #ifdef FC06_WRSINGLEREG /* only include supported functions */
      case 6:
         local_FC6PresetSingleReg (sock, msg, msg_siz);
         break;
      #endif

      #ifdef FC07_RDEXCEPTSTAT /* only include supported functions */
      case 7:
         local_FC7ReadExceptionStatus (sock, msg, msg_siz);
         break;
      #endif

      #ifdef FC15_WRMULTCOILS /* only include supported functions */
      case 15:
         local_FC15ForceMultCoils (sock, msg, msg_siz);
         break;
      #endif

      #ifdef FC16_WRMULTREGS /* only include supported functions */
      case 16:
         local_FC16PresetMultRegs (sock, msg, msg_siz);
         break;
      #endif

      #ifdef FC23_RDWRMULTREGS /* only include supported functions */
      case 23:
         local_FC23ReadWriteMultRegs (sock, msg, msg_siz);
         break;
      #endif

      // return "Invalid Function Code"
      default:
         local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
         break;
   }
} /* END-> "mbtcp_server_process()" */

/**/
/* ******************************************************************* */
/*                      LOCAL FUNCTIONS                                */
/* ******************************************************************* */
/* ====================================================================
Function:     local_sendErrorMsg
Parameters:   socket structure pointer
              message pointer
              error code  
Returns:      N/A

This function builds the error message and sends it.
======================================================================= */
void local_sendErrorMsg (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint8 error_code)
{
   uint8 error_rsp[9], i;
   uint16 error_rsp_size;

   // return if the Unit ID isn't 0
   if(msg[MBTCP_OFFSET_UNITID] == 0)
      return;

   // echo the first 7 bytes
   error_rsp_size = 0;
   for(i=0; i<7; i++)
      error_rsp[error_rsp_size++] = msg[i];  

   // turn bit 7 ON for the function code
   error_rsp[error_rsp_size++] = (uint8)(msg[MBTCP_OFFSET_FUNCCODE] | MBTCP_FCERRBIT);  
 
   // store the error code as the first data byte
   error_rsp[error_rsp_size++] = error_code;

   // store the length
   rta_PutBigEndian16(((uint16)(error_rsp_size-6)),(error_rsp+MBTCP_OFFSET_LENGTH));

   // send the response (fixed at 9 bytes)
   mbtcp_usersock_sendData(sock->sock_id, error_rsp,error_rsp_size);
}

/* ====================================================================
Function:     local_FC1ReadCoilStatus
Parameters:   socket structure pointer
              message pointer
              message size 
Returns:      N/A

This function processes the Read Coil Status function code.
======================================================================= */
#ifdef FC01_RDCOILSTAT /* only include supported functions */
void local_FC1ReadCoilStatus (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz)
{
    static uint8  rsp_buf[300]; // max size set by Modbus TCP spec
    uint16 rsp_buf_siz;
    uint16 temp16, i;
    uint8  shift_byte_right, shift_byte_left;
    uint8  byte_cnt, temp8;
    uint16 start_addr, num_pts;
    uint8 *coil_ptr;
    uint8  bit_pos;

    MBTCP_UNUSED_PARAM(msg_siz);

    /* this is a broadcast address, return */
    if(msg[MBTCP_OFFSET_UNITID] == 0)
        return;

    /* make sure we have exactly 6 bytes of data */
    if(rta_GetBigEndian16(msg+MBTCP_OFFSET_LENGTH) != 6)
    {
        local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
        return;
    }

    /* calculate the starting address and number of points */
    start_addr = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA));
    num_pts = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA+2));

    /* they are asking for too much data */
    if(num_pts > 2000) // max size set by Modbus TCP spec
    {
        local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
        return;
    }

    /* check with the user to see if the pointer is valid */
    coil_ptr = mbtcp_userobj_getOutputCoilPtr((uint16)(start_addr+ADDR_OFFSET), num_pts, &bit_pos);
    if(!coil_ptr)
    {
        local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
        return;
    }

    /* calculate how many bytes are to be sent */
    temp16 = (uint16)((num_pts + 7)/8);
    byte_cnt = (uint8)temp16;

    /* based on the bit position, we may need perform a binary shift to
       the right */
    shift_byte_right = (uint8)(MBTCP_MODX(bit_pos,8));
    shift_byte_left  = (uint8)(8 - shift_byte_right);

    /* ************************** */
    /* build the response message */
    /* ************************** */
    rsp_buf_siz = 0;   

    // echo the first 8 bytes
    for(i=0; i<8; i++)
        rsp_buf[rsp_buf_siz++] = msg[i];  
 
    // store the byte count
    rsp_buf[rsp_buf_siz++] = byte_cnt;

    /* add data to transmit queue */
    for(i=0; i<byte_cnt; i++, coil_ptr++)
    {
        /* store the data from the first index */
        temp8 = (uint8)((*coil_ptr&MBCOIL_SHIFT_MASK_RIGHT[shift_byte_right]) >> shift_byte_right);

        /* if the data isn't byte aligned, add the bits from the next byte */
        if (shift_byte_right)
        {
            temp8 |= ((*(coil_ptr+1))&MBCOIL_SHIFT_MASK_LEFT[shift_byte_right]) << shift_byte_left;
        }
        /* if this is the last byte or only byte, mask off unused bits */
        if((i+1) == byte_cnt)
        {
            if((shift_byte_right) || (num_pts < 8))
            {                
                temp8 &= MBCOIL_SHIFT_MASK_UNUSED[(MBTCP_MODX(num_pts,8))];
            }
        }    

        // store data byte
        rsp_buf[rsp_buf_siz++] = temp8;
    }
    
    // store the length
    rta_PutBigEndian16(((uint16)(rsp_buf_siz-6)),rsp_buf+MBTCP_OFFSET_LENGTH);

    // send the response
    mbtcp_usersock_sendData(sock->sock_id, rsp_buf, rsp_buf_siz);
}
#endif

/* ====================================================================
Function:     local_FC2InputCoils
Parameters:   socket structure pointer
              message pointer
              message size 
Returns:      N/A

This function processes the Read Input Coils function code.
======================================================================= */
#ifdef FC02_RDINPUTSTAT /* only include supported functions */
void local_FC2InputCoils (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz)
{
   uint8  rsp_buf[300]; // max size set by Modbus TCP spec
   uint16 rsp_buf_siz;
   uint16 temp16, i;
   uint8  shift_byte_right, shift_byte_left;
   uint8  byte_cnt, temp8;
   uint16 start_addr, num_pts;
   uint8 *coil_ptr;
   uint8  bit_pos;
   
   MBTCP_UNUSED_PARAM(msg_siz);

   /* this is a broadcast address, return */
   if(msg[MBTCP_OFFSET_UNITID] == 0)
      return;

   /* make sure we have exactly 6 bytes of data */
   if(rta_GetBigEndian16(msg+MBTCP_OFFSET_LENGTH) != 6)
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
      return;
   }

   /* calculate the starting address and number of points */
   start_addr = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA));
   num_pts = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA+2));

   /* they are asking for too much data */
   if(num_pts > 2000) // max size set by Modbus TCP spec
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
      return;
   }

   /* check with the user to see if the pointer is valid */
   coil_ptr = mbtcp_userobj_getInputCoilPtr((uint16)(start_addr+ADDR_OFFSET), num_pts, &bit_pos);
   if(!coil_ptr)
   {
       local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
       return;
   }

   /* calculate how many bytes are to be sent */
   temp16 = (uint16)((num_pts + 7)/8);
   byte_cnt = (uint8)temp16;

   /* based on the bit position, we may need perform a binary shift to
      the right */
   shift_byte_right = (uint8)(MBTCP_MODX(bit_pos,8));
   shift_byte_left  = (uint8)(8 - shift_byte_right);

   /* ************************** */
   /* build the response message */
   /* ************************** */
   rsp_buf_siz = 0;   

   // echo the first 8 bytes
   for(i=0; i<8; i++)
      rsp_buf[rsp_buf_siz++] = msg[i];  
 
   // store the byte count
   rsp_buf[rsp_buf_siz++] = byte_cnt;

   /* add data to transmit queue */
   for(i=0; i<byte_cnt; i++, coil_ptr++)
   {
      /* store the data from the first index */
      temp8 = (uint8)((*coil_ptr&MBCOIL_SHIFT_MASK_RIGHT[shift_byte_right]) >> shift_byte_right);

      /* if the data isn't byte aligned, add the bits from the next byte */
      if (shift_byte_right)
      {
         temp8 |= ((*(coil_ptr+1))&MBCOIL_SHIFT_MASK_LEFT[shift_byte_right]) << shift_byte_left;
      }

      /* if this is the last byte, mask off unused bits */
      if((i+1) == byte_cnt)
      {
            if((shift_byte_right) || (num_pts < 8))
            {                
                temp8 &= MBCOIL_SHIFT_MASK_UNUSED[(MBTCP_MODX(num_pts,8))];
            }
      }

      // store data byte
      rsp_buf[rsp_buf_siz++] = temp8;
   }

   // store the length
   rta_PutBigEndian16(((uint16)(rsp_buf_siz-6)),rsp_buf+MBTCP_OFFSET_LENGTH);

   // send the response
   mbtcp_usersock_sendData(sock->sock_id, rsp_buf, rsp_buf_siz);
}
#endif

/* ====================================================================
Function:     local_FC3ReadHoldRegs
Parameters:   socket structure pointer
              message pointer
              message size 
Returns:      N/A

This function processes the Read Holding Register function code.
======================================================================= */
#ifdef FC03_RDHOLDREG /* only include supported functions */
void local_FC3ReadHoldRegs (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz)
{
   uint8  rsp_buf[300]; // max size set by Modbus TCP spec
   uint16 rsp_buf_siz;
   uint16 i, start_addr, num_pts;
   uint16 *reg_ptr;

   MBTCP_UNUSED_PARAM(msg_siz);

#if (ALLOW_BROADCAST_ADDRESS == 1 ) // @NOTE the MP8000 current code does allow Modbus Address of 0, so this check has to be taken out.
   /* this is a broadcast address, return */
   if(msg[MBTCP_OFFSET_UNITID] == 0)
      return;
#endif
   /* mark the pointer as NULL */
   reg_ptr = NULL;

   /* make sure we have exactly 6 bytes of data */
   if(rta_GetBigEndian16(msg+MBTCP_OFFSET_LENGTH) != 6)
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
      return;
   }

   /* calculate the starting address and number of points */
   start_addr = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA));
   num_pts = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA+2));

   /* they are asking for too much data */
   if(num_pts > 125) // max size set by Modbus TCP spec
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
      return;
   }

   /* check with the user to see if the pointer is valid */
   reg_ptr = mbtcp_userobj_getOutputRegPtr((uint16)(start_addr+ADDR_OFFSET), num_pts);
   if(!reg_ptr)
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
      return;
   }

   /* ************************** */
   /* build the response message */
   /* ************************** */
   rsp_buf_siz = 0;   

   // echo the first 8 bytes
   for(i=0; i<8; i++)
      rsp_buf[rsp_buf_siz++] = msg[i];  
 
   // store the byte count
   rsp_buf[rsp_buf_siz++] = (uint8)(num_pts * 2);

   // store the data
   for(i=0; i<num_pts; i++)
   {
     rsp_buf[rsp_buf_siz++] = MBTCP_GETHI(*(reg_ptr+i));
     rsp_buf[rsp_buf_siz++] = MBTCP_GETLO(*(reg_ptr+i));
   }

   // store the length
   rta_PutBigEndian16(((uint16)(rsp_buf_siz-6)),rsp_buf+MBTCP_OFFSET_LENGTH);

   // send the response
   mbtcp_usersock_sendData(sock->sock_id, rsp_buf, rsp_buf_siz);
}
#endif

/* ====================================================================
Function:     local_FC4ReadInputRegs
Parameters:   socket structure pointer
              message pointer
              message size 
Returns:      N/A

This function processes the Read Input Register function code.
======================================================================= */
#ifdef FC04_RDINPUTREG /* only include supported functions */
void local_FC4ReadInputRegs (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz)
{
   uint8  rsp_buf[300]; // max size set by Modbus TCP spec
   uint16 rsp_buf_siz;
   uint16 i, start_addr, num_pts;
   uint16 *reg_ptr;

   MBTCP_UNUSED_PARAM(msg_siz);

   // @NOTE the MP8000 current code does allow Modbus Address of 0, so this check has to be taken out.
//   /* this is a broadcast address, return */
//   if(msg[MBTCP_OFFSET_UNITID] == 0)
//      return;

   /* make sure we have exactly 6 bytes of data */
   if(rta_GetBigEndian16(msg+MBTCP_OFFSET_LENGTH) != 6)
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
      return;
   }

   /* calculate the starting address and number of points */
   start_addr = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA));
   num_pts = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA+2));

   /* they are asking for too much data */
   if(num_pts > 125) // max size set by Modbus TCP spec
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
      return;
   }

    /* check with the user to see if the pointer is valid */
    reg_ptr = mbtcp_userobj_getInputRegPtr((uint16)(start_addr+ADDR_OFFSET), num_pts);
    if(!reg_ptr)
    {
        local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
        return;
    }

   /* ************************** */
   /* build the response message */
   /* ************************** */
   rsp_buf_siz = 0;   

   // echo the first 8 bytes
   for(i=0; i<8; i++)
      rsp_buf[rsp_buf_siz++] = msg[i];  
 
   // store the byte count
   rsp_buf[rsp_buf_siz++] = (uint8)(num_pts * 2);

   // store the data
   for(i=0; i<num_pts; i++)
   {
      rsp_buf[rsp_buf_siz++] = MBTCP_GETHI(*(reg_ptr+i));
      rsp_buf[rsp_buf_siz++] = MBTCP_GETLO(*(reg_ptr+i));
   }

   // store the length
   rta_PutBigEndian16(((uint16)(rsp_buf_siz-6)),rsp_buf+MBTCP_OFFSET_LENGTH);

   // send the response
   mbtcp_usersock_sendData(sock->sock_id, rsp_buf, rsp_buf_siz);
}
#endif

/* ====================================================================
Function:     local_FC5ForceSingleCoil
Parameters:   socket structure pointer
              message pointer
              message size 
Returns:      N/A

This function processes the Write Single Coil function code.
======================================================================= */
#ifdef FC05_WRSINGLECOIL /* only include supported functions */
void local_FC5ForceSingleCoil (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz)
{
   uint8  rsp_buf[12]; // response is fixed at 12 bytes
   uint16 rsp_buf_siz;
   uint16 i, start_addr, num_pts;
   uint8  bit_pos;
   uint8  *coil_ptr;

   MBTCP_UNUSED_PARAM(msg_siz);

   /* first make sure we have only 6 bytes of data */
   if(rta_GetBigEndian16(msg+MBTCP_OFFSET_LENGTH) != 6)
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
      return;
   }

   /* calculate the starting address and number of points */
   start_addr = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA));
   num_pts = 1; // this is fixed, since this is a single write

   /* check with the user to see if the pointer is valid */
   coil_ptr = mbtcp_userobj_getOutputCoilPtr((uint16)(start_addr+ADDR_OFFSET), num_pts, &bit_pos);
   if(!coil_ptr)
   {
       local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
       return;
   }

   /* turn bit on */
   if(rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA+2)) == 0xFF00)
   {
      coil_ptr[0] |= (1 << bit_pos);
   }
   /* turn bit off */
   else if(rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA+2)) == 0x0000)
   {
      coil_ptr[0] &= ~(1 << bit_pos);
   }
   /* if the data isn't 0xFF00 or 0x0000, don't modify */

   // inform the user data was modified
   mbtcp_userobj_coilsWritten((uint16)(start_addr+ADDR_OFFSET),num_pts);

   /* this is a broadcast address, return */
   if(msg[MBTCP_OFFSET_UNITID] == 0)
      return;

   /* ************************** */
   /* build the response message */
   /* ************************** */
   rsp_buf_siz = 0;   

   // echo the first 12 bytes
   for(i=0; i<12; i++)
      rsp_buf[rsp_buf_siz++] = msg[i];  
 
   // store the length
   rta_PutBigEndian16(((uint16)(rsp_buf_siz-6)),rsp_buf+MBTCP_OFFSET_LENGTH);

   // send the response
   mbtcp_usersock_sendData(sock->sock_id, rsp_buf, rsp_buf_siz);
}
#endif

/* ====================================================================
Function:     local_FC6PresetSingleReg
Parameters:   socket structure pointer
              message pointer
              message size 
Returns:      N/A

This function processes the Preset Single Register function code.
======================================================================= */
#ifdef FC06_WRSINGLEREG /* only include supported functions */
void local_FC6PresetSingleReg (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz)
{
   uint8  rsp_buf[12]; // response is fixed at 12 bytes
   uint16 rsp_buf_siz;
   uint16 i, start_addr, num_pts;
   uint16 *reg_ptr;

   MBTCP_UNUSED_PARAM(msg_siz);

   /* mark the pointer as NULL */
   reg_ptr = NULL;

   /* first make sure we have only 6 bytes of data */
   if(rta_GetBigEndian16(msg+MBTCP_OFFSET_LENGTH) != 6)
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
      return;
   }

   /* calculate the starting address and number of points */
   start_addr = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA));
   num_pts = 1; // this is fixed, since this is a single write

    /* check with the user to see if the pointer is valid */
    reg_ptr = mbtcp_userobj_getOutputRegPtr((uint16)(start_addr+ADDR_OFFSET), num_pts);
    if(!reg_ptr)
    {
        local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
        return;
    }

   /* ************** */
   /* store the data */
   /* ************** */
   reg_ptr[0] = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA+2));

   // inform the user data was modified
   mbtcp_userobj_registersWritten((uint16)(start_addr+ADDR_OFFSET),num_pts);

   /* ************************** */
   /* build the response message */
   /* ************************** */

#if (ALLOW_BROADCAST_ADDRESS == 1 ) // @NOTE the MP8000 current code does allow Modbus Address of 0, so this check has to be taken out.
   /* this is a broadcast address, return */
   if(msg[MBTCP_OFFSET_UNITID] == 0)
      return;
#endif

   rsp_buf_siz = 0;   

   // echo the first 12 bytes
   for(i=0; i<12; i++)
      rsp_buf[rsp_buf_siz++] = msg[i];  
 
   // store the length
   rta_PutBigEndian16(((uint16)(rsp_buf_siz-6)),rsp_buf+MBTCP_OFFSET_LENGTH);

   // send the response
   mbtcp_usersock_sendData(sock->sock_id, rsp_buf, rsp_buf_siz);
}
#endif

/* ====================================================================
Function:     local_FC7ReadExceptionStatus
Parameters:   socket structure pointer
              message pointer
              message size 
Returns:      N/A

This function processes the Read Exception Status function code.
======================================================================= */
#ifdef FC07_RDEXCEPTSTAT /* only include supported functions */
void local_FC7ReadExceptionStatus (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz)
{
   uint8  rsp_buf[9]; // response is fixed at 12 bytes
   uint16 rsp_buf_siz;
   uint16 i;

   MBTCP_UNUSED_PARAM(msg_siz);

   /* make sure we have exactly 2 bytes of data */
   if(rta_GetBigEndian16(msg+MBTCP_OFFSET_LENGTH) != 2)
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
      return;
   }

   /* ************************** */
   /* build the response message */
   /* ************************** */

   /* this is a broadcast address, return */
   if(msg[MBTCP_OFFSET_UNITID] == 0)
      return;
 
   rsp_buf_siz = 0;   

   // echo the first 8 bytes
   for(i=0; i<8; i++)
      rsp_buf[rsp_buf_siz++] = msg[i];  
 
   // store the exception status
   rsp_buf[rsp_buf_siz++] = mbtcp_userobj_getExceptionStatus();

   // store the length
   rta_PutBigEndian16(((uint16)(rsp_buf_siz-6)),rsp_buf+MBTCP_OFFSET_LENGTH);

   // send the response
   mbtcp_usersock_sendData(sock->sock_id, rsp_buf, rsp_buf_siz);
 
}
#endif

/* ====================================================================
Function:     local_FC15ForceMultCoils
Parameters:   socket structure pointer
              message pointer
              message size 
Returns:      N/A

This function processes the Force Multiple Coils function code.
======================================================================= */
#ifdef FC15_WRMULTCOILS /* only include supported functions */
void local_FC15ForceMultCoils (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz)
{
    uint8  rsp_buf[12]; // max size set by Modbus TCP spec
    uint16 rsp_buf_siz, temp16;
    uint16 ix, i, j, k;
    uint16 rel_coil_addr;
    uint8  shift_byte_right = 0, shift_byte_left = 0;
    uint8  bit_val, bit_pos, bits_to_write, byte_cnt;    
    uint16 start_addr, num_pts;
    uint8  *coil_ptr;
        
    MBTCP_UNUSED_PARAM(msg_siz);

    /* first make sure we have at least 6 bytes of data */
    if(rta_GetBigEndian16(msg+MBTCP_OFFSET_LENGTH) < 6)
    {
        local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
        return;
    }

    /* calculate the starting address and number of points */
    start_addr = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA));
    num_pts = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA+2));

    /* they are asking for too much data */
    if(num_pts > 2000) // max size set by Modbus TCP spec
    {
        local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
        return;
    }

    /* check with the user to see if the pointer is valid */
    coil_ptr = mbtcp_userobj_getOutputCoilPtr((uint16)(start_addr+ADDR_OFFSET), num_pts, &bit_pos);
    if(!coil_ptr)
    {
       local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
       return;
    }

    /* calculate how many bytes are to be sent */
    temp16 = (uint16)((num_pts + 7)/8);

    /* read the passed number of bytes from the network */
    byte_cnt = msg[(MBTCP_OFFSET_DATA+4)];

    /* make sure the byte count equals */   
    if(byte_cnt != (uint8)temp16)
    {
        local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
        return;
    }

   /* calculate the relative starting bit position */
   rel_coil_addr = (uint16)bit_pos;

   /* start the index at 0 */
   ix = 0;

   /* based on the bit position, we may need perform a binary shift to
      the right */
   shift_byte_left = (uint8)(MBTCP_MODX(rel_coil_addr,8));
   if (shift_byte_left)
   {
      shift_byte_right  = (uint8)(8 - shift_byte_left);
   }

   /* store data */
   for(i=0; i<byte_cnt; i++, ix++)
   {
      /* ********************************************** */
      /* This is the last byte, account for unused bits */
      /* ********************************************** */
      if((i+1) == byte_cnt)
      {
         /* findout how many bits we need to write */
         bits_to_write = (uint8)(MBTCP_MODX(num_pts,8));

         /* we have a full byte to write */
         if (bits_to_write == 0)
            bits_to_write = 8;

         /* find the starting bit */
         rel_coil_addr = (uint16)(rel_coil_addr + ((byte_cnt-1)*8));

         /* write up to the last 8 bits manually */
         for(j=0; j<bits_to_write; j++, rel_coil_addr++)
         {
            /* calculate the byte index (integer divide) */
            ix = (uint16)(rel_coil_addr/8);

            /* find the bit position to write */
            for(k=0,bit_pos = 0x01; k<MBTCP_MODX(rel_coil_addr,8); k++)
               bit_pos = (uint8)(bit_pos << 1);

            /* turn the bit to the new state */
            bit_val = (uint8)(0x01 << j);

            /* turn bit ON */
            if (msg[(MBTCP_OFFSET_DATA+5+i)] & bit_val)
               coil_ptr[ix] |= bit_pos;
            /* turn bit OFF */
            else
               coil_ptr[ix] &= ~bit_pos;
         }
      }

      /* This isn't the last byte, process as normal */
      else
      {
         /* store the data from the first index */
         coil_ptr[ix] = (uint8)((coil_ptr[ix] & MBCOIL_SHIFT_MASK_LEFT[shift_byte_left]) |
                            (msg[(MBTCP_OFFSET_DATA+5+i)] << shift_byte_left));

         /* if the data isn't byte aligned, add the bits from the next byte */
         if (shift_byte_left)
         {
            coil_ptr[(ix+1)] = (uint8)((coil_ptr[(ix+1)] & MBCOIL_SHIFT_MASK_RIGHT[shift_byte_right]) |
                                   (msg[(MBTCP_OFFSET_DATA+5+i)] >> shift_byte_right));
         }
      }
   }

    // inform the user data was modified
    mbtcp_userobj_coilsWritten((uint16)(start_addr+ADDR_OFFSET),num_pts);

    /* ************************** */
    /* build the response message */
    /* ************************** */

    /* this is a broadcast address, return */
    if(msg[MBTCP_OFFSET_UNITID] == 0)
        return;

    rsp_buf_siz = 0;   

    // echo the first 12 bytes
    for(i=0; i<12; i++)
        rsp_buf[rsp_buf_siz++] = msg[i];  
 
    // store the length
    rta_PutBigEndian16(((uint16)(rsp_buf_siz-6)),rsp_buf+MBTCP_OFFSET_LENGTH);

    // send the response
    mbtcp_usersock_sendData(sock->sock_id, rsp_buf, rsp_buf_siz);
}
#endif

/* ====================================================================
Function:     local_FC16PresetMultRegs
Parameters:   socket structure pointer
              message pointer
              message size 
Returns:      N/A

This function processes the Preset Multiple Registers function code.
======================================================================= */
#ifdef FC16_WRMULTREGS /* only include supported functions */
void local_FC16PresetMultRegs (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz)
{
   uint8  rsp_buf[12]; // response is fixed at 12 bytes
   uint16 rsp_buf_siz;
   uint16 i, ix, start_addr, num_pts;
   uint8  byte_cnt;
   uint16 *reg_ptr;

   MBTCP_UNUSED_PARAM(msg_siz);

   /* mark the pointer as NULL */
   reg_ptr = NULL;

   /* first make sure we have at least 7 bytes of data */
   if(rta_GetBigEndian16(msg+MBTCP_OFFSET_LENGTH) < 7)
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
      return;
   }

   /* calculate the starting address and number of points */
   start_addr = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA));
   num_pts = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA+2));

   /* make sure we have the whole message */
   if(rta_GetBigEndian16(msg+MBTCP_OFFSET_LENGTH) != ((num_pts*2)+7))
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
      return;
   }

   /* they are trying to write too much data */
   if(num_pts > 100) // max size set by Modbus TCP spec
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
      return;
   }

   /* make sure the byte count equals (num_pts * 2) */
   byte_cnt = msg[(MBTCP_OFFSET_DATA+4)];
   if(byte_cnt != (num_pts * 2))
   {
      local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
      return;
   }

    /* check with the user to see if the pointer is valid */
    reg_ptr = mbtcp_userobj_getOutputRegPtr((uint16)(start_addr+ADDR_OFFSET), num_pts);
    if(!reg_ptr)
    {
        local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
        return;
    }

   /* ************** */
   /* store the data */
   /* ************** */
   ix = 0;
   for(i=0; i<byte_cnt; i+=2)
      reg_ptr[ix++] = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA+5+i));

   // inform the user data was modified
   mbtcp_userobj_registersWritten((uint16)(start_addr+ADDR_OFFSET),num_pts);

   /* ************************** */
   /* build the response message */
   /* ************************** */

#if (ALLOW_BROADCAST_ADDRESS == 1 ) // @NOTE the MP8000 current code does allow Modbus Address of 0, so this check has to be taken out.
   /* this is a broadcast address, return */
   if(msg[MBTCP_OFFSET_UNITID] == 0)
      return;
#endif

   rsp_buf_siz = 0;   

   // echo the first 12 bytes
   for(i=0; i<12; i++)
      rsp_buf[rsp_buf_siz++] = msg[i];  
 
   // store the length
   rta_PutBigEndian16(((uint16)(rsp_buf_siz-6)),rsp_buf+MBTCP_OFFSET_LENGTH);

   // send the response
   mbtcp_usersock_sendData(sock->sock_id, rsp_buf, rsp_buf_siz);
}
#endif

/* ====================================================================
Function:     local_FC23ReadWriteMultRegs
Parameters:   socket structure pointer
              message pointer
              message size 
Returns:      N/A

This function processes the Read/Write Registers function code.
======================================================================= */
#ifdef FC23_RDWRMULTREGS /* only include supported functions */
void local_FC23ReadWriteMultRegs (MBTCP_SOCKET_STRUCT *sock, uint8 *msg, uint16 msg_siz)
{
    uint8  rsp_buf[300]; // max size set by Modbus TCP spec
    uint16 rsp_buf_siz;
    uint16 i, ix, rd_start_addr, rd_num_pts, wr_start_addr, wr_num_pts;
    uint8  byte_cnt;
    uint16 *rd_reg_ptr, *wr_reg_ptr;

    MBTCP_UNUSED_PARAM(msg_siz);

    /* mark the pointer as NULL */
    rd_reg_ptr = NULL;
    wr_reg_ptr = NULL;

    /* first make sure we have at least 10 bytes of data */
    if(rta_GetBigEndian16(msg+MBTCP_OFFSET_LENGTH) < 10)
    {
        local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
        return;
    }

    // *********************************
    // READ PORTION
    // *********************************
      
    /* calculate the starting address and number of points */
    rd_start_addr = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA));
    rd_num_pts = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA+2));

    /* they are asking for too much data */
    if(rd_num_pts > 125) // max size set by Modbus TCP spec
    {
        local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
        return;
    }

    /* check with the user to see if the pointer is valid */
    rd_reg_ptr = mbtcp_userobj_getOutputRegPtr((uint16)(rd_start_addr+ADDR_OFFSET), rd_num_pts);
    if(!rd_reg_ptr)
    {
        local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
        return;
    }
   
    // *********************************
    // WRITE PORTION
    // *********************************
   
    /* calculate the starting address and number of points */
    wr_start_addr = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA+4));
    wr_num_pts = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA+6));
      
    /* make sure we have the whole message */
    if(rta_GetBigEndian16(msg+MBTCP_OFFSET_LENGTH) != ((wr_num_pts*2)+11))
    {
        local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
        return;
    }

    /* they are trying to write too much data */
    if(wr_num_pts > 100) // max size set by Modbus TCP spec
    {
        local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
        return;
    }

    /* make sure the byte count equals (num_pts * 2) */
    byte_cnt = msg[(MBTCP_OFFSET_DATA+8)];
    if(byte_cnt != (wr_num_pts * 2))
    {
        local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_FUNCTION);
        return;
    }

    /* check with the user to see if the pointer is valid */
    wr_reg_ptr = mbtcp_userobj_getOutputRegPtr((uint16)(wr_start_addr+ADDR_OFFSET), wr_num_pts);
    if(!wr_reg_ptr)
    {
        local_sendErrorMsg(sock, msg, MBTCP_ERRCODE_ADDRESS);
        return;
    }
        
    /* ************** */
    /* store the data */
    /* ************** */                    
    ix = 0;
    for(i=0; i<byte_cnt; i+=2)
        wr_reg_ptr[ix++] = rta_GetBigEndian16(msg+(MBTCP_OFFSET_DATA+9+i));

    // inform the user data was modified
    mbtcp_userobj_registersWritten((uint16)(wr_start_addr+ADDR_OFFSET),wr_num_pts);

    /* ************************** */
    /* build the response message */
    /* ************************** */

    /* this is a broadcast address, return */
    if(msg[MBTCP_OFFSET_UNITID] == 0)
        return;

    rsp_buf_siz = 0;   

    /* ************************** */
    /* build the response message */
    /* ************************** */
    rsp_buf_siz = 0;   

    // echo the first 8 bytes
    for(i=0; i<8; i++)
        rsp_buf[rsp_buf_siz++] = msg[i];  
 
    // store the byte count
    rsp_buf[rsp_buf_siz++] = (uint8)(rd_num_pts * 2);

    // store the data
    for(i=0; i<rd_num_pts; i++)
    {
        rsp_buf[rsp_buf_siz++] = MBTCP_GETHI(*(rd_reg_ptr+i));
        rsp_buf[rsp_buf_siz++] = MBTCP_GETLO(*(rd_reg_ptr+i));
    }

    // store the length
    rta_PutBigEndian16(((uint16)(rsp_buf_siz-6)),rsp_buf+MBTCP_OFFSET_LENGTH);

    // send the response
    mbtcp_usersock_sendData(sock->sock_id, rsp_buf, rsp_buf_siz);
}
#endif

/* *********** */
/* END OF FILE */
/* *********** */
