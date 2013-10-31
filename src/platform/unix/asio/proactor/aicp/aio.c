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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		aio.c
 * @ingroup 	platform
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include <aio.h>
#include <errno.h>

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// the aio list post maxn
#define TB_AIO_LIST_POST_MAXN 		(256)

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the aio aice type
typedef struct __tb_aio_aice_t
{
	// the base
	struct aiocb 			base;
	
	// the aice
	tb_aice_t 				aice;

	// the ptor
	tb_pointer_t 			ptor;

}tb_aio_aice_t;

// the aio mutx type
typedef struct __tb_aio_mutx_t
{
	// the post mutx
	tb_handle_t 			post;

	// the resp mutx
	tb_handle_t 			resp;

}tb_aio_mutx_t;

// the aio proactor type
typedef struct __tb_aicp_proactor_aio_t
{
	// the proactor base
	tb_aicp_proactor_t 		base;

	// the mutx
	tb_aio_mutx_t 			mutx;

	// the post pool
	tb_handle_t 			post;

	// the resp queue
	tb_queue_t* 			resp;

	// the resp wait
	tb_handle_t 			wait;
	
}tb_aicp_proactor_aio_t;

/* ///////////////////////////////////////////////////////////////////////
 * post
 */
static tb_aio_aice_t* tb_aio_aice_init(tb_aicp_proactor_aio_t* ptor)
{
	// check
	tb_assert_and_check_return_val(ptor, tb_null);

	// enter 
	if (ptor->mutx.post) tb_mutex_enter(ptor->mutx.post);

	// make data
	tb_pointer_t data = ptor->post? tb_rpool_malloc0(ptor->post) : tb_null;

	// leave 
	if (ptor->mutx.post) tb_mutex_leave(ptor->mutx.post);
	
	// ok?
	return (tb_aio_aice_t*)data;
}
static tb_void_t tb_aio_aice_exit(tb_aicp_proactor_aio_t* ptor, tb_aio_aice_t* data)
{
	// check
	tb_assert_and_check_return(ptor);

	// enter 
	if (ptor->mutx.post) tb_mutex_enter(ptor->mutx.post);

	// free data
	if (data) tb_rpool_free(ptor->post, data);

	// leave 
	if (ptor->mutx.post) tb_mutex_leave(ptor->mutx.post);
}

/* ///////////////////////////////////////////////////////////////////////
 * resp
 */
