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
 * @file		file.c
 * @ingroup 	platform
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#undef TB_TRACE_IMPL_TAG
#define TB_TRACE_IMPL_TAG 				"aicp_file"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_handle_t tb_aicp_file_init(tb_aicp_proactor_aiop_t* ptor)
{
	return ptor;
}
static tb_void_t tb_aicp_file_exit(tb_handle_t file)
{
}
static tb_bool_t tb_aicp_file_addo(tb_handle_t file, tb_aico_t* aico)
{
	return tb_true;
}
static tb_bool_t tb_aicp_file_delo(tb_handle_t file, tb_aico_t* aico)
{
	return tb_true;
}
static tb_bool_t tb_aicp_file_post(tb_handle_t file, tb_aice_t const* list, tb_size_t size)
{
	// check
	tb_aicp_proactor_aiop_t* ptor = (tb_aicp_proactor_aiop_t*)file;
	tb_assert_and_check_return_val(ptor && list && size, tb_false);
	
	// enter 
	if (ptor->mutx) tb_mutex_enter(ptor->mutx);

	// walk list
	tb_size_t i = 0;
	tb_bool_t ok = tb_true;
	for (i = 0; i < size && ok; i++)
	{
		// the aice
		tb_aice_t const* aice = &list[i];

		// post aice
		if (!tb_queue_full(ptor->spak)) 
		{
			// put
			tb_queue_put(ptor->spak, aice);

			// trace
			tb_trace_impl("post: code: %lu, size: %lu", aice->code, tb_queue_size(ptor->spak));
		}
		else
		{
			// failed
			ok = tb_false;

			// assert
			tb_assert(0);
		}

	}

	// leave 
	if (ptor->mutx) tb_mutex_leave(ptor->mutx);

	// ok?
	return ok;
}
static tb_long_t tb_aicp_file_spak(tb_handle_t file, tb_aice_t* aice)
{
	// check
	tb_aicp_proactor_aiop_t* ptor = (tb_aicp_proactor_aiop_t*)file;
	tb_assert_and_check_return_val(ptor && aice, -1);

	// spak it
	tb_long_t ok = -1;
	switch (aice->code)
	{
	case TB_AICE_CODE_READ:
		{
			// the handle 
			tb_handle_t handle = aice->aico? aice->aico->handle : tb_null;
			tb_assert_and_check_break(handle);

			// seek
			tb_long_t real = -1;
			if (tb_file_seek(handle, aice->u.read.seek))
			{
				// read
				real = tb_file_read(handle, aice->u.read.data, aice->u.read.size);

				// trace
				tb_trace_impl("read[%p]: %ld", handle, real);
			}

			// ok?
			if (real > 0) 
			{
				aice->u.read.real = real;
				aice->state = TB_AICE_STATE_OK;
			}
			// closed
			else if (!real) aice->state = TB_AICE_STATE_CLOSED;
			// failed?
			else aice->state = TB_AICE_STATE_FAILED;

			// ok
			ok = 1;
		}
		break;
	case TB_AICE_CODE_WRIT:
		{
			// the handle 
			tb_handle_t handle = aice->aico? aice->aico->handle : tb_null;
			tb_assert_and_check_break(handle);

			// seek
			tb_long_t real = -1;
			if (tb_file_seek(handle, aice->u.writ.seek))
			{
				// writ
				real = tb_file_writ(handle, aice->u.writ.data, aice->u.writ.size);
		
				// writ
				tb_trace_impl("writ[%p]: %ld", handle, real);
			}
			
			// ok?
			if (real > 0) 
			{
				aice->u.writ.real = real;
				aice->state = TB_AICE_STATE_OK;
			}
			// closed
			else if (!real) aice->state = TB_AICE_STATE_CLOSED;
			// failed?
			else aice->state = TB_AICE_STATE_FAILED;

			// ok
			ok = 1;
		}
		break;
	default:
		break;
	}

	// ok?
	return ok;
}
static tb_void_t tb_aicp_file_kill(tb_handle_t file)
{
}
static tb_void_t tb_aicp_file_poll(tb_handle_t file)
{
}

