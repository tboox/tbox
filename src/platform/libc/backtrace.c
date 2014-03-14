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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author		ruki
 * @file		backtrace.c
 * @ingroup 	platform
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 			"backtrace"
#define TB_TRACE_MODULE_DEBUG 			(0)

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../arch/frame.h"
#include "../../libc/libc.h"
#if defined(TB_CONFIG_LIBC_HAVE_BACKTRACE)
# 	include <execinfo.h>
#elif 	defined(TB_FIRST_FRAME_POINTER) \
	&& 	defined(TB_CURRENT_STACK_FRAME) \
	&& 	defined(TB_ADVANCE_STACK_FRAME) \
	&& 	defined(TB_STACK_INNER_THAN)
# 	include <dlfcn.h>
#endif

/* ///////////////////////////////////////////////////////////////////////
 * backtrace implementation
 */
#if defined(TB_CONFIG_LIBC_HAVE_BACKTRACE)
tb_size_t tb_backtrace_frames(tb_pointer_t* frames, tb_size_t nframe, tb_size_t nskip)
{
	// note: cannot use assert
	tb_check_return_val(frames && nframe, 0);

	// skip some frames?
	if (nskip)
	{
		// init temp frames
		tb_pointer_t 	temp[256] = {0};
		tb_check_return_val(nframe + nskip < 256, 0);

		// done backtrace
		tb_size_t 		size = backtrace(temp, nframe + nskip);
		tb_check_return_val(nskip < size, 0);

		// update nframe
		nframe = tb_min(nframe, size - nskip);

		// save to frames
		tb_memcpy(frames, temp + nskip, nframe * sizeof(tb_pointer_t));
	}
	// backtrace
	else nframe = backtrace(frames, nframe);

	// ok?
	return nframe;
}
#elif 	defined(TB_FIRST_FRAME_POINTER) \
	&& 	defined(TB_CURRENT_STACK_FRAME) \
	&& 	defined(TB_ADVANCE_STACK_FRAME) \
	&& 	defined(TB_STACK_INNER_THAN)
//extern tb_pointer_t __libc_stack_end;
tb_size_t tb_backtrace_frames(tb_pointer_t* frames, tb_size_t nframe, tb_size_t nskip)
{
	// the libc stack end pointer
	tb_pointer_t* __plibc_stack_end = dlsym(tb_null, "__libc_stack_end");
	tb_pointer_t __libc_stack_end = __plibc_stack_end? *__plibc_stack_end : tb_null;

	// trace
//	tb_trace_d("__libc_stack_end: %p", __libc_stack_end);

	// the top frame and stack address
	tb_pointer_t top_frame = TB_FIRST_FRAME_POINTER;
	tb_pointer_t top_stack = TB_CURRENT_STACK_FRAME;

	// trace
//	tb_trace_d("top_frame: %p", top_frame);
//	tb_trace_d("top_stack: %p", top_stack);

	// the current frame
	tb_frame_layout_t* current = ((tb_frame_layout_t*)top_frame);

	// the top frame not contain this func, nskip--
	if (nskip) nskip--;

	// walk frames
	tb_size_t n = 0;
	tb_size_t m = __libc_stack_end? 100 : 16;
	while (n < nframe && m--)
	{
		// trace
//		tb_trace_d("current: %p", current);

		// out of range?
		if ( 	(tb_pointer_t)current TB_STACK_INNER_THAN top_stack
			|| 	(__libc_stack_end && !((tb_pointer_t)current TB_STACK_INNER_THAN __libc_stack_end)))
		{
			break;
		}

#if 1
		// save the return address
		if (!nskip) frames[n++] = current->return_address;
		// skip this frame
		else nskip--;
#else
		frames[n++] = current->return_address;
#endif

		// the next frame address
		current = TB_ADVANCE_STACK_FRAME(current->next);
	}

	// ok?
	return n;
}
#else
tb_size_t tb_backtrace_frames(tb_pointer_t* frames, tb_size_t nframe, tb_size_t nskip)
{
	tb_trace_noimpl();
	return 0;
}
#endif

/* ///////////////////////////////////////////////////////////////////////
 * symbols implementation
 */
#if defined(TB_CONFIG_LIBC_HAVE_BACKTRACE)
tb_handle_t tb_backtrace_symbols_init(tb_pointer_t* frames, tb_size_t nframe)
{
	tb_check_return_val(frames && nframe, tb_null);
	return (tb_handle_t)backtrace_symbols(frames, nframe);
}
tb_char_t const* tb_backtrace_symbols_name(tb_handle_t symbols, tb_pointer_t* frames, tb_size_t nframe, tb_size_t iframe)
{
	tb_check_return_val(symbols && frames && nframe && iframe < nframe, tb_null);
	return ((tb_char_t const**)symbols)[iframe];
}
tb_void_t tb_backtrace_symbols_exit(tb_handle_t symbols)
{
	if (symbols) free(symbols);
}
#else
tb_handle_t tb_backtrace_symbols_init(tb_pointer_t* frames, tb_size_t nframe)
{
	// check
	tb_check_return_val(frames && nframe, tb_null);

	// init symbols
	return malloc(8192);
}
tb_char_t const* tb_backtrace_symbols_name(tb_handle_t symbols, tb_pointer_t* frames, tb_size_t nframe, tb_size_t iframe)
{
	// check
	tb_check_return_val(symbols && frames && nframe && iframe < nframe, tb_null);

	// the frame address
	tb_pointer_t frame = frames[iframe];
	tb_check_return_val(frame, tb_null);

	// the frame dlinfo
	Dl_info dlinfo = {0};
	if (!dladdr(frame, &dlinfo)) return tb_null;

	// format
	tb_long_t size = 0;
	tb_size_t maxn = 8192;
	if (dlinfo.dli_fname) size = tb_snprintf((tb_char_t*)symbols, maxn, "%s(", dlinfo.dli_fname);
	if (dlinfo.dli_sname && size >= 0) size += tb_snprintf((tb_char_t*)symbols + size, maxn - size, "%s", dlinfo.dli_sname);
	if (dlinfo.dli_sname && frame >= dlinfo.dli_saddr && size >= 0) size += tb_snprintf((tb_char_t*)symbols + size, maxn - size, "+%#lx", (tb_size_t)(frame - dlinfo.dli_saddr));
	if (size >= 0) size += tb_snprintf((tb_char_t*)symbols + size, maxn - size, ") [%p]", frame);
	if (size >= 0) ((tb_char_t*)symbols)[size] = '\0';

	// ok
	return symbols;
}
tb_void_t tb_backtrace_symbols_exit(tb_handle_t symbols)
{
	// exit symbols
	if (symbols) free(symbols);
}
#endif
