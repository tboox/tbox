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
 * includes
 */
#include "prefix.h"
#include "../../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the file stream type
typedef struct __tb_astream_file_t
{
	// the base
	tb_astream_t 		base;

	// the file handle
	tb_handle_t 		file;

	// the file mode
	tb_size_t 			mode;

}tb_astream_file_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_long_t tb_astream_file_open(tb_astream_t* ast, tb_astream_open_func_t func, tb_pointer_t priv)
{
	return -1;
}
static tb_long_t tb_astream_file_read(tb_astream_t* ast, tb_astream_read_func_t func, tb_pointer_t priv)
{
	return -1;
}
static tb_long_t tb_astream_file_writ(tb_astream_t* ast, tb_byte_t const* data, tb_size_t size, tb_astream_writ_func_t func, tb_pointer_t priv)
{
	return -1;
}
static tb_long_t tb_astream_file_save(tb_astream_t* ast, tb_astream_t* ost, tb_astream_save_func_t func, tb_pointer_t priv)
{
	return -1;
}
static tb_long_t tb_astream_file_seek(tb_astream_t* ast, tb_hize_t offset, tb_astream_seek_func_t func, tb_pointer_t priv)
{
	return -1;
}
static tb_long_t tb_astream_file_sync(tb_astream_t* ast, tb_astream_sync_func_t func, tb_pointer_t priv)
{
	return -1;
}
static tb_void_t tb_astream_file_kill(tb_astream_t* ast)
{
}
static tb_void_t tb_astream_file_exit(tb_astream_t* ast)
{
}
static tb_bool_t tb_astream_file_ctrl(tb_astream_t* ast, tb_size_t ctrl, tb_va_list_t args)
{
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
	ast->base.open 	= tb_astream_file_open;
	ast->base.read 	= tb_astream_file_read;
	ast->base.writ 	= tb_astream_file_writ;
	ast->base.save 	= tb_astream_file_save;
	ast->base.seek 	= tb_astream_file_seek;
	ast->base.sync 	= tb_astream_file_sync;
	ast->base.kill 	= tb_astream_file_kill;
	ast->base.exit 	= tb_astream_file_exit;
	ast->base.ctrl 	= tb_astream_file_ctrl;
	ast->mode 		= TB_FILE_MODE_RO | TB_FILE_MODE_BINARY;

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
