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
 * \file		file.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "../tplat.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>

/* /////////////////////////////////////////////////////////
 * macros
 */
//#define TPLAT_FILE_PATH_PREFIX 		"/home/enrich"
#define TPLAT_FILE_PATH_PREFIX 		""
/* /////////////////////////////////////////////////////////
 * types
 */
typedef struct __tplat_file_list_t
{
	DIR* 				pdir; 
	tplat_char_t 		dir[TPLAT_FILENAME_MAX_SIZE];
	tplat_file_entry_t 	entry;

}tplat_file_list_t;

/* /////////////////////////////////////////////////////////
 * implemention
 */

// file
tplat_handle_t tplat_file_open(tplat_char_t const* filename, tplat_int_t flags)
{
	tplat_int_t flag = 0, mode = 0, fd = -1;
	if (!filename) return TPLAT_INVALID_HANDLE;

	//TPLAT_DBG("tplat_file_open:%s flags:%d", filename, flags);

	if (flags & TPLAT_FILE_RO) flag |= O_RDONLY;
	else if (flags & TPLAT_FILE_WO) flag |= O_WRONLY;
	else if (flags & TPLAT_FILE_RW) flag |= O_RDWR;

	if (flags & TPLAT_FILE_CREAT) flag |= O_CREAT;
	if (flags & TPLAT_FILE_APPEND) flag |= O_APPEND;
	if (flags & TPLAT_FILE_TRUNC) flag |= O_TRUNC;

	if (flags & TPLAT_FILE_CREAT) 
	{
		//if ((flags & TPLAT_FILE_RO) | (flags & TPLAT_FILE_RW)) mode |= S_IREAD;
		//if ((flags & TPLAT_FILE_WO) | (flags & TPLAT_FILE_RW)) mode |= S_IWRITE;
		mode = 0777;
	}

	tplat_char_t path[TPLAT_FILENAME_MAX_SIZE];
	snprintf(path, TPLAT_FILENAME_MAX_SIZE, "%s%s", TPLAT_FILE_PATH_PREFIX, filename);
	fd = open(path, flag, mode);

	if (fd < 0) return TPLAT_INVALID_HANDLE;
	else return ((tplat_handle_t)fd);
}
void tplat_file_close(tplat_handle_t hfile)
{
	//TPLAT_DBG("tplat_file_close");
	if (hfile != TPLAT_INVALID_HANDLE) close((tplat_int_t)hfile);
}
tplat_int_t tplat_file_read(tplat_handle_t hfile, tplat_byte_t* data, tplat_int_t read_n)
{
	//TPLAT_DBG("tplat_file_read: %d bytes", read_n);
	if (hfile == TPLAT_INVALID_HANDLE) return 0;
	else return read((tplat_int_t)hfile, data, read_n);
}
tplat_int_t tplat_file_write(tplat_handle_t hfile, tplat_byte_t const* data, tplat_int_t write_n)
{
	//TPLAT_DBG("tplat_file_write: %d bytes", write_n);
	if (hfile == TPLAT_INVALID_HANDLE) return 0;
	else return write((tplat_int_t)hfile, data, write_n);
}
void tplat_file_flush(tplat_handle_t hfile)
{
	//TPLAT_DBG("tplat_file_flush");
}
tplat_int64_t tplat_file_seek(tplat_handle_t hfile, tplat_int64_t offset, tplat_int_t flags)
{
	if (hfile == TPLAT_INVALID_HANDLE) return -1;
	//TPLAT_DBG("tplat_file_seek: offset:%d flag: %d", (off_t)offset, flags);

	if (flags == TPLAT_FILE_SEEK_BEG) return lseek((tplat_int_t)hfile, (off_t)offset, SEEK_SET);
	else if (flags == TPLAT_FILE_SEEK_CUR) return lseek((tplat_int_t)hfile, (off_t)offset, SEEK_CUR);
	else if (flags == TPLAT_FILE_SEEK_END) return lseek((tplat_int_t)hfile, (off_t)offset, SEEK_END);
	else if (flags == TPLAT_FILE_SEEK_SIZE) 
	{
		off_t cur = lseek((tplat_int_t)hfile, 0, SEEK_CUR);
		off_t ret = lseek((tplat_int_t)hfile, 0, SEEK_END);
		if (-1 == lseek((tplat_int_t)hfile, cur, SEEK_SET)) return -1;
		else return ret;
	}
	else TPLAT_DBG("unknown seek flag: %d", flags);

	return -1;
}

