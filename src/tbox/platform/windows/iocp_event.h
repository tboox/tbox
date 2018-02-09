/*!The Treasure Box Library
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * Copyright (C) 2009 - 2018, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        iocp_event.h
 * @ingroup     platform
 *
 */
#ifndef TB_PLATFORM_WINDOWS_IOCP_EVENT_H
#define TB_PLATFORM_WINDOWS_IOCP_EVENT_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the iocp event code enum
typedef enum __tb_iocp_event_code_e
{
    TB_IOCP_EVENT_CODE_NONE   = 0

,   TB_IOCP_EVENT_CODE_ACPT   = 1     //!< accept it
,   TB_IOCP_EVENT_CODE_CONN   = 2     //!< connect to the host address
,   TB_IOCP_EVENT_CODE_RECV   = 3     //!< recv data for tcp
,   TB_IOCP_EVENT_CODE_SEND   = 4     //!< send data for tcp
,   TB_IOCP_EVENT_CODE_URECV  = 5     //!< recv data for udp
,   TB_IOCP_EVENT_CODE_USEND  = 6     //!< send data for udp
,   TB_IOCP_EVENT_CODE_RECVV  = 7     //!< recv iovec data for tcp
,   TB_IOCP_EVENT_CODE_SENDV  = 8     //!< send iovec data for tcp
,   TB_IOCP_EVENT_CODE_URECVV = 9     //!< recv iovec data for udp
,   TB_IOCP_EVENT_CODE_USENDV = 10    //!< send iovec data for udp
,   TB_IOCP_EVENT_CODE_SENDF  = 11    //!< maybe return TB_STATE_NOT_SUPPORTED

,   TB_IOCP_EVENT_CODE_MAXN   = 12

}tb_iocp_event_code_e;

// the accept iocp event type
typedef struct __tb_iocp_event_acpt_t
{
    // the client address
    tb_ipaddr_t                     addr;
    
}tb_iocp_event_acpt_t;

// the connection iocp event type
typedef struct __tb_iocp_event_conn_t
{
    // the connected address
    tb_ipaddr_t                     addr;

}tb_iocp_event_conn_t;

// the recv iocp event type, base: tb_iovec_t
typedef struct __tb_iocp_event_recv_t
{
    // the data size for (tb_iovec_t*)->size
    tb_iovec_size_t                 size;

    // the recv data for (tb_iovec_t*)->data
    tb_byte_t*                      data;

    // the data real
    tb_size_t                       real;

}tb_iocp_event_recv_t;

// the send iocp event type, base: tb_iovec_t
typedef struct __tb_iocp_event_send_t
{
    // the data size for (tb_iovec_t*)->size
    tb_iovec_size_t                 size;

    // the send data for (tb_iovec_t*)->data
    tb_byte_t const*                data;

    // the data real
    tb_size_t                       real;

}tb_iocp_event_send_t;

// the urecv iocp event type, base: tb_iovec_t
typedef struct __tb_iocp_event_urecv_t
{
    // the data size for (tb_iovec_t*)->size
    tb_iovec_size_t                 size;

    // the recv data for (tb_iovec_t*)->data
    tb_byte_t*                      data;

    // the data real
    tb_size_t                       real;

    // the addr
    tb_ipaddr_t                     addr;

}tb_iocp_event_urecv_t;

// the usend iocp event type, base: tb_iovec_t
typedef struct __tb_iocp_event_usend_t
{
    // the data size for (tb_iovec_t*)->size
    tb_iovec_size_t                 size;

    // the send data for (tb_iovec_t*)->data
    tb_byte_t const*                data;

    // the data real
    tb_size_t                       real;

    // the peer addr
    tb_ipaddr_t                     addr;

}tb_iocp_event_usend_t;

// the recvv iocp event type
typedef struct __tb_iocp_event_recvv_t
{
    // the recv list
    tb_iovec_t const*               list;

    // the list size
    tb_size_t                       size;

    // the data real
    tb_size_t                       real;

}tb_iocp_event_recvv_t;

// the sendv iocp event type
typedef struct __tb_iocp_event_sendv_t
{
    // the send list
    tb_iovec_t const*               list;

    // the list size
    tb_size_t                       size;

    // the data real
    tb_size_t                       real;

}tb_iocp_event_sendv_t;

// the urecvv iocp event type
typedef struct __tb_iocp_event_urecvv_t
{
    // the recv list
    tb_iovec_t const*               list;

    // the list size
    tb_size_t                       size;

    // the data real
    tb_size_t                       real;

    // the peer addr
    tb_ipaddr_t                     addr;

}tb_iocp_event_urecvv_t;

// the usendv iocp event type
typedef struct __tb_iocp_event_usendv_t
{
    // the send list
    tb_iovec_t const*               list;

    // the list size
    tb_size_t                       size;

    // the data real
    tb_size_t                       real;

    // the addr
    tb_ipaddr_t                     addr;

}tb_iocp_event_usendv_t;

// the sendfile iocp event type
typedef struct __tb_iocp_event_sendf_t
{
    // the file
    tb_file_ref_t                   file;

    // the real
    tb_size_t                       real;

    // the size
    tb_hize_t                       size;

    // the seek
    tb_hize_t                       seek;

}tb_iocp_event_sendf_t;

// the iocp event type
typedef __tb_cpu_aligned__ struct __tb_iocp_event_t
{
    // the event code
    tb_uint8_t                      code;

    // the overlapped data
    OVERLAPPED                      olap;

    /* the events 
     *
     * tb_iovec_t must be aligned by cpu-bytes for WSABUF
     */
    __tb_cpu_aligned__ union
    {
        tb_iocp_event_acpt_t        acpt;
        tb_iocp_event_conn_t        conn;
        tb_iocp_event_recv_t        recv;
        tb_iocp_event_send_t        send;
        tb_iocp_event_urecv_t       urecv;
        tb_iocp_event_usend_t       usend;
        tb_iocp_event_recvv_t       recvv;
        tb_iocp_event_sendv_t       sendv;
        tb_iocp_event_urecvv_t      urecvv;
        tb_iocp_event_usendv_t      usendv;
        tb_iocp_event_sendf_t       sendf;

    } u;

}tb_iocp_event_t, *tb_iocp_event_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
