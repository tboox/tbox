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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>

/* /////////////////////////////////////////////////////////
 * macros
 */
#define TB_FILE_PATH_PREFIX 		""
/* /////////////////////////////////////////////////////////
 * types
 */
typedef struct __tb_file_list_t
{
	DIR* 				pdir; 
	tb_char_t 			dir[TB_FILENAME_MAX_SIZE];
	tb_file_entry_t 	entry;

}tb_file_list_t;

/* /////////////////////////////////////////////////////////
 * implemention
 */

// file
tb_handle_t tb_file_open(tb_char_t const* filename, tb_int_t flags)
{
	tb_int_t flag = 0, mode = 0, fd = -1;
	if (!filename) return TB_NULL;

	//TB_DBG("tb_file_open:%s flags:%d", filename, flags);

	if (flags & TB_FILE_RO) flag |= O_RDONLY;
	else if (flags & TB_FILE_WO) flag |= O_WRONLY;
	else if (flags & TB_FILE_RW) flag |= O_RDWR;

	if (flags & TB_FILE_CREAT) flag |= O_CREAT;
	if (flags & TB_FILE_APPEND) flag |= O_APPEND;
	if (flags & TB_FILE_TRUNC) flag |= O_TRUNC;

	if (flags & TB_FILE_CREAT) 
	{
		//if ((flags & TB_FILE_RO) | (flags & TB_FILE_RW)) mode |= S_IREAD;
		//if ((flags & TB_FILE_WO) | (flags & TB_FILE_RW)) mode |= S_IWRITE;
		mode = 0777;
	}

	tb_char_t path[TB_FILENAME_MAX_SIZE];
	tb_snprintf(path, TB_FILENAME_MAX_SIZE, "%s%s", TB_FILE_PATH_PREFIX, filename);
	fd = open(path, flag, mode);

	if (fd < 0) return TB_NULL;
	else return ((tb_handle_t)fd);
}
tb_void_t tb_file_close(tb_handle_t hfile)
{
	//TB_DBG("tb_file_close");
	if (hfile) close((tb_int_t)hfile);
}
tb_int_t tb_file_read(tb_handle_t hfile, tb_byte_t* data, tb_int_t size)
{
	//TB_DBG("tb_file_read: %d bytes", size);
	if (hfile) return read((tb_int_t)hfile, data, size);
	else return -1;
}
tb_int_t tb_file_write(tb_handle_t hfile, tb_byte_t const* data, tb_int_t size)
{
	//TB_DBG("tb_file_write: %d bytes", size);
	if (hfile) return write((tb_int_t)hfile, data, size);
	else return -1;
}
tb_void_t tb_file_flush(tb_handle_t hfile)
{
	TB_NOT_IMPLEMENT();
}
tb_int_t tb_file_seek(tb_handle_t hfile, tb_int_t offset, tb_int_t flags)
{
	if (!hfile) return -1;
	//TB_DBG("tb_file_seek: offset:%d flag: %d", (off_t)offset, flags);

	if (flags == TB_FILE_SEEK_BEG) return lseek((tb_int_t)hfile, (off_t)offset, SEEK_SET);
	else if (flags == TB_FILE_SEEK_CUR) return lseek((tb_int_t)hfile, (off_t)offset, SEEK_CUR);
	else if (flags == TB_FILE_SEEK_END) return lseek((tb_int_t)hfile, (off_t)offset, SEEK_END);
	else if (flags == TB_FILE_SEEK_SIZE) 
	{
		off_t cur = lseek((tb_int_t)hfile, 0, SEEK_CUR);
		off_t ret = lseek((tb_int_t)hfile, 0, SEEK_END);
		if (-1 == lseek((tb_int_t)hfile, cur, SEEK_SET)) return -1;
		else return ret;
	}
	else TB_DBG("unknown seek flag: %d", flags);

	return -1;
}
tb_size_t tb_file_size(tb_char_t const* path, tb_file_type_t type)
{
	TB_NOT_IMPLEMENT();
	return 0;
}
tb_bool_t tb_file_exists(tb_char_t const* path)
{
	return !access(path, F_OK)? TB_TRUE : TB_FALSE;
}
tb_bool_t tb_file_create(tb_char_t const* path, tb_file_type_t type)
{
	TB_ASSERT_RETURN_VAL(path, TB_FALSE);
	switch (type)
	{
	case TB_FILE_TYPE_DIR:
		return !mkdir(path, S_IRWXU)? TB_TRUE : TB_FALSE;
	case TB_FILE_TYPE_FILE:
		TB_NOT_IMPLEMENT();
		break;
	default:
		break;
	}
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
	TB_ASSERT_RETURN_VAL(dir, TB_NULL);

	tb_file_list_t flist;

	// append prefix
	tb_char_t path[TB_FILENAME_MAX_SIZE];
	tb_snprintf(path, TB_FILENAME_MAX_SIZE, "%s%s", TB_FILE_PATH_PREFIX, dir);

	// open directory
	flist.pdir = opendir(path);
	TB_ASSERT(flist.pdir);
	if (!flist.pdir) return TB_NULL;

	// save current directory
	strncpy(flist.dir, path, TB_FILENAME_MAX_SIZE - 1);
	flist.dir[TB_FILENAME_MAX_SIZE - 1] = '\0';

	// malloc 
	tb_file_list_t* pflist = (tb_file_list_t*)malloc(sizeof(tb_file_list_t));
	if (!pflist) return TB_NULL;
	
	// return list
	*pflist = flist;
	return ((tb_handle_t)pflist);
}

