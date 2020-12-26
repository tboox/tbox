/*!The Treasure Box Library
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2009-present, TBOOX Open Source Group.
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
#include "../impl/pollerdata.h"
#include "../../container/list_entry.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// this iocp object is pipe?
#define tb_iocp_object_is_pipe(iocp_object)     ((iocp_object)->code >= TB_IOCP_OBJECT_CODE_READ && (iocp_object)->code <= TB_IOCP_OBJECT_CODE_CONNPIPE)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the iocp object code enum
typedef enum __tb_iocp_object_code_e
{
    TB_IOCP_OBJECT_CODE_NONE     = 0
,   TB_IOCP_OBJECT_CODE_ACPT     = 1       //!< accept it
,   TB_IOCP_OBJECT_CODE_CONN     = 2       //!< connect to the host address
,   TB_IOCP_OBJECT_CODE_RECV     = 3       //!< recv data for tcp
,   TB_IOCP_OBJECT_CODE_SEND     = 4       //!< send data for tcp
,   TB_IOCP_OBJECT_CODE_URECV    = 5       //!< recv data for udp
,   TB_IOCP_OBJECT_CODE_USEND    = 6       //!< send data for udp
,   TB_IOCP_OBJECT_CODE_RECVV    = 7       //!< recv iovec data for tcp
,   TB_IOCP_OBJECT_CODE_SENDV    = 8       //!< send iovec data for tcp
,   TB_IOCP_OBJECT_CODE_URECVV   = 9       //!< recv iovec data for udp
,   TB_IOCP_OBJECT_CODE_USENDV   = 10      //!< send iovec data for udp
,   TB_IOCP_OBJECT_CODE_SENDF    = 11      //!< maybe return TB_STATE_NOT_SUPPORTED
,   TB_IOCP_OBJECT_CODE_READ     = 12      //!< read data from pipe
,   TB_IOCP_OBJECT_CODE_WRITE    = 13      //!< write data to pipe
,   TB_IOCP_OBJECT_CODE_CONNPIPE = 14      //!< connect to pipe

,   TB_IOCP_OBJECT_CODE_MAXN     = 15

}tb_iocp_object_code_e;

// the accept iocp object type
typedef struct __tb_iocp_object_acpt_t
{
    // the client address
    tb_ipaddr_t                     addr;

    // the result socket
    tb_socket_ref_t                 result;

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
    tb_long_t                       result;  //<! at same position for all iorw objects

}tb_iocp_object_recv_t;

// the send iocp object type, base: tb_iovec_t
typedef struct __tb_iocp_object_send_t
{
    // the data size for (tb_iovec_t*)->size
    tb_iovec_size_t                 size;

    // the send data for (tb_iovec_t*)->data
    tb_byte_t const*                data;

    // the result
    tb_long_t                       result;  //<! at same position for all iorw objects

}tb_iocp_object_send_t;

// the urecv iocp object type, base: tb_iovec_t
typedef struct __tb_iocp_object_urecv_t
{
    // the data size for (tb_iovec_t*)->size
    tb_iovec_size_t                 size;

    // the recv data for (tb_iovec_t*)->data
    tb_byte_t*                      data;

    // the result
    tb_long_t                       result;  //<! at same position for all iorw objects

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
    tb_long_t                       result;  //<! at same position for all iorw objects

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
    tb_long_t                       result;  //<! at same position for all iorw objects

}tb_iocp_object_recvv_t;

// the sendv iocp object type
typedef struct __tb_iocp_object_sendv_t
{
    // the send list
    tb_iovec_t const*               list;

    // the list size
    tb_size_t                       size;

    // the result
    tb_long_t                       result;  //<! at same position for all iorw objects

}tb_iocp_object_sendv_t;

// the urecvv iocp object type
typedef struct __tb_iocp_object_urecvv_t
{
    // the recv list
    tb_iovec_t const*               list;

    // the list size
    tb_size_t                       size;

    // the result
    tb_long_t                       result;  //<! at same position for all iorw objects

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
    tb_long_t                       result;  //<! at same position for all iorw objects

    // the addr
    tb_ipaddr_t                     addr;

}tb_iocp_object_usendv_t;

/* the sendfile iocp object type
 *
 * @note hack result struct member offset for hacking the same result offset for the other iocp object
 *
 * see poller_iocp.c: tb_poller_iocp_event_spak_iorw()
 */
