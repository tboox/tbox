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
#include <fcntl.h>
#include <errno.h>
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
    pid_t                       pid;

    // the attributes
    tb_process_attr_t           attr;

#ifdef TB_CONFIG_POSIX_HAVE_POSIX_SPAWNP
    // the spawn attributes
    posix_spawnattr_t           spawn_attr;

    // the spawn action
    posix_spawn_file_actions_t  spawn_action;
#else
    // the redirect stdout fd
    tb_int_t                    outfd;

    // the redirect stderr fd
    tb_int_t                    errfd;
#endif

}tb_process_t; 

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the user environment
extern tb_char_t**  environ;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_int_t tb_process_file_flags(tb_size_t mode)
{
    // no mode? uses the default mode
    if (!mode) mode = TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC;

    // make flags
    tb_size_t flags = 0;
    if (mode & TB_FILE_MODE_RO)         flags |= O_RDONLY;
    else if (mode & TB_FILE_MODE_WO)    flags |= O_WRONLY;
    else if (mode & TB_FILE_MODE_RW)    flags |= O_RDWR;
    if (mode & TB_FILE_MODE_CREAT)      flags |= O_CREAT;
    if (mode & TB_FILE_MODE_APPEND)     flags |= O_APPEND;
    if (mode & TB_FILE_MODE_TRUNC)      flags |= O_TRUNC;

    // ok?
    return flags;
}
static tb_int_t tb_process_file_modes(tb_size_t mode)
{
    // no mode? uses the default mode
    if (!mode) mode = TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC;

    // make modes
    tb_size_t modes = 0;
    if (mode & TB_FILE_MODE_CREAT) modes = 0777;

    // ok?
    return modes;
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if defined(TB_CONFIG_POSIX_HAVE_POSIX_SPAWNP)
tb_process_ref_t tb_process_init(tb_char_t const* pathname, tb_char_t const* argv[], tb_process_attr_ref_t attr)
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
        if (attr)
        {
            // save it
            process->attr = *attr;

            // do not save envp, maybe stack pointer
            process->attr.envp = tb_null;
        }

        // init spawn attributes
        posix_spawnattr_init(&process->spawn_attr);

        // init spawn action
        posix_spawn_file_actions_init(&process->spawn_action);

        // redirect the stdout
        if (attr && attr->outfile)
        {
            // open stdout
            tb_int_t result = posix_spawn_file_actions_addopen(&process->spawn_action, STDOUT_FILENO, attr->outfile, tb_process_file_flags(attr->outmode), tb_process_file_modes(attr->outmode));
            tb_assertf_pass_and_check_break(!result, "cannot redirect stdout to file: %s, error: %d", attr->outfile, result);
        }

        // redirect the stderr
        if (attr && attr->errfile)
        {
            // open stderr
            tb_int_t result = posix_spawn_file_actions_addopen(&process->spawn_action, STDERR_FILENO, attr->errfile, tb_process_file_flags(attr->errmode), tb_process_file_modes(attr->errmode));
            tb_assertf_pass_and_check_break(!result, "cannot redirect stderr to file: %s, error: %d", attr->errfile, result);
        }

        // suspend it first
        if (attr && attr->flags & TB_PROCESS_FLAG_SUSPEND)
        {
#ifdef POSIX_SPAWN_START_SUSPENDED
            posix_spawnattr_setflags(&process->spawn_attr, POSIX_SPAWN_START_SUSPENDED);
#else
            tb_assertf(!suspend, "suspend process not supported!");
#endif
        }

        // no given environment? uses the current user environment
        tb_char_t const** envp = attr? attr->envp : tb_null;
        if (!envp) envp = (tb_char_t const**)environ;

        // spawn the process
        tb_long_t status = posix_spawnp(&process->pid, pathname, &process->spawn_action, &process->spawn_attr, (tb_char_t* const*)argv, (tb_char_t* const*)envp);
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
tb_process_ref_t tb_process_init(tb_char_t const* pathname, tb_char_t const* argv[], tb_process_attr_ref_t attr)
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
        if (attr)
        {
            // save it
            process->attr = *attr;

            // do not save envp, maybe stack pointer
            process->attr.envp = tb_null;
        }

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
            tb_assertf(!attr || !(attr->flags & TB_PROCESS_FLAG_SUSPEND), "suspend process not supported!");

            // redirect the stdout
            if (attr && attr->outfile)
            {
                // open file
                process->outfd = open(attr->outfile, tb_process_file_flags(attr->outmode), tb_process_file_modes(attr->outmode));
                tb_assertf_pass_and_check_break(process->outfd, "cannot redirect stdout to file: %s, error: %d", attr->outfile, errno);

                // redirect it
                dup2(process->outfd, STDOUT_FILENO);
            }

            // redirect the stderr
            if (attr && attr->outfile)
            {
                // open file
                process->errfd = open(attr->errfile, tb_process_file_flags(attr->errmode), tb_process_file_modes(attr->errmode));
                tb_assertf_pass_and_check_break(process->errfd, "cannot redirect stderr to file: %s, error: %d", attr->errfile, errno);

                // redirect it
                dup2(process->errfd, STDOUT_FILENO);
            }

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
            tb_char_t const** envp = attr? attr->envp : tb_null;
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
                        tb_environment_set(name, values);
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

#ifdef TB_CONFIG_POSIX_HAVE_POSIX_SPAWNP

    // close the stdout
    if (process->attr.outfile) posix_spawn_file_actions_addclose(&process->spawn_action, STDOUT_FILENO);
    process->attr.outfile = tb_null;

    // close the stderr
    if (process->attr.errfile) posix_spawn_file_actions_addclose(&process->spawn_action, STDERR_FILENO);
    process->attr.errfile = tb_null;

    // exit spawn attributes
    posix_spawnattr_destroy(&process->spawn_attr);

    // exit spawn action 
    posix_spawn_file_actions_destroy(&process->spawn_action);
#else

    // close stdout fd
    if (process->outfd) close(process->outfd);
    process->outfd = 0;

    // close stderr fd
    if (process->errfd) close(process->errfd);
    process->errfd = 0;
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
