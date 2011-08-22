/*!The Tiny Platform Library
 * 
 * TPlat is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TPlat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TPlat; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		file.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"


/* /////////////////////////////////////////////////////////
 * implemention
 */

// file
tb_handle_t tb_file_open(tb_char_t const* filename, tb_int_t flags)
{
	TB_NOT_IMPLEMENT();
	return TB_NULL;
}
tb_void_t tb_file_close(tb_handle_t hfile)
{
	TB_NOT_IMPLEMENT();
}
tb_int_t tb_file_read(tb_handle_t hfile, tb_byte_t* data, tb_int_t read_n)
{
	TB_NOT_IMPLEMENT();
	return -1;
}
tb_int_t tb_file_write(tb_handle_t hfile, tb_byte_t const* data, tb_int_t write_n)
{
	TB_NOT_IMPLEMENT();
	return -1;
}
tb_void_t tb_file_flush(tb_handle_t hfile)
{
	TB_NOT_IMPLEMENT();
}
tb_int_t tb_file_seek(tb_handle_t hfile, tb_int_t offset, tb_int_t flags)
{
	TB_NOT_IMPLEMENT();
	return -1;
}
tb_size_t tb_file_size(tb_char_t const* path, tb_file_type_t type)
{
	TB_NOT_IMPLEMENT();
	return 0;
}
tb_bool_t tb_file_exists(tb_char_t const* path)
{
	TB_NOT_IMPLEMENT();
	return TB_FALSE;
}
tb_bool_t tb_file_create(tb_char_t const* path, tb_file_type_t type)
{
	TB_NOT_IMPLEMENT();
	return TB_FALSE;
}
tb_bool_t tb_file_delete(tb_char_t const* path, tb_file_type_t type)
{
	TB_NOT_IMPLEMENT();
	return TB_FALSE;
}

// open file list
tb_handle_t tb_file_list_open(tb_char_t const* dir)
{
	TB_NOT_IMPLEMENT();
	return TB_NULL;
}

// get file list entry, end: return NULL
tb_file_entry_t const* tb_file_list_entry(tb_handle_t hflist)
{
	TB_NOT_IMPLEMENT();
	return TB_NULL;
}

// close file list
tb_void_t tb_file_list_close(tb_handle_t hflist)
{
	TB_NOT_IMPLEMENT();
}