// open file list
tplat_handle_t tplat_file_list_open(tplat_char_t const* dir)
{
	TPLAT_ASSERT(dir);
	if (!dir) return TPLAT_INVALID_HANDLE;

	tplat_file_list_t flist;

	// append prefix
	tplat_char_t path[TPLAT_FILENAME_MAX_SIZE];
	snprintf(path, TPLAT_FILENAME_MAX_SIZE, "%s%s", TPLAT_FILE_PATH_PREFIX, dir);

	// open directory
	flist.pdir = opendir(path);
	TPLAT_ASSERT(flist.pdir);
	if (!flist.pdir) return TPLAT_INVALID_HANDLE;

	// save current directory
	strncpy(flist.dir, path, TPLAT_FILENAME_MAX_SIZE - 1);
	flist.dir[TPLAT_FILENAME_MAX_SIZE - 1] = '\0';

	// malloc 
	tplat_file_list_t* pflist = (tplat_file_list_t*)malloc(sizeof(tplat_file_list_t));
	if (!pflist) return TPLAT_INVALID_HANDLE;
	
	// return list
	*pflist = flist;
	return ((tplat_handle_t)pflist);
}

// get file list entry, end: return NULL
tplat_file_entry_t const* tplat_file_list_entry(tplat_handle_t hflist)
{
	TPLAT_ASSERT(hflist != TPLAT_INVALID_HANDLE);
	if (hflist == TPLAT_INVALID_HANDLE) return TPLAT_NULL;

	tplat_file_list_t* pflist = (tplat_file_list_t*)hflist;
	if (!pflist) return TPLAT_NULL;

	// get file entry
	struct dirent* pdirent = readdir(pflist->pdir);
	if (!pdirent) return TPLAT_NULL;

	// init entry
	memset(&pflist->entry, 0, sizeof(tplat_file_entry_t));

	// save file name
	if (pdirent->d_reclen <= 0)
		return TPLAT_NULL;

	if (pdirent->d_reclen >= TPLAT_FILENAME_MAX_SIZE)
		pflist->entry.namesize = TPLAT_FILENAME_MAX_SIZE - 1;
	else pflist->entry.namesize = (tplat_size_t)pdirent->d_reclen;

	strncpy(pflist->entry.name, pdirent->d_name, pflist->entry.namesize);
	pflist->entry.name[pflist->entry.namesize] = '\0';

	// get file type
	if (!strcmp(pflist->entry.name, ".")) pflist->entry.type = TPLAT_FILE_TYPE_IS_DOT;
	else if (!strcmp(pflist->entry.name, "..")) pflist->entry.type = TPLAT_FILE_TYPE_IS_DOT2;
	else
	{
		// stat
		struct stat fstat;
		snprintf(pflist->entry.path, TPLAT_FILENAME_MAX_SIZE - 1, "%s/%s", pflist->dir, pflist->entry.name);
		pflist->entry.path[TPLAT_FILENAME_MAX_SIZE - 1] = '\0';
		stat(pflist->entry.path, &fstat);
		
		// save path
		snprintf(pflist->entry.path, TPLAT_FILENAME_MAX_SIZE - 1, "%s/%s", &(pflist->dir[sizeof(TPLAT_FILE_PATH_PREFIX) - 1]), pflist->entry.name);
		pflist->entry.path[TPLAT_FILENAME_MAX_SIZE - 1] = '\0';

		if (S_ISDIR(fstat.st_mode)) pflist->entry.type = TPLAT_FILE_TYPE_IS_DIR;
		else pflist->entry.type = TPLAT_FILE_TYPE_IS_FILE;
	}

	return (&(pflist->entry));
}

// close file list
void tplat_file_list_close(tplat_handle_t hflist)
{
	TPLAT_ASSERT(hflist != TPLAT_INVALID_HANDLE);
	if (hflist == TPLAT_INVALID_HANDLE) return ;

	tplat_file_list_t* pflist = (tplat_file_list_t*)hflist;
	if (pflist) 
	{
		if (pflist->pdir) 
			closedir(pflist->pdir);
		
		free(pflist);
	}
}

// delete file or empty directory
tplat_bool_t tplat_file_delete(tplat_char_t const* path, tplat_file_type_t type)
{
	// no implemention
	return TPLAT_FALSE;
}

// get the size of file or directory 
tplat_size_t tplat_file_size(tplat_char_t const* path, tplat_file_type_t type)
{
	// no implemention
	return 0;
}

tplat_bool_t tplat_file_create(tplat_char_t const* path, tplat_file_type_t type)
{
	return TPLAT_TRUE;
}

