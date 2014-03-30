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
 * @author		ruki
 * @naio		naio.c
 * @ingroup 	platform
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#undef TB_TRACE_MODULE_NAME
#define TB_TRACE_MODULE_NAME 				"aicp_naio"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include <errno.h>
#include <sys/eventfd.h>
#include <linux/aio_abi.h>
#ifndef TB_CONFIG_OS_ANDROID
# 	include <sys/unistd.h>
#endif

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the naio iocb type
typedef struct __tb_naio_iocb_t
{
	// the base
	struct iocb 				base;

	// the aice
	tb_aice_t 					aice;

}tb_naio_iocb_t;

// the naio type
typedef struct __tb_naio_t
{
	// the ptor
	tb_aicp_proactor_aiop_t* 	ptor;

	// the spak
	tb_handle_t 				spak;
	
	// the aioo
	tb_aioo_t const* 			aioo;

	// the aico
	tb_aico_t 					aico;

	// the aice
	tb_aice_t 					aice;

	// the ictx
	aio_context_t 				ictx;

	// the lock
	tb_handle_t 				lock;

	// the iocb pool
	tb_handle_t 				pool_iocb;

	// the data pool
	tb_handle_t 				pool_data;

}tb_naio_t;

/* ///////////////////////////////////////////////////////////////////////
 * naio interfaces
 */
tb_syscall2(__NR_io_setup, 		tb_naio_setup, 		tb_int_t, 		maxn, aio_context_t*, 	pctx)
tb_syscall3(__NR_io_submit, 	tb_naio_submit, 	aio_context_t, 	ictx, tb_long_t, 		size, struct iocb**, 	iocbs)
tb_syscall5(__NR_io_getevents, 	tb_naio_getevents, 	aio_context_t, 	ictx, tb_long_t, 		minn, tb_long_t, 		maxn, struct io_event*, ioces, struct timespec*, timeout)
tb_syscall3(__NR_io_cancel, 	tb_naio_cancel, 	aio_context_t, 	ictx, struct iocb*, 	iocb, struct io_event*, ioce)
tb_syscall1(__NR_io_destroy, 	tb_naio_destroy, 	aio_context_t, 	ictx)

/* ///////////////////////////////////////////////////////////////////////
 * iocb
 */
