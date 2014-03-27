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
 * @file		mstream.c
 * @ingroup 	stream
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 				"mstream"
#define TB_TRACE_MODULE_DEBUG 				(1)
 
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "mstream.h"
#include "tstream.h"
#include "../network/network.h"
#include "../platform/platform.h"
#include "../container/container.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the mstream transfer type
typedef struct __tb_mstream_transfer_t
{
	// the tstream
	tb_handle_t 			tstream;

	// the mstream
	tb_handle_t 			mstream;

	// the func
	tb_tstream_save_func_t 	func;

	// the priv
	tb_pointer_t 			priv;

	// the itor for the working list
	tb_size_t 				itor;

}tb_mstream_transfer_t;

// the mstream type
typedef struct __tb_mstream_t
{
	// the aicp
	tb_aicp_t* 				aicp;

	// the loop
	tb_handle_t 			loop;

	// the aicp is referneced?
	tb_bool_t 				bref;

	// the concurrent transfer count
	tb_size_t 				conc;

	// the lock 
	tb_spinlock_t 			lock;

	// the transfer pool
	tb_handle_t 			pool;

	// the working list
	tb_dlist_t* 			working;

	// the waiting list
	tb_slist_t* 			waiting;

	// the timeout
	tb_long_t 				timeout;

	// is stoped?
	tb_bool_t 				bstoped;

}tb_mstream_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_mstream_transfer_exit(tb_mstream_transfer_t* transfer, tb_bool_t bcalling)
{
	// check
	tb_assert_and_check_return(transfer);

	// the mstream
	tb_mstream_t* mstream = (tb_mstream_t*)transfer->mstream;
	tb_assert_and_check_return(mstream && mstream->pool);

	// trace
	tb_trace_d("transfer[%p]: exit", transfer);

	// exit tstream
	if (transfer->tstream) tb_tstream_exit(transfer->tstream, bcalling);
	transfer->tstream = tb_null;

	// free it
	tb_rpool_free(mstream->pool, transfer);
}
static tb_bool_t tb_mstream_transfer_save(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_pointer_t priv)
{
	// the transfer
	tb_mstream_transfer_t* transfer = (tb_mstream_transfer_t*)priv;
	tb_assert_and_check_return_val(transfer && transfer->func && transfer->tstream, tb_false);

	// the mstream
	tb_mstream_t* mstream = (tb_mstream_t*)transfer->mstream;
	tb_assert_and_check_return_val(mstream, tb_false);

	// trace
	tb_trace_d("transfer[%p]: save: %llu bytes, rate: %lu bytes/s, state: %s", transfer, save, rate, tb_stream_state_cstr(state));

	// done func
	tb_bool_t ok = transfer->func(state, offset, size, save, rate, transfer->priv);	

	// failed, killed or closed?
	if (state != TB_STREAM_STATE_OK && state != TB_STREAM_STATE_PAUSED)
	{
		// enter
		tb_spinlock_enter(&mstream->lock);

		// done
		tb_bool_t next_ok = tb_false;
		do
		{
			// check working
			tb_assert_and_check_break(mstream->working);

			// remove transfer from the working list
			tb_dlist_remove(mstream->working, transfer->itor);
		
			// exit transfer
			tb_mstream_transfer_exit(transfer, tb_true);
			transfer = tb_null;

			// continue the next waiting transfer
			tb_check_break(!mstream->bstoped);

			// need work it?
			tb_bool_t next = (mstream->conc && tb_dlist_size(mstream->working) >= mstream->conc)? tb_false : tb_true;
			tb_check_break(next && mstream->waiting && tb_slist_size(mstream->waiting));
			
			// get the head transfer from the waiting list
			transfer = tb_slist_head(mstream->waiting);
			tb_assert_and_check_break(transfer && transfer->tstream);

			// append the transfer to the working list
			transfer->itor = tb_dlist_insert_tail(mstream->working, transfer);
			tb_assert_and_check_break(transfer->itor != tb_iterator_tail(mstream->working));

			// remove the transfer from the waiting list
			tb_slist_remove_head(mstream->waiting);

			// trace
			tb_trace_d("next: working: %lu, waiting: %lu", tb_dlist_size(mstream->working), mstream->waiting? tb_slist_size(mstream->waiting) : 0);

			// ok
			next_ok = tb_true;

		} while (0);

		// leave
		tb_spinlock_leave(&mstream->lock);

		// ok and work it?
		if (next_ok && transfer && transfer->tstream)
		{
			// done
			if (!tb_tstream_osave(transfer->tstream, tb_mstream_transfer_save, transfer))
			{
				// enter
				tb_spinlock_enter(&mstream->lock);

				// remove transfer from the working list
				tb_dlist_remove(mstream->working, transfer->itor);

				// exit transfer
				tb_mstream_transfer_exit(transfer, tb_false);

				// leave
				tb_spinlock_leave(&mstream->lock);
			}
		}
	}

	// ok?
	return ok;
}
static tb_bool_t tb_mstream_transfer_working_kill(tb_dlist_t* working, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data)
{
	// the transfer 
	tb_mstream_transfer_t* transfer = item? *((tb_mstream_transfer_t**)item) : tb_null;
	tb_check_return_val(transfer, tb_false);

	// check
	tb_assert_and_check_return_val(transfer->tstream, tb_false);

	// kill it
	tb_tstream_kill(transfer->tstream);

	// ok
	return tb_true;
}
static tb_bool_t tb_mstream_transfer_working_copy(tb_dlist_t* working, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data)
{
	// the transfer 
	tb_mstream_transfer_t* transfer = item? *((tb_mstream_transfer_t**)item) : tb_null;
	tb_check_return_val(transfer && data, tb_false);

	// save it
	tb_dlist_insert_tail((tb_dlist_t*)data, transfer);

	// ok
	return tb_true;
}
static tb_bool_t tb_mstream_transfer_waiting_exit(tb_slist_t* waiting, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data)
{
	// the transfer 
	tb_mstream_transfer_t* transfer = item? *((tb_mstream_transfer_t**)item) : tb_null;
	tb_check_return_val(transfer, tb_false);

	// exit it
	tb_mstream_transfer_exit(transfer, tb_false);

	// ok
	return tb_true;
}
static tb_pointer_t tb_mstream_loop(tb_pointer_t data)
{
	// aicp
	tb_handle_t aicp = data;

	// trace
	tb_trace_d("loop: init");

	// loop aicp
	if (aicp) tb_aicp_loop(aicp);
	
	// trace
	tb_trace_d("loop: exit");

	// exit
	tb_thread_return(tb_null);
	return tb_null;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_handle_t tb_mstream_init(tb_aicp_t* aicp, tb_size_t conc, tb_long_t timeout)
{
	// done
	tb_bool_t 		ok = tb_false;
	tb_mstream_t* 	mstream = tb_null;
	do
	{
		// make mstream
		mstream = (tb_handle_t)tb_malloc0(sizeof(tb_mstream_t));
		tb_assert_and_check_break(mstream);

		// init lock
		if (!tb_spinlock_init(&mstream->lock)) break;

		// init mstream
		mstream->bref 		= aicp? tb_true : tb_false;
		mstream->aicp 		= aicp? aicp : tb_aicp_init(conc);
		mstream->conc 		= conc;
		mstream->timeout 	= timeout;
		mstream->bstoped 	= tb_false;
		tb_assert_and_check_break(mstream->aicp);

		// init pool
		mstream->pool = tb_rpool_init(conc? conc : 16, sizeof(tb_mstream_transfer_t), 0);
		tb_assert_and_check_break(mstream->pool);

		// init working list
		mstream->working = tb_dlist_init(conc? conc : 16, tb_item_func_ptr(tb_null, tb_null));
		tb_assert_and_check_break(mstream->working);

		// init waiting list delay, maybe not used if conc be zero
		mstream->waiting = tb_null;

		// ok
		ok = tb_true;

	} while (0);

	// failed?
	if (!ok) 
	{
		// exit it
		if (mstream) tb_mstream_exit((tb_handle_t)mstream);
		mstream = tb_null;
	}

	// ok?
	return (tb_handle_t)mstream;
}
tb_void_t tb_mstream_kill(tb_handle_t handle)
{
	// check
	tb_mstream_t* mstream = (tb_mstream_t*)handle;
	tb_assert_and_check_return(mstream);

	// trace
	tb_trace_d("kill: ..");

	// enter
	tb_spinlock_enter(&mstream->lock);

	// kill it if be not stoped
	tb_dlist_t* working = tb_null;
	if (!mstream->bstoped && mstream->working && tb_dlist_size(mstream->working))
	{
		// stop it
		mstream->bstoped = tb_true;

		// init the working list
		working = tb_dlist_init(mstream->conc? mstream->conc : 16, tb_item_func_ptr(tb_null, tb_null));

		// copy it
		tb_dlist_walk(mstream->working, tb_mstream_transfer_working_copy, working);
	}

	// leave
	tb_spinlock_leave(&mstream->lock);

	// kill it
	if (working)
	{
		tb_dlist_walk(working, tb_mstream_transfer_working_kill, tb_null);
		tb_dlist_exit(working);
		working = tb_null;
	}
}
tb_void_t tb_mstream_exit(tb_handle_t handle)
{
	// check
	tb_mstream_t* mstream = (tb_mstream_t*)handle;
	tb_assert_and_check_return(mstream);

	// trace
	tb_trace_d("exit: ..");

	// kill it
	tb_mstream_kill(mstream);

	// wait all working transfers exit
	tb_size_t tryn = 10;
	while (tryn--)
	{
		// the working count
		tb_spinlock_enter(&mstream->lock);
		tb_size_t nworking = mstream->working? tb_dlist_size(mstream->working) : 0;
		tb_spinlock_leave(&mstream->lock);

		// ok?
		tb_check_break(nworking);

		// too long? failed
		if (!tryn)
		{
			// trace
			tb_trace_e("mstream: exit failed: nworking: %lu", nworking);
			return ;
		}

		// wait some time
		tb_msleep(500);
	}

	// enter
	tb_spinlock_enter(&mstream->lock);

	// exit working
	if (mstream->working)
	{
		// check
		tb_assert(!tb_dlist_size(mstream->working));

		// exit it
		tb_dlist_exit(mstream->working);
		mstream->working = tb_null;
	}

	// exit waiting
	if (mstream->waiting) 
	{
		// exit it
		tb_slist_walk(mstream->waiting, tb_mstream_transfer_waiting_exit, tb_null);
		tb_slist_exit(mstream->waiting);
		mstream->waiting = tb_null;
	}

	// exit pool
	if (mstream->pool) tb_rpool_exit(mstream->pool);
	mstream->pool = tb_null;

	// leave
	tb_spinlock_leave(&mstream->lock);

	// kill aicp
	if (mstream->aicp && !mstream->bref)
		tb_aicp_kill(mstream->aicp);

	// exit loop
	if (mstream->loop)
	{
		// wait loop
		if (!tb_thread_wait(mstream->loop, 5000))
			tb_thread_kill(mstream->loop);

		// exit loop
		tb_thread_exit(mstream->loop);
		mstream->loop = tb_null;
	}

	// exit aicp
	if (mstream->aicp && !mstream->bref)
		tb_aicp_exit(mstream->aicp);
	mstream->aicp = tb_null;

	// exit lock
	tb_spinlock_exit(&mstream->lock);

	// exit it
	tb_free(mstream);

	// trace
	tb_trace_d("exit: ok");
}
tb_size_t tb_mstream_size(tb_handle_t handle)
{
	// check
	tb_mstream_t* mstream = (tb_mstream_t*)handle;
	tb_assert_and_check_return_val(mstream, 0);

	// enter
	tb_spinlock_enter(&mstream->lock);

	// the size
	tb_size_t size = mstream->pool? tb_rpool_size(mstream->pool) : 0;

	// leave
	tb_spinlock_leave(&mstream->lock);

	// ok?
	return size;
}
tb_bool_t tb_mstream_done(tb_handle_t handle, tb_char_t const* iurl, tb_char_t const* ourl, tb_hize_t offset, tb_tstream_save_func_t func, tb_pointer_t priv)
{
	// check
	tb_mstream_t* mstream = (tb_mstream_t*)handle;
	tb_assert_and_check_return_val(mstream && mstream->aicp && iurl && ourl, tb_false);
	
	// enter
	tb_spinlock_enter(&mstream->lock);

	// done
	tb_bool_t 				ok = tb_false;
	tb_bool_t 				bworking = tb_false;
	tb_mstream_transfer_t* 	transfer = tb_null;
	do
	{
		// stoped?
		tb_assert_and_check_break(!mstream->bstoped);

		// check
		tb_assert_and_check_break(mstream->working && mstream->pool);

		// init loop
		if (!mstream->loop)
		{
			mstream->loop = tb_thread_init(tb_null, tb_mstream_loop, mstream->aicp, 0);
			tb_assert_and_check_break(mstream->loop);
		}

		// make transfer
		transfer = tb_rpool_malloc0(mstream->pool);
		tb_assert_and_check_break(transfer);

		// init transfer
		transfer->mstream 	= mstream;
		transfer->tstream 	= tb_tstream_init_uu(mstream->aicp, iurl, ourl, offset);
		transfer->func 		= func;
		transfer->priv 		= priv;
		tb_assert_and_check_break(transfer->tstream);

		// init timeout
		if (mstream->timeout) tb_tstream_timeout_set(transfer->tstream, mstream->timeout);

		// working now?
		bworking = (mstream->conc && tb_dlist_size(mstream->working) >= mstream->conc)? tb_false : tb_true;
		if (bworking)
		{
			// append to the working list
			transfer->itor = tb_dlist_insert_tail(mstream->working, transfer);
			tb_assert_and_check_break(transfer->itor != tb_iterator_tail(mstream->working));
		}
		// waiting
		else
		{
			// init waiting list
			if (!mstream->waiting)
			{
				mstream->waiting = tb_slist_init(mstream->conc? mstream->conc : 16, tb_item_func_ptr(tb_null, tb_null));
				tb_assert_and_check_break(mstream->waiting);
			}

			// append to the waiting list
			tb_size_t itor = tb_slist_insert_tail(mstream->waiting, transfer);
			tb_assert_and_check_break(itor != tb_iterator_tail(mstream->waiting));
		}

		// ok
		ok = tb_true;

	} while (0);

	// trace
	tb_trace_d("done: %s => %s, working: %lu, waiting: %lu, state: %s", iurl, ourl, tb_dlist_size(mstream->working), mstream->waiting? tb_slist_size(mstream->waiting) : 0, ok? "ok" : "no");

	// failed? exit transfer
	if (!ok && transfer) tb_mstream_transfer_exit(transfer, tb_false);

	// leave
	tb_spinlock_leave(&mstream->lock);

	// ok and work it?
	if (ok && bworking && transfer && transfer->tstream)
	{
		// done
		if (!tb_tstream_osave(transfer->tstream, tb_mstream_transfer_save, transfer))
		{
			// enter
			tb_spinlock_enter(&mstream->lock);

			// remove transfer from the working list
			tb_dlist_remove(mstream->working, transfer->itor);

			// exit transfer
			tb_mstream_transfer_exit(transfer, tb_false);

			// leave
			tb_spinlock_leave(&mstream->lock);

			// failed
			ok = tb_false;
		}
	}

	// ok?
	return ok;
}
