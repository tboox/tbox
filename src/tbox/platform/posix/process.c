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
#include <unistd.h>
#include <sys/wait.h>
#ifdef TB_CONFIG_POSIX_HAVE_POSIX_SPAWNP
#   include <spawn.h>
#endif
#ifdef TB_CONFIG_LIBC_HAVE_KILL
#   include <signal.h>
#   include <sys/types.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the process type
typedef struct __tb_process_t
{
    // the pid
    pid_t               pid;

#ifdef TB_CONFIG_POSIX_HAVE_POSIX_SPAWNP
    // the attributes
    posix_spawnattr_t   attr;
#endif

}tb_process_t; 

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the user environment
extern tb_char_t**  environ;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if defined(TB_CONFIG_POSIX_HAVE_POSIX_SPAWNP)
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

        // init attributes
        posix_spawnattr_init(&process->attr);

        // suspend it first
        if (suspend)
        {
#ifdef POSIX_SPAWN_START_SUSPENDED
            posix_spawnattr_setflags(&process->attr, POSIX_SPAWN_START_SUSPENDED);
#else
            tb_assertf(!suspend, "suspend process not supported!");
#endif
        }

        // no given environment? uses the current user environment
        if (!envp) envp = (tb_char_t const**)environ;

        // spawn the process
        tb_long_t status = posix_spawnp(&process->pid, pathname, tb_null, &process->attr, (tb_char_t* const*)argv, (tb_char_t* const*)envp);
        tb_check_break(status == 0);

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
#else
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

        // fork it
#if defined(TB_CONFIG_POSIX_HAVE_VFORK) && \
        defined(TB_CONFIG_POSIX_HAVE_EXECVPE)
        switch ((process->pid = vfork()))
#else
        switch ((process->pid = fork()))
#endif
        {
        case -1:

            // trace
            tb_trace_e("fork failed!");

            // exit it
            _exit(-1);

        case 0: 

            // TODO
            // check
            tb_assertf(!suspend, "suspend process not supported!");

#if defined(TB_CONFIG_POSIX_HAVE_EXECVPE)
            // no given environment? uses the current user environment
            if (!envp) envp = (tb_char_t const**)environ;

            // exec it in the child process
            execvpe(pathname, (tb_char_t* const*)argv, (tb_char_t* const*)envp);
#elif defined(TB_CONFIG_POSIX_HAVE_EXECVP)

            /* set environment variables
             *
             * uses fork because it will modify the parent environment
             */
            if (envp)
            {
                // done
                tb_char_t const* env = tb_null;
                while ((env = *envp++))
                {
                    // get name and values
                    tb_char_t const* p = tb_strchr(env, '=');
                    if (p)
                    {
                        // get name
                        tb_char_t name[256];
                        tb_size_t size = tb_min(p - env, sizeof(name) - 1);
                        tb_strncpy(name, env, size);
                        name[size] = '\0';

                        // get values
                        tb_char_t const* values = p + 1;

                        // set values to the environment
                        tb_environment_set(name, values, tb_false);
                    }
                }
            }

            // exec it in the child process
            execvp(pathname, (tb_char_t* const*)argv);
#else
#   error 
#endif
            // exit it
            _exit(-1);

        default:
            // parent
            break;
        }

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
#endif
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

    // exit attributes
#ifdef TB_CONFIG_POSIX_HAVE_POSIX_SPAWNP
    posix_spawnattr_destroy(&process->attr);
#endif

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
#ifdef TB_CONFIG_LIBC_HAVE_KILL
        kill(process->pid, SIGKILL);
#else
        // noimpl
        tb_trace_noimpl();
#endif
    }
}
tb_void_t tb_process_resume(tb_process_ref_t self)
{
    // check
    tb_process_t* process = (tb_process_t*)self;
    tb_assert_and_check_return(process);

    // resume it
    if (process->pid > 0)
    {
#ifdef TB_CONFIG_LIBC_HAVE_KILL
        kill(process->pid, SIGCONT);
#else
        // noimpl
        tb_trace_noimpl();
#endif
    }
}
tb_void_t tb_process_suspend(tb_process_ref_t self)
{
    // check
    tb_process_t* process = (tb_process_t*)self;
    tb_assert_and_check_return(process);

    // suspend it
    if (process->pid > 0)
    {
#ifdef TB_CONFIG_LIBC_HAVE_KILL
        kill(process->pid, SIGSTOP);
#else
        // noimpl
        tb_trace_noimpl();
#endif
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
        // wait it
        tb_int_t    status = -1;
        tb_long_t   result = waitpid(process->pid, &status, timeout < 0? 0 : WNOHANG | WUNTRACED);
        tb_check_return_val(result != -1, -1);

        // exited?
        if (result != 0)
        {
            /* save status, only get 8bits retval
             *
             * tt's limited to 8-bits, which means 1 byte, 
             * which means the int from WEXITSTATUS can only range from 0-255. 
             *
             * in fact, any unix program will only ever return a max of 255.
             */
            if (pstatus) *pstatus = WIFEXITED(status)? WEXITSTATUS(status) : -1;

            // clear pid
            process->pid = 0;

            // wait ok
            ok = 1;

            // end
            break;
        }

        // wait some time
        tb_msleep(200);

    } while (timeout > 0 && tb_mclock() - time < (tb_hong_t)timeout);

    // ok?
    return ok;
}
