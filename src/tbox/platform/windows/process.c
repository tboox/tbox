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
 * includes
 */
#include "prefix.h"
#include "../process.h"
#include "../environment.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the process type
typedef struct __tb_process_t
{
    // the pid
    pid_t               pid;

}tb_process_t; 

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_process_ref_t tb_process_init(tb_char_t const* pathname, tb_char_t const* argv[], tb_char_t const* envp[], tb_bool_t suspend)
{
    // check
    tb_assert_and_check_return_val(pathname, tb_null);

    // done
    tb_bool_t       ok = tb_false;
    tb_process_t*   process = tb_null;
    do
    {
        // make process
        process = tb_malloc0_type(tb_process_t);
        tb_assert_and_check_break(process);

        // TODO
        tb_trace_noimpl();

        // check pid
        tb_assert_and_check_break(process->pid > 0);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (process) tb_process_exit((tb_process_ref_t)process);
        process = tb_null;
    }

    // ok?
    return (tb_process_ref_t)process;
}
tb_void_t tb_process_exit(tb_process_ref_t self)
{
    // check
    tb_process_t* process = (tb_process_t*)self;
    tb_assert_and_check_return(process);

    // the process has not exited?
    if (process->pid > 0)
    {
        // trace
        tb_trace_e("kill: %ld ..", process->pid);

        // kill it first
        tb_process_kill(self);

        // wait it again
        tb_process_wait(self, tb_null, -1);
    }

    // exit it
    tb_free(process);
}
tb_void_t tb_process_kill(tb_process_ref_t self)
{
    // check
    tb_process_t* process = (tb_process_t*)self;
    tb_assert_and_check_return(process);

    // kill it
    if (process->pid > 0)
    {
        // TODO
        tb_trace_noimpl();
    }
}
tb_void_t tb_process_resume(tb_process_ref_t self)
{
    // check
    tb_process_t* process = (tb_process_t*)self;
    tb_assert_and_check_return(process);

    // kill it
    if (process->pid > 0)
    {
        // TODO
        tb_trace_noimpl();
    }
}
tb_void_t tb_process_suspend(tb_process_ref_t self)
{
    // check
    tb_process_t* process = (tb_process_t*)self;
    tb_assert_and_check_return(process);

    // kill it
    if (process->pid > 0)
    {
        // TODO
        tb_trace_noimpl();
    }
}
tb_long_t tb_process_wait(tb_process_ref_t self, tb_long_t* pstatus, tb_long_t timeout)
{
    // check
    tb_process_t* process = (tb_process_t*)self;
    tb_assert_and_check_return_val(process, -1);

    // done
    tb_long_t ok = 0;
    tb_hong_t time = tb_mclock();
    do
    {
        // TODO
        tb_trace_noimpl();

        // wait some time
        tb_msleep(200);

    } while (timeout > 0 && tb_mclock() - time < (tb_hong_t)timeout);

    // ok?
    return ok;
}
