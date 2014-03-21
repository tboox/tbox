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

// the mstream item type
typedef struct __tb_mstream_item_t
{
	// the tstream
	tb_handle_t 			tstream;

	// the func
	tb_tstream_save_func_t 	func;

	// the priv
	tb_pointer_t 			priv;

	// is working?
	tb_bool_t 				bworking;

}tb_mstream_item_t;

// the mstream type
typedef struct __tb_mstream_t
{
	// the aicp
	tb_aicp_t* 				aicp;

	// the concurrent transfer count
	tb_size_t 				conc;

	// the lock
	tb_spinlock_t 			lock;

	// the tstream list
	tb_slist_t* 			list;

}tb_mstream_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_mstream_item_kill(tb_slist_t* slist, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data)
{
	return tb_false;
}
 
/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_handle_t tb_mstream_init(tb_aicp_t* aicp, tb_size_t conc)
{
	// check
	tb_assert_and_check_return_val(aicp, tb_null);

	// done
	tb_bool_t 		ok = tb_false;
	tb_mstream_t* 	mstream = tb_null;
	do
	{
		// make mstream
		mstream = (tb_handle_t)tb_malloc0(sizeof(tb_mstream_t));
		tb_assert_and_check_break(mstream);

		// init mstream
		mstream->aicp = aicp;
		mstream->conc = conc;

		// init list
		mstream->list = tb_slist_init(conc? conc : 16, tb_item_func_ifm(sizeof(tb_mstream_item_t), tb_null, tb_null));
		tb_assert_and_check_break(mstream->list);

		// init lock
		if (!tb_spinlock_init(&mstream->lock)) break;

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

	// enter
	tb_spinlock_enter(&mstream->lock);

	// kill
	tb_slist_walk(mstream->list, tb_mstream_item_kill, tb_null);

	// leave
	tb_spinlock_leave(&mstream->lock);
}
tb_void_t tb_mstream_exit(tb_handle_t handle)
{
	// check
	tb_mstream_t* mstream = (tb_mstream_t*)handle;
	tb_assert_and_check_return(mstream);

	// kill it
	tb_mstream_kill(mstream);

	// exit lock
	tb_spinlock_exit(&mstream->lock);

	// exit it
	tb_free(mstream);
}
tb_bool_t tb_mstream_done(tb_handle_t handle, tb_char_t const* iurl, tb_char_t const* ourl, tb_hize_t offset, tb_tstream_save_func_t func, tb_pointer_t priv)
{
	// check
	tb_mstream_t* mstream = (tb_mstream_t*)handle;
	tb_assert_and_check_return_val(mstream && mstream->aicp && iurl && ourl, tb_false);
	
	// enter
	tb_spinlock_enter(&mstream->lock);

	// done
	tb_bool_t 	ok = tb_false;
	tb_handle_t tstream = tb_null;
	do
	{
		// init tstream
		tstream = tb_tstream_init_uu(mstream->aicp, iurl, ourl, offset);
		tb_assert_and_check_break(tstream);

		// init item
		tb_mstream_item_t item = {0};
		item.tstream 	= tstream;
		item.func 		= func;
		item.priv 		= priv;
		item.bworking 	= (mstream->conc && tb_slist_size(mstream->list) >= mstream->conc)? tb_false : tb_true;

		// add item to list
		if (tb_slist_insert_tail(mstream->list, &item) == tb_iterator_tail(mstream->list)) break;

		// work it?
		if (item.bworking && !tb_tstream_osave(tstream, func, priv))
			tb_slist_remove_last(mstream->list);

		// ok
		ok = tb_true;
		tstream = tb_null;

	} while (0);

	// leave
	tb_spinlock_leave(&mstream->lock);

	// failed? exit tstream
	if (!ok && tstream) tb_tstream_exit(tstream, tb_false);

	// ok?
	return ok;
}
