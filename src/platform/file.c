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
 * @file		file.c
 * @ingroup 	platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "file.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef TB_CONFIG_OS_WINDOWS
# 	include "windows/file.c"
#elif defined(TB_CONFIG_API_HAVE_POSIX)
# 	include "posix/file.c"
#else
tb_handle_t tb_file_init(tb_char_t const* path, tb_size_t mode)
{
	tb_trace_noimpl();
	return tb_null;
}
tb_bool_t tb_file_exit(tb_handle_t file)
{
	tb_trace_noimpl();
	return tb_false;
}
tb_long_t tb_file_read(tb_handle_t file, tb_byte_t* data, tb_size_t size)
{
	tb_trace_noimpl();
	return -1;
}
tb_long_t tb_file_writ(tb_handle_t file, tb_byte_t const* data, tb_size_t size)
{
	tb_trace_noimpl();
	return -1;
}
tb_long_t tb_file_pread(tb_handle_t file, tb_byte_t* data, tb_size_t size, tb_hize_t offset)
{
	tb_trace_noimpl();
	return -1;
}
tb_long_t tb_file_pwrit(tb_handle_t file, tb_byte_t const* data, tb_size_t size, tb_hize_t offset)
{
	tb_trace_noimpl();
	return -1;
}
tb_long_t tb_file_readv(tb_handle_t file, tb_iovec_t const* list, tb_size_t size)
{
	tb_trace_noimpl();
	return -1;
}
tb_long_t tb_file_writv(tb_handle_t file, tb_iovec_t const* list, tb_size_t size)
{
	tb_trace_noimpl();
	return -1;
}
tb_hong_t tb_file_writf(tb_handle_t file, tb_handle_t ifile, tb_hize_t offset, tb_hize_t size)
{
	tb_trace_noimpl();
	return -1;
}
tb_long_t tb_file_preadv(tb_handle_t file, tb_iovec_t const* list, tb_size_t size, tb_hize_t offset)
{
	tb_trace_noimpl();
	return -1;
}
tb_long_t tb_file_pwritv(tb_handle_t file, tb_iovec_t const* list, tb_size_t size, tb_hize_t offset)
{
	tb_trace_noimpl();
	return -1;
}
tb_bool_t tb_file_sync(tb_handle_t file)
{
	tb_trace_noimpl();
	return tb_false;
}
tb_hong_t tb_file_seek(tb_handle_t file, tb_hong_t offset, tb_size_t mode)
{
	tb_trace_noimpl();
	return -1;
}
tb_hong_t tb_file_offset(tb_handle_t file)
{
	tb_trace_noimpl();
	return -1;
}
tb_hize_t tb_file_size(tb_handle_t file)
{
	tb_trace_noimpl();
	return 0;
}
tb_bool_t tb_file_info(tb_char_t const* path, tb_file_info_t* info)
{
	tb_trace_noimpl();
	return tb_false;
}
tb_bool_t tb_file_copy(tb_char_t const* path, tb_char_t const* dest)
{
	tb_trace_noimpl();
	return tb_false;
}
tb_bool_t tb_file_create(tb_char_t const* path)
{
	tb_trace_noimpl();
	return tb_false;
}
tb_bool_t tb_file_remove(tb_char_t const* path)
{
	tb_trace_noimpl();
	return tb_false;
}
tb_bool_t tb_file_rename(tb_char_t const* path, tb_char_t const* dest)
{
	tb_trace_noimpl();
	return tb_false;
}
tb_bool_t tb_file_link(tb_char_t const* path, tb_char_t const* dest)
{
	tb_trace_noimpl();
	return tb_false;
}
#endif
