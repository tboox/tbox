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
 * @file        process.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "process"
#define TB_TRACE_MODULE_DEBUG               (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "process.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef TB_CONFIG_OS_WINDOWS
#   include "windows/process.c"
#elif defined(TB_CONFIG_POSIX_HAVE_WAITPID) && \
    (defined(TB_CONFIG_POSIX_HAVE_POSIX_SPAWNP) || \
        (defined(TB_CONFIG_POSIX_HAVE_EXECVPE) && \
         defined(TB_CONFIG_POSIX_HAVE_FORK)))
#   include "posix/process.c"
#else
tb_process_ref_t tb_process_init(tb_char_t const* pathname, tb_char_t* const argv[], tb_char_t* const envp[], tb_bool_t suspend)
{
    tb_trace_noimpl();
    return tb_null;
}
tb_void_t tb_process_exit(tb_process_ref_t self)
{
    tb_trace_noimpl();
}
tb_void_t tb_process_kill(tb_process_ref_t self)
{
    tb_trace_noimpl();
}
tb_void_t tb_process_resume(tb_process_ref_t self);
{
    tb_trace_noimpl();
}
tb_void_t tb_process_suspend(tb_process_ref_t self)
{
    tb_trace_noimpl();
}
tb_long_t tb_process_wait(tb_process_ref_t self, tb_long_t* pstatus, tb_long_t timeout)
{
    tb_trace_noimpl();
    return 0;
}
#endif
tb_long_t tb_process_run(tb_char_t const* pathname, tb_char_t* const argv[], tb_char_t* const envp[])
{
    // init process
    tb_long_t           ok = -1;
    tb_process_ref_t    process = tb_process_init(pathname, argv, envp, tb_false);
    if (process)
    {
        // wait process
        tb_long_t status = 0;
        if (tb_process_wait(process, &status, -1) > 0)
            ok = status;

        // exit process
        tb_process_exit(process);
    }

    // ok?
    return ok;
}
