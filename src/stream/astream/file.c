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
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
//#define TB_TRACE_IMPL_TAG 				"afile"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// the file read maxn
#define TB_ASTREAM_FILE_READ_MAXN 			TB_FILE_DIRECT_CSIZE

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the file stream type
typedef struct __tb_astream_file_t
{
	// the base
	tb_astream_t 				base;

	// the file handle
	tb_handle_t 				file;

	// the aico
	tb_handle_t 				aico;

	// the file handle is referenced? need not exit it
	tb_bool_t 					bref;

	// the file mode
	tb_size_t 					mode;

	// the file offset
	tb_atomic64_t 				offset;

	// the file data
	tb_byte_t 					data[TB_ASTREAM_FILE_READ_MAXN];

	// the func
	union
	{
		tb_astream_read_func_t 	read;
		tb_astream_writ_func_t 	writ;
		tb_astream_sync_func_t 	sync;

	} 							func;

	// the priv
	tb_pointer_t 				priv;

}tb_astream_file_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_astream_file_t* tb_astream_file_cast(tb_astream_t* ast)
{
	tb_assert_and_check_return_val(ast && ast->type == TB_ASTREAM_TYPE_FILE, tb_null);
	return (tb_astream_file_t*)ast;
}
static tb_bool_t tb_astream_file_try_open(tb_astream_t* ast)
{
	// check
	tb_astream_file_t* fst = tb_astream_file_cast(ast);
	tb_assert_and_check_return_val(fst, tb_false);

	// done
	tb_bool_t ok = tb_false;
	do
	{
		// init file
		if (!fst->file)
		{
			// the url
			tb_char_t const* url = tb_url_get(&ast->url);
			tb_assert_and_check_break(url);

			// open file
			fst->file = tb_file_init(url, fst->mode);
			fst->bref = tb_false;
		}
		tb_check_break(fst->file);

		// addo file
		fst->aico = tb_aico_init_file(ast->aicp, fst->file);
		tb_assert_and_check_break(fst->aico);

		// init offset
		fst->offset = 0;

		// opened
		tb_atomic_set(&ast->opened, 1);

		// ok
		ok = tb_true;

	} while (0);

	// ok?
	return ok;
}
static tb_bool_t tb_astream_file_open(tb_astream_t* ast, tb_astream_open_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_file_t* fst = tb_astream_file_cast(ast);
	tb_assert_and_check_return_val(fst && func, tb_false);

	// open it
	tb_size_t state = tb_astream_file_try_open(ast)? TB_ASTREAM_STATE_OK : TB_ASTREAM_STATE_UNKNOWN_ERROR;

	// done func
	func(ast, state, priv);

	// ok
	return tb_true;
}
static tb_bool_t tb_astream_file_read_func(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_READ, tb_false);

	// the stream
	tb_astream_file_t* fst = (tb_astream_file_t*)aice->data;
	tb_assert_and_check_return_val(fst && fst->func.read, tb_false);

	// done state
	tb_size_t state = TB_ASTREAM_STATE_UNKNOWN_ERROR;
	switch (aice->state)
	{
		// ok
	case TB_AICE_STATE_OK:
		tb_assert_and_check_break(aice->u.read.real && aice->u.read.real <= sizeof(fst->data));
		tb_atomic64_fetch_and_add(&fst->offset, aice->u.read.real);
		state = TB_ASTREAM_STATE_OK;
		break;
		// closed
	case TB_AICE_STATE_CLOSED:
		state = TB_ASTREAM_STATE_CLOSED;
		break;
	default:
		tb_trace_impl("read: unknown state: %s", tb_aice_state_cstr(aice));
		break;
	}

	// done func
	if (fst->func.read((tb_astream_t*)fst, state, aice->u.read.data, aice->u.read.real, fst->priv))
	{
		// continue?
		if (aice->state == TB_AICE_STATE_OK)
		{
			// continue to post read
			tb_aico_read(aice->aico, (tb_hize_t)tb_atomic64_get(&fst->offset), fst->data, sizeof(fst->data), tb_astream_file_read_func, (tb_astream_t*)fst);
		}
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_astream_file_read(tb_astream_t* ast, tb_astream_read_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_file_t* fst = tb_astream_file_cast(ast);
	tb_assert_and_check_return_val(fst && fst->file && fst->aico && func, tb_false);

	// save func and priv
	fst->priv 		= priv;
	fst->func.read 	= func;

	// post read
	return tb_aico_read(fst->aico, (tb_hize_t)tb_atomic64_get(&fst->offset), fst->data, sizeof(fst->data), tb_astream_file_read_func, ast);
}
static tb_bool_t tb_astream_file_writ_func(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_WRIT, tb_false);

	// the stream
	tb_astream_file_t* fst = (tb_astream_file_t*)aice->data;
	tb_assert_and_check_return_val(fst && fst->func.writ, tb_false);

	// done state
	tb_size_t state = TB_ASTREAM_STATE_UNKNOWN_ERROR;
	switch (aice->state)
	{
		// ok
	case TB_AICE_STATE_OK:
		tb_assert_and_check_break(aice->u.writ.data && aice->u.writ.real && aice->u.writ.real <= aice->u.writ.size);
		tb_atomic64_fetch_and_add(&fst->offset, aice->u.writ.real);
		state = TB_ASTREAM_STATE_OK;
		break;
		// closed
	case TB_AICE_STATE_CLOSED:
		state = TB_ASTREAM_STATE_CLOSED;
		break;
	default:
		tb_trace_impl("writ: unknown state: %s", tb_aice_state_cstr(aice));
		break;
	}

	// done func
	if (fst->func.writ((tb_astream_t*)fst, state, aice->u.writ.real, aice->u.writ.size, fst->priv))
	{
		// continue?
		if (aice->state == TB_AICE_STATE_OK && aice->u.writ.real < aice->u.writ.size)
		{
			// continue to post writ
			tb_aico_writ(aice->aico, (tb_hize_t)tb_atomic64_get(&fst->offset), aice->u.writ.data + aice->u.writ.real, aice->u.writ.size - aice->u.writ.real, tb_astream_file_writ_func, (tb_astream_t*)fst);
		}
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_astream_file_writ(tb_astream_t* ast, tb_byte_t const* data, tb_size_t size, tb_astream_writ_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_file_t* fst = tb_astream_file_cast(ast);
	tb_assert_and_check_return_val(fst && fst->file && fst->aico && data && size && func, tb_false);

	// save func and priv
	fst->priv 		= priv;
	fst->func.writ 	= func;

	// post writ
	return tb_aico_writ(fst->aico, (tb_hize_t)tb_atomic64_get(&fst->offset), data, size, tb_astream_file_writ_func, ast);
}
static tb_bool_t tb_astream_file_save(tb_astream_t* ast, tb_astream_t* ost, tb_astream_save_func_t func, tb_pointer_t priv)
{
	return tb_false;
}
static tb_bool_t tb_astream_file_try_seek(tb_astream_t* ast, tb_hize_t offset)
{
	// check
	tb_astream_file_t* fst = tb_astream_file_cast(ast);
	tb_assert_and_check_return_val(fst, tb_false);

	// done
	tb_bool_t ok = tb_false;
	do
	{
		// check
		tb_assert_and_check_break(fst->file);

		// update offset
		tb_atomic64_set(&fst->offset, offset);

		// ok
		ok = tb_true;

	} while (0);

	// ok?
	return ok;
}
static tb_bool_t tb_astream_file_seek(tb_astream_t* ast, tb_hize_t offset, tb_astream_seek_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_file_t* fst = tb_astream_file_cast(ast);
	tb_assert_and_check_return_val(fst && func, tb_false);

	// open it
	tb_size_t state = tb_astream_file_try_seek(ast, offset)? TB_ASTREAM_STATE_OK : TB_ASTREAM_STATE_UNKNOWN_ERROR;

	// done func
	func(ast, state, priv);

	// ok
	return tb_true;
}
static tb_bool_t tb_astream_file_sync_func(tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_FSYNC, tb_false);

	// the stream
	tb_astream_file_t* fst = (tb_astream_file_t*)aice->data;
	tb_assert_and_check_return_val(fst && fst->func.sync, tb_false);

	// done func
	fst->func.sync((tb_astream_t*)fst, aice->state == TB_AICE_STATE_OK? TB_ASTREAM_STATE_OK : TB_ASTREAM_STATE_UNKNOWN_ERROR, fst->priv);

	// ok
	return tb_true;
}
static tb_bool_t tb_astream_file_sync(tb_astream_t* ast, tb_astream_sync_func_t func, tb_pointer_t priv)
{
	// check
	tb_astream_file_t* fst = tb_astream_file_cast(ast);
	tb_assert_and_check_return_val(fst && fst->file && fst->aico && func, tb_false);

	// save func and priv
	fst->priv 		= priv;
	fst->func.sync 	= func;

	// post sync
	return tb_aico_fsync(fst->aico, tb_astream_file_sync_func, ast);
}
static tb_void_t tb_astream_file_kill(tb_astream_t* ast)
{	
	// check
	tb_astream_file_t* fst = tb_astream_file_cast(ast);
	tb_assert_and_check_return(fst);

	// is pending?
	if (fst->aico && tb_aico_pending(fst->aico))
	{
		// kill it
		if (!fst->bref && fst->file) tb_file_exit(fst->file);
	}
}
static tb_void_t tb_astream_file_exit(tb_astream_t* ast)
{	
	// check
	tb_astream_file_t* fst = tb_astream_file_cast(ast);
	tb_assert_and_check_return(fst);

	// exit aico
	if (fst->aico) tb_aico_exit(fst->aico);
	fst->aico = tb_null;

	// exit offset
	fst->offset = 0;

	// exit it
	if (!fst->bref && fst->file) tb_file_exit(fst->file);
	fst->file = tb_null;
	fst->bref = tb_false;
}
static tb_bool_t tb_astream_file_ctrl(tb_astream_t* ast, tb_size_t ctrl, tb_va_list_t args)
{
	// check
	tb_astream_file_t* fst = tb_astream_file_cast(ast);
	tb_assert_and_check_return_val(fst, tb_false);

	// ctrl
	switch (ctrl)
	{
	case TB_ASTREAM_CTRL_GET_SIZE:
		{
			// check
			tb_assert_and_check_return_val(tb_atomic_get(&ast->opened) && fst->file, tb_false);

			// get size
			tb_hize_t* psize = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
			tb_assert_and_check_return_val(psize, tb_false);
			*psize = tb_file_size(fst->file);
			return tb_true;
		}
	case TB_ASTREAM_CTRL_GET_OFFSET:
		{
			// check
			tb_assert_and_check_return_val(tb_atomic_get(&ast->opened) && fst->file, tb_false);

			// get offset
			tb_hong_t* poffset = (tb_hong_t*)tb_va_arg(args, tb_hong_t*);
			tb_assert_and_check_return_val(poffset, tb_false);
			*poffset = (tb_hize_t)tb_atomic64_get(&fst->offset);
			return tb_true;
		}
	case TB_ASTREAM_CTRL_FILE_SET_MODE:
		{
			// check
			tb_assert_and_check_return_val(!tb_atomic_get(&ast->opened), tb_false);

			// set mode
			fst->mode = (tb_size_t)tb_va_arg(args, tb_size_t);
			return tb_true;
		}
	case TB_ASTREAM_CTRL_FILE_SET_HANDLE:
		{
			// check
			tb_assert_and_check_return_val(!tb_atomic_get(&ast->opened), tb_false);

			// set handle
			tb_handle_t handle = (tb_handle_t)tb_va_arg(args, tb_handle_t);
			fst->file = handle;
			fst->bref = handle? tb_true : tb_false;
			return tb_true;
		}
	case TB_ASTREAM_CTRL_FILE_GET_HANDLE:
		{
			// get handle
			tb_handle_t* phandle = (tb_handle_t*)tb_va_arg(args, tb_handle_t*);
			tb_assert_and_check_return_val(phandle, tb_false);
			*phandle = fst->file;
			return tb_true;
		}
	default:
		break;
	}
	return tb_false;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_astream_t* tb_astream_init_file(tb_aicp_t* aicp)
{
	// check
	tb_assert_and_check_return_val(aicp, tb_null);

	// make stream
	tb_astream_file_t* ast = (tb_astream_file_t*)tb_malloc0(sizeof(tb_astream_file_t));
	tb_assert_and_check_return_val(ast, tb_null);

	// init stream
	if (!tb_astream_init((tb_astream_t*)ast, aicp, TB_ASTREAM_TYPE_FILE)) goto fail;
	ast->base.open 		= tb_astream_file_open;
	ast->base.read 		= tb_astream_file_read;
	ast->base.writ 		= tb_astream_file_writ;
	ast->base.save 		= tb_astream_file_save;
	ast->base.seek 		= tb_astream_file_seek;
	ast->base.sync 		= tb_astream_file_sync;
	ast->base.kill 		= tb_astream_file_kill;
	ast->base.exit 		= tb_astream_file_exit;
	ast->base.ctrl 		= tb_astream_file_ctrl;
	ast->base.try_open 	= tb_astream_file_try_open;
	ast->base.try_seek 	= tb_astream_file_try_seek;
	ast->mode 			= TB_FILE_MODE_RO | TB_FILE_MODE_BINARY;

	// ok
	return (tb_astream_t*)ast;

fail:
	if (ast) tb_astream_exit((tb_astream_t*)ast);
	return tb_null;
}
tb_astream_t* tb_astream_init_from_file(tb_aicp_t* aicp, tb_char_t const* path)
{
	// check
	tb_assert_and_check_return_val(aicp && path, tb_null);

	// init file stream
	tb_astream_t* ast = tb_astream_init_file(aicp);
	tb_assert_and_check_return_val(ast, tb_null);

	// set path
	if (!tb_astream_ctrl(ast, TB_ASTREAM_CTRL_SET_URL, path)) goto fail;
	
	// ok
	return ast;
fail:
	if (ast) tb_astream_exit(ast);
	return tb_null;
}
