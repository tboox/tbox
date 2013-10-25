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
 * @file		file.h
 * @ingroup 	platform
 *
 */
#ifndef TB_PLATFORM_FILE_H
#define TB_PLATFORM_FILE_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/// the file mode type
typedef enum __tb_file_mode_t
{
	TB_FILE_MODE_RO 		= 1 	//!< read only
, 	TB_FILE_MODE_WO 		= 2 	//!< writ only
, 	TB_FILE_MODE_RW 		= 4 	//!< read and writ
, 	TB_FILE_MODE_CREAT 		= 8 	//!< create
, 	TB_FILE_MODE_APPEND 	= 16 	//!< append
, 	TB_FILE_MODE_TRUNC 		= 32 	//!< truncate
, 	TB_FILE_MODE_BINARY 	= 64 	//!< binary
, 	TB_FILE_MODE_AICP 		= 128 	//!< support for aicp

}tb_file_mode_t;

/// the file seek type
typedef enum __tb_file_seek_flag_t
{
 	TB_FILE_SEEK_BEG 		= 0
, 	TB_FILE_SEEK_CUR 		= 1
, 	TB_FILE_SEEK_END 		= 2

}tb_file_seek_flag_t;

/// the file type
typedef enum __tb_file_type_t
{
	TB_FILE_TYPE_NONE 		= 0
,	TB_FILE_TYPE_DIRECTORY 	= 1
, 	TB_FILE_TYPE_FILE 		= 2 
, 	TB_FILE_TYPE_DOT 		= 3
, 	TB_FILE_TYPE_DOT2 		= 4

}tb_file_type_t;

/// the file info type
typedef struct __tb_file_info_t
{
	/// the file type
	tb_size_t 				type;

	/// the file size
	tb_hize_t 				size;

	/// the last access time
	tb_time_t 				atime;

	/// the last modify time
	tb_time_t 				mtime;

}tb_file_info_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init the file
 * 
 * @param path 			the file path
 * @param mode 			the file mode
 *
 * @return 				the file handle
 */
tb_handle_t 			tb_file_init(tb_char_t const* path, tb_size_t mode);

/*! the file native handle
 * 
 * @param file 			the file handle
 *
 * @return 				the file native handle
 */
tb_handle_t 			tb_file_native(tb_handle_t file);

/*! exit the file
 * 
 * @param file 			the file handle
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_file_exit(tb_handle_t file);

/*! read the file data
 * 
 * @param file 			the file handle
 * @param data 			the data
 * @param size 			the size
 *
 * @return 				the real size or -1
 */
tb_long_t 				tb_file_read(tb_handle_t file, tb_byte_t* data, tb_size_t size);

/*! writ the file data
 * 
 * @param file 			the file handle
 * @param data 			the data
 * @param size 			the size
 *
 * @return 				the real size or -1
 */
tb_long_t 				tb_file_writ(tb_handle_t file, tb_byte_t const* data, tb_size_t size);

/*! seek the file offset
 * 
 * @param file 			the file handle
 * @param offset 		the file offset
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_file_seek(tb_handle_t file, tb_hize_t offset);

/*! skip the file size
 * 
 * @param file 			the file handle
 * @param size 			the file size
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_file_skip(tb_handle_t file, tb_hize_t size);

/*! sync the file data
 * 
 * @param file 			the file handle
 */
tb_void_t 				tb_file_sync(tb_handle_t file);

/*! the file size
 * 
 * @param file 			the file handle
 *
 * @return 				the file size
 */
tb_hize_t 				tb_file_size(tb_handle_t file);

/*! the file info for file or directory
 * 
 * @param file 			the file handle
 * @param info 			the file info
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_file_info(tb_char_t const* path, tb_file_info_t* info);

/*! copy the file
 * 
 * @param path 			the file path
 * @param dest 			the dest path
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_file_copy(tb_char_t const* path, tb_char_t const* dest);

/*! create the file
 * 
 * @param path 			the file path
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_file_create(tb_char_t const* path);

/*! remove the file
 * 
 * @param path 			the file path
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_file_remove(tb_char_t const* path);

/*! rename the file
 * 
 * @param path 			the source file path
 * @param dest 			the destination file path
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_file_rename(tb_char_t const* path, tb_char_t const* dest);

/*! the file full path
 * 
 * @param path 			the file path
 * @param full 			the full path
 * @param maxn 			the path maxn
 *
 * @return 				the full path
 */
tb_char_t const* 		tb_file_full(tb_char_t const* path, tb_char_t* full, tb_size_t maxn);

/*! link the file
 * 
 * @param path 			the source file path
 * @param dest 			the destination file path
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_file_link(tb_char_t const* path, tb_char_t const* dest);

#endif
