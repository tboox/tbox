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
#include "../path.h"
#include "../file.h"
#include "../process.h"
#include "../environment.h"
#include "../../string/string.h"
#include "interface/interface.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the process type
typedef struct __tb_process_t
{
    // the startup info
    STARTUPINFO             si;

    // the process info
    PROCESS_INFORMATION     pi;

    // the attributes
    tb_process_attr_t       attr;

}tb_process_t; 

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_process_ref_t tb_process_init(tb_char_t const* pathname, tb_char_t const* argv[], tb_process_attr_ref_t attr)
{
    // check
    tb_assert_and_check_return_val(pathname, tb_null);

    // done
    tb_bool_t       ok          = tb_false;
    tb_process_t*   process     = tb_null;
    tb_char_t*      environment = tb_null;
    tb_bool_t       userenv     = tb_false;
    tb_string_t     args;
    do
    {
        // make process
        process = tb_malloc0_type(tb_process_t);
        tb_assert_and_check_break(process);

        // init startup info
        process->si.cb = sizeof(process->si);

        // init args
        if (!tb_string_init(&args)) break;

        // init attributes
        if (attr)
        {
            // save it
            process->attr = *attr;

            // do not save envp, maybe stack pointer
            process->attr.envp = tb_null;
        }

        // make arguments
        tb_char_t const* p = tb_null;
        while ((p = *argv++)) 
        {
            tb_string_cstrcat(&args, p);
            tb_string_cstrcat(&args, " ");
        }

        // make command
        tb_wchar_t       cmd[TB_PATH_MAXN];
        tb_char_t const* cstr = tb_string_cstr(&args);
        tb_size_t size = tb_atow(cmd, cstr, tb_arrayn(cmd));
        if (size < tb_arrayn(cmd)) cmd[size] = L'\0';

        // init flags
        DWORD flags = 0;
        if (attr && attr->flags & TB_PROCESS_FLAG_SUSPEND) flags |= CREATE_SUSPENDED;
//        if (attr && attr->envp) flags |= CREATE_UNICODE_ENVIRONMENT;

        // reset size
        size = 0;

        // FIXME no effect
        // make environment
        tb_size_t           maxn = 0;
        tb_char_t const**   envp = attr? attr->envp : tb_null;
        while (envp && (p = *envp++))
        {
            // get size
            tb_size_t n = tb_strlen(p);

            // ensure data space
            if (!environment) 
            {
                maxn = n + 2 + TB_PATH_MAXN;
                environment = (tb_char_t*)tb_malloc(maxn);
            }
            else if (size + n + 2 > maxn)
            {
                maxn = size + n + 2 + TB_PATH_MAXN;
                environment = (tb_char_t*)tb_ralloc(environment, maxn);
            }
            tb_assert_and_check_break(environment);

            // append it
            tb_memcpy(environment + size, p, n);

            // fill '\0'
            environment[size + n] = '\0'; 

            // update size
            size += n + 1;
        }

        // end
        if (environment) environment[size++] = '\0';
        // uses the current user environment if be null
        else
        {
            // uses the unicode environment
            flags |= CREATE_UNICODE_ENVIRONMENT;

            // get user environment
            environment = (tb_char_t*)tb_kernel32()->GetEnvironmentStringsW();

            // mark as the user environment
            userenv = tb_true;
        }

        // redirect the stdout
        if (attr && attr->outfile)
        {
            // the outmode
            tb_size_t outmode = attr->outmode;

            // no mode? uses the default mode
            if (!outmode) outmode = TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC;

            // enable handles
            process->si.dwFlags |= STARTF_USESTDHANDLES;

            // open file
            process->si.hStdOutput = (HANDLE)tb_file_init(attr->outfile, outmode);
            tb_assertf_pass_and_check_break(process->si.hStdOutput, "cannot redirect stdout to file: %s", attr->outfile);
        }

        // redirect the stderr
        if (attr && attr->errfile)
        {
            // the errmode
            tb_size_t errmode = attr->errmode;

            // no mode? uses the default mode
            if (!errmode) errmode = TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC;

            // enable handles
            process->si.dwFlags |= STARTF_USESTDHANDLES;

            // open file
            process->si.hStdError = (HANDLE)tb_file_init(attr->errfile, errmode);
            tb_assertf_pass_and_check_break(process->si.hStdError, "cannot redirect stderr to file: %s", attr->errfile);
        }

        // create process
        if (!tb_kernel32()->CreateProcessW(tb_null, cmd, tb_null, tb_null, FALSE, flags, (LPVOID)environment, tb_null, &process->si, &process->pi))
            break;

        // check it
        tb_assert_and_check_break(process->pi.hThread != INVALID_HANDLE_VALUE);
        tb_assert_and_check_break(process->pi.hProcess != INVALID_HANDLE_VALUE);

        // ok
        ok = tb_true;

    } while (0);

    // exit arguments
    tb_string_exit(&args);

    // uses the user environment?
    if (userenv)
    {
        // exit it
        if (environment) tb_kernel32()->FreeEnvironmentStringsW((LPWCH)environment);
        environment = tb_null;
    }
    else
    {
        // exit it
        if (environment) tb_free(environment);
        environment = tb_null;
    }

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
    if (process->pi.hProcess != INVALID_HANDLE_VALUE)
    {
        // trace
        tb_trace_e("kill: %u ..", process->pi.dwProcessId);

        // kill it first
        tb_process_kill(self);

        // wait it again
        tb_process_wait(self, tb_null, -1);
    }

    // close thread handle
    if (process->pi.hThread != INVALID_HANDLE_VALUE)
        tb_kernel32()->CloseHandle(process->pi.hThread);
    process->pi.hThread = INVALID_HANDLE_VALUE;

    // close process handle
    if (process->pi.hProcess != INVALID_HANDLE_VALUE)
        tb_kernel32()->CloseHandle(process->pi.hProcess);
    process->pi.hProcess = INVALID_HANDLE_VALUE;

    // exit stdout file
    if (process->si.hStdOutput) tb_file_exit((tb_file_ref_t)process->si.hStdOutput);
    process->si.hStdOutput = tb_null;

    // exit stderr file
    if (process->si.hStdError) tb_file_exit((tb_file_ref_t)process->si.hStdError);
    process->si.hStdError = tb_null;

    // exit it
    tb_free(process);
}
tb_void_t tb_process_kill(tb_process_ref_t self)
{
    // check
    tb_process_t* process = (tb_process_t*)self;
    tb_assert_and_check_return(process);

    // kill it
    if (process->pi.hProcess != INVALID_HANDLE_VALUE)
        tb_kernel32()->TerminateProcess(process->pi.hProcess, -1);
}
tb_void_t tb_process_resume(tb_process_ref_t self)
{
    // check
    tb_process_t* process = (tb_process_t*)self;
    tb_assert_and_check_return(process);

    // resume it
    if (process->pi.hThread != INVALID_HANDLE_VALUE)
        tb_kernel32()->ResumeThread(process->pi.hThread);
}
tb_void_t tb_process_suspend(tb_process_ref_t self)
{
    // check
    tb_process_t* process = (tb_process_t*)self;
    tb_assert_and_check_return(process);

    // suspend it
    if (process->pi.hThread != INVALID_HANDLE_VALUE)
        tb_kernel32()->SuspendThread(process->pi.hThread);
}
tb_long_t tb_process_wait(tb_process_ref_t self, tb_long_t* pstatus, tb_long_t timeout)
{
    // check
    tb_process_t* process = (tb_process_t*)self;
    tb_assert_and_check_return_val(process && process->pi.hProcess != INVALID_HANDLE_VALUE && process->pi.hThread != INVALID_HANDLE_VALUE, -1);

    // wait it
    tb_long_t   ok = -1;
    DWORD       result = tb_kernel32()->WaitForSingleObject(process->pi.hProcess, timeout < 0? INFINITE : (DWORD)timeout);
    switch (result)
    {
    case WAIT_OBJECT_0: // ok
        {
            // save exit code
            DWORD dwExitCode;
            if (pstatus) *pstatus = tb_kernel32()->GetExitCodeProcess(process->pi.hProcess, &dwExitCode)? (tb_long_t)dwExitCode : -1;  

            // close thread handle
            tb_kernel32()->CloseHandle(process->pi.hThread);
            process->pi.hThread = INVALID_HANDLE_VALUE;

            // close process
            tb_kernel32()->CloseHandle(process->pi.hProcess);
            process->pi.hProcess = INVALID_HANDLE_VALUE;

            // ok
            ok = 1;
        }
        break;
    case WAIT_TIMEOUT: // timeout 
        ok = 0;
        break;
    case WAIT_FAILED: // failed
    default:
        break;
    }

    // ok?
    return ok;
}
