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
#include "../../math/math.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <sys/unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdio.h>

/* /////////////////////////////////////////////////////////
 * implemention
 */

// file
tb_handle_t tb_file_open(tb_char_t const* path, tb_size_t flags)
{
	tb_assert_and_check_return_val(path, TB_NULL);

	// flag
	tb_size_t flag = 0;
	if (flags & TB_FILE_RO) flag |= O_RDONLY;
	else if (flags & TB_FILE_WO) flag |= O_WRONLY;
	else if (flags & TB_FILE_RW) flag |= O_RDWR;

	if (flags & TB_FILE_CREAT) flag |= O_CREAT;
	if (flags & TB_FILE_APPEND) flag |= O_APPEND;
	if (flags & TB_FILE_TRUNC) flag |= O_TRUNC;

	// mode
	tb_size_t mode = 0;
	if (flags & TB_FILE_CREAT) 
	{
		//if ((flags & TB_FILE_RO) | (flags & TB_FILE_RW)) mode |= S_IREAD;
		//if ((flags & TB_FILE_WO) | (flags & TB_FILE_RW)) mode |= S_IWRITE;
		mode = 0777;
	}

	// open it
	tb_long_t fd = open(path, flag, mode);

	// ok?
	return (fd < 0)? TB_NULL : ((tb_handle_t)fd);
}
tb_void_t tb_file_close(tb_handle_t hfile)
{
	if (hfile) close(hfile);
}
tb_long_t tb_file_read(tb_handle_t hfile, tb_byte_t* data, tb_size_t size)
{
	tb_assert_and_check_return_val(hfile, -1);
	return read(hfile, data, size);
}
tb_long_t tb_file_write(tb_handle_t hfile, tb_byte_t const* data, tb_size_t size)
{
	tb_assert_and_check_return_val(hfile, -1);
	return write(hfile, data, size);
}
tb_void_t tb_file_flush(tb_handle_t hfile)
{
	if (hfile) flush(hfile);
}
tb_int64_t tb_file_seek(tb_handle_t hfile, tb_int64_t offset, tb_size_t flags)
{
	tb_assert_and_check_return_val(hfile, -1);

	switch (flags)
	{
	case TB_FILE_SEEK_BEG:
		offset = lseek(hfile, offset, SEEK_SET);
	case TB_FILE_SEEK_CUR:
		offset = lseek(hfile, offset, SEEK_CUR);
	case TB_FILE_SEEK_END:
		offset = lseek(hfile, offset, SEEK_END);
	default:
		tb_trace("unknown file seek flag: %d", flags);
		break;
	}

	return offset;
}

tb_uint64_t tb_file_size(tb_handle_t hfile)
{
	tb_assert_and_check_return_val(hfile, 0);

	struct stat st = {0};
	return !fstat(hfile, &st) && st.st_size >= 0? (tb_uint64_t)st.st_size : 0;
}
tb_bool_t tb_file_create(tb_char_t const* path, tb_file_type_t type)
{
	tb_assert_and_check_return_val(path, TB_FALSE);
	switch (type)
	{
	case TB_FILE_TYPE_DIR:
		return !mkdir(path, S_IRWXU)? TB_TRUE : TB_FALSE;
	case TB_FILE_TYPE_FILE:
		{
			tb_long_t fd = open(path, O_CREAT | O_TRUNC, 0777);
			if (fd >= 0) 
			{
				close(fd); 
				return TB_TRUE;
			}
		}
		break;
	default:
		break;
	}
	return TB_FALSE;
}
tb_void_t tb_file_delete(tb_char_t const* path, tb_file_type_t type)
{
	tb_assert_and_check_return(path);
	remove(path);
}

tb_bool_t tb_file_info(tb_char_t const* path, tb_file_info_t* info)
{
	tb_assert_and_check_return_val(path, TB_FALSE);

	// exists?
	tb_check_return_val(!access(path, F_OK), TB_FALSE);

	// get info
	if (info)
	{
		// init info
		tb_memset(info, 0, sizeof(tb_file_info_t));

		// get stat
		struct stat st = {0};
		if (!stat(path, &st))
		{
			info->size = st.st_size >= 0? (tb_uint64_t)st.st_size : 0;
		}
	}
	return TB_FALSE;
}
