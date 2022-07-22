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
 * @file        poller.h
 * @ingroup     platform
 *
 */
#ifndef TB_PLATFORM_POLLER_H
#define TB_PLATFORM_POLLER_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "pipe.h"
#include "socket.h"
#include "process.h"
#include "fwatcher.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the poller type enum
typedef enum __tb_poller_type_e
{
    TB_POLLER_TYPE_NONE         = 0
,   TB_POLLER_TYPE_IOCP         = 1
,   TB_POLLER_TYPE_POLL         = 2
,   TB_POLLER_TYPE_EPOLL        = 3
,   TB_POLLER_TYPE_KQUEUE       = 4
,   TB_POLLER_TYPE_SELECT       = 5

}tb_poller_type_e;

/// the poller event enum, only for sock
typedef enum __tb_poller_event_e
{
    // the waited events
    TB_POLLER_EVENT_NONE        = TB_SOCKET_EVENT_NONE
,   TB_POLLER_EVENT_CONN        = TB_SOCKET_EVENT_CONN
,   TB_POLLER_EVENT_ACPT        = TB_SOCKET_EVENT_ACPT
,   TB_POLLER_EVENT_RECV        = TB_SOCKET_EVENT_RECV
,   TB_POLLER_EVENT_SEND        = TB_SOCKET_EVENT_SEND
,   TB_POLLER_EVENT_EALL        = TB_SOCKET_EVENT_EALL

    // the event flags after waiting
,   TB_POLLER_EVENT_CLEAR       = 0x0010 //!< edge trigger. after the event is retrieved by the user, its state is reset
,   TB_POLLER_EVENT_ONESHOT     = 0x0020 //!< causes the event to return only the first occurrence of the filter being triggered
,   TB_POLLER_EVENT_NOEXTRA     = 0x0040 //!< do not pass and storage the extra userdata for events

    /*! the event flag will be marked if the connection be closed in the edge trigger (TB_POLLER_EVENT_CLEAR)
     *
     * be similar to epoll.EPOLLRDHUP and kqueue.EV_EOF
     */
,   TB_POLLER_EVENT_EOF         = 0x0100

    /// socket error after waiting
,   TB_POLLER_EVENT_ERROR       = 0x0200

}tb_poller_event_e;

/// the poller object type enum
typedef enum __tb_poller_object_type_e
{
    TB_POLLER_OBJECT_NONE         = 0
,   TB_POLLER_OBJECT_SOCK         = 1 //!< socket
,   TB_POLLER_OBJECT_PIPE         = 2 //!< pipe
,   TB_POLLER_OBJECT_PROC         = 3 //!< process
,   TB_POLLER_OBJECT_FWATCHER     = 4 //!< fwatcher

}tb_poller_object_type_e;

/// the poller ref type
typedef __tb_typeref__(poller);

/// the poller object type
typedef struct __tb_poller_object_t
{
    /// the object type
    tb_uint8_t              type;

    /// the object reference
    union
    {
        tb_socket_ref_t     sock;
        tb_pipe_file_ref_t  pipe;
        tb_process_ref_t    proc;
        tb_fwatcher_ref_t   fwatcher;
        tb_pointer_t        ptr;

    }ref;

}tb_poller_object_t, *tb_poller_object_ref_t;

/*! the poller event func type
 *
 * @param poller    the poller
 * @param object    the poller object
 * @param events    the poller events or process status
 * @param priv      the user private data for this socket
 */
typedef tb_void_t   (*tb_poller_event_func_t)(tb_poller_ref_t poller, tb_poller_object_ref_t object, tb_long_t events, tb_cpointer_t priv);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init poller
 *
 * @param priv      the user private data
 *
 * @return          the poller
 */
tb_poller_ref_t     tb_poller_init(tb_cpointer_t priv);

/*! exit poller
 *
 * @param poller    the poller
 */
tb_void_t           tb_poller_exit(tb_poller_ref_t poller);

/*! get the poller type
 *
 * @param poller    the poller
 *
 * @return          the poller type
 */
tb_size_t           tb_poller_type(tb_poller_ref_t poller);

/*! get the user private data
 *
 * @param poller    the poller
 *
 * @return          the user private data
 */
tb_cpointer_t       tb_poller_priv(tb_poller_ref_t poller);

/*! kill all waited events, tb_poller_wait() will return -1
 *
 * @param poller    the poller
 */
tb_void_t           tb_poller_kill(tb_poller_ref_t poller);

/*! spank the poller, break the tb_poller_wait() and return all events
 *
 * @param poller    the poller
 */
tb_void_t           tb_poller_spak(tb_poller_ref_t poller);