// get file list entry, end: return NULL
tb_file_entry_t const* tb_file_list_entry(tb_handle_t hflist)
{
	TB_ASSERT_RETURN_VAL(hflist, TB_NULL);
	tb_file_list_t* pflist = (tb_file_list_t*)hflist;

	// get file entry
	struct dirent* pdirent = readdir(pflist->pdir);
	if (!pdirent) return TB_NULL;

	// init entry
	memset(&pflist->entry, 0, sizeof(tb_file_entry_t));

	// save file name
	if (pdirent->d_reclen <= 0)
		return TB_NULL;

	if (pdirent->d_reclen >= TB_FILENAME_MAX_SIZE)
		pflist->entry.namesize = TB_FILENAME_MAX_SIZE - 1;
	else pflist->entry.namesize = (tb_size_t)pdirent->d_reclen;

	strncpy(pflist->entry.name, pdirent->d_name, pflist->entry.namesize);
	pflist->entry.name[pflist->entry.namesize] = '\0';

	// get file type
	if (!strcmp(pflist->entry.name, ".")) pflist->entry.type = TB_FILE_TYPE_DOT;
	else if (!strcmp(pflist->entry.name, "..")) pflist->entry.type = TB_FILE_TYPE_DOT2;
	else
	{
		// stat
		struct stat fstat;
		tb_snprintf(pflist->entry.path, TB_FILENAME_MAX_SIZE - 1, "%s/%s", pflist->dir, pflist->entry.name);
		pflist->entry.path[TB_FILENAME_MAX_SIZE - 1] = '\0';
		stat(pflist->entry.path, &fstat);
		
		// save path
		tb_snprintf(pflist->entry.path, TB_FILENAME_MAX_SIZE - 1, "%s/%s", &(pflist->dir[sizeof(TB_FILE_PATH_PREFIX) - 1]), pflist->entry.name);
		pflist->entry.path[TB_FILENAME_MAX_SIZE - 1] = '\0';

		if (S_ISDIR(fstat.st_mode)) pflist->entry.type = TB_FILE_TYPE_DIR;
		else pflist->entry.type = TB_FILE_TYPE_FILE;
	}

	return (&(pflist->entry));
}

// close file list
tb_void_t tb_file_list_close(tb_handle_t hflist)
{
	TB_ASSERT_RETURN(hflist);
	tb_file_list_t* pflist = (tb_file_list_t*)hflist;

	if (pflist->pdir) closedir(pflist->pdir);
	free(pflist);
}

