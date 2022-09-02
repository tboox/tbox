/*!The Treasure Box Library
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2009-present, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        file.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../file.h"
#include "../path.h"
#include "../directory.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>
#ifdef TB_CONFIG_POSIX_HAVE_COPYFILE
#   include <copyfile.h>
#endif
#ifdef TB_CONFIG_POSIX_HAVE_SENDFILE
#   include <sys/sendfile.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_file_ref_t tb_file_init(tb_char_t const* path, tb_size_t mode)
{
    // check
    tb_assert_and_check_return_val(path, tb_null);

    // the full path
    tb_char_t full[TB_PATH_MAXN];
    path = tb_path_absolute(path, full, TB_PATH_MAXN);
    tb_assert_and_check_return_val(path, tb_null);

    // flags
    tb_size_t flags = 0;
    if (mode & TB_FILE_MODE_RO) flags |= O_RDONLY;
    else if (mode & TB_FILE_MODE_WO) flags |= O_WRONLY;
    else if (mode & TB_FILE_MODE_RW) flags |= O_RDWR;

    if (mode & TB_FILE_MODE_CREAT) flags |= O_CREAT;
    if (mode & TB_FILE_MODE_APPEND) flags |= O_APPEND;
    if (mode & TB_FILE_MODE_TRUNC) flags |= O_TRUNC;

    // dma mode, no cache
#if defined(TB_CONFIG_OS_LINUX) && defined(O_DIRECT)
    if (mode & TB_FILE_MODE_DIRECT) flags |= O_DIRECT;
#endif

    // noblock
    flags |= O_NONBLOCK;

    // modes
    tb_size_t modes = 0;
    if (mode & TB_FILE_MODE_CREAT)
    {
        // 0644: -rw-r--r--
        modes = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    }

    // open it, @note need absolute path
    tb_long_t fd = open(path, flags, modes);
    if (fd < 0 && (mode & TB_FILE_MODE_CREAT) && (errno != EPERM && errno != EACCES))
    {
#ifndef TB_CONFIG_MICRO_ENABLE
        // open it again after creating the file directory
        tb_int_t errno_bak = errno;
        tb_char_t dir[TB_PATH_MAXN];
        if (tb_directory_create(tb_path_directory(path, dir, sizeof(dir))))
            fd = open(path, flags, modes);
        else errno = errno_bak;
#endif
    }
    tb_check_return_val(fd >= 0, tb_null);

    // trace
    tb_trace_d("open: %p", tb_fd2file(fd));

    // ok?
    return tb_fd2file(fd);
}
tb_bool_t tb_file_exit(tb_file_ref_t file)
{
    // check
    tb_assert_and_check_return_val(file, tb_false);

    // trace
    tb_trace_d("close: %p", file);

    // close it
    tb_bool_t ok = !close(tb_file2fd(file))? tb_true : tb_false;

    // failed?
    if (!ok)
    {
        // trace
        tb_trace_e("close: %p failed, errno: %d", file, errno);
    }

    // ok?
    return ok;
}
tb_long_t tb_file_read(tb_file_ref_t file, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(file && data, -1);

    // read it
    return read(tb_file2fd(file), data, size);
}
tb_long_t tb_file_writ(tb_file_ref_t file, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(file && data, -1);

    // writ it
    return write(tb_file2fd(file), data, size);
}
tb_bool_t tb_file_sync(tb_file_ref_t file)
{
    // check
    tb_assert_and_check_return_val(file, tb_false);

    // sync
#ifdef TB_CONFIG_POSIX_HAVE_FDATASYNC
    return !fdatasync(tb_file2fd(file))? tb_true : tb_false;
#else
    return !fsync(tb_file2fd(file))? tb_true : tb_false;
#endif
}
tb_hong_t tb_file_seek(tb_file_ref_t file, tb_hong_t offset, tb_size_t mode)
{
    // check
    tb_assert_and_check_return_val(file, -1);

    // seek
    return lseek(tb_file2fd(file), offset, mode);
}
tb_hong_t tb_file_offset(tb_file_ref_t file)
{
    // check
    tb_assert_and_check_return_val(file, -1);

    // the offset
    return tb_file_seek(file, (tb_hong_t)0, TB_FILE_SEEK_CUR);
}
tb_hize_t tb_file_size(tb_file_ref_t file)
{
    // check
    tb_assert_and_check_return_val(file, 0);

    // the file size
    tb_hize_t size = 0;
    struct stat st = {0};
    if (!fstat(tb_file2fd(file), &st))
        size = st.st_size;

    // ok?
    return size;
}
tb_bool_t tb_file_info(tb_char_t const* path, tb_file_info_t* info)
{
    // check
    tb_assert_and_check_return_val(path, tb_false);

    // the full path (need translate "~/")
    tb_char_t full[TB_PATH_MAXN];
    path = tb_path_absolute(path, full, TB_PATH_MAXN);
    tb_assert_and_check_return_val(path, tb_false);

    // get info
    if (info)
    {
        // init info
        tb_memset(info, 0, sizeof(tb_file_info_t));

        // get stat, even if the file does not exist, it may be a dead symbolic link
#if defined(TB_CONFIG_POSIX_HAVE_LSTAT64)
        struct stat64 st = {0};
        if (!lstat64(path, &st))
#else
        struct stat st = {0};
        if (!lstat(path, &st))
#endif
        {
            // get file type
            if (S_ISDIR(st.st_mode)) info->type = TB_FILE_TYPE_DIRECTORY;
            else info->type = TB_FILE_TYPE_FILE;

            // is symlink?
            info->flags = TB_FILE_FLAG_NONE;
            if (S_ISLNK(st.st_mode))
            {
                // we need get more file info about symlink, does it point to directory?
                tb_memset(&st, 0, sizeof(st));
#if defined(TB_CONFIG_POSIX_HAVE_STAT64)
                if (!stat64(path, &st))
#else
                if (!stat(path, &st))
#endif
                {
                    if (S_ISDIR(st.st_mode)) info->type = TB_FILE_TYPE_DIRECTORY;
                    else info->type = TB_FILE_TYPE_FILE;
                }
                info->flags |= TB_FILE_FLAG_LINK;
            }

            // file size
            info->size = st.st_size >= 0? (tb_hize_t)st.st_size : 0;

            // the last access time
            info->atime = (tb_time_t)st.st_atime;

            // the last modify time
            info->mtime = (tb_time_t)st.st_mtime;
            return tb_true;
        }
    }
    else if (!access(path, F_OK))
    {
        return tb_true;
    }
    return tb_false;
}
#ifndef TB_CONFIG_MICRO_ENABLE
tb_long_t tb_file_pread(tb_file_ref_t file, tb_byte_t* data, tb_size_t size, tb_hize_t offset)
{
    // check
    tb_assert_and_check_return_val(file, -1);

    // read it
#ifdef TB_CONFIG_POSIX_HAVE_PREAD64
    return pread64(tb_file2fd(file), data, (size_t)size, offset);
#else
    return pread(tb_file2fd(file), data, (size_t)size, offset);
#endif
}
tb_long_t tb_file_pwrit(tb_file_ref_t file, tb_byte_t const* data, tb_size_t size, tb_hize_t offset)
{
    // check
    tb_assert_and_check_return_val(file, -1);

    // writ it
#ifdef TB_CONFIG_POSIX_HAVE_PWRITE64
    return pwrite64(tb_file2fd(file), data, (size_t)size, offset);
#else
    return pwrite(tb_file2fd(file), data, (size_t)size, offset);
#endif
}
tb_long_t tb_file_readv(tb_file_ref_t file, tb_iovec_t const* list, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(file && list && size, -1);

    // check iovec
    tb_assert_static(sizeof(tb_iovec_t) == sizeof(struct iovec));
    tb_assert(tb_memberof_eq(tb_iovec_t, data, struct iovec, iov_base));
    tb_assert(tb_memberof_eq(tb_iovec_t, size, struct iovec, iov_len));

    // read it
    return readv(tb_file2fd(file), (struct iovec const*)list, size);
}
tb_long_t tb_file_writv(tb_file_ref_t file, tb_iovec_t const* list, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(file && list && size, -1);

    // check iovec
    tb_assert_static(sizeof(tb_iovec_t) == sizeof(struct iovec));
    tb_assert(tb_memberof_eq(tb_iovec_t, data, struct iovec, iov_base));
    tb_assert(tb_memberof_eq(tb_iovec_t, size, struct iovec, iov_len));

    // writ it
    return writev(tb_file2fd(file), (struct iovec const*)list, size);
}
tb_hong_t tb_file_writf(tb_file_ref_t file, tb_file_ref_t ifile, tb_hize_t offset, tb_hize_t size)
{
    // check
    tb_assert_and_check_return_val(file && ifile && size, -1);

#ifdef TB_CONFIG_POSIX_HAVE_SENDFILE

    // writ it
    off_t       seek = offset;
    tb_hong_t   real = sendfile(tb_file2fd(file), tb_file2fd(ifile), &seek, (size_t)size);

    // ok?
    if (real >= 0) return real;

    // continue?
    if (errno == EINTR || errno == EAGAIN) return 0;

    // error
    return -1;

#else

    // read data
    tb_byte_t data[8192];
    tb_long_t read = tb_file_pread(ifile, data, sizeof(data), offset);
    tb_check_return_val(read > 0, read);

    // writ data
    tb_size_t writ = 0;
    while (writ < read)
    {
        tb_long_t real = tb_file_writ(file, data + writ, read - writ);
        if (real > 0) writ += real;
        else break;
    }

    // ok?
    return writ == read? writ : -1;
#endif
}
tb_long_t tb_file_preadv(tb_file_ref_t file, tb_iovec_t const* list, tb_size_t size, tb_hize_t offset)
{
    // check
    tb_assert_and_check_return_val(file && list && size, -1);

    // check iovec
    tb_assert_static(sizeof(tb_iovec_t) == sizeof(struct iovec));
    tb_assert(tb_memberof_eq(tb_iovec_t, data, struct iovec, iov_base));
    tb_assert(tb_memberof_eq(tb_iovec_t, size, struct iovec, iov_len));

    // read it
#ifdef TB_CONFIG_POSIX_HAVE_PREADV
    return preadv(tb_file2fd(file), (struct iovec const*)list, size, offset);
#else

    // FIXME: lock it

    // save offset
    tb_hong_t current = tb_file_offset(file);
    tb_assert_and_check_return_val(current >= 0, -1);

    // seek it
    if (current != offset && tb_file_seek(file, offset, TB_FILE_SEEK_BEG) != offset) return -1;

    // read it
    tb_long_t real = tb_file_readv(file, list, size);

    // restore offset
    if (current != offset && tb_file_seek(file, current, TB_FILE_SEEK_BEG) != current) return -1;

    // ok
    return real;
#endif
}
tb_long_t tb_file_pwritv(tb_file_ref_t file, tb_iovec_t const* list, tb_size_t size, tb_hize_t offset)
{
    // check
    tb_assert_and_check_return_val(file && list && size, -1);

    // check iovec
    tb_assert_static(sizeof(tb_iovec_t) == sizeof(struct iovec));
    tb_assert(tb_memberof_eq(tb_iovec_t, data, struct iovec, iov_base));
    tb_assert(tb_memberof_eq(tb_iovec_t, size, struct iovec, iov_len));

    // writ it
#ifdef TB_CONFIG_POSIX_HAVE_PWRITEV
    return pwritev(tb_file2fd(file), (struct iovec const*)list, size, offset);
#else

    // FIXME: lock it

    // save offset
    tb_hong_t current = tb_file_offset(file);
    tb_assert_and_check_return_val(current >= 0, -1);

    // seek it
    if (current != offset && tb_file_seek(file, offset, TB_FILE_SEEK_BEG) != offset) return -1;

    // writ it
    tb_long_t real = tb_file_writv(file, list, size);

    // restore offset
    if (current != offset && tb_file_seek(file, current, TB_FILE_SEEK_BEG) != current) return -1;

    // ok
    return real;
#endif
}
tb_bool_t tb_file_copy(tb_char_t const* path, tb_char_t const* dest, tb_size_t flags)
{
    // check
    tb_assert_and_check_return_val(path && dest, tb_false);

    // the full path
    tb_char_t data[TB_PATH_MAXN];
    path = tb_path_absolute(path, data, TB_PATH_MAXN);
    tb_assert_and_check_return_val(path, tb_false);

    // copy link
    tb_file_info_t info = {0};
    if (flags & TB_FILE_COPY_LINK && tb_file_info(path, &info) && info.flags & TB_FILE_FLAG_LINK)
    {
        // read link first
        tb_char_t srcpath[TB_PATH_MAXN];
        tb_long_t size = readlink(path, srcpath, TB_PATH_MAXN);
        tb_char_t const* linkpath = srcpath;
        if (size == TB_PATH_MAXN)
        {
            tb_size_t  maxn = TB_PATH_MAXN * 2;
            tb_char_t* buff = (tb_char_t*)tb_malloc(maxn);
            if (buff)
            {
                tb_long_t size = readlink(path, buff, maxn);
                if (size > 0 && size < maxn)
                {
                    buff[size] = '\0';
                    linkpath = buff;
                }
            }
        }
        else if (size >= 0 && size < TB_PATH_MAXN)
            srcpath[size] = '\0';

        // do link
        tb_bool_t ok = tb_file_link(linkpath, dest);

        // free link path
        if (linkpath && linkpath != srcpath)
        {
            tb_free((tb_pointer_t)linkpath);
            linkpath = tb_null;
        }
        return ok;
    }

#ifdef TB_CONFIG_POSIX_HAVE_COPYFILE

    // the dest path
    tb_char_t full1[TB_PATH_MAXN];
    dest = tb_path_absolute(dest, full1, TB_PATH_MAXN);
    tb_assert_and_check_return_val(dest, tb_false);

    // attempt to copy it directly
    if (!copyfile(path, dest, 0, COPYFILE_ALL)) return tb_true;
    else if (errno != EPERM && errno != EACCES)
    {
        // attempt to copy it again after creating directory
        tb_char_t dir[TB_PATH_MAXN];
        tb_int_t errno_bak = errno;
        if (tb_directory_create(tb_path_directory(dest, dir, sizeof(dir))))
            return !copyfile(path, dest, 0, COPYFILE_ALL);
        else errno = errno_bak;
    }

    // failed
    return tb_false;
#else
    tb_int_t    ifd = -1;
    tb_int_t    ofd = -1;
    tb_bool_t   ok = tb_false;
    do
    {
        // get stat.st_mode first
#ifdef TB_CONFIG_POSIX_HAVE_STAT64
        struct stat64 st = {0};
        if (stat64(path, &st)) break;
#else
        struct stat st = {0};
        if (stat(path, &st)) break;
#endif

        // open source file
        ifd = open(path, O_RDONLY);
        tb_check_break(ifd >= 0);

        // get the absolute source path
        dest = tb_path_absolute(dest, data, sizeof(data));
        tb_assert_and_check_break(dest);

        // open destinate file and copy file mode
        ofd = open(dest, O_RDWR | O_CREAT | O_TRUNC, st.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
        if (ofd < 0 && (errno != EPERM && errno != EACCES))
        {
            // attempt to open it again after creating directory
            tb_int_t errno_bak = errno;
            tb_char_t dir[TB_PATH_MAXN];
            if (tb_directory_create(tb_path_directory(dest, dir, sizeof(dir))))
                ofd = open(dest, O_RDWR | O_CREAT | O_TRUNC, st.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
            else errno = errno_bak;
        }
        tb_check_break(ofd >= 0);

        // get file size
        tb_hize_t size = tb_file_size(tb_fd2file(ifd));

        // init write size
        tb_hize_t writ = 0;

        // attempt to copy file using `sendfile`
#ifdef TB_CONFIG_POSIX_HAVE_SENDFILE
        while (writ < size)
        {
            off_t seek = writ;
            tb_hong_t real = sendfile(ofd, ifd, &seek, (size_t)(size - writ));
            if (real > 0) writ += real;
            else break;
        }

        /* attempt to copy file directly if sendfile failed
         *
         * sendfile() supports regular file only after "since Linux 2.6.33".
         */
        if (writ != size)
        {
            lseek(ifd, 0, SEEK_SET);
            lseek(ofd, 0, SEEK_SET);
        }
        else
        {
            ok = tb_true;
            break;
        }
