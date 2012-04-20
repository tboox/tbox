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
 * @file		systemv.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_event_init(tb_char_t const* name)
{
	// make key
	tb_size_t k = IPC_PRIVATE;
	if (name)
	{
		tb_byte_t const* p = name;
		while (*p) k = 16777619 * k ^ (tb_size_t)(*p++);
	}

	// init semaphore
	tb_long_t h = semget((key_t)k, 1, IPC_CREAT | IPC_EXCL | 0666);
	tb_assert_and_check_return_val(h >= 0 || errno == EEXIST, TB_NULL);

	// exists?
	if (errno == EEXIST)
	{
		h = semget((key_t)k, 1, 0);
		tb_assert_and_check_return_val(h >= 0, TB_NULL);
	}

ok:
	// ok
	return (tb_handle_t)(h + 1);
}
tb_void_t tb_event_exit(tb_handle_t handle)
{
	tb_assert_and_check_return(handle);
	tb_long_t h = (tb_long_t)handle - 1;

	// post first
	tb_event_post(handle);

	// remove semaphore
	tb_long_t r = semctl(h, 0, IPC_RMID);
    tb_assert(r != -1);
}
tb_void_t tb_event_post(tb_handle_t handle)
{
	tb_assert_and_check_return(handle);
	tb_long_t h = (tb_long_t)handle - 1;

	// init
	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_op = 1;
	sb.sem_flg = SEM_UNDO;

	// release semaphore
	tb_long_t r = semop(h, &sb, 1);
    tb_assert(r != -1);
}
tb_long_t tb_event_wait(tb_handle_t handle, tb_long_t timeout)
{
	tb_assert_and_check_return_val(handle, -1);
	tb_long_t h = (tb_long_t)handle - 1;

	// init time
	struct timeval t = {0};
	if (timeout > 0)
	{
		t.tv_sec = timeout / 1000;
		t.tv_usec = (timeout % 1000) * 1000;
	}

	// init
	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_op = -1;
	sb.sem_flg = SEM_UNDO;

	// wait semaphore
	tb_long_t r = semtimedop(h, &sb, 1, timeout >= 0? &t : TB_NULL);

	// ok?
    tb_check_return_val(r, 1);

	// timeout?
    tb_check_return_val(errno != EAGAIN, 0);

	// error
	return -1;
}