/*! the events(clear, oneshot, ..) is supported for the poller?
 *
 * @param poller    the poller
 * @param events    the poller events
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_poller_support(tb_poller_ref_t poller, tb_size_t events);

/*! insert object to poller
 *
 * @param poller    the poller
 * @param object    the poller object
 * @param events    the poller events, it will be ignored if be process object
 * @param priv      the private data
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_poller_insert(tb_poller_ref_t poller, tb_poller_object_ref_t object, tb_size_t events, tb_cpointer_t priv);

/*! remove object from poller
 *
 * @param poller    the poller
 * @param object    the poller object
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_poller_remove(tb_poller_ref_t poller, tb_poller_object_ref_t object);

/*! modify events for the given poller object
 *
 * @param poller    the poller
 * @param object    the poller object
 * @param events    the poller events, it will be ignored if be process object
 * @param priv      the private data
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_poller_modify(tb_poller_ref_t poller, tb_poller_object_ref_t object, tb_size_t events, tb_cpointer_t priv);

/*! wait all sockets
 *
 * @param poller    the poller
 * @param func      the events function
 * @param timeout   the timeout, infinity: -1
 *
 * @return          > 0: the events number, 0: timeout or interrupted, -1: failed
 */
tb_long_t           tb_poller_wait(tb_poller_ref_t poller, tb_poller_event_func_t func, tb_long_t timeout);

/*! attach the poller to the current thread (only for windows/iocp now)
 *
 * @param poller    the poller
 */
tb_void_t           tb_poller_attach(tb_poller_ref_t poller);

/* //////////////////////////////////////////////////////////////////////////////////////
 * inline implementation
 */

/*! insert socket to poller
 *
 * @param poller    the poller
 * @param sock      the socket
 * @param events    the poller events
 * @param priv      the private data
 *
 * @return          tb_true or tb_false
 */
static __tb_inline__ tb_bool_t tb_poller_insert_sock(tb_poller_ref_t poller, tb_socket_ref_t sock, tb_size_t events, tb_cpointer_t priv)
{
    tb_poller_object_t object;
    object.type     = TB_POLLER_OBJECT_SOCK;
    object.ref.sock = sock;
    return tb_poller_insert(poller, &object, events, priv);
}

/*! remove socket from poller
 *
 * @param poller    the poller
 * @param sock      the socket
 *
 * @return          tb_true or tb_false
 */
static __tb_inline__ tb_bool_t tb_poller_remove_sock(tb_poller_ref_t poller, tb_socket_ref_t sock)
{
    tb_poller_object_t object;
    object.type     = TB_POLLER_OBJECT_SOCK;
    object.ref.sock = sock;
    return tb_poller_remove(poller, &object);
}

/*! modify events for the given socket
 *
 * @param poller    the poller
 * @param sock      the socket
 * @param events    the poller events
 * @param priv      the private data
 *
 * @return          tb_true or tb_false
 */
static __tb_inline__ tb_bool_t tb_poller_modify_sock(tb_poller_ref_t poller, tb_socket_ref_t sock, tb_size_t events, tb_cpointer_t priv)
{
    tb_poller_object_t object;
    object.type     = TB_POLLER_OBJECT_SOCK;
    object.ref.sock = sock;
    return tb_poller_modify(poller, &object, events, priv);
}

/*! insert pipe to poller
 *
 * @param poller    the poller
 * @param pipe      the pipe
 * @param events    the poller events
 * @param priv      the private data
 *
 * @return          tb_true or tb_false
 */
static __tb_inline__ tb_bool_t tb_poller_insert_pipe(tb_poller_ref_t poller, tb_pipe_file_ref_t pipe, tb_size_t events, tb_cpointer_t priv)
{
    tb_poller_object_t object;
    object.type     = TB_POLLER_OBJECT_PIPE;
    object.ref.pipe = pipe;
    return tb_poller_insert(poller, &object, events, priv);
}

/*! remove pipe from poller
 *
 * @param poller    the poller
 * @param pipe      the pipe
 *
 * @return          tb_true or tb_false
 */
static __tb_inline__ tb_bool_t tb_poller_remove_pipe(tb_poller_ref_t poller, tb_pipe_file_ref_t pipe)
{
    tb_poller_object_t object;
    object.type     = TB_POLLER_OBJECT_PIPE;
    object.ref.pipe = pipe;
    return tb_poller_remove(poller, &object);
}

/*! modify events for the given pipe
 *
 * @param poller    the poller
 * @param pipe      the pipe
 * @param events    the poller events
 * @param priv      the private data
 *
 * @return          tb_true or tb_false
 */
static __tb_inline__ tb_bool_t tb_poller_modify_pipe(tb_poller_ref_t poller, tb_pipe_file_ref_t pipe, tb_size_t events, tb_cpointer_t priv)
{
    tb_poller_object_t object;
    object.type     = TB_POLLER_OBJECT_PIPE;
    object.ref.pipe = pipe;
    return tb_poller_modify(poller, &object, events, priv);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
