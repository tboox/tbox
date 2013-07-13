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
 * @file		exception.h
 *
 */
#ifndef TB_PLATFORM_UNIX_EXCEPTION_H
#define TB_PLATFORM_UNIX_EXCEPTION_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../libc/misc/signal.h"
#include "../../libc/misc/setjmp.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

#if defined(tb_signal) && defined(tb_setjmp) && defined(tb_longjmp)

// the exception data type
typedef struct __tb_exception_data_t
{
	// the tstore base
	tb_tstore_data_t 	base;

	// the jmpbuf
	tb_jmpbuf_t 		jmpbuf;

}tb_exception_data_t;

#endif

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

#if defined(tb_signal) && defined(tb_setjmp) && defined(tb_longjmp)

	// try
# 	define __tb_try \
	{ \
		/* init exception data */ \
		tb_exception_data_t* __e = tb_null; \
		if (!(__e = (tb_exception_data_t*)tb_tstore_getp())) \
		{ \
			__e = tb_malloc0(sizeof(tb_exception_data_t)); \
			if (__e) \
			{ \
				__e->base.type = TB_TSTORE_DATA_TYPE_EXCEPTION; \
				__e->base.free = tb_exception_data_free; \
				tb_tstore_setp(__e); \
			} \
		} \
 		\
		/* save jmpbuf */ \
		tb_int_t __j = __e? tb_setjmp(__e->jmpbuf) : 0; \
		if(!__j) \
		{

	// except
# 	define __tb_except \
	} \
	\
	if (__j)

	// end
# 	define __tb_end }

	// init
# 	define tb_exception_init tb_exception_init_impl

#endif

/* ///////////////////////////////////////////////////////////////////////
 * handler
 */

#if defined(tb_signal) && defined(tb_setjmp) && defined(tb_longjmp)
static tb_void_t tb_exception_func_impl(tb_int_t sig)
{
	tb_exception_data_t* e = (tb_exception_data_t*)tb_tstore_getp();
	if (e) tb_longjmp(e->jmpbuf, 1);
}
static tb_bool_t tb_exception_init_impl()
{
	tb_signal(TB_SIGINT, tb_exception_func_impl);
	tb_signal(TB_SIGFPE, tb_exception_func_impl);
	tb_signal(TB_SIGBUS, tb_exception_func_impl);
	tb_signal(TB_SIGSEGV, tb_exception_func_impl);
	tb_signal(TB_SIGTRAP, tb_exception_func_impl);
	return tb_true;
}
static tb_void_t tb_exception_data_free(tb_tstore_data_t* data)
{
	if (data) tb_free(data);
}
#endif

#endif


