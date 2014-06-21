/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author      ruki
 * @file        aicp_iocp.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../ntstatus.h"
#include "../../ltimer.h"
#include "../../../asio/pool.h"
#include "../../../utils/utils.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the olap list maxn for GetQueuedCompletionStatusEx and adapter to queue 
#ifdef __tb_small__
#   define TB_IOCP_OLAP_LIST_MAXN                   (63)
#else
#   define TB_IOCP_OLAP_LIST_MAXN                   (255)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the iocp func type
typedef struct __tb_iocp_func_t
{
    // the AcceptEx func
    tb_mswsock_AcceptEx_t                       AcceptEx;

    // the ConnectEx func
    tb_mswsock_ConnectEx_t                      ConnectEx;

    // the TransmitFile func
    tb_mswsock_TransmitFile_t                   TransmitFile;

    // the GetQueuedCompletionStatusEx func
    tb_kernel32_GetQueuedCompletionStatusEx_t   GetQueuedCompletionStatusEx;
 
    // WSAGetLastError
    tb_ws2_32_WSAGetLastError_t                 WSAGetLastError;

    // WSASend
    tb_ws2_32_WSASend_t                         WSASend;

    // WSARecv
    tb_ws2_32_WSARecv_t                         WSARecv;

    // WSASendTo
    tb_ws2_32_WSASendTo_t                       WSASendTo;

    // WSARecvFrom
    tb_ws2_32_WSARecvFrom_t                     WSARecvFrom;

    // bind
    tb_ws2_32_bind_t                            bind;

}tb_iocp_func_t;

// the iocp ptor type
typedef struct __tb_iocp_ptor_t
{
    // the proactor base
    tb_aicp_proactor_t                          base;

    // the i/o completion port
    HANDLE                                      port;

    // the timer for task
    tb_handle_t                                 timer;

    // the low precision timer for timeout
    tb_handle_t                                 ltimer;

    // the post loop
    tb_handle_t                                 loop;

    // the post wait
    tb_handle_t                                 wait;

    /* the aice post
     *
     * index: 0: higher priority for conn, acpt and task
     * index: 1: lower priority for io aice 
     */
    tb_queue_t*                                 post[2];
    
    // the post lock
    tb_spinlock_t                               lock;

    // the post func
    tb_iocp_func_t                              func;

}tb_iocp_ptor_t;

// the iocp olap type
typedef struct __tb_iocp_olap_t
{
    // the base
    OVERLAPPED                                  base;
    
    // the aice
    tb_aice_t                                   aice;

}tb_iocp_olap_t;

// the iocp aico type
typedef struct __tb_iocp_aico_t
{
    // the base
    tb_aico_t                                   base;

    // the ptor
    tb_iocp_ptor_t*                             ptor;

    // the olap
    tb_iocp_olap_t                              olap;
    
    // the task
    tb_handle_t                                 task;

    // is ltimer?
    tb_uint8_t                                  bltimer : 1;

}tb_iocp_aico_t;

// the iocp loop type
typedef struct __tb_iocp_loop_t
{
    // the self
    tb_size_t                                   self;

    // the olap list
    tb_OVERLAPPED_ENTRY_t                       list[TB_IOCP_OLAP_LIST_MAXN];

    // the aice spak 
    tb_queue_t*                                 spak;                   

}tb_iocp_loop_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * timeout 
 */