typedef struct __tb_iocp_object_sendf_t
{
#if TB_CPU_BIT64

    // the file
    tb_file_ref_t                   file;

    // the size
    tb_hize_t                       size;

    // the result
    tb_long_t                       result;  //<! at same position for all iorw objects

    // the offset
    tb_hize_t                       offset;
#else

    // the size
    tb_hize_t                       size;

    // the result
    tb_long_t                       result;  //<! at same position for all iorw objects

    // the file
    tb_file_ref_t                   file;

    // the offset
    tb_hize_t                       offset;
#endif

}tb_iocp_object_sendf_t;

// the read iocp object type for ReadFile
typedef struct __tb_iocp_object_read_t
{
    // the result
    tb_long_t                       result;

}tb_iocp_object_read_t;

// the write iocp object type for WriteFile
typedef struct __tb_iocp_object_write_t
{
    // the result
    tb_long_t                       result;

}tb_iocp_object_write_t;

// the pipe connection iocp object type
typedef struct __tb_iocp_object_connpipe_t
{
    // the result
    tb_long_t                       result;

}tb_iocp_object_connpipe_t;

// the iocp object type
typedef __tb_cpu_aligned__ struct __tb_iocp_object_t
{
    // the overlapped data
    OVERLAPPED                      olap;

    // the bound iocp port
    HANDLE                          port;

    // the user private data
    tb_cpointer_t                   priv;

    // the list entry
    tb_list_entry_t                 entry;

    /* the private buffer for iocp poller
     *
     * acpt: sizeof(struct sockaddr_storage)) * 2
     * urecv and recvv: sizeof(struct sockaddr_storage)) + sizeof(tb_int_t) + sizeof(DWORD)
     */
    tb_pointer_t                    buffer;

    /// the object reference
    union
    {
        tb_socket_ref_t             sock;
        tb_pipe_file_ref_t          pipe;
        tb_pointer_t                ptr;

    } ref;

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
        tb_iocp_object_read_t       read;
        tb_iocp_object_write_t      write;
        tb_iocp_object_connpipe_t   connpipe;
    } u;

    // the object code
    tb_uint8_t                      code;

    /* the object state
     *
     * TB_STATE_OK
     * TB_STATE_KILLING
     * TB_STATE_PENDING
     * TB_STATE_WAITING
     * TB_STATE_FINISHED
     */
    tb_uint8_t                      state;

    /* skip completion port on success?
     *
     * SetFileCompletionNotificationModes(fd, FILE_SKIP_COMPLETION_PORT_ON_SUCCESS)
     */
    tb_uint8_t                      skip_cpos;

}tb_iocp_object_t, *tb_iocp_object_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* get or new an iocp object from the given poller object in local thread
 *
 * @note only init object once in every thread
 *
 * @param object            the poller object
 * @param waitevent         the poller event which will be waited
 *
 * @return                  the iocp object
 */
tb_iocp_object_ref_t        tb_iocp_object_get_or_new(tb_poller_object_ref_t object, tb_size_t waitevent);

/* get iocp object from the given poller object in local thread
 *
 * @param object            the poller object
 * @param waitevent         the poller event which will be waited
 *
 * @return                  the iocp object
 */
tb_iocp_object_ref_t        tb_iocp_object_get(tb_poller_object_ref_t object, tb_size_t waitevent);

/* remove iocp object for the given poller object in local thread
 *
 * @param object            the poller object
 */
tb_void_t                   tb_iocp_object_remove(tb_poller_object_ref_t object);

/* clear iocp object state and buffer
 *
 * @param object            the iocp object
 */
tb_void_t                   tb_iocp_object_clear(tb_iocp_object_ref_t object);

/* read the pipe data
 *
 * @param object            the iocp object
 * @param data              the data
 * @param size              the size
 *
 * @return                  the real size or -1
 */
tb_long_t                   tb_iocp_object_read(tb_iocp_object_ref_t object, tb_byte_t* data, tb_size_t size);

/* write the pipe data
 *
 * @param object            the iocp object
 * @param data              the data
 * @param size              the size
 *
 * @return                  the real size or -1
 */
tb_long_t                   tb_iocp_object_write(tb_iocp_object_ref_t object, tb_byte_t const* data, tb_size_t size);

/* connect to the pipe (server-side)
 *
 * @param object            the iocp object
 *
 * @return                  ok: 1, continue: 0; failed: -1
 */
tb_long_t                   tb_iocp_object_connect_pipe(tb_iocp_object_ref_t object);

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

/* recv the socket data for tcp
 *
 * @param object            the iocp object
 * @param data              the data
 * @param size              the size
 *
 * @return                  the real size or -1
 */
