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

// the iocp post type
typedef struct __tb_iocp_post_t
{
    // the lock
    tb_spinlock_t                               lock;

    // the wait event
    tb_handle_t                                 wait;

    /* the post queue
     *
     * index: 0: higher priority for conn, acpt and task
     * index: 1: lower priority for io aice 
     */
    tb_queue_t*                                 queue[2];

}tb_iocp_post_t;

// the iocp spak type
typedef struct __tb_iocp_spak_t
{
    // the lock
    tb_spinlock_t                               lock;

    // the wait semaphore
    tb_handle_t                                 wait;

    /* the spak queue
     *
     * index: 0: higher priority for conn, acpt and task
     * index: 1: lower priority for io aice 
     */
    tb_queue_t*                                 queue[2];

}tb_iocp_spak_t;

// the iocp proactor type
typedef struct __tb_iocp_ptor_t
{
    // the proactor base
    tb_aicp_proactor_t                          base;

    // the i/o completion port
    HANDLE                                      port;

    // the spak timer for task
    tb_handle_t                                 timer;

    // the spak low precision timer for timeout
    tb_handle_t                                 ltimer;

    // the spak loop
    tb_handle_t                                 loop;

    // the AcceptEx func
    tb_mswsock_AcceptEx_t                       AcceptEx;

    // the ConnectEx func
    tb_mswsock_ConnectEx_t                      ConnectEx;

    // the TransmitFile func
    tb_mswsock_TransmitFile_t                   TransmitFile;

    // the CancelIoEx func
    tb_kernel32_CancelIoEx_t                    CancelIoEx;

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

/* //////////////////////////////////////////////////////////////////////////////////////
 * loop 
 */


/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_pointer_t tb_iocp_ptor_loop(tb_cpointer_t priv)
{
    // check
    tb_iocp_ptor_t*    ptor = (tb_iocp_ptor_t*)priv;
    tb_aicp_t*                  aicp = ptor? ptor->base.aicp : tb_null;
    tb_assert_and_check_return_val(ptor && ptor->timer && ptor->ltimer && ptor->event && aicp, tb_null);

    // trace
    tb_trace_d("loop: init");

    // loop 
    while (!tb_atomic_get(&aicp->kill))
    {
        // the delay
        tb_size_t delay = tb_timer_delay(ptor->timer);

        // the ldelay
        tb_size_t ldelay = tb_ltimer_delay(ptor->ltimer);
        tb_assert_and_check_break(ldelay != -1);

        // using the min delay
        if (ldelay < delay) delay = ldelay;

        // wait some time
        if (delay)
        {
            if (tb_event_wait(ptor->event, delay) < 0) break;
        }

        // spak ctime
        tb_cache_time_spak();

        // spak timer
        if (!tb_timer_spak(ptor->timer)) break;

        // spak ltimer
        if (!tb_ltimer_spak(ptor->ltimer)) break;
    }

    // trace
    tb_trace_d("loop: exit");

    // kill
    tb_atomic_set(&aicp->kill, 1);

    // exit
    tb_thread_return(tb_null);
    return tb_null;
}
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

    // exit the timeout task
    if (iocp_aico->task) 
    {
        if (iocp_aico->bltimer) tb_ltimer_task_exit(ptor->ltimer, iocp_aico->task);
        else tb_timer_task_exit(ptor->timer, iocp_aico->task);
        iocp_aico->bltimer = 0;
    }
    iocp_aico->task = tb_null;

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
    tb_assert_and_check_return(ptor && ptor->event && aico);
        
    // trace
    tb_trace_d("kilo: aico: %p, handle: %p, type: %u", aico, aico->handle, aico->type);

    // kill the task
    if (((tb_iocp_aico_t*)aico)->task) 
    {
        if (((tb_iocp_aico_t*)aico)->bltimer) tb_ltimer_task_kill(ptor->ltimer, ((tb_iocp_aico_t*)aico)->task);
        else tb_timer_task_kill(ptor->timer, ((tb_iocp_aico_t*)aico)->task);
    }

    // sock: kill
    if (aico->type == TB_AICO_TYPE_SOCK && aico->handle) tb_socket_kill(aico->handle, TB_SOCKET_KILL_RW);
    // file: kill
    else if (aico->type == TB_AICO_TYPE_FILE && aico->handle) tb_file_exit(aico->handle);

    /* the iocp will wait long time if the lastest task wait period is too long
     * so spak the iocp manually for spak the timer
     */
    tb_event_post(ptor->event);
}
static tb_bool_t tb_iocp_ptor_post(tb_aicp_proactor_t* proactor, tb_aice_t const* aice)
{
    // check
    tb_iocp_ptor_t* ptor = (tb_iocp_ptor_t*)proactor;
    tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp && aice, tb_false);

    // the aico
    tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
    tb_assert_and_check_return_val(aico, tb_false);

    // trace
    tb_trace_d("post: aico: %p, code: %u, type: %lu: ..", aico, aice->code, aico->base.type);

    // post aice
    return tb_false;
}
static tb_void_t tb_iocp_ptor_kill(tb_aicp_proactor_t* proactor)
{
    // check
    tb_iocp_ptor_t* ptor = (tb_iocp_ptor_t*)proactor;
    tb_assert_and_check_return(ptor && ptor->port && ptor->event && proactor->aicp);

    // the workers
    tb_size_t work = tb_atomic_get(&proactor->aicp->work);
    
    // trace
    tb_trace_d("kill: %lu", work);

    // post the timer event
    tb_event_post(ptor->event);

    // using GetQueuedCompletionStatusEx?
    if (ptor->GetQueuedCompletionStatusEx)
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

        // post the timer event
        if (ptor->event) tb_event_post(ptor->event);

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

        // exit port
        if (ptor->port) CloseHandle(ptor->port);
        ptor->port = tb_null;

        // exit timer
        if (ptor->timer) tb_timer_exit(ptor->timer);
        ptor->timer = tb_null;

        // exit ltimer
        if (ptor->ltimer) tb_ltimer_exit(ptor->ltimer);
        ptor->ltimer = tb_null;

        // exit event
        if (ptor->event) tb_event_exit(ptor->event);
        ptor->event = tb_null;

        // free it
        tb_free(ptor);
    }
}
static tb_long_t tb_iocp_ptor_spak(tb_aicp_proactor_t* proactor, tb_handle_t loop, tb_aice_t* resp, tb_long_t timeout)
{
    // check
    tb_iocp_ptor_t* ptor = (tb_iocp_ptor_t*)proactor;
    tb_assert_and_check_return_val(ptor && ptor->port && ptor->timer && resp, -1);


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
    tb_bool_t                   ok = tb_false;
    tb_iocp_ptor_t*    ptor = tb_null;
    do
    {
        // make proactor
        ptor = tb_malloc0(sizeof(tb_iocp_ptor_t));
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
        ptor->base.spak         = tb_iocp_ptor_spak;

        // init func
        ptor->AcceptEx                      = tb_mswsock()->AcceptEx;
        ptor->ConnectEx                     = tb_mswsock()->ConnectEx;
        ptor->TransmitFile                  = tb_mswsock()->TransmitFile;
        ptor->CancelIoEx                    = tb_kernel32()->CancelIoEx;
        ptor->GetQueuedCompletionStatusEx   = tb_kernel32()->GetQueuedCompletionStatusEx;
        ptor->WSAGetLastError               = tb_ws2_32()->WSAGetLastError;
        ptor->WSASend                       = tb_ws2_32()->WSASend;
        ptor->WSARecv                       = tb_ws2_32()->WSARecv;
        ptor->WSASendTo                     = tb_ws2_32()->WSASendTo;
        ptor->WSARecvFrom                   = tb_ws2_32()->WSARecvFrom;
        ptor->bind                          = tb_ws2_32()->bind;
        tb_assert_and_check_break(ptor->AcceptEx && ptor->ConnectEx);

        // init port
        ptor->port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, tb_null, 0, 0);
        tb_assert_and_check_break(ptor->port && ptor->port != INVALID_HANDLE_VALUE);

        // init timer and using cache time
        ptor->timer = tb_timer_init(aicp->maxn >> 3, tb_true);
        tb_assert_and_check_break(ptor->timer);

        // init ltimer and using cache time
        ptor->ltimer = tb_ltimer_init(aicp->maxn, TB_LTIMER_TICK_S, tb_true);
        tb_assert_and_check_break(ptor->ltimer);

        // init the timer event
        ptor->event = tb_event_init();
        tb_assert_and_check_break(ptor->event);

        // init loop
        ptor->loop = tb_thread_init(tb_null, tb_iocp_ptor_loop, ptor, 0);
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

