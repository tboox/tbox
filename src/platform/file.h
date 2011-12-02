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
 * \file		file.h
 *
 */
#ifndef TB_PLATFORM_FILE_H
#define TB_PLATFORM_FILE_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif


/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * macros
 */
#define TB_FILENAME_MAX_SIZE 	(4096)

/* /////////////////////////////////////////////////////////
 * types
 */

// the file flag type
typedef enum __tb_file_flag_t
{
	TB_FILE_RO 		= 1
, 	TB_FILE_WO 		= 2
, 	TB_FILE_RW 		= 4
, 	TB_FILE_CREAT 	= 8
, 	TB_FILE_APPEND 	= 16
, 	TB_FILE_TRUNC 	= 32
, 	TB_FILE_BINARY 	= 64

}tb_file_flag_t;

// the file seek type
typedef enum __tb_file_seek_flag_t
{
 	TB_FILE_SEEK_BEG 	= 0
, 	TB_FILE_SEEK_CUR 	= 1
, 	TB_FILE_SEEK_END 	= 2

}tb_file_seek_flag_t;

// the file type
typedef enum __tb_file_type_t
{
	TB_FILE_TYPE_DIR 	= 1
, 	TB_FILE_TYPE_FILE 	= 2 
, 	TB_FILE_TYPE_DOT 	= 3
, 	TB_FILE_TYPE_DOT2 	= 4

}tb_file_type_t;

// the file info type
typedef struct __tb_file_info_t
{
	// the file type
	tb_size_t 			type;

	// the file size
	tb_uint64_t 		size;

}tb_file_info_t;

/* /////////////////////////////////////////////////////////
 * file interfaces
 */

// open & close
tb_handle_t 			tb_file_open(tb_char_t const* path, tb_size_t flags);
tb_void_t 				tb_file_close(tb_handle_t hfile);

// file operations 
tb_long_t 				tb_file_read(tb_handle_t hfile, tb_byte_t* data, tb_size_t size);
tb_long_t 				tb_file_writ(tb_handle_t hfile, tb_byte_t const* data, tb_size_t size);
tb_int64_t 				tb_file_seek(tb_handle_t hfile, tb_int64_t offset, tb_size_t flags);
tb_uint64_t 			tb_file_size(tb_handle_t hfile);
tb_void_t 				tb_file_flush(tb_handle_t hfile);

// file modifiors
tb_bool_t 				tb_file_create(tb_char_t const* path, tb_file_type_t type);
tb_void_t 				tb_file_delete(tb_char_t const* path, tb_file_type_t type);

// file attributes
tb_bool_t 				tb_file_info(tb_char_t const* path, tb_file_info_t* info);

// c plus plus
#ifdef __cplusplus
}
#endif

#endif