static tb_void_t tb_iocp_spak_timeout(tb_bool_t killed, tb_cpointer_t priv)
{
    // the aico
    tb_iocp_aico_t* aico = (tb_iocp_aico_t*)priv;
    tb_assert_and_check_return(aico);

    // the ptor
    tb_iocp_ptor_t* ptor = aico->ptor;
    tb_assert_and_check_return(ptor);

    // cancel it
    switch (aico->base.type)
    {
    case TB_AICO_TYPE_SOCK:
    case TB_AICO_TYPE_FILE:
        {
            // check
            tb_assert_and_check_break(aico->base.handle);

            // trace
            tb_trace_d("spak: timeout[%p]: code: %lu", aico->base.handle, aico->olap.aice.code);

            // the handle
            HANDLE handle = aico->base.type == TB_AICO_TYPE_SOCK? (HANDLE)((SOCKET)aico->base.handle - 1) : aico->base.handle;

            // CancelIo it
            if (!CancelIo(handle))
            {
                tb_trace_e("cancel: failed: %u", GetLastError());
            }
        }
        break;
    default:
        tb_assert(0);
        break;
    }
}
static tb_void_t tb_iocp_spak_timeout_runtask(tb_bool_t killed, tb_cpointer_t priv)
{
    // the aico
    tb_iocp_aico_t* aico = (tb_iocp_aico_t*)priv;
    tb_assert_and_check_return(aico);

    // the ptor
    tb_iocp_ptor_t* ptor = aico->ptor;
    tb_assert_and_check_return(ptor);

    // trace
    tb_trace_d("runtask: timeout: when: %llu", aico->olap.aice.u.runtask.when);

    // post ok
    aico->olap.aice.state = killed? TB_STATE_KILLED : TB_STATE_OK;
    PostQueuedCompletionStatus(ptor->port, 0, (ULONG_PTR)aico, (LPOVERLAPPED)&aico->olap);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * spak 
 */
static tb_long_t tb_iocp_spak_acpt(tb_iocp_ptor_t* ptor, tb_aice_t* resp, tb_size_t real, tb_size_t error)
{
    // check?
    tb_assert_and_check_return_val(ptor && resp && resp->aico, -1);

    // done
    switch (error)
    {
        // ok or pending?
    case ERROR_SUCCESS:
    case WAIT_TIMEOUT:
    case ERROR_IO_PENDING:
        {
            // done state
            switch (resp->state)
            {
            case TB_STATE_OK:
            case TB_STATE_PENDING:
                resp->state = resp->u.acpt.sock? TB_STATE_OK : TB_STATE_FAILED;
                break;
            default:
                // using the self state here
                break;
            }
        }
        break;
        // canceled? timeout?
    case WSAEINTR:
    case ERROR_OPERATION_ABORTED:
        {
            resp->state = TB_STATE_TIMEOUT;
        }
        break;
        // unknown error
    default:
        {
            resp->state = TB_STATE_FAILED;
            tb_trace_d("acpt: unknown error: %u", error);
        }
        break;
    }

    // failed? exit sock
    if (resp->state != TB_STATE_OK)
    {
        if (resp->u.acpt.sock) tb_socket_clos(resp->u.acpt.sock);
        resp->u.acpt.sock = tb_null;
    }

    // exit data
    if (resp->u.acpt.priv[0]) tb_aico_pool_free((tb_aico_t*)resp->aico, resp->u.acpt.priv[0]);
    resp->u.acpt.priv[0] = tb_null;

    // ok
    return 1;
}
static tb_long_t tb_iocp_spak_conn(tb_iocp_ptor_t* ptor, tb_aice_t* resp, tb_size_t real, tb_size_t error)
{
    // check?
    tb_assert_and_check_return_val(resp, -1);

    // done
    switch (error)
    {
        // ok or pending?
    case ERROR_SUCCESS:
    case WAIT_TIMEOUT:
    case ERROR_IO_PENDING:
        {
            // done state
            switch (resp->state)
            {
            case TB_STATE_OK:
            case TB_STATE_PENDING:
                resp->state = TB_STATE_OK;
                break;
            default:
                // using the self state here
                break;
            }
        }
        break;
        // failed?
    case WSAENOTCONN:
    case WSAECONNREFUSED:
    case ERROR_CONNECTION_REFUSED:
        {
            resp->state = TB_STATE_FAILED;
        }
        break;
        // timeout?
    case WSAEINTR:
    case ERROR_SEM_TIMEOUT:
    case ERROR_OPERATION_ABORTED:
        {
            resp->state = TB_STATE_TIMEOUT;
        }
        break;
        // unknown error
    default:
        {
            resp->state = TB_STATE_FAILED;
            tb_trace_d("conn: unknown error: %u", error);
        }
        break;
    }

    // ok
    return 1;
}
static tb_long_t tb_iocp_spak_iorw(tb_iocp_ptor_t* ptor, tb_aice_t* resp, tb_size_t real, tb_size_t error)
{
    // check?
    tb_assert_and_check_return_val(resp, -1);

    // ok?
    if (real)
    {
        // trace
        tb_trace_d("iorw: aico: %p, code: %lu, real: %lu", resp->aico, resp->code, real);

        // save the real size, @note: hack the real offset for the other io aice
        resp->u.recv.real = real;

        // ok
        resp->state = TB_STATE_OK;
        return 1;
    }

    // error? 
    switch (error)
    {       
        // ok or pending?
    case ERROR_SUCCESS:
    case WAIT_TIMEOUT:
    case ERROR_IO_PENDING:
        {
            // done state
            switch (resp->state)
            {
            case TB_STATE_OK:
            case TB_STATE_PENDING:
                resp->state = TB_STATE_CLOSED;
                break;
            default:
                // using the self state here
                break;
            }
        }
        break;
        // closed?
    case WSAECONNRESET:
    case ERROR_HANDLE_EOF:
    case ERROR_NETNAME_DELETED:
    case ERROR_BAD_COMMAND:
        {
            resp->state = TB_STATE_CLOSED;
        }
        break;
        // canceled? timeout 
    case WSAEINTR:
    case ERROR_OPERATION_ABORTED:
        {
            resp->state = TB_STATE_TIMEOUT;
        }
        break;
        // unknown error
    default:
        {
            // trace
            tb_trace_e("iorw: code: %lu, unknown error: %lu", resp->code, error);

            // failed
            resp->state = TB_STATE_FAILED;
        }
        break;
    }

    // ok
    return 1;
}
static tb_long_t tb_iocp_spak_fsync(tb_iocp_ptor_t* ptor, tb_aice_t* resp, tb_size_t real, tb_size_t error)
{
    // check?
    tb_assert_and_check_return_val(resp, -1);

    // done 
    switch (error)
    {   
        // ok or pending?
    case ERROR_SUCCESS:
    case WAIT_TIMEOUT:
    case ERROR_IO_PENDING:
        {
            // done state
            switch (resp->state)
            {
            case TB_STATE_OK:
            case TB_STATE_PENDING:
                resp->state = TB_STATE_OK;
                break;
            default:
                break;
            }
        }
        break;
        // closed?
    case ERROR_HANDLE_EOF:
    case ERROR_NETNAME_DELETED:
        {
            resp->state = TB_STATE_CLOSED;
        }
        break;
        // unknown error
    default:
        {
            resp->state = TB_STATE_FAILED;
            tb_trace_d("fsync: unknown error: %u", error);
        }
        break;
    }

    // ok
    return 1;
}
static tb_long_t tb_iocp_spak_runtask(tb_iocp_ptor_t* ptor, tb_aice_t* resp, tb_size_t real, tb_size_t error)
{
    // check?
    tb_assert_and_check_return_val(resp, -1);

    // done 
    switch (error)
    {   
        // ok or pending?
    case ERROR_SUCCESS:
    case WAIT_TIMEOUT:
    case ERROR_IO_PENDING:
        {
            // done state
            switch (resp->state)
            {
            case TB_STATE_OK:
            case TB_STATE_PENDING:
                resp->state = TB_STATE_OK;
                break;
            default:
                break;
            }
        }
        break;
        // unknown error
    default:
        {
            resp->state = TB_STATE_FAILED;
            tb_trace_d("runtask: unknown error: %u", error);
        }
        break;
    }

    // ok
    return 1;
}
static tb_long_t tb_iocp_spak_done(tb_iocp_ptor_t* ptor, tb_aice_t* resp, tb_size_t real, tb_size_t error)
{
    // check?
    tb_assert_and_check_return_val(resp && resp->aico, -1);

    // no pending? spak it directly
    tb_check_return_val(resp->state == TB_STATE_PENDING, 1);

    // killed?
    if (tb_aico_is_killed((tb_iocp_aico_t*)resp->aico))
    {
        // save state
        resp->state = TB_STATE_KILLED;

        // trace
        tb_trace_d("spak: code: %u: killed", resp->code);

        // ok
        return 1;
    }

    // init spak
    static tb_long_t (*s_spak[])(tb_iocp_ptor_t* , tb_aice_t* , tb_size_t , tb_size_t ) = 
    {
        tb_null
    ,   tb_iocp_spak_acpt
    ,   tb_iocp_spak_conn
    ,   tb_iocp_spak_iorw
    ,   tb_iocp_spak_iorw
    ,   tb_iocp_spak_iorw
    ,   tb_iocp_spak_iorw
    ,   tb_iocp_spak_iorw
    ,   tb_iocp_spak_iorw
    ,   tb_iocp_spak_iorw
    ,   tb_iocp_spak_iorw
    ,   tb_iocp_spak_iorw
    ,   tb_iocp_spak_iorw
    ,   tb_iocp_spak_iorw
    ,   tb_iocp_spak_iorw
    ,   tb_iocp_spak_iorw
    ,   tb_iocp_spak_fsync
    ,   tb_iocp_spak_runtask
    };
    tb_assert_and_check_return_val(resp->code < tb_arrayn(s_spak), -1);

    // trace
    tb_trace_d("spak: aico: %p, code: %u: done: ..", resp->aico, resp->code);

    // done spak
    return (s_spak[resp->code])? s_spak[resp->code](ptor, resp, real, error) : -1;
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * post
 */
static tb_void_t tb_iocp_post_timeout(tb_iocp_ptor_t* ptor, tb_iocp_aico_t* aico)
{
    // check
    tb_assert_and_check_return(ptor && ptor->ltimer && aico);
    
    // only for sock
    tb_check_return(aico->base.type == TB_AICO_TYPE_SOCK);

    // add timeout task
    tb_long_t timeout = tb_aico_timeout_from_code(aico, aico->olap.aice.code);
    if (timeout >= 0)
    {
        // trace
        tb_trace_d("post: timeout: aico: %p, code: %lu: ..", aico, aico->olap.aice.code);

        // check
        tb_assert_and_check_return(!aico->task);

        // add the new task
        aico->task = tb_ltimer_task_init(ptor->ltimer, timeout, tb_false, tb_iocp_spak_timeout, aico);
        aico->bltimer = 1;
    }
}
static tb_void_t tb_iocp_post_timeout_cancel(tb_iocp_ptor_t* ptor, tb_iocp_aico_t* aico)
{
    // check
    tb_assert_and_check_return(ptor && ptor->ltimer && aico);
   
    // remove timeout task
    if (aico->task) 
    {
        if (aico->bltimer) tb_ltimer_task_exit(ptor->ltimer, aico->task);
        else tb_timer_task_exit(ptor->timer, aico->task);
        aico->bltimer = 0;
    }
    aico->task = tb_null;
}
static tb_bool_t tb_iocp_post_acpt(tb_iocp_ptor_t* ptor, tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(ptor && ptor->port && ptor->func.AcceptEx && ptor->base.aicp, tb_false);

    // check aice
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_ACPT, tb_false);

    // the aico
    tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
    tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

    // trace
    tb_trace_d("accept: aico: %p: ..", aico);

    // done
    tb_bool_t       ok = tb_false;
    tb_bool_t       init_ok = tb_false;
    tb_bool_t       AcceptEx_ok = tb_false;
    do
    {
        // init olap
        tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));

        // init aice, hack: sizeof(tb_iocp_olap_t) >= ((sizeof(SOCKADDR_IN) + 16) << 1)
        aico->olap.aice                 = *aice;
        aico->olap.aice.u.acpt.sock     = tb_socket_open(TB_SOCKET_TYPE_TCP);
        aico->olap.aice.u.acpt.priv[0] = (tb_handle_t)tb_aico_pool_malloc0((tb_aico_t*)aico, ((sizeof(SOCKADDR_IN) + 16) << 1));
        tb_assert_static(tb_arrayn(aico->olap.aice.u.acpt.priv));
        tb_assert_and_check_break(aico->olap.aice.u.acpt.priv[0] && aico->olap.aice.u.acpt.sock);
        init_ok = tb_true;

        // post timeout first
        tb_iocp_post_timeout(ptor, aico);

        // done AcceptEx
        DWORD real = 0;
        AcceptEx_ok = ptor->func.AcceptEx(  (SOCKET)aico->base.handle - 1
                                        ,   (SOCKET)aico->olap.aice.u.acpt.sock - 1
                                        ,   (tb_byte_t*)aico->olap.aice.u.acpt.priv[0]
                                        ,   0
                                        ,   sizeof(SOCKADDR_IN) + 16
                                        ,   sizeof(SOCKADDR_IN) + 16
                                        ,   &real
                                        ,   (LPOVERLAPPED)&aico->olap)? tb_true : tb_false;
        tb_trace_d("AcceptEx: aico: %p, ok: %d, error: %d", aico, AcceptEx_ok, ptor->func.WSAGetLastError());
        tb_check_break(AcceptEx_ok);

        // post ok
        aico->olap.aice.state = TB_STATE_OK;
        if (!PostQueuedCompletionStatus(ptor->port, 0, (ULONG_PTR)aico, (LPOVERLAPPED)&aico->olap)) break;

        // ok
        ok = tb_true;

    } while (0);

    // AcceptEx failed? 
    if (init_ok && !AcceptEx_ok)
    {
        // pending? continue it
        if (WSA_IO_PENDING == ptor->func.WSAGetLastError()) 
        {
            // ok
            ok = tb_true;
        }
        // failed? 
        else
        {
            // post failed
            aico->olap.aice.state = TB_STATE_FAILED;
            if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG_PTR)aico, (LPOVERLAPPED)&aico->olap)) ok = tb_true;

            // trace
            tb_trace_d("AcceptEx: unknown error: %d", ptor->func.WSAGetLastError());
        }
    }

    // error? 
    if (!ok)
    {
        // exit data
        if (aico->olap.aice.u.acpt.priv[0]) tb_aico_pool_free((tb_aico_t*)aico, aico->olap.aice.u.acpt.priv[0]);
        aico->olap.aice.u.acpt.priv[0] = tb_null;

        // exit sock
        if (aico->olap.aice.u.acpt.sock) tb_socket_clos(aico->olap.aice.u.acpt.sock);
        aico->olap.aice.u.acpt.sock = tb_null;

        // remove timeout task
        tb_iocp_post_timeout_cancel(ptor, aico);
    }

    // ok?
    return ok;
}
static tb_bool_t tb_iocp_post_conn(tb_iocp_ptor_t* ptor, tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(ptor && ptor->port && ptor->base.aicp, tb_false);

    // check aice
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_CONN, tb_false);
    tb_assert_and_check_return_val(aice->u.conn.port, tb_false);
    
    // the aico
    tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
    tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

    // trace
    tb_trace_d("connect: aico: %p, %u.%u.%u.%u: %lu: ..", aico, aice->u.conn.addr.u8[0], aice->u.conn.addr.u8[1], aice->u.conn.addr.u8[2], aice->u.conn.addr.u8[3], aice->u.conn.port);

    // done
    tb_bool_t       ok = tb_false;
    tb_bool_t       init_ok = tb_false;
    tb_bool_t       ConnectEx_ok = tb_false;
    do
    {
        // init olap
        tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));

        // init aice
        aico->olap.aice = *aice;

        // bind it first for ConnectEx
        SOCKADDR_IN local;
        local.sin_family = AF_INET;
        local.sin_addr.S_un.S_addr = INADDR_ANY;
        local.sin_port = 0;
        if (SOCKET_ERROR == ptor->func.bind((SOCKET)aico->base.handle - 1, (LPSOCKADDR)&local, sizeof(local))) break;
        init_ok = tb_true;

        // post timeout first
        tb_iocp_post_timeout(ptor, aico);

        // done ConnectEx
        DWORD real = 0;
        SOCKADDR_IN addr = {0};
        addr.sin_family = AF_INET;
        addr.sin_port = tb_bits_ne_to_be_u16(aice->u.conn.port);
        addr.sin_addr.S_un.S_addr = aice->u.conn.addr.u32;
        ConnectEx_ok = ptor->func.ConnectEx(     (SOCKET)aico->base.handle - 1
                                        ,   (struct sockaddr const*)&addr
                                        ,   sizeof(addr)
                                        ,   tb_null
                                        ,   0
                                        ,   &real
                                        ,   (LPOVERLAPPED)&aico->olap)? tb_true : tb_false;
        tb_trace_d("ConnectEx: aico: %p, ok: %d, error: %d", aico, ConnectEx_ok, ptor->func.WSAGetLastError());
        tb_check_break(ConnectEx_ok);

        // post ok
        aico->olap.aice.state = TB_STATE_OK;
        if (!PostQueuedCompletionStatus(ptor->port, 0, (ULONG_PTR)aico, (LPOVERLAPPED)&aico->olap)) break;

        // ok
        ok = tb_true;

    } while (0);

    // ConnectEx failed?
    if (init_ok && !ConnectEx_ok)
    {
        // pending? continue it
        if (WSA_IO_PENDING == ptor->func.WSAGetLastError()) 
        {   
            // ok
            ok = tb_true;
        }
        // failed?
        else
        {
            // post failed
            aico->olap.aice.state = TB_STATE_FAILED;
            if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG_PTR)aico, (LPOVERLAPPED)&aico->olap)) ok = tb_true;

            // trace
            tb_trace_d("ConnectEx: unknown error: %d", ptor->func.WSAGetLastError());
        }
    }

    // error? remove timeout task
    if (!ok) tb_iocp_post_timeout_cancel(ptor, aico);

    // ok?
    return ok;
}
static tb_bool_t tb_iocp_post_recv(tb_iocp_ptor_t* ptor, tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(ptor && ptor->port && ptor->base.aicp, tb_false);

    // check aice
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_RECV, tb_false);
    tb_assert_and_check_return_val(aice->u.recv.data && aice->u.recv.size, tb_false);
    
    // the aico
    tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
    tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

    // init olap
    tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));

    // init aice
    aico->olap.aice = *aice;

    // trace
    tb_trace_d("recv: aico: %p: ..", aico);

    // post timeout first
    tb_iocp_post_timeout(ptor, aico);

    // done recv
    DWORD       flag = 0;
    tb_long_t   ok = ptor->func.WSARecv((SOCKET)aico->base.handle - 1, (WSABUF*)&aico->olap.aice.u.recv, 1, tb_null, &flag, (LPOVERLAPPED)&aico->olap, tb_null);
    tb_trace_d("WSARecv: aico: %p, %ld, error: %d", aico, ok, ptor->func.WSAGetLastError());

    // ok or pending? continue it
    if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == ptor->func.WSAGetLastError()))) return tb_true;

    // error?
    if (ok == SOCKET_ERROR)
    {
        // done error
        switch (ptor->func.WSAGetLastError())
        {
        // closed?
        case WSAECONNABORTED:
        case WSAECONNRESET:
            aico->olap.aice.state = TB_STATE_CLOSED;
            break;
        // failed?
        default:
            aico->olap.aice.state = TB_STATE_FAILED;
            break;
        }

        // post closed or failed
        if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG_PTR)aico, (LPOVERLAPPED)&aico->olap)) return tb_true;
    }

    // remove timeout task
    tb_iocp_post_timeout_cancel(ptor, aico);

    // failed
    return tb_false;
}
static tb_bool_t tb_iocp_post_send(tb_iocp_ptor_t* ptor, tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(ptor && ptor->port && ptor->base.aicp, tb_false);

    // check aice
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_SEND, tb_false);
    tb_assert_and_check_return_val(aice->u.send.data && aice->u.send.size, tb_false);
    
    // the aico
    tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
    tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

    // init olap
    tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));

    // init aice
    aico->olap.aice = *aice;

    // trace
    tb_trace_d("send: aico: %p: ..", aico);

    // post timeout first
    tb_iocp_post_timeout(ptor, aico);

    // done send
    tb_long_t ok = ptor->func.WSASend((SOCKET)aico->base.handle - 1, (WSABUF*)&aico->olap.aice.u.send, 1, tb_null, 0, (LPOVERLAPPED)&aico->olap, tb_null);
    tb_trace_d("WSASend: aico: %p, %ld, error: %d", aico, ok, ptor->func.WSAGetLastError());

    // ok or pending? continue it
    if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == ptor->func.WSAGetLastError()))) return tb_true;

    // error?
    if (ok == SOCKET_ERROR)
    {
        // done error
        switch (ptor->func.WSAGetLastError())
        {
        // closed?
        case WSAECONNABORTED:
        case WSAECONNRESET:
            aico->olap.aice.state = TB_STATE_CLOSED;
            break;
        // failed?
        default:
            aico->olap.aice.state = TB_STATE_FAILED;
            break;
        }

        // post closed or failed
        if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG_PTR)aico, (LPOVERLAPPED)&aico->olap)) return tb_true;
    }

    // remove timeout task
    tb_iocp_post_timeout_cancel(ptor, aico);

    // failed
    return tb_false;
}
static tb_bool_t tb_iocp_post_urecv(tb_iocp_ptor_t* ptor, tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(ptor && ptor->port && ptor->base.aicp, tb_false);

    // check aice
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_URECV, tb_false);
    tb_assert_and_check_return_val(aice->u.urecv.data && aice->u.urecv.size, tb_false);
    tb_assert_and_check_return_val(aice->u.urecv.addr.u32 && aice->u.urecv.port, tb_false);
    
    // the aico
    tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
    tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

    // init olap
    tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));

    // init aice
    aico->olap.aice = *aice;

    // init addr
    SOCKADDR_IN                 addr = {0};
    addr.sin_family             = AF_INET;
    addr.sin_port               = tb_bits_ne_to_be_u16(aice->u.urecv.port);
    addr.sin_addr.S_un.S_addr   = aice->u.urecv.addr.u32;

    // trace
    tb_trace_d("urecv: aico: %p: ..", aico);

    // post timeout first
    tb_iocp_post_timeout(ptor, aico);

    // done recv
    DWORD       flag = 0;
    tb_int_t    size = sizeof(addr);
    tb_long_t   ok = ptor->func.WSARecvFrom((SOCKET)aico->base.handle - 1, (WSABUF*)&aico->olap.aice.u.urecv, 1, tb_null, &flag, (struct sockaddr*)&addr, &size, (LPOVERLAPPED)&aico->olap, tb_null);
    tb_trace_d("WSARecvFrom: aico: %p, %ld, error: %d", aico, ok, ptor->func.WSAGetLastError());

    // ok or pending? continue it
    if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == ptor->func.WSAGetLastError()))) return tb_true;

    // error?
    if (ok == SOCKET_ERROR)
    {
        // done error
        switch (ptor->func.WSAGetLastError())
        {
        // closed?
        case WSAECONNABORTED:
        case WSAECONNRESET:
            aico->olap.aice.state = TB_STATE_CLOSED;
            break;
        // failed?
        default:
            aico->olap.aice.state = TB_STATE_FAILED;
            break;
        }

        // post closed or failed
        if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG_PTR)aico, (LPOVERLAPPED)&aico->olap)) return tb_true;
    }

    // remove timeout task
    tb_iocp_post_timeout_cancel(ptor, aico);

    // failed
    return tb_false;
}
static tb_bool_t tb_iocp_post_usend(tb_iocp_ptor_t* ptor, tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(ptor && ptor->port && ptor->base.aicp, tb_false);

    // check aice
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_USEND, tb_false);
    tb_assert_and_check_return_val(aice->u.usend.data && aice->u.usend.size, tb_false);
    tb_assert_and_check_return_val(aice->u.usend.addr.u32 && aice->u.usend.port, tb_false);
    
    // the aico
    tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
    tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

    // init olap
    tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));

    // init aice
    aico->olap.aice = *aice;

    // init addr
    SOCKADDR_IN                 addr = {0};
    addr.sin_family             = AF_INET;
    addr.sin_port               = tb_bits_ne_to_be_u16(aice->u.usend.port);
    addr.sin_addr.S_un.S_addr   = aice->u.usend.addr.u32;

    // trace
    tb_trace_d("usend: aico: %p: ..", aico);

    // post timeout first
    tb_iocp_post_timeout(ptor, aico);

    // done send
    tb_long_t ok = ptor->func.WSASendTo((SOCKET)aico->base.handle - 1, (WSABUF*)&aico->olap.aice.u.usend, 1, tb_null, 0, (struct sockaddr*)&addr, sizeof(addr), (LPOVERLAPPED)&aico->olap, tb_null);
    tb_trace_d("WSASendTo: aico: %p, %ld, error: %d", aico, ok, ptor->func.WSAGetLastError());

    // ok or pending? continue it
    if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == ptor->func.WSAGetLastError()))) return tb_true;

    // error?
    if (ok == SOCKET_ERROR)
    {
        // done error
        switch (ptor->func.WSAGetLastError())
        {
        // closed?
        case WSAECONNABORTED:
        case WSAECONNRESET:
            aico->olap.aice.state = TB_STATE_CLOSED;
            break;
        // failed?
        default:
            aico->olap.aice.state = TB_STATE_FAILED;
            break;
        }

        // post closed or failed
        if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG_PTR)aico, (LPOVERLAPPED)&aico->olap)) return tb_true;
    }

    // remove timeout task
    tb_iocp_post_timeout_cancel(ptor, aico);

    // failed
    return tb_false;
}
static tb_bool_t tb_iocp_post_recvv(tb_iocp_ptor_t* ptor, tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(ptor && ptor->port && ptor->base.aicp, tb_false);

    // check aice
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_RECVV, tb_false);
    tb_assert_and_check_return_val(aice->u.recvv.list && aice->u.recvv.size, tb_false);
    
    // the aico
    tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
    tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

    // init olap
    tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));

    // init aice
    aico->olap.aice = *aice;

    // trace
    tb_trace_d("recvv: aico: %p: ..", aico);

    // post timeout first
    tb_iocp_post_timeout(ptor, aico);

    // done recv
    DWORD       flag = 0;
    tb_long_t   ok = ptor->func.WSARecv((SOCKET)aico->base.handle - 1, (WSABUF*)aico->olap.aice.u.recvv.list, (DWORD)aico->olap.aice.u.recvv.size, tb_null, &flag, (LPOVERLAPPED)&aico->olap, tb_null);
    tb_trace_d("WSARecv: %ld, error: %d", ok, ptor->func.WSAGetLastError());

    // ok or pending? continue it
    if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == ptor->func.WSAGetLastError()))) return tb_true;

    // error?
    if (ok == SOCKET_ERROR)
    {
        // done error
        switch (ptor->func.WSAGetLastError())
        {
        // closed?
        case WSAECONNABORTED:
        case WSAECONNRESET:
            aico->olap.aice.state = TB_STATE_CLOSED;
            break;
        // failed?
        default:
            aico->olap.aice.state = TB_STATE_FAILED;
            break;
        }

        // post closed or failed
        if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG_PTR)aico, (LPOVERLAPPED)&aico->olap)) return tb_true;
    }

    // remove timeout task
    tb_iocp_post_timeout_cancel(ptor, aico);

    // failed
    return tb_false;
}
static tb_bool_t tb_iocp_post_sendv(tb_iocp_ptor_t* ptor, tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(ptor && ptor->port && ptor->base.aicp, tb_false);

    // check aice
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_SENDV, tb_false);
    tb_assert_and_check_return_val(aice->u.sendv.list && aice->u.sendv.size, tb_false);
    
    // the aico
    tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
    tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

    // init olap
    tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));

    // init aice
    aico->olap.aice = *aice;

    // trace
    tb_trace_d("sendv: aico: %p: ..", aico);

    // post timeout first
    tb_iocp_post_timeout(ptor, aico);

    // done send
    tb_long_t ok = ptor->func.WSASend((SOCKET)aico->base.handle - 1, (WSABUF*)aico->olap.aice.u.sendv.list, (DWORD)aico->olap.aice.u.sendv.size, tb_null, 0, (LPOVERLAPPED)&aico->olap, tb_null);
    tb_trace_d("WSASend: %ld, error: %d", ok, ptor->func.WSAGetLastError());

    // ok or pending? continue it
    if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == ptor->func.WSAGetLastError()))) return tb_true;

    // error?
    if (ok == SOCKET_ERROR)
    {
        // done error
        switch (ptor->func.WSAGetLastError())
        {
        // closed?
        case WSAECONNABORTED:
        case WSAECONNRESET:
            aico->olap.aice.state = TB_STATE_CLOSED;
            break;
        // failed?
        default:
            aico->olap.aice.state = TB_STATE_FAILED;
            break;
        }

        // post closed or failed
        if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG_PTR)aico, (LPOVERLAPPED)&aico->olap)) return tb_true;
    }

    // remove timeout task
    tb_iocp_post_timeout_cancel(ptor, aico);

    // failed
    return tb_false;
}
static tb_bool_t tb_iocp_post_urecvv(tb_iocp_ptor_t* ptor, tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(ptor && ptor->port && ptor->base.aicp, tb_false);

    // check aice
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_URECVV, tb_false);
    tb_assert_and_check_return_val(aice->u.urecvv.list && aice->u.urecvv.size, tb_false);
    tb_assert_and_check_return_val(aice->u.urecvv.addr.u32 && aice->u.urecvv.port, tb_false);
    
    // the aico
    tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
    tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

    // init olap
    tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));

    // init aice
    aico->olap.aice = *aice;

    // init addr
    SOCKADDR_IN                 addr = {0};
    addr.sin_family             = AF_INET;
    addr.sin_port               = tb_bits_ne_to_be_u16(aice->u.urecvv.port);
    addr.sin_addr.S_un.S_addr   = aice->u.urecv.addr.u32;

    // trace
    tb_trace_d("urecvv: aico: %p: ..", aico);

    // post timeout first
    tb_iocp_post_timeout(ptor, aico);

    // done recv
    DWORD       flag = 0;
    tb_int_t    size = sizeof(addr);
    tb_long_t   ok = ptor->func.WSARecvFrom((SOCKET)aico->base.handle - 1, (WSABUF*)aico->olap.aice.u.urecvv.list, (DWORD)aico->olap.aice.u.urecvv.size, tb_null, &flag, (struct sockaddr*)&addr, &size, (LPOVERLAPPED)&aico->olap, tb_null);
    tb_trace_d("WSARecvFrom: %ld, error: %d", ok, ptor->func.WSAGetLastError());

    // ok or pending? continue it
    if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == ptor->func.WSAGetLastError()))) return tb_true;

    // error?
    if (ok == SOCKET_ERROR)
    {
        // done error
        switch (ptor->func.WSAGetLastError())
        {
        // closed?
        case WSAECONNABORTED:
        case WSAECONNRESET:
            aico->olap.aice.state = TB_STATE_CLOSED;
            break;
        // failed?
        default:
            aico->olap.aice.state = TB_STATE_FAILED;
            break;
        }

        // post closed or failed
        if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG_PTR)aico, (LPOVERLAPPED)&aico->olap)) return tb_true;
    }

    // remove timeout task
    tb_iocp_post_timeout_cancel(ptor, aico);

    // failed
    return tb_false;
}
static tb_bool_t tb_iocp_post_usendv(tb_iocp_ptor_t* ptor, tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(ptor && ptor->port && ptor->base.aicp, tb_false);

    // check aice
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_USENDV, tb_false);
    tb_assert_and_check_return_val(aice->u.usendv.list && aice->u.usendv.size, tb_false);
    tb_assert_and_check_return_val(aice->u.usendv.addr.u32 && aice->u.usendv.port, tb_false);
    
    // the aico
    tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
    tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

    // init olap
    tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));

    // init aice
    aico->olap.aice = *aice;

    // init addr
    SOCKADDR_IN                 addr = {0};
    addr.sin_family             = AF_INET;
    addr.sin_port               = tb_bits_ne_to_be_u16(aice->u.usendv.port);
    addr.sin_addr.S_un.S_addr   = aice->u.urecv.addr.u32;

    // trace
    tb_trace_d("usendv: aico: %p: ..", aico);

    // post timeout first
    tb_iocp_post_timeout(ptor, aico);

    // done send
    tb_long_t ok = ptor->func.WSASendTo((SOCKET)aico->base.handle - 1, (WSABUF*)aico->olap.aice.u.usendv.list, (DWORD)aico->olap.aice.u.usendv.size, tb_null, 0, (struct sockaddr*)&addr, sizeof(addr), (LPOVERLAPPED)&aico->olap, tb_null);
    tb_trace_d("WSASendTo: %ld, error: %d", ok, ptor->func.WSAGetLastError());

    // ok or pending? continue it
    if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == ptor->func.WSAGetLastError()))) return tb_true;

    // error?
    if (ok == SOCKET_ERROR)
    {
        // done error
        switch (ptor->func.WSAGetLastError())
        {
        // closed?
        case WSAECONNABORTED:
        case WSAECONNRESET:
            aico->olap.aice.state = TB_STATE_CLOSED;
            break;
        // failed?
        default:
            aico->olap.aice.state = TB_STATE_FAILED;
            break;
        }

        // post closed or failed
        if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG_PTR)aico, (LPOVERLAPPED)&aico->olap)) return tb_true;
    }

    // remove timeout task
    tb_iocp_post_timeout_cancel(ptor, aico);

    // failed
    return tb_false;
}
static tb_bool_t tb_iocp_post_sendf(tb_iocp_ptor_t* ptor, tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(ptor && ptor->port && ptor->base.aicp, tb_false);

    // check aice
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_SENDF, tb_false);
    tb_assert_and_check_return_val(aice->u.sendf.file && aice->u.sendf.size, tb_false);
    
    // the aico
    tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
    tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

    // init olap
    tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));
    aico->olap.base.Offset  = (DWORD)aice->u.sendf.seek;

    // init aice
    aico->olap.aice = *aice;

    // trace
    tb_trace_d("sendf: aico: %p: ..", aico);

    // not supported?
    if (!ptor->func.TransmitFile)
    {
        // post not supported
        aico->olap.aice.state = TB_STATE_NOT_SUPPORTED;
        return PostQueuedCompletionStatus(ptor->port, 0, (ULONG_PTR)aico, (LPOVERLAPPED)&aico->olap)? tb_true : tb_false;
    }

    // post timeout first
    tb_iocp_post_timeout(ptor, aico);

    // done send
    tb_long_t real = ptor->func.TransmitFile((SOCKET)aico->base.handle - 1, (HANDLE)aice->u.sendf.file, (DWORD)aice->u.sendf.size, (1 << 16), (LPOVERLAPPED)&aico->olap, tb_null, 0);
    tb_trace_d("TransmitFile: %ld, size: %llu, error: %d", real, aice->u.sendf.size, ptor->func.WSAGetLastError());

    // pending? continue it
    if (!real || WSA_IO_PENDING == ptor->func.WSAGetLastError()) return tb_true;

    // ok?
    if (real > 0)
    {
        // post ok
        aico->olap.aice.state = TB_STATE_OK;
        aico->olap.aice.u.sendf.real = real;
        if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG_PTR)aico, (LPOVERLAPPED)&aico->olap)) return tb_true;
    }
    else
    {
        // done error
        switch (ptor->func.WSAGetLastError())
        {
        // closed?
        case WSAECONNABORTED:
        case WSAECONNRESET:
            aico->olap.aice.state = TB_STATE_CLOSED;
            break;
        // failed?
        default:
            aico->olap.aice.state = TB_STATE_FAILED;
            break;
        }

        // post closed or failed
        if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG_PTR)aico, (LPOVERLAPPED)&aico->olap)) return tb_true;
    }

    // remove timeout task
    tb_iocp_post_timeout_cancel(ptor, aico);

    // failed
    return tb_false;
}
static tb_bool_t tb_iocp_post_read(tb_iocp_ptor_t* ptor, tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(ptor && ptor->port && ptor->base.aicp, tb_false);

    // check aice
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_READ, tb_false);
    tb_assert_and_check_return_val(aice->u.read.data && aice->u.read.size, tb_false);

    // the aico
    tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
    tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

    // init olap
    tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));
    aico->olap.base.Offset  = (DWORD)aice->u.read.seek;

    // init aice
    aico->olap.aice = *aice;

    // trace
    tb_trace_d("read: aico: %p: ..", aico);

    // done read
    DWORD       real = 0;
    BOOL        ok = ReadFile((HANDLE)aico->base.handle, aice->u.read.data, (DWORD)aice->u.read.size, &real, (LPOVERLAPPED)&aico->olap);
    tb_trace_d("ReadFile: aico: %p, real: %u, size: %lu, error: %d, ok: %d", aico, real, aice->u.read.size, GetLastError(), ok);

    // finished or pending? continue it
    if (ok || ERROR_IO_PENDING == GetLastError()) return tb_true;

    // post failed
    aico->olap.aice.state = TB_STATE_FAILED;
    if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG_PTR)aico, (LPOVERLAPPED)&aico->olap)) return tb_true;

    // failed
    return tb_false;
}
static tb_bool_t tb_iocp_post_writ(tb_iocp_ptor_t* ptor, tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(ptor && ptor->port && ptor->base.aicp, tb_false);

    // check aice
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_WRIT, tb_false);
    tb_assert_and_check_return_val(aice->u.writ.data && aice->u.writ.size, tb_false);

    // the aico
    tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
    tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

    // init olap
    tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));
    aico->olap.base.Offset  = (DWORD)aice->u.writ.seek;

    // init aice
    aico->olap.aice = *aice;

    // trace
    tb_trace_d("writ: aico: %p: ..", aico);

    // done writ
    DWORD       real = 0;
    BOOL        ok = WriteFile((HANDLE)aico->base.handle, aice->u.writ.data, (DWORD)aice->u.writ.size, &real, (LPOVERLAPPED)&aico->olap);
    tb_trace_d("WriteFile: aico: %p, real: %u, size: %lu, error: %d, ok: %d", aico, real, aice->u.writ.size, GetLastError(), ok);

    // finished or pending? continue it
    if (ok || ERROR_IO_PENDING == GetLastError()) return tb_true;

    // post failed
    aico->olap.aice.state = TB_STATE_FAILED;
    if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG_PTR)aico, (LPOVERLAPPED)&aico->olap)) return tb_true;

    // failed
    return tb_false;
}
static tb_bool_t tb_iocp_post_readv(tb_iocp_ptor_t* ptor, tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(ptor && ptor->port && ptor->base.aicp, tb_false);

    // check aice
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_READV, tb_false);
    tb_assert_and_check_return_val(aice->u.readv.list && aice->u.readv.size, tb_false);

    // the aico
    tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
    tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

    // init olap
    tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));
    aico->olap.base.Offset  = (DWORD)aice->u.readv.seek;

    // init aice
    aico->olap.aice = *aice;

    // trace
    tb_trace_d("readv: aico: %p: ..", aico);

    // done read
    DWORD       real = 0;
    BOOL        ok = ReadFile((HANDLE)aico->base.handle, aice->u.readv.list[0].data, (DWORD)aice->u.readv.list[0].size, &real, (LPOVERLAPPED)&aico->olap);
    tb_trace_d("ReadFile: %u, error: %d", real, GetLastError());

    // finished or pending? continue it
    if (ok || ERROR_IO_PENDING == GetLastError()) return tb_true;

    // post failed
    aico->olap.aice.state = TB_STATE_FAILED;
    if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG_PTR)aico, (LPOVERLAPPED)&aico->olap)) return tb_true;

    // failed
    return tb_false;
}
static tb_bool_t tb_iocp_post_writv(tb_iocp_ptor_t* ptor, tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(ptor && ptor->port && ptor->base.aicp, tb_false);

    // check aice
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_WRITV, tb_false);
    tb_assert_and_check_return_val(aice->u.writv.list && aice->u.writv.size, tb_false);

    // the aico
    tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
    tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

    // init olap
    tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));
    aico->olap.base.Offset  = (DWORD)aice->u.writv.seek;

    // init aice
    aico->olap.aice = *aice;

    // trace
    tb_trace_d("writv: aico: %p: ..", aico);

    // done writ
    DWORD       real = 0;
    BOOL        ok = WriteFile((HANDLE)aico->base.handle, aice->u.writv.list[0].data, (DWORD)aice->u.writv.list[0].size, &real, (LPOVERLAPPED)&aico->olap);
    tb_trace_d("WriteFile: %u, error: %d", real, GetLastError());

    // finished or pending? continue it
    if (ok || ERROR_IO_PENDING == GetLastError()) return tb_true;

    // post failed
    aico->olap.aice.state = TB_STATE_FAILED;
    if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG_PTR)aico, (LPOVERLAPPED)&aico->olap)) return tb_true;

    // failed
    return tb_false;
}
static tb_bool_t tb_iocp_post_fsync(tb_iocp_ptor_t* ptor, tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(ptor && ptor->port && ptor->base.aicp, tb_false);

    // check aice
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_FSYNC, tb_false);
    
    // the aico
    tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
    tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

    // trace
    tb_trace_d("fsync: ..");

    // init olap
    tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));

    // init aice
    aico->olap.aice = *aice;

    // post ok
    aico->olap.aice.state = TB_STATE_OK;
    if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG_PTR)aico, (LPOVERLAPPED)&aico->olap)) return tb_true;

    // failed
    return tb_false;
}
static tb_bool_t tb_iocp_post_runtask(tb_iocp_ptor_t* ptor, tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(ptor && ptor->port && ptor->timer && ptor->ltimer && ptor->wait && ptor->base.aicp, tb_false);

    // check aice
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_RUNTASK, tb_false);
    tb_assert_and_check_return_val(aice->state == TB_STATE_PENDING, tb_false);
    tb_assert_and_check_return_val(aice->u.runtask.when, tb_false);
    
    // the aico
    tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
    tb_assert_and_check_return_val(aico && !aico->task, tb_false);

    // init olap
    tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));

    // init aice
    aico->olap.aice = *aice;

    // now
    tb_hong_t now = tb_cache_time_mclock();

    // timeout?
    if (aice->u.runtask.when <= (tb_hize_t)now)
    {
        // trace
        tb_trace_d("runtask: when: %llu, now: %lld: ok", aice->u.runtask.when, now);

        // post ok
        aico->olap.aice.state = TB_STATE_OK;
        if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG_PTR)aico, (LPOVERLAPPED)&aico->olap)) return tb_true;
    }
    else
    {
        // trace
        tb_trace_d("runtask: when: %llu, now: %lld: ..", aice->u.runtask.when, now);

        // add timeout task, is the higher precision timer?
        if (aico->base.handle)
        {
            // the top when
            tb_hize_t top = tb_timer_top(ptor->timer);

            // add task
            aico->task = tb_timer_task_init_at(ptor->timer, aice->u.runtask.when, 0, tb_false, tb_iocp_spak_timeout_runtask, aico);
            aico->bltimer = 0;

            // the top task is changed? spak the timer
            if (aico->task && aice->u.runtask.when < top)
                tb_event_post(ptor->wait);
        }
        else
        {
            aico->task = tb_ltimer_task_init_at(ptor->ltimer, aice->u.runtask.when, 0, tb_false, tb_iocp_spak_timeout_runtask, aico);
            aico->bltimer = 1;
        }

        // pending
        return tb_true;
    }

    // failed
    return tb_false;
}
static tb_bool_t tb_iocp_post_done(tb_iocp_ptor_t* ptor, tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(ptor && ptor->port && ptor->base.aicp && aice, tb_false);

    // the aico
    tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
    tb_assert_and_check_return_val(aico, tb_false);

    // no pending? post it directly
    if (aice->state != TB_STATE_PENDING)
    {
        // trace
        tb_trace_d("post: done: aico: %p, code: %u, type: %lu: directly", aico, aice->code, aico->base.type);

        // post it directly
        aico->olap.aice = *aice;
        return PostQueuedCompletionStatus(ptor->port, 0, (ULONG_PTR)aico, (LPOVERLAPPED)&aico->olap)? tb_true : tb_false;  
    }
    
    // killed?
    if (tb_aico_is_killed(aico) || tb_atomic_get(&ptor->base.aicp->kill))
    {
        // trace
        tb_trace_d("post: done: aico: %p, code: %u, type: %lu: killed", aico, aice->code, aico->base.type);

        // post the killed state
        aico->olap.aice = *aice;
        aico->olap.aice.state = TB_STATE_KILLED;
        return PostQueuedCompletionStatus(ptor->port, 0, (ULONG_PTR)aico, (LPOVERLAPPED)&aico->olap)? tb_true : tb_false;  
    }

    // init post
    static tb_bool_t (*s_post[])(tb_iocp_ptor_t* , tb_aice_t const*) = 
    {
        tb_null
    ,   tb_iocp_post_acpt
    ,   tb_iocp_post_conn
    ,   tb_iocp_post_recv
    ,   tb_iocp_post_send
    ,   tb_iocp_post_urecv
    ,   tb_iocp_post_usend
    ,   tb_iocp_post_recvv
    ,   tb_iocp_post_sendv
    ,   tb_iocp_post_urecvv
    ,   tb_iocp_post_usendv
    ,   tb_iocp_post_sendf
    ,   tb_iocp_post_read
    ,   tb_iocp_post_writ
    ,   tb_iocp_post_readv
    ,   tb_iocp_post_writv
    ,   tb_iocp_post_fsync
    ,   tb_iocp_post_runtask
    };
    tb_assert_and_check_return_val(aice->code < tb_arrayn(s_post) && s_post[aice->code], tb_false);

    // trace
    tb_trace_d("post: done: aico: %p, code: %u, type: %lu: ..", aico, aice->code, aico->base.type);

    // post aice
    tb_bool_t ok = s_post[aice->code](ptor, aice);
    if (!ok)
    {
        // trace
        tb_trace_e("post: done: aico: %p, code: %u, type: %lu: failed", aico, aice->code, aico->base.type);
    }

    // ok?
    return ok;
}
static tb_pointer_t tb_iocp_post_loop(tb_cpointer_t priv)
{
    // check
    tb_iocp_ptor_t*     ptor = (tb_iocp_ptor_t*)priv;
    tb_aicp_t*          aicp = ptor? ptor->base.aicp : tb_null;
    tb_assert_and_check_return_val(ptor && ptor->wait && aicp, tb_null);
    tb_assert_and_check_return_val(ptor->timer && ptor->ltimer, tb_null);
    tb_assert_and_check_return_val(ptor->post[0] && ptor->post[1], tb_null);

    // trace
    tb_trace_d("loop: init");

    // loop 
    tb_aice_t post = {0};
    while (!tb_atomic_get(&aicp->kill))
    {
        // clear post
        post.code = TB_AICE_CODE_NONE;

        // enter 
        tb_spinlock_enter(&ptor->lock);

        // post aice from the higher priority queue first
        if (!tb_queue_null(ptor->post[0])) 
        {
            // get resp
            tb_aice_t const* aice = (tb_aice_t const*)tb_queue_get(ptor->post[0]);
            if (aice) 
            {
                // save post
                post = *aice;

                // trace
                tb_trace_d("loop: post: code: %lu, priority: 0, size: %lu", aice->code, tb_queue_size(ptor->post[0]));

                // pop it
                tb_queue_pop(ptor->post[0]);
            }
        }

        // no aice? post aice from the lower priority queue next
        if (post.code == TB_AICE_CODE_NONE && !tb_queue_null(ptor->post[1]))
        {
            // get resp
            tb_aice_t const* aice = (tb_aice_t const*)tb_queue_get(ptor->post[1]);
            if (aice) 
            {
                // save post
                post = *aice;

                // trace
                tb_trace_d("loop: post: code: %lu, priority: 1, size: %lu", aice->code, tb_queue_size(ptor->post[1]));

                // pop it
                tb_queue_pop(ptor->post[1]);
            }
        }

        // leave 
        tb_spinlock_leave(&ptor->lock);

        // done post
        if (post.code != TB_AICE_CODE_NONE && !tb_iocp_post_done(ptor, &post)) break;

        // spak ctime
        tb_cache_time_spak();

        // spak timer
        if (!tb_timer_spak(ptor->timer)) break;

        // spak ltimer
        if (!tb_ltimer_spak(ptor->ltimer)) break;

        // null? wait it
        tb_check_continue(post.code == TB_AICE_CODE_NONE);
        
        // killed? break it
        tb_check_break(!tb_atomic_get(&aicp->kill));

        // the delay
        tb_size_t delay = tb_timer_delay(ptor->timer);

        // the ldelay
        tb_size_t ldelay = tb_ltimer_delay(ptor->ltimer);
        tb_assert_and_check_break(ldelay != -1);

        // using the min delay
        if (ldelay < delay) delay = ldelay;

        // trace
        tb_trace_d("loop: wait: %lu ms: ..", delay);

        // wait some time
        if (delay && tb_event_wait(ptor->wait, delay) < 0) break;
    }

    // trace
    tb_trace_d("loop: exit");

    // kill
    tb_atomic_set(&aicp->kill, 1);

    // exit
    tb_thread_return(tb_null);
    return tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_iocp_ptor_addo(tb_aicp_proactor_t* proactor, tb_aico_t* aico)
{
    // check
    tb_iocp_ptor_t* ptor = (tb_iocp_ptor_t*)proactor;
    tb_assert_and_check_return_val(ptor && aico, tb_false);

    // trace
    tb_trace_d("addo: aico: %p, handle: %p", aico, aico->handle);

    // done
    switch (aico->type)
    {
    case TB_AICO_TYPE_SOCK:
        {
            // check
            tb_assert_and_check_return_val(ptor->port && aico->handle, tb_false);

            // add aico to port
            HANDLE port = CreateIoCompletionPort((HANDLE)((SOCKET)aico->handle - 1), ptor->port, (ULONG_PTR)aico, 0);
            if (port != ptor->port)
            {
                // trace
                tb_trace_e("CreateIoCompletionPort failed: %d, aico: %p, handle: %p", GetLastError(), aico, aico->handle);
                return tb_false;
            }
        }
        break;
    case TB_AICO_TYPE_FILE:
        {
            // check
            tb_assert_and_check_return_val(ptor->port && aico->handle, tb_false);

            // add aico to port
            HANDLE port = CreateIoCompletionPort((HANDLE)aico->handle, ptor->port, (ULONG_PTR)aico, 0);
            if (port != ptor->port)
            {
                // trace
                tb_trace_e("CreateIoCompletionPort failed: %d, aico: %p, handle: %p", GetLastError(), aico, aico->handle);
                return tb_false;
            }
        }
        break;
    case TB_AICO_TYPE_TASK:
        {
        }
        break;
    default:
        tb_assert_and_check_return_val(0, tb_false);
        break;
    }
    
    // the iocp aico
    tb_iocp_aico_t* iocp_aico = (tb_iocp_aico_t*)aico;
    iocp_aico->ptor = ptor;

    // ok
    return tb_true;
}
static tb_bool_t tb_iocp_ptor_delo(tb_aicp_proactor_t* proactor, tb_aico_t* aico)
{
    // check
    tb_iocp_ptor_t* ptor = (tb_iocp_ptor_t*)proactor;
    tb_assert_and_check_return_val(ptor && aico, tb_false);
        
    // the iocp aico
    tb_iocp_aico_t* iocp_aico = (tb_iocp_aico_t*)aico;
    
    // trace
    tb_trace_d("delo: aico: %p, handle: %p", aico, aico->handle);

    // remove the timeout task
    tb_iocp_post_timeout_cancel(ptor, iocp_aico);

    // exit olap
    tb_memset(&iocp_aico->olap, 0, sizeof(tb_iocp_olap_t));
    
    // exit ptor
    iocp_aico->ptor = tb_null;

    // ok
    return tb_true;
}
static tb_void_t tb_iocp_ptor_kilo(tb_aicp_proactor_t* proactor, tb_aico_t* aico)
{
    // check
    tb_iocp_ptor_t* ptor = (tb_iocp_ptor_t*)proactor;
    tb_assert_and_check_return(ptor && ptor->wait && aico);
        
    // trace
    tb_trace_d("kilo: aico: %p, handle: %p, type: %u", aico, aico->handle, aico->type);

    // task: kill
    if (((tb_iocp_aico_t*)aico)->task) 
    {
        // kill it
        if (((tb_iocp_aico_t*)aico)->bltimer) tb_ltimer_task_kill(ptor->ltimer, ((tb_iocp_aico_t*)aico)->task);
        else tb_timer_task_kill(ptor->timer, ((tb_iocp_aico_t*)aico)->task);

        /* the iocp will wait long time if the lastest task wait period is too long
         * so spak the iocp manually for spak the timer
         */
        tb_event_post(ptor->wait);
    }

    // sock: kill
    if (aico->type == TB_AICO_TYPE_SOCK && aico->handle) tb_socket_kill(aico->handle, TB_SOCKET_KILL_RW);
    // file: kill
    else if (aico->type == TB_AICO_TYPE_FILE && aico->handle) tb_file_exit(aico->handle);
}
static tb_bool_t tb_iocp_ptor_post(tb_aicp_proactor_t* proactor, tb_aice_t const* aice)
{
    // check
    tb_iocp_ptor_t* ptor = (tb_iocp_ptor_t*)proactor;
    tb_assert_and_check_return_val(ptor && ptor->wait && aice && aice->aico, tb_false);
    
    // the priority
    tb_size_t priority = tb_aice_priority(aice);
    tb_assert_and_check_return_val(priority < tb_arrayn(ptor->post) && ptor->post[priority], tb_false);

    // enter 
    tb_spinlock_enter(&ptor->lock);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // post aice
        if (tb_queue_full(ptor->post[priority])) 
        {
            // trace
            tb_trace_e("post: code: %lu, priority: %lu, size: %lu: failed", aice->code, priority, tb_queue_size(ptor->post[priority]));
            break;
        }

        // put
        tb_queue_put(ptor->post[priority], aice);

        // trace
        tb_trace_d("post: code: %lu, priority: %lu, size: %lu: ..", aice->code, priority, tb_queue_size(ptor->post[priority]));

        // ok
        ok = tb_true;

    } while (0);

    // leave 
    tb_spinlock_leave(&ptor->lock);

    // work it 
    if (ok) tb_event_post(ptor->wait);

    // ok?
    return ok;
}
static tb_void_t tb_iocp_ptor_kill(tb_aicp_proactor_t* proactor)
{
    // check
    tb_iocp_ptor_t* ptor = (tb_iocp_ptor_t*)proactor;
    tb_assert_and_check_return(ptor && ptor->port && ptor->wait && proactor->aicp);

    // the workers
    tb_size_t work = tb_atomic_get(&proactor->aicp->work);
    
    // trace
    tb_trace_d("kill: %lu", work);

    // post the timer wait
    tb_event_post(ptor->wait);

    // using GetQueuedCompletionStatusEx?
    if (ptor->func.GetQueuedCompletionStatusEx)
    {
        // kill workers
        while (work--) 
        {
            // post kill
            PostQueuedCompletionStatus(ptor->port, 0, 0, tb_null);
            
            // wait some time
            tb_msleep(200);
        }
    }
    else
    {
        // kill workers
        while (work--) PostQueuedCompletionStatus(ptor->port, 0, 0, tb_null);
    }
}
static tb_void_t tb_iocp_ptor_exit(tb_aicp_proactor_t* proactor)
{
    tb_iocp_ptor_t* ptor = (tb_iocp_ptor_t*)proactor;
    if (ptor)
    {
        // trace
        tb_trace_d("exit");

        // post the timer wait
        if (ptor->wait) tb_event_post(ptor->wait);

        // exit loop
        if (ptor->loop)
        {
            tb_long_t wait = 0;
            if ((wait = tb_thread_wait(ptor->loop, 5000)) <= 0)
            {
                // trace
                tb_trace_e("loop[%p]: wait failed: %ld!", ptor->loop, wait);
            }
            tb_thread_exit(ptor->loop);
            ptor->loop = tb_null;
        }

        // exit post
        tb_spinlock_enter(&ptor->lock);
        if (ptor->post[0]) tb_queue_exit(ptor->post[0]);
        if (ptor->post[1]) tb_queue_exit(ptor->post[1]);
        ptor->post[0] = tb_null;
        ptor->post[1] = tb_null;
        tb_spinlock_leave(&ptor->lock);

        // exit port
        if (ptor->port) CloseHandle(ptor->port);
        ptor->port = tb_null;

        // exit timer
        if (ptor->timer) tb_timer_exit(ptor->timer);
        ptor->timer = tb_null;

        // exit ltimer
        if (ptor->ltimer) tb_ltimer_exit(ptor->ltimer);
        ptor->ltimer = tb_null;

        // exit wait
        if (ptor->wait) tb_event_exit(ptor->wait);
        ptor->wait = tb_null;

        // exit lock
        tb_spinlock_exit(&ptor->lock);

        // free it
        tb_free(ptor);
    }
}
static tb_void_t tb_iocp_ptor_loop_exit(tb_aicp_proactor_t* proactor, tb_handle_t hloop)
{
    // check
    tb_iocp_ptor_t* ptor = (tb_iocp_ptor_t*)proactor;
    tb_assert_and_check_return(ptor);

    // the loop
    tb_iocp_loop_t* loop = (tb_iocp_loop_t*)hloop;
    tb_assert_and_check_return(loop);

    // exit spak
    if (loop->spak) tb_queue_exit(loop->spak);
    loop->spak = tb_null;

    // exit self
    loop->self = 0;

    // exit loop
    tb_free(loop);
}
static tb_handle_t tb_iocp_ptor_loop_init(tb_aicp_proactor_t* proactor)
{
    // check
    tb_iocp_ptor_t* ptor = (tb_iocp_ptor_t*)proactor;
    tb_assert_and_check_return_val(ptor, tb_null);

    // make loop
    tb_iocp_loop_t* loop = (tb_iocp_loop_t*)tb_malloc0(sizeof(tb_iocp_loop_t));
    tb_assert_and_check_return_val(loop, tb_null);

    // init self
    loop->self = tb_thread_self();
    tb_assert_and_check_goto(loop->self, fail);

    // init spak
    if (ptor->func.GetQueuedCompletionStatusEx)
    {
        loop->spak = tb_queue_init(TB_IOCP_OLAP_LIST_MAXN, tb_item_func_mem(sizeof(tb_OVERLAPPED_ENTRY_t), tb_null, tb_null));
        tb_assert_and_check_goto(loop->spak, fail);
    }

    // ok
    return (tb_handle_t)loop;
fail:
    tb_iocp_ptor_loop_exit(proactor, (tb_handle_t)loop);
    return tb_null;
}
static tb_long_t tb_iocp_ptor_loop_spak(tb_aicp_proactor_t* proactor, tb_handle_t hloop, tb_aice_t* resp, tb_long_t timeout)
{
    // check
    tb_iocp_ptor_t* ptor = (tb_iocp_ptor_t*)proactor;
    tb_assert_and_check_return_val(ptor && ptor->port && ptor->timer && resp, -1);

    // the loop
    tb_iocp_loop_t* loop = (tb_iocp_loop_t*)hloop;
    tb_assert_and_check_return_val(loop, -1);

    // trace
    tb_trace_d("spak[%lu]: ..", loop->self);

    // exists GetQueuedCompletionStatusEx? using it
    if (ptor->func.GetQueuedCompletionStatusEx)
    {
        // check
        tb_assert_and_check_return_val(loop->spak, -1);

        // exists olap? spak it first
        if (!tb_queue_null(loop->spak))
        {
            // the top entry
            tb_LPOVERLAPPED_ENTRY_t entry = (tb_LPOVERLAPPED_ENTRY_t)tb_queue_get(loop->spak);
            tb_assert_and_check_return_val(entry, -1);

            // init 
            tb_size_t           real = (tb_size_t)entry->dwNumberOfBytesTransferred;
            tb_iocp_aico_t*     aico = (tb_iocp_aico_t* )entry->lpCompletionKey;
            tb_iocp_olap_t*     olap = (tb_iocp_olap_t*)entry->lpOverlapped;
            tb_size_t           error = tb_ntstatus_to_winerror((tb_size_t)entry->Internal);
            tb_trace_d("spak[%lu]: aico: %p, ntstatus: %lx, winerror: %lu", loop->self, aico, (tb_size_t)entry->Internal, error);

            // pop the entry
            tb_queue_pop(loop->spak);
    
            // check
            tb_assert_and_check_return_val(olap && aico, -1);

            // save resp
            *resp = olap->aice;

            // spak resp
            return tb_iocp_spak_done(ptor, resp, real, error);
        }
        else
        {
            // wait
            DWORD       size = 0;
            BOOL        wait = ptor->func.GetQueuedCompletionStatusEx(ptor->port, loop->list, tb_arrayn(loop->list), &size, timeout, FALSE);

            // the last error
            tb_size_t   error = (tb_size_t)GetLastError();

            /* some error will be not clear immediately after success, .e.g ERROR_NETNAME_DELETED
             * we need clear it manually before calling GetQueuedCompletionStatus next
             */
            if (error != ERROR_SUCCESS) SetLastError(ERROR_SUCCESS);

            // trace
            tb_trace_d("spak[%lu]: wait: %d, size: %u, error: %lu", loop->self, wait, size, error);

            // timeout?
            if (!wait && error == WAIT_TIMEOUT) return 0;

            // error?
            tb_assert_and_check_return_val(wait, -1);

            // put entries to the spak queue
            tb_size_t i = 0;
            for (i = 0; i < size; i++) 
            {
                // the aico
                tb_iocp_aico_t* aico = (tb_iocp_aico_t* )loop->list[i].lpCompletionKey;

                // aicp killed?
                tb_check_return_val(aico, -1);

                // remove task first
                tb_iocp_post_timeout_cancel(ptor, aico);

                // full?
                if (!tb_queue_full(loop->spak))
                {
                    // put it
                    tb_queue_put(loop->spak, &loop->list[i]);
                }
                else 
                {
                    // full
                    tb_assert_and_check_return_val(0, -1);
                }
            }

            // continue 
            return 0;
        }
    }
    else
    {
        // wait
        DWORD               real = 0;
        tb_iocp_aico_t*     aico = tb_null;
        tb_iocp_olap_t*     olap = tb_null;
        BOOL                wait = GetQueuedCompletionStatus(ptor->port, (LPDWORD)&real, (LPDWORD)&aico, (LPOVERLAPPED*)&olap, timeout < 0? INFINITE : timeout);

        // the last error
        tb_size_t           error = (tb_size_t)GetLastError();

        /* some error will be not clear immediately after success, .e.g ERROR_NETNAME_DELETED
         * we need clear it manually before calling GetQueuedCompletionStatus next
         */
        if (error != ERROR_SUCCESS) SetLastError(ERROR_SUCCESS);

        // trace
        tb_trace_d("spak[%lu]: aico: %p, wait: %d, real: %u, error: %lu", loop->self, aico, wait, real, error);

        // timeout?
        if (!wait && error == WAIT_TIMEOUT) return 0;

        // aicp killed?
        if (wait && !aico) return -1;

        // exit the aico task
        if (aico)
        {
            // remove task
            if (aico->task) 
            {
                if (aico->bltimer) tb_ltimer_task_exit(ptor->ltimer, aico->task);
                else tb_timer_task_exit(ptor->timer, aico->task);
                aico->bltimer = 0;
            }
            aico->task = tb_null;
        }

        // check
        tb_assert_and_check_return_val(olap, -1);

        // save resp
        *resp = olap->aice;

        // spak resp
        return tb_iocp_spak_done(ptor, resp, (tb_size_t)real, error);
    }

    // failed
    return -1;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
static tb_aicp_proactor_t* tb_iocp_ptor_init(tb_aicp_t* aicp)
{
    // check
    tb_assert_and_check_return_val(aicp && aicp->maxn, tb_null);

    // check iovec
    tb_assert_static(sizeof(tb_iovec_t) == sizeof(WSABUF));
    tb_assert_and_check_return_val(tb_memberof_eq(tb_iovec_t, data, WSABUF, buf), tb_null);
    tb_assert_and_check_return_val(tb_memberof_eq(tb_iovec_t, size, WSABUF, len), tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_iocp_ptor_t*     ptor = tb_null;
    do
    {
        // make proactor
        ptor = (tb_iocp_ptor_t*)tb_malloc0(sizeof(tb_iocp_ptor_t));
        tb_assert_and_check_break(ptor);

        // init base
        ptor->base.aicp         = aicp;
        ptor->base.step         = sizeof(tb_iocp_aico_t);
        ptor->base.kill         = tb_iocp_ptor_kill;
        ptor->base.exit         = tb_iocp_ptor_exit;
        ptor->base.addo         = tb_iocp_ptor_addo;
        ptor->base.delo         = tb_iocp_ptor_delo;
        ptor->base.kilo         = tb_iocp_ptor_kilo;
        ptor->base.post         = tb_iocp_ptor_post;
        ptor->base.loop_init    = tb_iocp_ptor_loop_init;
        ptor->base.loop_exit    = tb_iocp_ptor_loop_exit;
        ptor->base.loop_spak    = tb_iocp_ptor_loop_spak;

        // init func
        ptor->func.AcceptEx                      = tb_mswsock()->AcceptEx;
        ptor->func.ConnectEx                     = tb_mswsock()->ConnectEx;
        ptor->func.TransmitFile                  = tb_mswsock()->TransmitFile;
        ptor->func.GetQueuedCompletionStatusEx   = tb_kernel32()->GetQueuedCompletionStatusEx;
        ptor->func.WSAGetLastError               = tb_ws2_32()->WSAGetLastError;
        ptor->func.WSASend                       = tb_ws2_32()->WSASend;
        ptor->func.WSARecv                       = tb_ws2_32()->WSARecv;
        ptor->func.WSASendTo                     = tb_ws2_32()->WSASendTo;
        ptor->func.WSARecvFrom                   = tb_ws2_32()->WSARecvFrom;
        ptor->func.bind                          = tb_ws2_32()->bind;
        tb_assert_and_check_break(ptor->func.AcceptEx);
        tb_assert_and_check_break(ptor->func.ConnectEx);
        tb_assert_and_check_break(ptor->func.WSAGetLastError);
        tb_assert_and_check_break(ptor->func.WSASend);
        tb_assert_and_check_break(ptor->func.WSARecv);
        tb_assert_and_check_break(ptor->func.WSASendTo);
        tb_assert_and_check_break(ptor->func.WSARecvFrom);
        tb_assert_and_check_break(ptor->func.bind);

        // init lock
        if (!tb_spinlock_init(&ptor->lock)) break;

        // init port
        ptor->port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, tb_null, 0, 0);
        tb_assert_and_check_break(ptor->port && ptor->port != INVALID_HANDLE_VALUE);

        // init timer and using cache time
        ptor->timer = tb_timer_init(aicp->maxn >> 3, tb_true);
        tb_assert_and_check_break(ptor->timer);

        // init ltimer and using cache time
        ptor->ltimer = tb_ltimer_init(aicp->maxn, TB_LTIMER_TICK_S, tb_true);
        tb_assert_and_check_break(ptor->ltimer);

        // init wait
        ptor->wait = tb_event_init();
        tb_assert_and_check_break(ptor->wait);

        // init post
        ptor->post[0] = tb_queue_init(aicp->maxn + 16, tb_item_func_mem(sizeof(tb_aice_t), tb_null, tb_null));
        ptor->post[1] = tb_queue_init(aicp->maxn + 16, tb_item_func_mem(sizeof(tb_aice_t), tb_null, tb_null));
        tb_assert_and_check_break(ptor->post[0] && ptor->post[1]);

        // register lock profiler
#ifdef TB_LOCK_PROFILER_ENABLE
        tb_lock_profiler_register(tb_lock_profiler(), (tb_pointer_t)&ptor->lock, "aicp_iocp");
#endif

        // init the timer loop
        ptor->loop = tb_thread_init(tb_null, tb_iocp_post_loop, ptor, 0);
        tb_assert_and_check_break(ptor->loop);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (ptor) tb_iocp_ptor_exit((tb_aicp_proactor_t*)ptor);
        return tb_null;
    }

    // ok?
    return (tb_aicp_proactor_t*)ptor;
}

