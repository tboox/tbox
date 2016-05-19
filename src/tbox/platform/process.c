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
        defined(TB_CONFIG_POSIX_HAVE_POSIX_SPAWNP)
#   include "posix/process.c"
#elif defined(TB_CONFIG_POSIX_HAVE_WAITPID) && \
         (defined(TB_CONFIG_POSIX_HAVE_FORK) || defined(TB_CONFIG_POSIX_HAVE_VFORK)) && \
            (defined(TB_CONFIG_POSIX_HAVE_EXECVP) || defined(TB_CONFIG_POSIX_HAVE_EXECVPE)) 
#   include "posix/process.c"
#else
tb_process_ref_t tb_process_init(tb_char_t const* pathname, tb_char_t const* argv[], tb_process_attr_ref_t attr)
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
tb_process_ref_t tb_process_init_cmd(tb_char_t const* cmd, tb_process_attr_ref_t attr)
{
    // check
    tb_assert_and_check_return_val(cmd, tb_null);

    // done
    tb_process_ref_t process    = tb_null;
    tb_char_t const* argv[256]  = {tb_null};
    tb_char_t*       buffer     = tb_null;
    do
    {
        // make buffer
        tb_size_t maxn = TB_PATH_MAXN;
        buffer = (tb_char_t*)tb_malloc(maxn);
        tb_assert_and_check_break(buffer);

        // copy and translate command
        tb_char_t   ch;
        tb_size_t   i = 0;
        tb_size_t   j = 0;
        for (i = 0; j <= maxn && (ch = cmd[i]); i++)
        {
            // not enough? grow it
            if (j == maxn)
            {
                // grow it
                maxn    += TB_PATH_MAXN;
                buffer  = (tb_char_t*)tb_ralloc(buffer, maxn);
                tb_assert_and_check_break(buffer);
            }

            // copy it and translate '\\'
            if (ch != '\\') buffer[j++] = ch;
        }
        tb_assert_and_check_break(j < maxn);
        buffer[j] = '\0';

        // reset index
        i = 0;

        // parse command to the arguments
        tb_bool_t   s = 0;
        tb_size_t   m = tb_arrayn(argv);
        tb_char_t*  p = buffer;
        tb_char_t*  b = tb_null;
        while ((ch = *p))
        {
            // enter double quote?
            if (!s && ch == '\"') s = 2;
            // enter single quote?
            else if (!s && ch == '\'') s = 1;
            // leave quote?
            else if ((s == 2 && ch == '\"') || (s == 1 && ch == '\'')) s = 0;
            // is argument end with ' '?
            else if (!s && tb_isspace(ch))
            {
                // fill zero
                *p = '\0';

                // save this argument 
                if (b)
                {
                    // trace
                    tb_trace_d("argv: %s", b);

                    // save it
                    if (i < m - 1) argv[i++] = b;

                    // clear it
                    b = tb_null;
                }
            }

            // get the argument pointer
            if ((s || !tb_isspace(ch)) && !b) b = p;

            // next 
            p++;
        }

        // save this argument 
        if (b)
        {
            // trace
            tb_trace_d("argv: %s", b);

            // save it
            if (i < m - 1) argv[i++] = b;

            // clear it
            b = tb_null;
        }

        // check
        tb_assertf_and_check_break(i <= m - 1, "the command(%s) arguments are too much!", cmd);

        // init process
        process = tb_process_init(argv[0], argv, attr);
    
    } while (0);

    // exit buffer
    if (buffer) tb_free(buffer);
    buffer = tb_null;

    // ok?
    return process;
}
tb_long_t tb_process_run(tb_char_t const* pathname, tb_char_t const* argv[], tb_process_attr_ref_t attr)
{
    // remove suspend
    if (attr) attr->flags &= ~TB_PROCESS_FLAG_SUSPEND;

    // init process
    tb_long_t           ok = -1;
    tb_process_ref_t    process = tb_process_init(pathname, argv, attr);
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
tb_long_t tb_process_run_cmd(tb_char_t const* cmd, tb_process_attr_ref_t attr)
{
    // remove suspend
    if (attr) attr->flags &= ~TB_PROCESS_FLAG_SUSPEND;

    // init process
    tb_long_t           ok = -1;
    tb_process_ref_t    process = tb_process_init_cmd(cmd, attr);
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
