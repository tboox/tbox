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
 * @file		sched.c
 * @ingroup 	platform
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../sched.h"
#if defined(TB_CONFIG_OS_LINUX) || \
		defined(TB_CONFIG_OS_ANDROID)
# 	include <sched.h>
#else
# 	include <pthread.h>
#endif

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_sched_yield()
{
#if defined(TB_CONFIG_OS_LINUX) || \
		defined(TB_CONFIG_OS_ANDROID)
	return !sched_yield()? tb_true : tb_false;
#else
	return !pthread_yield()? tb_true : tb_false;
#endif
}