#endif

        // copy file using `read` and `write`
        writ = 0;
        while (writ < size)
        {
            // read some data
            tb_int_t real = read(ifd, data, (size_t)tb_min(size - writ, sizeof(data)));
            if (real > 0)
            {
                real = write(ofd, data, real);
                if (real > 0) writ += real;
                else break;
            }
            else break;
        }

        // ok?
        ok = (writ == size);

    } while (0);

    // close source file
    if (ifd >= 0) close(ifd);
    ifd = -1;

    // close destinate file
    if (ofd >= 0) close(ofd);
    ofd = -1;

    // ok?
    return ok;
#endif
}
tb_bool_t tb_file_create(tb_char_t const* path)
{
    // check
    tb_assert_and_check_return_val(path, tb_false);

    // make it
    tb_file_ref_t file = tb_file_init(path, TB_FILE_MODE_CREAT | TB_FILE_MODE_WO | TB_FILE_MODE_TRUNC);
    if (file) tb_file_exit(file);

    // ok?
    return file? tb_true : tb_false;
}
tb_bool_t tb_file_remove(tb_char_t const* path)
{
    // check
    tb_assert_and_check_return_val(path, tb_false);

    // the full path
    tb_char_t full[TB_PATH_MAXN];
    path = tb_path_absolute(path, full, TB_PATH_MAXN);
    tb_assert_and_check_return_val(path, tb_false);

    // remove it
    return !remove(path)? tb_true : tb_false;
}
tb_bool_t tb_file_rename(tb_char_t const* path, tb_char_t const* dest)
{
    // check
    tb_assert_and_check_return_val(path && dest, tb_false);

    // the full path
    tb_char_t full0[TB_PATH_MAXN];
    path = tb_path_absolute(path, full0, TB_PATH_MAXN);
    tb_assert_and_check_return_val(path, tb_false);

    // the dest path
    tb_char_t full1[TB_PATH_MAXN];
    dest = tb_path_absolute(dest, full1, TB_PATH_MAXN);
    tb_assert_and_check_return_val(dest, tb_false);

    // attempt to rename it directly
    if (!rename(path, dest)) return tb_true;
    else if (errno != EPERM && errno != EACCES)
    {
        // attempt to rename it again after creating directory
        tb_int_t errno_bak = errno;
        tb_char_t dir[TB_PATH_MAXN];
        if (tb_directory_create(tb_path_directory(dest, dir, sizeof(dir))))
            return !rename(path, dest);
        else errno = errno_bak;
    }
    return tb_false;
}
tb_bool_t tb_file_link(tb_char_t const* path, tb_char_t const* dest)
{
    // check
    tb_assert_and_check_return_val(path && dest, tb_false);

    // the dest path
    tb_char_t full1[TB_PATH_MAXN];
    dest = tb_path_absolute(dest, full1, TB_PATH_MAXN);
    tb_assert_and_check_return_val(dest, tb_false);

    // attempt to link it directly
    // @note we should not use absolute path, dest -> path (may be relative path)
    if (!symlink(path, dest)) return tb_true;
    else if (errno != EPERM && errno != EACCES)
    {
        // attempt to link it again after creating directory
        tb_int_t errno_bak = errno;
        tb_char_t dir[TB_PATH_MAXN];
        if (tb_directory_create(tb_path_directory(dest, dir, sizeof(dir))))
            return !symlink(path, dest);
        else errno = errno_bak;
    }
    return tb_false;
}
tb_bool_t tb_file_access(tb_char_t const* path, tb_size_t mode)
{
    // check
    tb_assert_and_check_return_val(path, tb_false);

    // the full path
    tb_char_t full[TB_PATH_MAXN];
    path = tb_path_absolute(path, full, TB_PATH_MAXN);
    tb_assert_and_check_return_val(path, tb_false);

    // flags
    tb_size_t flags = 0;
    if (mode & TB_FILE_MODE_RW) flags = R_OK | W_OK;
    else
    {
        if (mode & TB_FILE_MODE_RO) flags |= R_OK;
        if (mode & TB_FILE_MODE_WO) flags |= W_OK;
    }
    if (mode & TB_FILE_MODE_EXEC) flags |= X_OK;

    return !access(full, flags);
}
tb_bool_t tb_file_touch(tb_char_t const* path, tb_time_t atime, tb_time_t mtime)
{
    // check
    tb_assert_and_check_return_val(path, tb_false);

    // the full path
    tb_char_t full[TB_PATH_MAXN];
    path = tb_path_absolute(path, full, TB_PATH_MAXN);
    tb_assert_and_check_return_val(path, tb_false);

    // file exists?
    tb_bool_t ok = tb_false;
    struct timespec ts[2];
    tb_memset(ts, 0, sizeof(ts));
    if (!access(path, F_OK))
    {
        if (atime > 0 || mtime > 0)
        {
#ifdef TB_CONFIG_POSIX_HAVE_UTIMENSAT
            if (atime > 0) ts[0].tv_sec = atime;
            else ts[0].tv_nsec = UTIME_OMIT;
            if (mtime > 0) ts[1].tv_sec = mtime;
            else ts[1].tv_nsec = UTIME_OMIT;
            ok = !utimensat(AT_FDCWD, path, ts, 0);
#endif
        }
        else ok = tb_true;
    }
    else
    {
        // create a new file if not exists
        tb_file_ref_t file = tb_file_init(path, TB_FILE_MODE_RW | TB_FILE_MODE_CREAT);
        if (file)
        {
            if (atime > 0 || mtime > 0)
            {
#ifdef TB_CONFIG_POSIX_HAVE_FUTIMENS
                if (atime > 0) ts[0].tv_sec = atime;
                else ts[0].tv_nsec = UTIME_OMIT;
                if (mtime > 0) ts[1].tv_sec = mtime;
                else ts[1].tv_nsec = UTIME_OMIT;
                ok = !futimens(tb_file2fd(file), ts);
#endif
            }
            else ok = tb_true;
            tb_file_exit(file);
        }
    }
    return ok;
}
#endif


