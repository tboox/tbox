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
 * @file        iocp_object.h
 * @ingroup     platform
 *
 */
#ifndef TB_PLATFORM_WINDOWS_IOCP_OBJECT_H
#define TB_PLATFORM_WINDOWS_IOCP_OBJECT_H

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

// the iocp object code enum
typedef enum __tb_iocp_object_code_e
{
    TB_IOCP_OBJECT_CODE_NONE   = 0
,   TB_IOCP_OBJECT_CODE_ACPT   = 1       //!< accept it
,   TB_IOCP_OBJECT_CODE_CONN   = 2       //!< connect to the host address
,   TB_IOCP_OBJECT_CODE_RECV   = 3       //!< recv data for tcp
,   TB_IOCP_OBJECT_CODE_SEND   = 4       //!< send data for tcp
,   TB_IOCP_OBJECT_CODE_URECV  = 5       //!< recv data for udp
,   TB_IOCP_OBJECT_CODE_USEND  = 6       //!< send data for udp
,   TB_IOCP_OBJECT_CODE_RECVV  = 7       //!< recv iovec data for tcp
,   TB_IOCP_OBJECT_CODE_SENDV  = 8       //!< send iovec data for tcp
,   TB_IOCP_OBJECT_CODE_URECVV = 9       //!< recv iovec data for udp
,   TB_IOCP_OBJECT_CODE_USENDV = 10      //!< send iovec data for udp
,   TB_IOCP_OBJECT_CODE_SENDF  = 11      //!< maybe return TB_STATE_NOT_SUPPORTED

,   TB_IOCP_OBJECT_CODE_MAXN   = 12

}tb_iocp_object_code_e;

// the accept iocp object type
typedef struct __tb_iocp_object_acpt_t
{
    // the client address
    tb_ipaddr_t                     addr;

    // the result socket
    tb_socket_ref_t                 result;

    // the accept address buffer, sizeof(struct sockaddr_storage)) * 2
    tb_pointer_t                    buffer;
    
}tb_iocp_object_acpt_t;

// the connection iocp object type
typedef struct __tb_iocp_object_conn_t
{
    // the connected address
    tb_ipaddr_t                     addr;

    // the result
    tb_long_t                       result;

}tb_iocp_object_conn_t;

// the recv iocp object type, base: tb_iovec_t
typedef struct __tb_iocp_object_recv_t
{
    // the data size for (tb_iovec_t*)->size
    tb_iovec_size_t                 size;

    // the recv data for (tb_iovec_t*)->data
    tb_byte_t*                      data;

    // the result
    tb_long_t                       result;

}tb_iocp_object_recv_t;

// the send iocp object type, base: tb_iovec_t
typedef struct __tb_iocp_object_send_t
{
    // the data size for (tb_iovec_t*)->size
    tb_iovec_size_t                 size;

    // the send data for (tb_iovec_t*)->data
    tb_byte_t const*                data;

    // the result
    tb_long_t                       result;

}tb_iocp_object_send_t;

// the urecv iocp object type, base: tb_iovec_t
typedef struct __tb_iocp_object_urecv_t
{
    // the data size for (tb_iovec_t*)->size
    tb_iovec_size_t                 size;

    // the recv data for (tb_iovec_t*)->data
    tb_byte_t*                      data;

    // the result
    tb_long_t                       result;

    // the addr
    tb_ipaddr_t                     addr;

}tb_iocp_object_urecv_t;

// the usend iocp object type, base: tb_iovec_t
typedef struct __tb_iocp_object_usend_t
{
    // the data size for (tb_iovec_t*)->size
    tb_iovec_size_t                 size;

    // the send data for (tb_iovec_t*)->data
    tb_byte_t const*                data;

    // the result
    tb_long_t                       result;

    // the peer addr
    tb_ipaddr_t                     addr;

}tb_iocp_object_usend_t;

// the recvv iocp object type
typedef struct __tb_iocp_object_recvv_t
{
    // the recv list
    tb_iovec_t const*               list;

    // the list size
    tb_size_t                       size;

    // the result
    tb_long_t                       result;

}tb_iocp_object_recvv_t;