static tb_void_t tb_aio_resp_post(tb_aicp_proactor_aio_t* ptor, tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return(ptor && ptor->resp && aice);

	// enter 
	if (ptor->mutx.resp) tb_mutex_enter(ptor->mutx.resp);

	// post aice
	if (!tb_queue_full(ptor->resp)) tb_queue_put(ptor->resp, aice);
	else
	{
		// assert
		tb_assert(0);
	}

	// leave 
	if (ptor->mutx.resp) tb_mutex_leave(ptor->mutx.resp);
}
static tb_void_t tb_aio_resp_recv(sigval_t sigval)  
{
	// check
	tb_aio_aice_t* aio_aice = (tb_aio_aice_t*)sigval.sival_ptr;
	tb_assert_and_check_return(aio_aice);

	// done error
	tb_long_t error = aio_error((struct aiocb*)aio_aice);
	switch (error)
	{
		// ok
	case 0:
		{
			// the real size
			tb_long_t real = aio_return((struct aiocb*)aio_aice);

			// trace
			tb_trace_impl("recv[%p]: real: %ld", aio_aice->aice.handle, real);

			// save resp
			if (real > 0) 
			{
				aio_aice->aice.state 		= TB_AICE_STATE_OK;
				aio_aice->aice.u.recv.real  = real;
			}
			else if (!real)
			{
				aio_aice->aice.state 		= TB_AICE_STATE_CLOSED;
				aio_aice->aice.u.recv.real  = real;
			}
			else
			{
				aio_aice->aice.state 		= TB_AICE_STATE_FAILED;
				aio_aice->aice.u.recv.real  = 0;
			}

			// post resp
			tb_aio_resp_post(aio_aice->ptor, &aio_aice->aice);
	
			// exit aice
			tb_aio_aice_exit(aio_aice->ptor, aio_aice);
		}
		break;
		// canceled
	case ECANCELED:
		{
			// save resp
			aio_aice->aice.state 		= TB_AICE_STATE_FAILED;
			aio_aice->aice.u.recv.real  = 0;

			// post resp
			tb_aio_resp_post(aio_aice->ptor, &aio_aice->aice);
	
			// exit aice
			tb_aio_aice_exit(aio_aice->ptor, aio_aice);

			// trace
			tb_trace_impl("recv[%p]: canceled", aio_aice->aice.handle);
		}
		break;
		// inprogress
	case EINPROGRESS:
		{
			// trace
			tb_trace_impl("recv[%p]: inprogress", aio_aice->aice.handle);
		}
		break;
		// error
	default:
		{
			// save resp
			aio_aice->aice.state 		= TB_AICE_STATE_FAILED;
			aio_aice->aice.u.recv.real  = 0;

			// post resp
			tb_aio_resp_post(aio_aice->ptor, &aio_aice->aice);
	
			// exit aice
			tb_aio_aice_exit(aio_aice->ptor, aio_aice);

			// trace
			tb_trace_impl("recv[%p]: unknown error: %ld", aio_aice->aice.handle, error);
		}
		break;
	} 
}
static tb_void_t tb_aio_resp_send(sigval_t sigval)  
{  
	// check
	tb_aio_aice_t* aio_aice = (tb_aio_aice_t*)sigval.sival_ptr;
	tb_assert_and_check_return(aio_aice);

	// done error
	tb_long_t error = aio_error((struct aiocb*)aio_aice);
	switch (error)
	{
		// ok
	case 0:
		{
			// the real size
			tb_long_t real = aio_return((struct aiocb*)aio_aice);

			// trace
			tb_trace_impl("send[%p]: real: %ld", aio_aice->aice.handle, real);

			// save resp
			if (real > 0) 
			{
				aio_aice->aice.state 		= TB_AICE_STATE_OK;
				aio_aice->aice.u.send.real  = real;
			}
			else if (!real)
			{
				aio_aice->aice.state 		= TB_AICE_STATE_CLOSED;
				aio_aice->aice.u.send.real  = real;
			}
			else
			{
				aio_aice->aice.state 		= TB_AICE_STATE_FAILED;
				aio_aice->aice.u.send.real  = 0;
			}

			// post resp
			tb_aio_resp_post(aio_aice->ptor, &aio_aice->aice);
	
			// exit aice
			tb_aio_aice_exit(aio_aice->ptor, aio_aice);
		}
		break;
		// canceled
	case ECANCELED:
		{
			// save resp
			aio_aice->aice.state 		= TB_AICE_STATE_FAILED;
			aio_aice->aice.u.send.real  = 0;

			// post resp
			tb_aio_resp_post(aio_aice->ptor, &aio_aice->aice);
	
			// exit aice
			tb_aio_aice_exit(aio_aice->ptor, aio_aice);

			// trace
			tb_trace_impl("send[%p]: canceled", aio_aice->aice.handle);
		}
		break;
		// inprogress
	case EINPROGRESS:
		{
			// trace
			tb_trace_impl("send[%p]: inprogress", aio_aice->aice.handle);
		}
		break;
		// error
	default:
		{
			// save resp
			aio_aice->aice.state 		= TB_AICE_STATE_FAILED;
			aio_aice->aice.u.send.real  = 0;

			// post resp
			tb_aio_resp_post(aio_aice->ptor, &aio_aice->aice);
	
			// exit aice
			tb_aio_aice_exit(aio_aice->ptor, aio_aice);

			// trace
			tb_trace_impl("send[%p]: unknown error: %ld", aio_aice->aice.handle, error);
		}
		break;
	} 
}
static tb_void_t tb_aio_resp_read(sigval_t sigval)  
{  
	// check
	tb_aio_aice_t* aio_aice = (tb_aio_aice_t*)sigval.sival_ptr;
	tb_assert_and_check_return(aio_aice);

	// done error
	tb_long_t error = aio_error((struct aiocb*)aio_aice);
	switch (error)
	{
		// ok
	case 0:
		{
			// the real size
			tb_long_t real = aio_return((struct aiocb*)aio_aice);

			// trace
			tb_trace_impl("read[%p]: real: %ld", aio_aice->aice.handle, real);

			// save resp
			if (real > 0) 
			{
				aio_aice->aice.state 		= TB_AICE_STATE_OK;
				aio_aice->aice.u.read.real  = real;
			}
			else if (!real)
			{
				aio_aice->aice.state 		= TB_AICE_STATE_CLOSED;
				aio_aice->aice.u.read.real  = real;
			}
			else
			{
				aio_aice->aice.state 		= TB_AICE_STATE_FAILED;
				aio_aice->aice.u.read.real  = 0;
			}

			// post resp
			tb_aio_resp_post(aio_aice->ptor, &aio_aice->aice);
	
			// exit aice
			tb_aio_aice_exit(aio_aice->ptor, aio_aice);
		}
		break;
		// canceled
	case ECANCELED:
		{
			// save resp
			aio_aice->aice.state 		= TB_AICE_STATE_FAILED;
			aio_aice->aice.u.read.real  = 0;

			// post resp
			tb_aio_resp_post(aio_aice->ptor, &aio_aice->aice);
	
			// exit aice
			tb_aio_aice_exit(aio_aice->ptor, aio_aice);

			// trace
			tb_trace_impl("read[%p]: canceled", aio_aice->aice.handle);
		}
		break;
		// inprogress
	case EINPROGRESS:
		{
			// trace
			tb_trace_impl("read[%p]: inprogress", aio_aice->aice.handle);
		}
		break;
		// error
	default:
		{
			// save resp
			aio_aice->aice.state 		= TB_AICE_STATE_FAILED;
			aio_aice->aice.u.read.real  = 0;

			// post resp
			tb_aio_resp_post(aio_aice->ptor, &aio_aice->aice);
	
			// exit aice
			tb_aio_aice_exit(aio_aice->ptor, aio_aice);

			// trace
			tb_trace_impl("read[%p]: unknown error: %ld", aio_aice->aice.handle, error);
		}
		break;
	} 
}
static tb_void_t tb_aio_resp_writ(sigval_t sigval)  
{  
	// check
	tb_aio_aice_t* aio_aice = (tb_aio_aice_t*)sigval.sival_ptr;
	tb_assert_and_check_return(aio_aice);

	// done error
	tb_long_t error = aio_error((struct aiocb*)aio_aice);
	switch (error)
	{
		// ok
	case 0:
		{
			// the real size
			tb_long_t real = aio_return((struct aiocb*)aio_aice);

			// trace
			tb_trace_impl("writ[%p]: real: %ld", aio_aice->aice.handle, real);

			// save resp
			if (real > 0) 
			{
				aio_aice->aice.state 		= TB_AICE_STATE_OK;
				aio_aice->aice.u.writ.real  = real;
			}
			else if (!real)
			{
				aio_aice->aice.state 		= TB_AICE_STATE_CLOSED;
				aio_aice->aice.u.writ.real  = real;
			}
			else
			{
				aio_aice->aice.state 		= TB_AICE_STATE_FAILED;
				aio_aice->aice.u.writ.real  = 0;
			}

			// post resp
			tb_aio_resp_post(aio_aice->ptor, &aio_aice->aice);
	
			// exit aice
			tb_aio_aice_exit(aio_aice->ptor, aio_aice);
		}
		break;
		// canceled
	case ECANCELED:
		{
			// save resp
			aio_aice->aice.state 		= TB_AICE_STATE_FAILED;
			aio_aice->aice.u.writ.real  = 0;

			// post resp
			tb_aio_resp_post(aio_aice->ptor, &aio_aice->aice);
	
			// exit aice
			tb_aio_aice_exit(aio_aice->ptor, aio_aice);

			// trace
			tb_trace_impl("writ[%p]: canceled", aio_aice->aice.handle);
		}
		break;
		// inprogress
	case EINPROGRESS:
		{
			// trace
			tb_trace_impl("writ[%p]: inprogress", aio_aice->aice.handle);
		}
		break;
		// error
	default:
		{
			// save resp
			aio_aice->aice.state 		= TB_AICE_STATE_FAILED;
			aio_aice->aice.u.writ.real  = 0;

			// post resp
			tb_aio_resp_post(aio_aice->ptor, &aio_aice->aice);
	
			// exit aice
			tb_aio_aice_exit(aio_aice->ptor, aio_aice);

			// trace
			tb_trace_impl("writ[%p]: unknown error: %ld", aio_aice->aice.handle, error);
		}
		break;
	} 
}
/* ///////////////////////////////////////////////////////////////////////
 * post
 */
