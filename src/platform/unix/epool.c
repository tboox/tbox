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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		epool.c
 *
 */

#if 0
#include "../stub/epool.c"
#else
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../epool.h"
#include "../../container/container.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <errno.h>
#include <unistd.h>

/* /////////////////////////////////////////////////////////
 * types
 */

// the epool type
typedef struct __tb_epool_t
{
	// the maxn
	tb_size_t 		maxn;

	// the size
	tb_size_t 		size;

	// the semaphore
	tb_long_t 		hsem;

	// the semaphore buffers
	struct sembuf* 	bufs;
	
	// the objects
	tb_handle_t* 	objs;

}tb_epool_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_handle_t tb_epool_init(tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(maxn, TB_NULL);

	// alloc
	tb_epool_t* ep = tb_calloc(1, sizeof(tb_epool_t));
	tb_assert_and_check_return_val(ep, TB_NULL);

	// init
	ep->maxn = maxn;

	// init semaphore
	ep->hsem = semget(IPC_PRIVATE, maxn, IPC_CREAT | IPC_EXCL | 0666); 
	tb_assert_and_check_goto(ep->hsem >= 0, fail);

	// init bufs
	ep->bufs = tb_calloc(maxn, sizeof(struct sembuf));
	tb_assert_and_check_goto(ep->bufs, fail);

	tb_size_t i = 0;
	for (i = 0; i < maxn; i++)
	{
		struct sembuf* b = ep->bufs + i;
		b->sem_num = i;
		b->sem_op = -2;
		b->sem_flg = SEM_UNDO;
//		b->sem_flg = SEM_UNDO | IPC_NOWAIT;
	}

	// ok
	return (tb_handle_t)ep;

fail:
	if (ep) tb_epool_exit(ep);
	return TB_NULL;
}
tb_void_t tb_epool_exit(tb_handle_t epool)
{
	tb_epool_t* ep = (tb_epool_t*)epool;
	if (ep)
	{
		// kill 
		tb_epool_kill(ep);

		// free bufs
		if (ep->bufs) 
		{
			// free semaphores
			tb_size_t i = 0;
			tb_size_t m = ep->maxn;
			for (i = 0; i < m; i++)
			{
				struct sembuf* b = ep->bufs + i;
				tb_long_t r = semctl(ep->hsem, b->sem_num, IPC_RMID);
				tb_assert(r != -1);
			}
	
			// free it
			tb_free(ep->bufs);
		}

		// free objs
		if (ep->objs) tb_free(ep->objs);

		// free it
		tb_free(ep);
	}
}
tb_size_t tb_epool_maxn(tb_handle_t epool)
{
	tb_epool_t* ep = (tb_epool_t*)epool;
	tb_assert_and_check_return_val(ep, 0);

	return ep->maxn;
}
tb_size_t tb_epool_size(tb_handle_t epool)
{
	tb_epool_t* ep = (tb_epool_t*)epool;
	tb_assert_and_check_return_val(ep, 0);

	return ep->size;
}
tb_handle_t tb_epool_adde(tb_handle_t epool, tb_pointer_t edata, tb_bool_t bsignal)
{
	tb_epool_t* ep = (tb_epool_t*)epool;
	tb_assert_and_check_return_val(ep, TB_NULL);

	tb_size_t i = 0;
	tb_size_t m = ep->maxn;
	for (i = 0; i < m; i++)
	{
		struct sembuf* b = ep->bufs + i;
		if (b->sem_op == -2)
		{
			b->sem_op = -1;
			ep->size++;
			return (tb_handle_t)(i + 1);
		}
	}

	return TB_NULL;
}
tb_void_t tb_epool_dele(tb_handle_t epool, tb_handle_t event)
{
	tb_epool_t* ep = (tb_epool_t*)epool;
	tb_assert_and_check_return(ep && ep->bufs && event);

	// get buffer
	tb_size_t i = (tb_long_t)event - 1;
	tb_assert_and_check_return(i < ep->maxn);
	struct sembuf* b = ep->bufs + i;
	
	// check
	tb_assert_and_check_return(b->sem_op > -2);

	// reset
	if (b->sem_op > -2)
	{
		// post semaphore
		b->sem_op = 1;
		tb_long_t r = semop(ep->hsem, b, 1);
		tb_assert(r != -1);

		// remove it
		b->sem_op = -2;
		ep->size--;
	}
}
tb_void_t tb_epool_post(tb_handle_t epool, tb_handle_t event)
{
	tb_epool_t* ep = (tb_epool_t*)epool;
	tb_assert_and_check_return(ep && ep->bufs && event);

	// get buffer
	tb_size_t i = (tb_long_t)event - 1;
	tb_assert_and_check_return(i < ep->maxn);
	struct sembuf* b = ep->bufs + i;

	// check
	tb_assert_and_check_return(b->sem_op > -2);

	// post semaphore
	b->sem_op = 1;
	tb_long_t r = semop(ep->hsem, b, 1);
    tb_assert(r != -1);
	b->sem_op = -1;
}
tb_void_t tb_epool_kill(tb_handle_t epool)
{
	tb_epool_t* ep = (tb_epool_t*)epool;
	tb_assert_and_check_return(ep && ep->bufs);

	tb_size_t i = 0;
	tb_size_t m = ep->maxn;
	for (i = 0; i < m; i++)
	{
		struct sembuf* b = ep->bufs + i;
		if (b->sem_op > -2)
		{
			// post semaphore
			b->sem_op = 1;
			tb_long_t r = semop(ep->hsem, b, 1);
			tb_assert(r != -1);
			b->sem_op = -1;
		}
	}
}
tb_long_t tb_epool_wait(tb_handle_t epool, tb_long_t timeout)
{
	tb_epool_t* ep = (tb_epool_t*)epool;
	tb_assert_and_check_return_val(ep && ep->bufs, -1);

	// init time
	struct timeval t = {0};
	if (timeout > 0)
	{
		t.tv_sec = timeout / 1000;
		t.tv_usec = (timeout % 1000) * 1000;
	}

	// wait semaphore
	tb_long_t r = semtimedop(ep->hsem, ep->bufs, ep->maxn, timeout >= 0? &t : TB_NULL);
    tb_check_return_val(!r || errno == EAGAIN, -1);

	// timeout?
    tb_check_return_val(errno != EAGAIN, 0);

#if 0
	// init grow
	tb_size_t grow = tb_align8((ep->maxn >> 3) + 1);

	// init objs
	if (!ep->objs)
	{
		ep->objn = nsem + grow;
		ep->objs = tb_calloc(ep->objn, sizeof(tb_handle_t));
		tb_assert_and_check_return_val(ep->objs, -1);
	}
	// grow objs if not enough
	else if (nsem > ep->objn)
	{
		// grow size
		ep->objn = nsem + grow;
		if (ep->objn > ep->maxn) ep->objn = ep->maxn;

		// grow data
		ep->objs = tb_realloc(ep->objs, ep->objn * sizeof(tb_aioo_t));
		tb_assert_and_check_return_val(ep->objs, -1);
	}
	tb_assert(nsem <= ep->objn);

	// sync objs
#endif

	// ok
	return 1;
}
tb_handle_t* tb_epool_objs(tb_handle_t epool)
{
	return TB_NULL;
}
tb_pointer_t* tb_epool_data(tb_handle_t epool)
{
	return TB_NULL;
}

#endif