tb_long_t                   tb_iocp_object_recv(tb_iocp_object_ref_t object, tb_byte_t* data, tb_size_t size);

/* send the socket data for tcp
 *
 * @param object            the iocp object
 * @param data              the data
 * @param size              the size
 *
 * @return                  the real size or -1
 */
tb_long_t                   tb_iocp_object_send(tb_iocp_object_ref_t object, tb_byte_t const* data, tb_size_t size);

/*! recvv the socket data for tcp
 *
 * @param object            the iocp object
 * @param list              the iovec list
 * @param size              the iovec size
 *
 * @return                  the real size or -1
 */
tb_long_t                   tb_iocp_object_recvv(tb_iocp_object_ref_t object, tb_iovec_t const* list, tb_size_t size);

/*! sendv the socket data for tcp
 *
 * @param object            the iocp object
 * @param list              the iovec list
 * @param size              the iovec size
 *
 * @return                  the real size or -1
 */
tb_long_t                   tb_iocp_object_sendv(tb_iocp_object_ref_t object, tb_iovec_t const* list, tb_size_t size);

/* recv the socket data for udp
 *
 * @param object            the iocp object
 * @param addr              the peer address(output)
 * @param data              the data
 * @param size              the size
 *
 * @return                  the real size or -1
 */
tb_long_t                   tb_iocp_object_urecv(tb_iocp_object_ref_t object, tb_ipaddr_ref_t addr, tb_byte_t* data, tb_size_t size);

/* send the socket data for udp
 *
 * @param object            the iocp object
 * @param addr              the address
 * @param data              the data
 * @param size              the size
 *
 * @return                  the real size or -1
 */
tb_long_t                   tb_iocp_object_usend(tb_iocp_object_ref_t object, tb_ipaddr_ref_t addr, tb_byte_t const* data, tb_size_t size);

/*! urecvv the socket data for udp
 *
 * @param object            the iocp object
 * @param addr              the peer address(output)
 * @param list              the iovec list
 * @param size              the iovec size
 *
 * @return                  the real size or -1
 */
tb_long_t                   tb_iocp_object_urecvv(tb_iocp_object_ref_t object, tb_ipaddr_ref_t addr, tb_iovec_t const* list, tb_size_t size);

/*! usendv the socket data for udp
 *
 * @param object            the iocp object
 * @param addr              the addr
 * @param list              the iovec list
 * @param size              the iovec size
 *
 * @return                  the real size or -1
 */
tb_long_t                   tb_iocp_object_usendv(tb_iocp_object_ref_t object, tb_ipaddr_ref_t addr, tb_iovec_t const* list, tb_size_t size);

/* send file data
 *
 * @param object            the iocp object
 * @param file              the file
 * @param offset            the offset
 * @param size              the size
 *
 * @return                  the real size or -1
 */
tb_hong_t                   tb_iocp_object_sendf(tb_iocp_object_ref_t object, tb_file_ref_t file, tb_hize_t offset, tb_hize_t size);

/* //////////////////////////////////////////////////////////////////////////////////////
 * inline implementation
 */
static __tb_inline__ tb_iocp_object_ref_t tb_iocp_object_get_or_new_from_sock(tb_socket_ref_t sock, tb_size_t waitevent)
{
    tb_poller_object_t object;
    object.type = TB_POLLER_OBJECT_SOCK;
    object.ref.sock = sock;
    return tb_iocp_object_get_or_new(&object, waitevent);
}
static __tb_inline__ tb_iocp_object_ref_t tb_iocp_object_get_from_sock(tb_socket_ref_t sock, tb_size_t waitevent)
{
    tb_poller_object_t object;
    object.type = TB_POLLER_OBJECT_SOCK;
    object.ref.sock = sock;
    return tb_iocp_object_get(&object, waitevent);
}
static __tb_inline__ tb_iocp_object_ref_t tb_iocp_object_get_or_new_from_pipe(tb_pipe_file_ref_t pipe, tb_size_t waitevent)
{
    tb_poller_object_t object;
    object.type = TB_POLLER_OBJECT_PIPE;
    object.ref.pipe = pipe;
    return tb_iocp_object_get_or_new(&object, waitevent);
}
static __tb_inline__ tb_iocp_object_ref_t tb_iocp_object_get_from_pipe(tb_pipe_file_ref_t pipe, tb_size_t waitevent)
{
    tb_poller_object_t object;
    object.type = TB_POLLER_OBJECT_PIPE;
    object.ref.pipe = pipe;
    return tb_iocp_object_get(&object, waitevent);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