static tb_bool_t tb_aio_post_acpt(tb_aicp_proactor_t* proactor, tb_aice_t const* aice, struct aiocb** paiocb)
{
	// check
	tb_aicp_proactor_aio_t* ptor = (tb_aicp_proactor_aio_t*)proactor;
	tb_assert_and_check_return_val(ptor, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->handle && aice->code == TB_AICE_CODE_ACPT, tb_false);
	
	// trace
	tb_trace_impl("accept: ..");

	// ok
	return tb_true;
}
static tb_bool_t tb_aio_post_conn(tb_aicp_proactor_t* proactor, tb_aice_t const* aice, struct aiocb** paiocb)
{
	// check
	tb_aicp_proactor_aio_t* ptor = (tb_aicp_proactor_aio_t*)proactor;
	tb_assert_and_check_return_val(ptor, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->handle && aice->code == TB_AICE_CODE_CONN, tb_false);
	tb_assert_and_check_return_val(aice->u.conn.host && aice->u.conn.port, tb_false);
	
	// trace
	tb_trace_impl("connect: %s:%lu", aice->u.conn.host, aice->u.conn.port);

	// ok
	return tb_true;
}
static tb_bool_t tb_aio_post_recv(tb_aicp_proactor_t* proactor, tb_aice_t const* aice, struct aiocb** paiocb)
{	
	// check
	tb_aicp_proactor_aio_t* ptor = (tb_aicp_proactor_aio_t*)proactor;
	tb_assert_and_check_return_val(ptor, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->handle && aice->code == TB_AICE_CODE_RECV, tb_false);
	tb_assert_and_check_return_val(aice->u.recv.data && aice->u.recv.size && paiocb, tb_false);

	// done
	tb_bool_t 		ok = tb_false;
	tb_aio_aice_t* 	aio_aice = tb_null;
	do
	{
		// make aio_aice
		aio_aice = tb_aio_aice_init(ptor);
		tb_assert_and_check_break(aio_aice);

		// init aio_aice
		aio_aice->base.aio_lio_opcode 									= LIO_READ;
		aio_aice->base.aio_fildes 										= (tb_long_t)aice->handle - 1;
		aio_aice->base.aio_buf 											= aice->u.recv.data;
		aio_aice->base.aio_nbytes 										= aice->u.recv.size;
		aio_aice->base.aio_offset 										= 0;
		aio_aice->base.aio_sigevent.sigev_notify 						= SIGEV_THREAD;
		aio_aice->base.aio_sigevent._sigev_un._sigev_thread._function 	= tb_aio_resp_recv;
		aio_aice->base.aio_sigevent._sigev_un._sigev_thread._attribute 	= tb_null;
		aio_aice->base.aio_sigevent.sigev_value.sival_ptr 				= aio_aice;
		aio_aice->aice 													=*aice;
		aio_aice->ptor 													= ptor;

		// save aio_aice
		*paiocb = (struct aiocb*)aio_aice;

		// ok
		ok = tb_true;

	} while (0);

	// ok?
	return ok;
}
static tb_bool_t tb_aio_post_send(tb_aicp_proactor_t* proactor, tb_aice_t const* aice, struct aiocb** paiocb)
{
	// check
	tb_aicp_proactor_aio_t* ptor = (tb_aicp_proactor_aio_t*)proactor;
	tb_assert_and_check_return_val(ptor, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->handle && aice->code == TB_AICE_CODE_SEND, tb_false);
	tb_assert_and_check_return_val(aice->u.send.data && aice->u.send.size && paiocb, tb_false);

	// done
	tb_bool_t 		ok = tb_false;
	tb_aio_aice_t* 	aio_aice = tb_null;
	do
	{
		// make aio_aice
		aio_aice = tb_aio_aice_init(ptor);
		tb_assert_and_check_break(aio_aice);

		// init aio_aice
		aio_aice->base.aio_lio_opcode 									= LIO_WRITE;
		aio_aice->base.aio_fildes 										= (tb_long_t)aice->handle - 1;
		aio_aice->base.aio_buf 											= aice->u.send.data;
		aio_aice->base.aio_nbytes 										= aice->u.send.size;
		aio_aice->base.aio_offset 										= 0;
		aio_aice->base.aio_sigevent.sigev_notify 						= SIGEV_THREAD;
		aio_aice->base.aio_sigevent._sigev_un._sigev_thread._function 	= tb_aio_resp_send;
		aio_aice->base.aio_sigevent._sigev_un._sigev_thread._attribute 	= tb_null;
		aio_aice->base.aio_sigevent.sigev_value.sival_ptr 				= aio_aice;
		aio_aice->aice 													=*aice;
		aio_aice->ptor 													= ptor;

		// save aio_aice
		*paiocb = (struct aiocb*)aio_aice;

		// ok
		ok = tb_true;

	} while (0);

	// ok?
	return ok;
}
static tb_bool_t tb_aio_post_read(tb_aicp_proactor_t* proactor, tb_aice_t const* aice, struct aiocb** paiocb)
{	
	// check
	tb_aicp_proactor_aio_t* ptor = (tb_aicp_proactor_aio_t*)proactor;
	tb_assert_and_check_return_val(ptor, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->handle && aice->code == TB_AICE_CODE_READ, tb_false);
	tb_assert_and_check_return_val(aice->u.read.data && aice->u.read.size && paiocb, tb_false);

	// done
	tb_bool_t 		ok = tb_false;
	tb_aio_aice_t* 	aio_aice = tb_null;
	do
	{
		// make aio_aice
		aio_aice = tb_aio_aice_init(ptor);
		tb_assert_and_check_break(aio_aice);

		// init aio_aice
		aio_aice->base.aio_lio_opcode 									= LIO_READ;
		aio_aice->base.aio_fildes 										= (tb_long_t)aice->handle - 1;
		aio_aice->base.aio_buf 											= aice->u.read.data;
		aio_aice->base.aio_nbytes 										= aice->u.read.size;
		aio_aice->base.aio_offset 										= aice->u.read.seek;
		aio_aice->base.aio_sigevent.sigev_notify 						= SIGEV_THREAD;
		aio_aice->base.aio_sigevent._sigev_un._sigev_thread._function 	= tb_aio_resp_read;
		aio_aice->base.aio_sigevent._sigev_un._sigev_thread._attribute 	= tb_null;
		aio_aice->base.aio_sigevent.sigev_value.sival_ptr 				= aio_aice;
		aio_aice->aice 													=*aice;
		aio_aice->ptor 													= ptor;

		// save aio_aice
		*paiocb = (struct aiocb*)aio_aice;

		// ok
		ok = tb_true;

	} while (0);

	// ok?
	return ok;
}
static tb_bool_t tb_aio_post_writ(tb_aicp_proactor_t* proactor, tb_aice_t const* aice, struct aiocb** paiocb)
{
	// check
	tb_aicp_proactor_aio_t* ptor = (tb_aicp_proactor_aio_t*)proactor;
	tb_assert_and_check_return_val(ptor, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->handle && aice->code == TB_AICE_CODE_WRIT, tb_false);
	tb_assert_and_check_return_val(aice->u.writ.data && aice->u.writ.size && paiocb, tb_false);

	// done
	tb_bool_t 		ok = tb_false;
	tb_aio_aice_t* 	aio_aice = tb_null;
	do
	{
		// make aio_aice
		aio_aice = tb_aio_aice_init(ptor);
		tb_assert_and_check_break(aio_aice);

		// init aio_aice
		aio_aice->base.aio_lio_opcode 									= LIO_WRITE;
		aio_aice->base.aio_fildes 										= (tb_long_t)aice->handle - 1;
		aio_aice->base.aio_buf 											= aice->u.writ.data;
		aio_aice->base.aio_nbytes 										= aice->u.writ.size;
		aio_aice->base.aio_offset 										= aice->u.writ.seek;
		aio_aice->base.aio_sigevent.sigev_notify 						= SIGEV_THREAD;
		aio_aice->base.aio_sigevent._sigev_un._sigev_thread._function 	= tb_aio_resp_writ;
		aio_aice->base.aio_sigevent._sigev_un._sigev_thread._attribute 	= tb_null;
		aio_aice->base.aio_sigevent.sigev_value.sival_ptr 				= aio_aice;
		aio_aice->aice 													=*aice;
		aio_aice->ptor 													= ptor;

		// save aio_aice
		*paiocb = (struct aiocb*)aio_aice;

		// ok
		ok = tb_true;

	} while (0);

	// ok?
	return ok;
}

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_aicp_proactor_aio_addo(tb_aicp_proactor_t* proactor, tb_handle_t handle, tb_size_t type)
{
	// check
	tb_aicp_proactor_aio_t* ptor = (tb_aicp_proactor_aio_t*)proactor;
	tb_assert_and_check_return_val(ptor && handle && type, tb_false);

	// ok
	return tb_true;
}
static tb_bool_t tb_aicp_proactor_aio_delo(tb_aicp_proactor_t* proactor, tb_handle_t handle)
{
	// check
	tb_aicp_proactor_aio_t* ptor = (tb_aicp_proactor_aio_t*)proactor;
	tb_assert_and_check_return_val(ptor && handle, tb_false);

	// ok
	return tb_true;
}
static tb_bool_t tb_aicp_proactor_aio_post(tb_aicp_proactor_t* proactor, tb_aice_t const* list, tb_size_t size)
{
	// check
	tb_aicp_proactor_aio_t* ptor = (tb_aicp_proactor_aio_t*)proactor;
	tb_assert_and_check_return_val(ptor && list && size, tb_false);

	// init aio list
	struct aiocb* aio_list[TB_AIO_LIST_POST_MAXN] = {0};
	tb_assert_and_check_return_val(size < TB_AIO_LIST_POST_MAXN, tb_false);

	// walk list
	tb_size_t i = 0;
	tb_size_t aio_size = 0;
	for (i = 0; i < size && aio_size < TB_AIO_LIST_POST_MAXN; i++)
	{
		// the aice
		tb_aice_t const* aice = &list[i];

		// init post
		static tb_long_t (*s_post[])(tb_aicp_proactor_t* , tb_aice_t*, struct aiocb**) = 
		{
			tb_null
		,	tb_aio_post_acpt
		,	tb_aio_post_conn
		,	tb_aio_post_recv
		,	tb_aio_post_send
		,	tb_aio_post_read
		,	tb_aio_post_writ
		};
		tb_assert_and_check_return_val(aice->code < tb_arrayn(s_post) && s_post[aice->code], tb_false);

		// post aice
		if (!s_post[aice->code](proactor, aice, &aio_list[aio_size])) return tb_false;

		// aio_size++
		if (aio_list[aio_size]) aio_size++;
	}

	// ok
	return !lio_listio(LIO_NOWAIT, aio_list, aio_size, tb_null)? tb_true : tb_false;
}
static tb_long_t tb_aicp_proactor_aio_spak(tb_aicp_proactor_t* proactor, tb_aice_t* resp, tb_long_t timeout)
{
	// check
	tb_aicp_proactor_aio_t* ptor = (tb_aicp_proactor_aio_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->wait && resp, -1);

	// trace
	tb_trace_impl("spak[%lu]: ..", tb_thread_self());

	// wait
	tb_long_t ok = tb_event_wait(ptor->wait, timeout);
	tb_assert_and_check_return_val(ok >= 0, -1);

	// timeout?
	tb_check_return_val(ok > 0, 0);

	// enter 
	if (ptor->mutx.resp) tb_mutex_enter(ptor->mutx.resp);

	// post aice
	ok = 0;
	if (!tb_queue_null(ptor->resp)) 
	{
		// get resp
		tb_aice_t const* aice = tb_queue_get(ptor->resp);
		if (aice) 
		{
			// save resp
			*resp = *aice;

			// pop it
			tb_queue_pop(ptor->resp);

			// ok
			ok = 1;
		}
	}

	// leave 
	if (ptor->mutx.resp) tb_mutex_leave(ptor->mutx.resp);

	// ok?
	return ok;
}
static tb_void_t tb_aicp_proactor_aio_kill(tb_aicp_proactor_t* proactor)
{
	// check
	tb_aicp_proactor_aio_t* ptor = (tb_aicp_proactor_aio_t*)proactor;
	tb_assert_and_check_return(ptor && proactor->aicp);

	// trace
	tb_trace_impl("kill");

	// post wait
	if (ptor->wait) tb_event_post(ptor->wait);
}
static tb_void_t tb_aicp_proactor_aio_exit(tb_aicp_proactor_t* proactor)
{
	tb_aicp_proactor_aio_t* ptor = (tb_aicp_proactor_aio_t*)proactor;
	if (ptor)
	{
		// trace
		tb_trace_impl("exit");

		// exit post
		if (ptor->mutx.post) tb_mutex_enter(ptor->mutx.post);
		if (ptor->post) tb_rpool_exit(ptor->post);
		ptor->post = tb_null;
		if (ptor->mutx.post) tb_mutex_leave(ptor->mutx.post);

		// exit resp
		if (ptor->mutx.resp) tb_mutex_enter(ptor->mutx.resp);
		if (ptor->resp) tb_queue_exit(ptor->resp);
		ptor->resp = tb_null;
		if (ptor->mutx.post) tb_mutex_leave(ptor->mutx.resp);

		// exit mutx
		if (ptor->mutx.post) tb_mutex_exit(ptor->mutx.post);
		if (ptor->mutx.resp) tb_mutex_exit(ptor->mutx.resp);
		ptor->mutx.post = tb_null;
		ptor->mutx.resp = tb_null;

		// exit wait
		if (ptor->wait) tb_event_exit(ptor->wait);
		ptor->wait = tb_null;

		// free it
		tb_free(ptor);
	}
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
static tb_aicp_proactor_t* tb_aicp_proactor_aio_init(tb_aicp_t* aicp)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->maxn, tb_null);

	// alloc proactor
	tb_aicp_proactor_aio_t* ptor = tb_malloc0(sizeof(tb_aicp_proactor_aio_t));
	tb_assert_and_check_return_val(ptor, tb_null);

	// init base
	ptor->base.kill = tb_aicp_proactor_aio_kill;
	ptor->base.exit = tb_aicp_proactor_aio_exit;
	ptor->base.addo = tb_aicp_proactor_aio_addo;
	ptor->base.delo = tb_aicp_proactor_aio_delo;
	ptor->base.post = tb_aicp_proactor_aio_post;
	ptor->base.spak = tb_aicp_proactor_aio_spak;

	// init mutx
	ptor->mutx.post = tb_mutex_init(tb_null);
	ptor->mutx.resp = tb_mutex_init(tb_null);
	tb_assert_and_check_goto(ptor->mutx.post && ptor->mutx.resp, fail);

	// init post
	ptor->post = tb_rpool_init((aicp->maxn << 1) + 16, sizeof(tb_aio_aice_t), 0);
	tb_assert_and_check_goto(ptor->post, fail);

	// init resp
	ptor->resp = tb_queue_init((aicp->maxn << 2) + 16, tb_item_func_ifm(sizeof(tb_aice_t), tb_null, tb_null));
	tb_assert_and_check_goto(ptor->resp, fail);

	// init wait
	ptor->wait = tb_event_init(tb_null);
	tb_assert_and_check_goto(ptor->wait, fail);

	// ok
	return (tb_aicp_proactor_t*)ptor;

fail:
	if (ptor) tb_aicp_proactor_aio_exit(ptor);
	return tb_null;
}

