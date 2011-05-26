/*!The Tiny Platform Library
 * 
 * TPlat is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TPlat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TPlat; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
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
#define TB_FILENAME_MAX_SIZE 	(1024)

/* /////////////////////////////////////////////////////////
 * types
 */
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

typedef enum __tb_file_seek_flag_t
{
 	TB_FILE_SEEK_BEG 	= 0
, 	TB_FILE_SEEK_CUR 	= 1
, 	TB_FILE_SEEK_END 	= 2
, 	TB_FILE_SEEK_SIZE 	= 3

}tb_file_seek_flag_t;

typedef enum __tb_file_type_t
{
	TB_FILE_TYPE_IS_DIR 	= 1 	//!< directory
, 	TB_FILE_TYPE_IS_FILE 	= 2 	//!< file
, 	TB_FILE_TYPE_IS_DOT 	= 3 	//!< .
, 	TB_FILE_TYPE_IS_DOT2 	= 4 	//!< ..

}tb_file_type_t;

typedef struct __tb_file_entry_t
{
	tb_file_type_t 	type;
	tb_char_t 		name[TB_FILENAME_MAX_SIZE];
	tb_char_t 		path[TB_FILENAME_MAX_SIZE];
	tb_size_t 		namesize;

}tb_file_entry_t;

/* /////////////////////////////////////////////////////////
 * file interfaces
 */

// open & close
tb_handle_t 			tb_file_open(tb_char_t const* filename, tb_int_t flags);
void 					tb_file_close(tb_handle_t hfile);

/* read data
 *
 * return: 
 * > 0: real sise
 * == 0: null
 * < 0: failure
 */
tb_int_t 				tb_file_read(tb_handle_t hfile, tb_byte_t* data, tb_int_t read_n);

/* write data
 *
 * return: 
 * > 0: real sise
 * == 0: null
 * < 0: failure
 */
tb_int_t 				tb_file_write(tb_handle_t hfile, tb_byte_t const* data, tb_int_t write_n);

// flush file buffer
void 					tb_file_flush(tb_handle_t hfile);

// seek
// flags == TB_FILE_SEEK_SIZE: return file_size
// return: -1 => fail, other => new offset
tb_int64_t 				tb_file_seek(tb_handle_t hfile, tb_int64_t offset, tb_int_t flags);

// delete file or directory
tb_bool_t 				tb_file_delete(tb_char_t const* path, tb_file_type_t type);

// get the size of file or directory 
tb_size_t 				tb_file_size(tb_char_t const* path, tb_file_type_t type);

// create file or directory
tb_bool_t 				tb_file_create(tb_char_t const* path, tb_file_type_t type);

/* /////////////////////////////////////////////////////////
 * file list interfaces
 */
// open file list
tb_handle_t 			tb_file_list_open(tb_char_t const* dir);

// get file list entry, end: return NULL
tb_file_entry_t const* 	tb_file_list_entry(tb_handle_t hflist);

// close file list
void 					tb_file_list_close(tb_handle_t hflist);

// c plus plus
#ifdef __cplusplus
}
#endif

#endif
