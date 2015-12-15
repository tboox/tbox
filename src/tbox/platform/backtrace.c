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
 * @author      ruki
 * @file        backtrace.c
 * @ingroup     platform
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "backtrace"
#define TB_TRACE_MODULE_DEBUG               (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "backtrace.h"
#if defined(TB_CONFIG_OS_WINDOWS)
#   include "windows/backtrace.c"
#elif defined(TB_CONFIG_OS_ANDROID)
#   include "android/backtrace.c"
#else
#   include "libc/backtrace.c"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_void_t tb_backtrace_dump(tb_char_t const* prefix, tb_pointer_t* frames, tb_size_t nframe)
{
    // check
    tb_check_return(nframe < 256);

    // the frames
    tb_pointer_t frames_data[256] = {0};
    if (!frames)
    {
        nframe = tb_backtrace_frames(frames_data, nframe, 2);
        frames = frames_data;
    }

    // dump frames
    if (frames && nframe)
    {
        // init symbols
        tb_handle_t symbols = tb_backtrace_symbols_init(frames, nframe);
        if (symbols)
        {
            // walk
            tb_size_t i = 0;
            for (i = 0; i < nframe; i++)
            {
#if TB_CPU_BIT64
                tb_trace_i("%s[%016p]: %s", prefix? prefix : "", frames[i], tb_backtrace_symbols_name(symbols, frames, nframe, i));
#else
                tb_trace_i("%s[%08p]: %s", prefix? prefix : "", frames[i], tb_backtrace_symbols_name(symbols, frames, nframe, i));
#endif
            }
        
            // exit symbols
            tb_backtrace_symbols_exit(symbols);
        }
        else
        {
            // walk
            tb_size_t i = 0;
            for (i = 0; i < nframe; i++)
            {
#if TB_CPU_BIT64
                tb_trace_i("%s[%016p]", prefix? prefix : "", frames[i]);
#else
                tb_trace_i("%s[%08p]", prefix? prefix : "", frames[i]);
#endif              
            }
        }
    }
}
