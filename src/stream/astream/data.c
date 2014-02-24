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
 * @data		data.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
//#define TB_TRACE_IMPL_TAG 				"adata"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../utils/utils.h"
#include "../../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the data read type
typedef struct __tb_astream_data_read_t
{
	// the func
	tb_astream_read_func_t 		func;

	// the maxn
	tb_size_t 					maxn;

	// the priv
	tb_pointer_t 				priv;

}tb_astream_data_read_t;

// the data writ type
typedef struct __tb_astream_data_writ_t
{
	// the func
	tb_astream_writ_func_t 		func;

	// the data
	tb_byte_t const* 			data;

	// the size
	tb_size_t 					size;

	// the priv
	tb_pointer_t 				priv;

}tb_astream_data_writ_t;

// the data task type
typedef struct __tb_astream_data_task_t
{
	// the func
	tb_astream_task_func_t 		func;

	// the priv
	tb_pointer_t 				priv;

}tb_astream_data_task_t;

// the data stream type
typedef struct __tb_astream_data_t
{
	// the base
	tb_astream_t 				base;

	// the aico for task
	tb_handle_t 				aico;

	// the data
	tb_byte_t* 					data;

	// the head
	tb_byte_t* 					head;

	// the size
	tb_size_t 					size;

	// the data is referenced?
	tb_bool_t 					bref;

	// the func
	union
	{
		tb_astream_data_read_t 	read;
		tb_astream_data_writ_t 	writ;
		tb_astream_data_task_t 	task;

	} 							func;

}tb_astream_data_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_astream_data_t* tb_astream_data_cast(tb_astream_t* astream)
{
	tb_assert_and_check_return_val(astream && astream->type == TB_ASTREAM_TYPE_DATA, tb_null);
	return (tb_astream_data_t*)astream;
}
static tb_bool_t tb_astream_data_open(tb_astream_t* astream, tb_astream_open_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_data_t* dstream = tb_astream_data_cast(astream);
	tb_assert_and_check_return_val(dstream, tb_false);

	// done
	tb_size_t state = TB_ASTREAM_STATE_UNKNOWN_ERROR;
	do
	{
		// check
		tb_assert_and_check_break(dstream->data && dstream->size);
		
		// init aico
		if (!dstream->aico) dstream->aico = tb_aico_init_task(astream->aicp, tb_false);
		tb_assert_and_check_break(dstream->aico);

		// init head
		dstream->head = dstream->data;

		// opened
		tb_atomic_set(&astream->opened, 1);

		// ok
		state = TB_ASTREAM_STATE_OK;

	} while (0);

	// done func
	if (func) func(astream, state, priv);

	// ok?
	return func? tb_true : ((state == TB_ASTREAM_STATE_OK)? tb_true : tb_false);
}
static tb_bool_t tb_astream_data_read_func(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_RUNTASK, tb_false);

	// the stream
	tb_astream_data_t* dstream = (tb_astream_data_t*)aice->data;
	tb_assert_and_check_return_val(dstream && dstream->func.read.func, tb_false);

	// done state
	tb_size_t state = TB_ASTREAM_STATE_UNKNOWN_ERROR;
	switch (aice->state)
	{
		// ok
	case TB_AICE_STATE_OK:
		{
			// check
			tb_assert_and_check_break(dstream->data && dstream->head);
			
			// the left
			tb_size_t left = dstream->data + dstream->size - dstream->head;

			// the real
			tb_size_t real = dstream->func.read.maxn;
			if (!real || real > left) real = left;

			// no data? closed
			if (!real) 
			{
				state = TB_ASTREAM_STATE_CLOSED;
				break;
			}

			// save data
			tb_byte_t* data = dstream->head;

			// save head
			dstream->head += real;

			// ok
			state = TB_ASTREAM_STATE_OK;

			// done func
			if (dstream->func.read.func((tb_astream_t*)dstream, state, data, real, dstream->func.read.maxn, dstream->func.read.priv))
			{
				// continue to post read
				tb_aico_task_run(aice->aico, 0, tb_astream_data_read_func, (tb_astream_t*)dstream);
			}
		}
		break;
		// killed
	case TB_AICE_STATE_KILLED:
		state = TB_ASTREAM_STATE_KILLED;
		break;
	default:
		tb_trace_impl("read: unknown state: %s", tb_aice_state_cstr(aice));
		break;
	}
 
	// done func
	if (state != TB_ASTREAM_STATE_OK) dstream->func.read.func((tb_astream_t*)dstream, state, tb_null, 0, dstream->func.read.maxn, dstream->func.read.priv);

	// ok
	return tb_true;
}
static tb_bool_t tb_astream_data_read(tb_astream_t* astream, tb_size_t delay, tb_size_t maxn, tb_astream_read_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_data_t* dstream = tb_astream_data_cast(astream);
	tb_assert_and_check_return_val(dstream && dstream->aico && func, tb_false);

	// save func and priv
	dstream->func.read.priv 	= priv;
	dstream->func.read.func 	= func;
	dstream->func.read.maxn 	= maxn;

	// post read
	return tb_aico_task_run(dstream->aico, delay, tb_astream_data_read_func, astream);
}
static tb_bool_t tb_astream_data_writ_func(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_RUNTASK, tb_false);

	// the stream
	tb_astream_data_t* dstream = (tb_astream_data_t*)aice->data;
	tb_assert_and_check_return_val(dstream && dstream->func.writ.func, tb_false);

	// done state
	tb_size_t state = TB_ASTREAM_STATE_UNKNOWN_ERROR;
	switch (aice->state)
	{
		// ok
	case TB_AICE_STATE_OK:
		{
			// check
			tb_assert_and_check_break(dstream->data && dstream->head);
			tb_assert_and_check_break(dstream->func.writ.data && dstream->func.writ.size);
	
			// the left
			tb_size_t left = dstream->data + dstream->size - dstream->head;

			// the real
			tb_size_t real = dstream->func.writ.size;
			if (real > left) real = left;

			// no data? closed
			if (!real) 
			{
				state = TB_ASTREAM_STATE_CLOSED;
				break;
			}

			// save data
			tb_memcpy(dstream->head, dstream->func.writ.data, real);

			// save head
			dstream->head += real;

			// ok
			state = TB_ASTREAM_STATE_OK;

			// done func
			if (dstream->func.writ.func((tb_astream_t*)dstream, state, dstream->func.writ.data, real, dstream->func.writ.size, dstream->func.writ.priv))
			{
				// not finished?
				if (real < dstream->func.writ.size)
				{
					// update data and size
					dstream->func.writ.data += real;
					dstream->func.writ.size -= real;

					// continue to post writ
					tb_aico_task_run(aice->aico, 0, tb_astream_data_writ_func, (tb_astream_t*)dstream);
				}
			}
		}
		break;
		// killed
	case TB_AICE_STATE_KILLED:
		state = TB_ASTREAM_STATE_KILLED;
		break;
	default:
		tb_trace_impl("read: unknown state: %s", tb_aice_state_cstr(aice));
		break;
	}
 
	// done func
	if (state != TB_ASTREAM_STATE_OK) dstream->func.writ.func((tb_astream_t*)dstream, state, dstream->func.writ.data, 0, dstream->func.writ.size, dstream->func.writ.priv);

	// ok
	return tb_true;
}
static tb_bool_t tb_astream_data_writ(tb_astream_t* astream, tb_size_t delay, tb_byte_t const* data, tb_size_t size, tb_astream_writ_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_data_t* dstream = tb_astream_data_cast(astream);
	tb_assert_and_check_return_val(dstream && data && size && func, tb_false);

	// save func and priv
	dstream->func.writ.priv 	= priv;
	dstream->func.writ.func 	= func;
	dstream->func.writ.data 	= data;
	dstream->func.writ.size 	= size;

	// post writ
	return tb_aico_task_run(dstream->aico, delay, tb_astream_data_writ_func, astream);
}
static tb_bool_t tb_astream_data_seek(tb_astream_t* astream, tb_hize_t offset, tb_astream_seek_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_data_t* dstream = tb_astream_data_cast(astream);
	tb_assert_and_check_return_val(dstream && func, tb_false);

	// done
	tb_size_t state = TB_ASTREAM_STATE_UNKNOWN_ERROR;
	do
	{
		// check
		tb_assert_and_check_break(dstream->data && offset <= dstream->size);

		// seek 
		dstream->head = dstream->data + offset;

		// ok
		state = TB_ASTREAM_STATE_OK;

	} while (0);

	// done func
	func(astream, state, offset, priv);

	// ok
	return tb_true;
}
static tb_bool_t tb_astream_data_task_func(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_RUNTASK, tb_false);

	// the stream
	tb_astream_data_t* dstream = (tb_astream_data_t*)aice->data;
	tb_assert_and_check_return_val(dstream && dstream->func.task.func, tb_false);

	// done func
	tb_bool_t ok = dstream->func.task.func((tb_astream_t*)dstream, aice->state == TB_AICE_STATE_OK? TB_ASTREAM_STATE_OK : TB_ASTREAM_STATE_UNKNOWN_ERROR, dstream->func.task.priv);

	// ok and continue?
	if (ok && aice->state == TB_AICE_STATE_OK)
	{
		// post task
		tb_aico_task_run(aice->aico, aice->u.runtask.delay, tb_astream_data_task_func, dstream);
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_astream_data_task(tb_astream_t* astream, tb_size_t delay, tb_astream_task_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_data_t* dstream = tb_astream_data_cast(astream);
	tb_assert_and_check_return_val(dstream && dstream->aico && func, tb_false);

	// save func and priv
	dstream->func.task.priv 	= priv;
	dstream->func.task.func 	= func;

	// post task
	return tb_aico_task_run(dstream->aico, delay, tb_astream_data_task_func, astream);
}
static tb_void_t tb_astream_data_kill(tb_astream_t* astream)
{	
	// check
	tb_astream_data_t* dstream = tb_astream_data_cast(astream);
	tb_assert_and_check_return(dstream);

	// kill the task aico
	if (dstream->aico) tb_aico_kill(dstream->aico);
}
static tb_void_t tb_astream_data_clos(tb_astream_t* astream, tb_bool_t bcalling)
{	
	// check
	tb_astream_data_t* dstream = tb_astream_data_cast(astream);
	tb_assert_and_check_return(dstream);

	// exit aico
	if (dstream->aico) tb_aico_exit(dstream->aico, bcalling);
	dstream->aico = tb_null;

	// clear head
	dstream->head = tb_null;
}
static tb_void_t tb_astream_data_exit(tb_astream_t* astream, tb_bool_t bcalling)
{	
	// check
	tb_astream_data_t* dstream = tb_astream_data_cast(astream);
	tb_assert_and_check_return(dstream);

	// exit aico
	if (dstream->aico) tb_aico_exit(dstream->aico, bcalling);
	dstream->aico = tb_null;

	// clear head
	dstream->head = tb_null;

	// exit data
	if (dstream->data && !dstream->bref) tb_free(dstream->data);
	dstream->data = tb_null;
	dstream->size = 0;
}
static tb_bool_t tb_astream_data_ctrl(tb_astream_t* astream, tb_size_t ctrl, tb_va_list_t args)
{
	// check
	tb_astream_data_t* dstream = tb_astream_data_cast(astream);
	tb_assert_and_check_return_val(dstream, tb_false);

	// ctrl
	switch (ctrl)
	{
	case TB_ASTREAM_CTRL_GET_SIZE:
		{
			// the psize
			tb_hize_t* psize = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
			tb_assert_and_check_return_val(psize, tb_false);

			// get size
			*psize = dstream->size;
			return tb_true;
		}
	case TB_ASTREAM_CTRL_GET_OFFSET:
		{
			// check
			tb_assert_and_check_return_val(dstream->data && dstream->size, tb_false);
			tb_assert_and_check_return_val(tb_atomic_get(&astream->opened) && dstream->head, tb_false);

			// the poffset
			tb_hong_t* poffset = (tb_hong_t*)tb_va_arg(args, tb_hong_t*);
			tb_assert_and_check_return_val(poffset, tb_false);

			// get offset
			*poffset = dstream->head - dstream->data;
			return tb_true;
		}
	case TB_ASTREAM_CTRL_DATA_SET_DATA:
		{
			// exit data first if exists
			if (dstream->data && !dstream->bref) tb_free(dstream->data);

			// save data
			dstream->data = (tb_byte_t*)tb_va_arg(args, tb_byte_t*);
			dstream->size = (tb_size_t)tb_va_arg(args, tb_size_t);
			dstream->head = tb_null;
			dstream->bref = tb_true;

			// check
			tb_assert_and_check_return_val(dstream->data && dstream->size, tb_false);
			return tb_true;
		}
	case TB_ASTREAM_CTRL_SET_URL:
		{
			// check
			tb_assert_and_check_return_val(!tb_atomic_get(&astream->opened), tb_false);

			// set url
			tb_char_t const* url = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			tb_assert_and_check_return_val(url, tb_false); 
			
			// the url size
			tb_size_t url_size = tb_strlen(url);
			tb_assert_and_check_return_val(url_size > 7, tb_false);

			// the base64 data and size
			tb_char_t const* 	base64_data = url + 7;
			tb_size_t 			base64_size = url_size - 7;

			// make data
			tb_size_t 	maxn = base64_size;
			tb_byte_t* 	data = tb_malloc(maxn); 
			tb_assert_and_check_return_val(data, tb_false);

			// decode base64 data
			tb_size_t 	size = tb_base64_decode(base64_data, base64_size, data, maxn);
			tb_assert_and_check_return_val(size, tb_false);

			// exit data first if exists
			if (dstream->data && !dstream->bref) tb_free(dstream->data);

			// save data
			dstream->data = data;
			dstream->size = size;
			dstream->bref = tb_false;
			dstream->head = tb_null;

			// ok
			return tb_true;
		}
		break;
	default:
		break;
	}
	return tb_false;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_astream_t* tb_astream_init_data(tb_aicp_t* aicp)
{
	// check
	tb_assert_and_check_return_val(aicp, tb_null);

	// make stream
	tb_astream_data_t* dstream = (tb_astream_data_t*)tb_malloc0(sizeof(tb_astream_data_t));
	tb_assert_and_check_return_val(dstream, tb_null);

	// init stream
	if (!tb_astream_init((tb_astream_t*)dstream, aicp, TB_ASTREAM_TYPE_DATA)) goto fail;
	dstream->base.open 		= tb_astream_data_open;
	dstream->base.read 		= tb_astream_data_read;
	dstream->base.writ 		= tb_astream_data_writ;
	dstream->base.seek 		= tb_astream_data_seek;
	dstream->base.task 		= tb_astream_data_task;
	dstream->base.kill 		= tb_astream_data_kill;
	dstream->base.clos 		= tb_astream_data_clos;
	dstream->base.exit 		= tb_astream_data_exit;
	dstream->base.ctrl 		= tb_astream_data_ctrl;

	// ok
	return (tb_astream_t*)dstream;

fail:
	if (dstream) tb_astream_exit((tb_astream_t*)dstream, tb_false);
	return tb_null;
}
tb_astream_t* tb_astream_init_from_data(tb_aicp_t* aicp, tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(aicp && data && size, tb_null);

	// init data stream
	tb_astream_t* dstream = tb_astream_init_data(aicp);
	tb_assert_and_check_return_val(dstream, tb_null);

	// set data
	if (!tb_astream_ctrl(dstream, TB_ASTREAM_CTRL_DATA_SET_DATA, data, size)) goto fail;
	
	// ok
	return dstream;
fail:
	if (dstream) tb_astream_exit(dstream, tb_false);
	return tb_null;
}