static tb_bool_t tb_aicp_iocb_init_read(tb_naio_t* naio, tb_aice_t const* aice, tb_naio_iocb_t* iocb)
{
	// check
	tb_assert_and_check_return_val(naio && naio->spak && aice && iocb, tb_false);
	tb_assert_and_check_return_val(aice->u.read.data && aice->u.read.size, tb_false);

	// the aico
	tb_aico_t const* aico = aice->aico;
	tb_assert_and_check_return_val(aico && aico->handle, tb_false);

	// aligned?
	tb_bool_t aligned = !((tb_hize_t)aice->u.read.data & (TB_FILE_DIRECT_ASIZE - 1))? tb_true : tb_false;

	// init iocb
	iocb->aice 					= *aice;
	iocb->base.aio_fildes 		= (tb_int_t)aico->handle - 1;
	iocb->base.aio_lio_opcode 	= IOCB_CMD_PREAD;
	iocb->base.aio_buf 			= aligned? aice->u.read.data : tb_spool_malloc(naio->pool_data, aice->u.read.size);
	iocb->base.aio_offset 		= aice->u.read.seek;
	iocb->base.aio_nbytes 		= aice->u.read.size;
	iocb->base.aio_flags 		= IOCB_FLAG_RESFD;
	iocb->base.aio_resfd 		= (tb_int_t)naio->spak - 1;
	iocb->base.aio_data 		= (tb_pointer_t)aligned;
	tb_assert_and_check_return_val(iocb->base.aio_buf, tb_false);
	tb_assert_and_check_return_val(!((tb_hize_t)iocb->base.aio_buf & (TB_FILE_DIRECT_ASIZE - 1)), tb_false);
	tb_assert_and_check_return_val(!((tb_hize_t)iocb->base.aio_nbytes & (TB_FILE_DIRECT_ASIZE - 1)), tb_false);

	// ok
	return tb_true;
}
static tb_bool_t tb_aicp_iocb_init_writ(tb_naio_t* naio, tb_aice_t const* aice, tb_naio_iocb_t* iocb)
{
	// check
	tb_assert_and_check_return_val(naio && naio->spak && aice && iocb, tb_false);
	tb_assert_and_check_return_val(aice->u.writ.data && aice->u.writ.size, tb_false);

	// the aico
	tb_aico_t const* aico = aice->aico;
	tb_assert_and_check_return_val(aico && aico->handle, tb_false);

	// aligned?
	tb_bool_t aligned = !((tb_hize_t)aice->u.writ.data & (TB_FILE_DIRECT_ASIZE - 1))? tb_true : tb_false;

	// init iocb
	iocb->aice 					= *aice;
	iocb->base.aio_fildes 		= (tb_int_t)aico->handle - 1;
	iocb->base.aio_lio_opcode 	= IOCB_CMD_PWRITE;
	iocb->base.aio_buf 			= aligned? aice->u.writ.data : tb_spool_malloc(naio->pool_data, aice->u.writ.size);
	iocb->base.aio_offset 		= aice->u.writ.seek;
	iocb->base.aio_nbytes 		= aice->u.writ.size;
	iocb->base.aio_flags 		= IOCB_FLAG_RESFD;
	iocb->base.aio_resfd 		= (tb_int_t)naio->spak - 1;
	iocb->base.aio_data 		= (tb_pointer_t)aligned;
	tb_assert_and_check_return_val(iocb->base.aio_buf, tb_false);
	tb_assert_and_check_return_val(!((tb_hize_t)iocb->base.aio_buf & (TB_FILE_DIRECT_ASIZE - 1)), tb_false);
	// FIXME
	tb_assert_and_check_return_val(!((tb_hize_t)iocb->base.aio_nbytes & (TB_FILE_DIRECT_ASIZE - 1)), tb_false);

	// copy data
	if (!aligned) tb_memcpy(iocb->base.aio_buf, aice->u.writ.data, aice->u.writ.size);

	// ok
	return tb_true;
}
static tb_naio_iocb_t* tb_aicp_iocb_init(tb_naio_t* naio, tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(naio && naio->pool_iocb && naio->pool_data && aice, tb_null);

	// enter 
	if (naio->lock) tb_mutex_enter(naio->lock);

	// make iocb
	tb_naio_iocb_t* iocb = (tb_naio_iocb_t*)tb_rpool_malloc0(naio->pool_iocb);

	// init iocb
	if (iocb)
	{
		// done
		tb_bool_t ok = tb_false;
		do
		{
			// init spak
			static tb_bool_t (*s_init[])(tb_naio_t* , tb_aice_t const* , tb_naio_iocb_t* ) = 
			{
				tb_null
			,	tb_null
			,	tb_null
			,	tb_null
			,	tb_null
			,	tb_aicp_iocb_init_read
			,	tb_aicp_iocb_init_writ
			};
			tb_assert_and_check_break(aice->code && aice->code < tb_arrayn(s_init) && s_init[aice->code]);

			// done init 
			ok = s_init[aice->code](naio, aice, iocb);

		} while (0);

		// failed?
		if (!ok) 
		{
			// exit iocb
			if (iocb) 
			{
				// exit data
				if (iocb->base.aio_buf && !iocb->base.aio_data) tb_spool_free(naio->pool_data, iocb->base.aio_buf);
				iocb->base.aio_buf = tb_null;

				// exit it
				tb_rpool_free(naio->pool_iocb, iocb);
			}
			iocb = tb_null;
		}
	}

	// leave 
	if (naio->lock) tb_mutex_leave(naio->lock);
	
	// ok?
	return iocb;
}
static tb_void_t tb_aicp_iocb_exit(tb_naio_t* naio, tb_naio_iocb_t* iocb)
{
	// check
	tb_assert_and_check_return(naio && naio->pool_iocb && naio->pool_data);

	// enter 
	if (naio->lock) tb_mutex_enter(naio->lock);

	// exit iocb
	if (iocb) 
	{
		// exit data
		if (iocb->base.aio_buf && !iocb->base.aio_data) tb_spool_free(naio->pool_data, iocb->base.aio_buf);
		iocb->base.aio_buf = tb_null;

		// exit it
		tb_rpool_free(naio->pool_iocb, iocb);
	}

	// leave 
	if (naio->lock) tb_mutex_leave(naio->lock);
}
static tb_void_t tb_aicp_iocb_spak(tb_naio_t* naio, tb_naio_iocb_t* iocb, tb_long_t res, tb_long_t res2)
{
	// check
	tb_assert_and_check_return(naio && naio->ptor && naio->ptor->spak && iocb);

	// the real
	tb_long_t real = res;
	
	// trace		
	tb_trace_d("spak: code: %x, real: %ld", iocb->base.aio_lio_opcode, real);

	// done
	tb_bool_t ok = tb_false;
	switch (iocb->base.aio_lio_opcode)
	{
	case IOCB_CMD_PREAD:
		{
			// trace
			tb_trace_d("spak: read: %ld, size: %lu", real, iocb->aice.u.read.size);

			// ok?
			if (real > 0) 
			{
				// check
				tb_assert_and_check_break(iocb->base.aio_buf);

				// save data
				if (iocb->aice.u.read.data != iocb->base.aio_buf)
					tb_memcpy(iocb->aice.u.read.data, iocb->base.aio_buf, real);

				// save size
				iocb->aice.u.read.real = real;

				// save state
				iocb->aice.state = TB_STATE_OK;
			}
			// closed?
			else if (!real) iocb->aice.state = TB_STATE_CLOSED;
			// failed?
			else 
			{
				// trace
				tb_trace_d("spak: error: %s", strerror(-real));

				// save state
				iocb->aice.state = TB_STATE_FAILED;
			}

			// ok
			ok = tb_true;
		}
		break;
	case IOCB_CMD_PWRITE:
		{
			// trace
			tb_trace_d("spak: writ: %ld, size: %lu", real, iocb->aice.u.writ.size);

			// ok?
			if (real > 0) 
			{
				iocb->aice.state = TB_STATE_OK;
				iocb->aice.u.writ.real = real;
			}
			// closed?
			else if (!real) iocb->aice.state = TB_STATE_CLOSED;
			// failed?
			else 
			{
				// trace
				tb_trace_d("spak: error: %s", strerror(-real));

				// save state
				iocb->aice.state = TB_STATE_FAILED;
			}

			// ok
			ok = tb_true;
		}
		break;
	default:
		tb_assert(0);
		break;
	}

	// ok?
	if (ok)
	{
		// spak aice
		if (!tb_queue_full(naio->ptor->spak)) tb_queue_put(naio->ptor->spak, &iocb->aice);
		else tb_assert(0);
	}

	// exit iocb
	if (iocb) tb_aicp_iocb_exit(naio, iocb);
}

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_handle_t tb_aicp_file_init(tb_aicp_proactor_aiop_t* ptor)
{
	// check
	tb_assert_and_check_return_val(ptor && ptor->aiop && ptor->base.aicp, tb_null);

	// make naio
	tb_naio_t* naio = tb_malloc0(sizeof(tb_naio_t));
	tb_assert_and_check_goto(naio, fail);

	// init ptor
	naio->ptor = ptor;

	// init lock
	naio->lock = tb_mutex_init();
	tb_assert_and_check_goto(naio->lock, fail);

	// init iocb pool
	naio->pool_iocb = tb_rpool_init((ptor->base.aicp->maxn >> 4) + 16, sizeof(tb_naio_iocb_t), 0);
	tb_assert_and_check_goto(naio->pool_iocb, fail);

	// init data pool, aligned by 512 bytes
	naio->pool_data = tb_spool_init(((ptor->base.aicp->maxn >> 4) + 16) << 16, TB_FILE_DIRECT_ASIZE);
	tb_assert_and_check_goto(naio->pool_data, fail);

	// init ictx
	if (tb_naio_setup((tb_int_t)ptor->base.aicp->maxn, &naio->ictx)) goto fail;
	tb_assert_and_check_goto(naio->ictx, fail);

	// init spak
	naio->spak = (tb_handle_t)eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	tb_assert_and_check_goto((tb_long_t)naio->spak >= 0, fail);
	naio->spak = (tb_handle_t)((tb_long_t)naio->spak + 1);

	// init aice
	naio->aico.type 		= TB_AICO_TYPE_FILE;
	naio->aico.handle 		= naio->spak;
	naio->aice.code 		= TB_AICE_CODE_READ;
	naio->aice.aico 		= &naio->aico;
	naio->aice.aicb 		= tb_null;
	naio->aice.data 		= tb_null;
	naio->aice.u.read.seek 	= 0;
	naio->aice.u.read.data 	= tb_null;
	naio->aice.u.read.size 	= 1;

	// addo spak
	naio->aioo = tb_aiop_addo(ptor->aiop, naio->spak, TB_AIOE_CODE_RECV, &naio->aice);
	tb_assert_and_check_goto(naio->aioo, fail);

	// ok
	return (tb_handle_t)naio;

fail:
	if (naio) tb_aicp_file_exit(naio);
	return tb_null;
}
static tb_void_t tb_aicp_file_exit(tb_handle_t handle)
{
	tb_naio_t* naio = (tb_naio_t*)handle;
	if (naio)
	{
		// exit ictx
		if (naio->ictx) tb_naio_destroy(naio->ictx);
		naio->ictx = tb_null;

		// delo spak
		if (naio->aioo && naio->ptor && naio->ptor->aiop) tb_aiop_delo(naio->ptor->aiop, naio->aioo);
		naio->aioo = tb_null;

		// exit spak
		if (naio->spak) close((tb_int_t)naio->spak - 1);
		naio->spak = tb_null;
	
		// exit pool
		if (naio->lock) tb_mutex_enter(naio->lock);
		if (naio->pool_iocb) tb_rpool_exit(naio->pool_iocb);
		if (naio->pool_data) tb_spool_exit(naio->pool_data);
		naio->pool_iocb = tb_null;
		naio->pool_data = tb_null;
		if (naio->lock) tb_mutex_leave(naio->lock);

		// exit lock
		if (naio->lock) tb_mutex_exit(naio->lock);
		naio->lock = tb_null;

		// exit it
		tb_free(naio);
	}
}
static tb_bool_t tb_aicp_file_addo(tb_handle_t handle, tb_aico_t* aico)
{
	return tb_true;
}
static tb_bool_t tb_aicp_file_delo(tb_handle_t handle, tb_aico_t* aico)
{
	return tb_true;
}
static tb_bool_t tb_aicp_file_post(tb_handle_t handle, tb_aice_t const* list, tb_size_t size)
{
	// check
	tb_naio_t* naio = (tb_naio_t*)handle;
	tb_assert_and_check_return_val(naio && naio->ictx && list && size && size <= TB_AICP_POST_MAXN, tb_false);

	// init iocb list
	tb_size_t 		i = 0;
	tb_naio_iocb_t* iocb_list[TB_AICP_POST_MAXN];
	for (i = 0; i < size; i++)
	{
		iocb_list[i] = tb_aicp_iocb_init(naio, &list[i]);
		tb_assert_and_check_break(iocb_list[i]);
	}

	// submit iocb list
	if (i == size && size == tb_naio_submit(naio->ictx, (tb_long_t)size, (struct iocb**)iocb_list)) return tb_true;

	// failed? exit iocb list
	size = i;
	for (i = 0; i < size; i++)
	{
		if (iocb_list[i]) tb_aicp_iocb_exit(naio, iocb_list[i]);
		iocb_list[i] = tb_null;
	}
	return tb_false;
}
static tb_long_t tb_aicp_file_spak(tb_handle_t handle, tb_aice_t* aice)
{
	return 1;
}
static tb_void_t tb_aicp_file_kill(tb_handle_t handle)
{
	// check
	tb_naio_t* naio = (tb_naio_t*)handle;
	tb_assert_and_check_return(naio);
}
static tb_void_t tb_aicp_file_poll(tb_handle_t handle)
{
	// check
	tb_naio_t* naio = (tb_naio_t*)handle;
	tb_assert_and_check_return(naio && naio->ictx && naio->spak);

	// read the finished ioce count
	tb_uint64_t finished = 0;
	if (read((tb_int_t)naio->spak - 1, &finished, sizeof(finished)) != sizeof(finished)) return ;
	tb_trace_d("poll: finished: %llu", finished);

	// walk ioces
	struct timespec timeout = {0};
	struct io_event ioces[256];
	while (finished)
	{
		// get aices
		tb_long_t real = tb_naio_getevents(naio->ictx, 1, tb_arrayn(ioces), ioces, &timeout);
		tb_trace_d("poll: real: %llu", real);
		if (real > 0 && real <= finished)
		{
			tb_size_t i = 0;
			for (i = 0; i < real; ++i) 
			{
				// spak it
				tb_aicp_iocb_spak(naio, (tb_naio_iocb_t*)ioces[i].obj, ioces[i].res, ioces[i].res2);
			}
			finished -= real;
		}
	}
}

