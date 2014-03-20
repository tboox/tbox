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
#include "../network/network.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the mstream type
typedef struct __tb_mstream_t
{
	// the aicp
	tb_aicp_t* 			aicp;

	// the concurrent transfer count
	tb_size_t 			conc;

	// the lock
	tb_spinlock_t 		lock;

	// the tstream list

}tb_mstream_t;

 
/* ///////////////////////////////////////////////////////////////////////
 * implementation
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

}
tb_void_t tb_mstream_exit(tb_handle_t handle)
{
	// check
	tb_mstream_t* mstream = (tb_mstream_t*)handle;
	tb_assert_and_check_return(mstream);


}
tb_bool_t tb_mstream_done(tb_handle_t handle, tb_char_t const* iurl, tb_char_t const* ourl, tb_hize_t offset, tb_tstream_save_func_t func, tb_pointer_t priv)
{
	// check
	tb_mstream_t* mstream = (tb_mstream_t*)handle;
	tb_assert_and_check_return_val(mstream && iurl && ourl, tb_false);

	return tb_false;
}