// the sendv iocp object type
typedef struct __tb_iocp_object_sendv_t
{
    // the send list
    tb_iovec_t const*               list;

    // the list size
    tb_size_t                       size;

    // the result
    tb_long_t                       result;

}tb_iocp_object_sendv_t;

// the urecvv iocp object type
typedef struct __tb_iocp_object_urecvv_t
{
    // the recv list
    tb_iovec_t const*               list;

    // the list size
    tb_size_t                       size;

    // the result
    tb_long_t                       result;

    // the peer addr
    tb_ipaddr_t                     addr;

}tb_iocp_object_urecvv_t;

// the usendv iocp object type
typedef struct __tb_iocp_object_usendv_t
{
    // the send list
    tb_iovec_t const*               list;

    // the list size
    tb_size_t                       size;

    // the result
    tb_long_t                       result;

    // the addr
    tb_ipaddr_t                     addr;

}tb_iocp_object_usendv_t;

// the sendfile iocp object type
typedef struct __tb_iocp_object_sendf_t
{
    // the file
    tb_file_ref_t                   file;

    // the real
    tb_size_t                       real;

    // the size
    tb_hize_t                       size;

    // the seek
    tb_hize_t                       seek;

}tb_iocp_object_sendf_t;

// the iocp object type
typedef __tb_cpu_aligned__ struct __tb_iocp_object_t
{
    // the overlapped data
    OVERLAPPED                      olap;

    // the socket
    tb_socket_ref_t                 sock;

    // the user private data
    tb_cpointer_t                   priv;

    /* the objects 
     *
     * tb_iovec_t must be aligned by cpu-bytes for WSABUF
     */
    __tb_cpu_aligned__ union
    {
        tb_iocp_object_acpt_t       acpt;
        tb_iocp_object_conn_t       conn;
        tb_iocp_object_recv_t       recv;
        tb_iocp_object_send_t       send;
        tb_iocp_object_urecv_t      urecv;
        tb_iocp_object_usend_t      usend;
        tb_iocp_object_recvv_t      recvv;
        tb_iocp_object_sendv_t      sendv;
        tb_iocp_object_urecvv_t     urecvv;
        tb_iocp_object_usendv_t     usendv;
        tb_iocp_object_sendf_t      sendf;

    } u;

    // the object code
    tb_uint8_t                      code;

    /* the object state
     *
     * TB_STATE_OK
     * TB_STATE_PENDING
     * TB_STATE_WAITING
     * TB_STATE_FINISHED
     */
    tb_uint8_t                      state;

}tb_iocp_object_t, *tb_iocp_object_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* get or new an iocp object from the given socket in local thread
 *
 * @note only init object once in every thread
 *
 * @param sock              the socket 
 *
 * @return                  the iocp object
 */
tb_iocp_object_ref_t         tb_iocp_object_get_or_new(tb_socket_ref_t sock);

/* get iocp object from the given socket in local thread
 *
 * @param sock              the socket 
 *
 * @return                  the iocp object
 */
tb_iocp_object_ref_t         tb_iocp_object_get(tb_socket_ref_t sock);

/* remove iocp object for the given socket in local thread
 *
 * @param sock              the socket 
 */
tb_void_t                   tb_iocp_object_remove(tb_socket_ref_t sock);

/*! accept socket
 *
 * @param object            the iocp object 
 * @param addr              the client address
 *
 * @return                  the client socket 
 */
tb_socket_ref_t             tb_iocp_object_accept(tb_iocp_object_ref_t object, tb_ipaddr_ref_t addr);

/* connect the given client address
 *
 * @param object            the iocp object 
 * @param addr              the client address
 *
 * @return                  ok: 1, continue: 0; failed: -1
 */
tb_long_t                   tb_iocp_object_connect(tb_iocp_object_ref_t object, tb_ipaddr_ref_t addr);

/* connect the given client address
 *
 * @param object            the iocp object 
 * @param data              the data
 * @param size              the size
 *
 * @return                  ok: 1, continue: 0; failed: -1
 */
tb_long_t                   tb_iocp_object_send(tb_iocp_object_ref_t object, tb_byte_t const* data, tb_size_t size);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
