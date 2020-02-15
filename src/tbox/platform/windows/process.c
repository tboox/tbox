/*!The Treasure Box Library
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * Copyright (C) 2009-2020, TBOOX Open Source Group.
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
#if defined(TB_CONFIG_MODULE_HAVE_COROUTINE) \
        && !defined(TB_CONFIG_MICRO_ENABLE)
#   include "../../coroutine/coroutine.h"
#   include "../../coroutine/impl/impl.h"
#endif

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

    /// the stdout redirect type
    tb_uint16_t             outtype;

    /// the stderr redirect type
    tb_uint16_t             errtype;

    // the user private data
    tb_cpointer_t           priv;

}tb_process_t; 

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
__tb_extern_c_enter__
HANDLE      tb_pipe_file_handle(tb_pipe_file_ref_t file);
HANDLE      tb_process_handle(tb_process_ref_t self);
tb_void_t   tb_process_handle_close(tb_process_ref_t self);
__tb_extern_c_leave__

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */
// the user environment
#ifdef TB_COMPILER_LIKE_UNIX
extern tb_char_t** environ;
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
HANDLE tb_process_handle(tb_process_ref_t self)
{
    // check
    tb_process_t* process = (tb_process_t*)self;
    tb_assert_and_check_return_val(process, tb_null);

    return process->pi.hProcess != INVALID_HANDLE_VALUE? process->pi.hProcess : tb_null;
}
tb_void_t tb_process_handle_close(tb_process_ref_t self)
{
    // check
    tb_process_t* process = (tb_process_t*)self;
    tb_assert_and_check_return(process);

    // close thread handle
    if (process->pi.hThread != INVALID_HANDLE_VALUE)
        CloseHandle(process->pi.hThread);
    process->pi.hThread = INVALID_HANDLE_VALUE;

    // close process handle
    if (process->pi.hProcess != INVALID_HANDLE_VALUE)
        CloseHandle(process->pi.hProcess);
    process->pi.hProcess = INVALID_HANDLE_VALUE;

    // exit stdout file
    if (process->outtype == TB_PROCESS_REDIRECT_TYPE_FILEPATH && process->si.hStdOutput && process->si.hStdOutput != INVALID_HANDLE_VALUE) 
        tb_file_exit((tb_file_ref_t)process->si.hStdOutput);
    process->si.hStdOutput = INVALID_HANDLE_VALUE;

    // exit stderr file
    if (process->errtype == TB_PROCESS_REDIRECT_TYPE_FILEPATH && process->si.hStdError && process->si.hStdError != INVALID_HANDLE_VALUE) 
        tb_file_exit((tb_file_ref_t)process->si.hStdError);
    process->si.hStdError = INVALID_HANDLE_VALUE;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_process_ref_t tb_process_init(tb_char_t const* pathname, tb_char_t const* argv[], tb_process_attr_ref_t attr)
{
    // check
    tb_assert_and_check_return_val(pathname || argv, tb_null);

    tb_string_t         args;
    tb_process_ref_t    process = tb_null;
    do
    {
        // init args
        if (!tb_string_init(&args)) break;

        // make arguments
        if (argv)
        {
            tb_char_t ch;
            tb_char_t const* p = tb_null;
            tb_char_t arg[8192];
            while ((p = *argv++)) 
            {
                // escape argument block
                tb_size_t i = 0;
                tb_size_t m = tb_arrayn(arg);
                tb_bool_t wrap_quote = tb_false;
                while ((ch = *p) && i < m)
                {
                    if (ch == '\"') 
                    {
                        if (i < m) arg[i++] = '\\';
                        wrap_quote = tb_true;
                    }
                    else if (ch == ' ') wrap_quote = tb_true;
                    if (i < m) arg[i++] = ch;
                    p++;
                }
                tb_assert_and_check_break(i < m);
                arg[i] = '\0';

                // wrap "arg" if exists escape characters and spaces?
                if (wrap_quote) tb_string_chrcat(&args, '\"');
                if (i) tb_string_cstrncat(&args, arg, i);
                if (wrap_quote) tb_string_chrcat(&args, '\"');
                
                // add space 
                tb_string_chrcat(&args, ' ');
            }
        }
        // only path name?
        else tb_string_cstrcpy(&args, pathname);

        // init process
        process = tb_process_init_cmd(tb_string_cstr(&args), attr);

    } while (0);

    // exit arguments
    tb_string_exit(&args);

    // ok?
    return process;
}
tb_process_ref_t tb_process_init_cmd(tb_char_t const* cmd, tb_process_attr_ref_t attr)
{
    // check
    tb_assert_and_check_return_val(cmd, tb_null);

    tb_bool_t       ok          = tb_false;
    tb_process_t*   process     = tb_null;
    tb_char_t*      environment = tb_null;
    tb_bool_t       userenv     = tb_false;
    tb_wchar_t*     command     = tb_null;
    do
    {
        // make process
        process = tb_malloc0_type(tb_process_t);
        tb_assert_and_check_break(process);

        // init startup info
        process->si.cb = sizeof(process->si);

        // save the user private data
        if (attr) process->priv = attr->priv;

        // init flags
        DWORD flags = 0;
        if (attr && attr->flags & TB_PROCESS_FLAG_SUSPEND) flags |= CREATE_SUSPENDED;
//        if (attr && attr->envp) flags |= CREATE_UNICODE_ENVIRONMENT;

        // get the cmd size
        tb_size_t cmdn = tb_strlen(cmd);
        tb_assert_and_check_break(cmdn);

        // init unicode command 
        command = tb_nalloc_type(cmdn + 1, tb_wchar_t);
        tb_assert_and_check_break(command);

        // make command
        tb_size_t size = tb_atow(command, cmd, cmdn + 1);
        tb_assert_and_check_break(size != -1);

        // reset size
        size = 0;

        // make environment
        tb_char_t const*    p = tb_null;
        tb_size_t           maxn = 0;
        tb_char_t const**   envp = attr? attr->envp : tb_null;
#ifdef TB_COMPILER_LIKE_UNIX
        // we use unix environments on msys/cygwin, because GetEnvironmentStringsW cannot get all envars
        if (!envp) envp = (tb_char_t const**)environ;
#endif
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
        BOOL bInheritHandle = FALSE;
        if (attr)
        {
            process->outtype = attr->outtype;
            process->errtype = attr->errtype;
            if (attr->outtype == TB_PROCESS_REDIRECT_TYPE_FILEPATH && attr->outpath)
            {
                // the outmode
                tb_size_t outmode = attr->outmode;

                // no mode? uses the default mode
                if (!outmode) outmode = TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC;

                // enable handles
                process->si.dwFlags |= STARTF_USESTDHANDLES;

                // open file
                process->si.hStdOutput = (HANDLE)tb_file_init(attr->outpath, outmode);
                tb_assertf_pass_and_check_break(process->si.hStdOutput, "cannot redirect stdout to file: %s", attr->outpath);

                // enable inherit
                tb_kernel32()->SetHandleInformation(process->si.hStdOutput, HANDLE_FLAG_INHERIT, TRUE);
                bInheritHandle = TRUE;
            }
            else if ((attr->outtype == TB_PROCESS_REDIRECT_TYPE_PIPE && attr->outpipe) ||
                     (attr->outtype == TB_PROCESS_REDIRECT_TYPE_FILE && attr->outfile))
            {
                // enable handles
                process->si.dwFlags |= STARTF_USESTDHANDLES;
                process->si.hStdOutput = attr->outtype == TB_PROCESS_REDIRECT_TYPE_PIPE? tb_pipe_file_handle(attr->outpipe) : (HANDLE)attr->outfile;

                // enable inherit
                tb_kernel32()->SetHandleInformation(process->si.hStdOutput, HANDLE_FLAG_INHERIT, TRUE);
                bInheritHandle = TRUE;
            }

            // redirect the stderr
            if (attr->errtype == TB_PROCESS_REDIRECT_TYPE_FILEPATH && attr->errpath)
            {
                // the errmode
                tb_size_t errmode = attr->errmode;

                // no mode? uses the default mode
                if (!errmode) errmode = TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC;

                // enable handles
                process->si.dwFlags |= STARTF_USESTDHANDLES;

                // open file
                process->si.hStdError = (HANDLE)tb_file_init(attr->errpath, errmode);
                tb_assertf_pass_and_check_break(process->si.hStdError, "cannot redirect stderr to file: %s", attr->errpath);

                // enable inherit
                tb_kernel32()->SetHandleInformation(process->si.hStdError, HANDLE_FLAG_INHERIT, TRUE);
                bInheritHandle = TRUE;
            }
            else if ((attr->errtype == TB_PROCESS_REDIRECT_TYPE_PIPE && attr->errpipe) ||
                     (attr->errtype == TB_PROCESS_REDIRECT_TYPE_FILE && attr->errfile))
            {
                // enable handles
                process->si.dwFlags |= STARTF_USESTDHANDLES;
                process->si.hStdError = attr->errtype == TB_PROCESS_REDIRECT_TYPE_PIPE? tb_pipe_file_handle(attr->errpipe) : (HANDLE)attr->errfile;

                // enable inherit
                tb_kernel32()->SetHandleInformation(process->si.hStdError, HANDLE_FLAG_INHERIT, TRUE);
                bInheritHandle = TRUE;
            }
        }

        // init process security attributes
        SECURITY_ATTRIBUTES sap     = {0};
        sap.nLength                 = sizeof(SECURITY_ATTRIBUTES);
        sap.lpSecurityDescriptor    = tb_null;
        sap.bInheritHandle          = bInheritHandle;

        // init thread security attributes
        SECURITY_ATTRIBUTES sat     = {0};
        sat.nLength                 = sizeof(SECURITY_ATTRIBUTES);
        sat.lpSecurityDescriptor    = tb_null;
        sat.bInheritHandle          = bInheritHandle;

        // create process
        if (!tb_kernel32()->CreateProcessW(tb_null, command, &sap, &sat, bInheritHandle, flags, (LPVOID)environment, tb_null, &process->si, &process->pi))
            break;

        // check it
        tb_assert_and_check_break(process->pi.hThread != INVALID_HANDLE_VALUE);
        tb_assert_and_check_break(process->pi.hProcess != INVALID_HANDLE_VALUE);

        // ok
        ok = tb_true;

    } while (0);

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

    // exit command 
    if (command) tb_free(command);
    command = tb_null;

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

#ifdef TB_CONFIG_MODULE_HAVE_COROUTINE
    // attempt to cancel waiting from coroutine first
    tb_pointer_t scheduler_io = tb_null;
    tb_poller_object_t object;
    object.type     = TB_POLLER_OBJECT_PROC;
    object.ref.proc = self;
#   ifndef TB_CONFIG_MICRO_ENABLE
    if ((scheduler_io = tb_co_scheduler_io_self()) && tb_co_scheduler_io_cancel((tb_co_scheduler_io_ref_t)scheduler_io, &object)) {}
    else
#   endif
    if ((scheduler_io = tb_lo_scheduler_io_self()) && tb_lo_scheduler_io_cancel((tb_lo_scheduler_io_ref_t)scheduler_io, &object)) {}
#endif

    // close process handles
    tb_process_handle_close(self);

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
tb_cpointer_t tb_process_priv(tb_process_ref_t self)
{
    // check
    tb_process_t* process = (tb_process_t*)self;
    tb_assert_and_check_return_val(process, tb_null);

    return process->priv;
}
tb_void_t tb_process_priv_set(tb_process_ref_t self, tb_cpointer_t priv)
{
    // check
    tb_process_t* process = (tb_process_t*)self;
    tb_assert_and_check_return(process);

    process->priv = priv;
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

#if defined(TB_CONFIG_MODULE_HAVE_COROUTINE) \
        && !defined(TB_CONFIG_MICRO_ENABLE)
    // attempt to wait it in coroutine if timeout is non-zero
    if (timeout && tb_coroutine_self()) 
    {
        tb_poller_object_t object;
        object.type = TB_POLLER_OBJECT_PROC;
        object.ref.proc = self;
        return tb_coroutine_waitproc(&object, pstatus, timeout);
    }
#endif

    // wait it
    tb_long_t   ok = -1;
    DWORD       result = WaitForSingleObject(process->pi.hProcess, timeout < 0? INFINITE : (DWORD)timeout);
    switch (result)
    {
    case WAIT_OBJECT_0: // ok
        {
            // save exit code
            DWORD exitcode = 0;
            if (pstatus) *pstatus = tb_kernel32()->GetExitCodeProcess(process->pi.hProcess, &exitcode)? (tb_long_t)exitcode : -1;  

            // close process handles
            tb_process_handle_close(self);

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
tb_long_t tb_process_waitlist(tb_process_ref_t const* processes, tb_process_waitinfo_ref_t infolist, tb_size_t infomaxn, tb_long_t timeout)
{
    // check
    tb_assert_and_check_return_val(processes && infolist && infomaxn, -1);

    // make the process list
    tb_size_t               procsize = 0;
    HANDLE                  proclist[256] = {0};
    tb_process_t const**    pprocess = (tb_process_t const**)processes;
    for (; *pprocess && procsize < tb_arrayn(proclist); pprocess++, procsize++)
        proclist[procsize] = (*pprocess)->pi.hProcess;
    tb_assertf(procsize < tb_arrayn(proclist), "too much waited processes!");

    // wait processes
    DWORD       exitcode = 0;
    tb_long_t   infosize = 0;
    DWORD result = tb_kernel32()->WaitForMultipleObjects((DWORD)procsize, proclist, FALSE, timeout < 0? INFINITE : (DWORD)timeout);
    switch (result)
    {
    case WAIT_TIMEOUT:
        break;
    case WAIT_FAILED:
        return -1;
    default:
        {
            // the process index
            DWORD index = result - WAIT_OBJECT_0;

            // the process
            tb_process_t* process = (tb_process_t*)processes[index];
            tb_assert_and_check_return_val(process, -1);

            // save process info
            infolist[infosize].index    = (tb_int_t)index;
            infolist[infosize].process  = (tb_process_ref_t)process;
            infolist[infosize].status   = tb_kernel32()->GetExitCodeProcess(process->pi.hProcess, &exitcode)? (tb_int_t)exitcode : -1;  
            infosize++;

            // close process handles
            tb_process_handle_close((tb_process_ref_t)process);

            // next index
            index++;
            while (index < procsize)
            {
                // attempt to wait next process
                result = tb_kernel32()->WaitForMultipleObjects((DWORD)(procsize - index), proclist + index, FALSE, 0);
                switch (result)
                {
                case WAIT_TIMEOUT:
                    // no more, exit loop
                    index = (DWORD)procsize;
                    break;
                case WAIT_FAILED:
                    return -1;
                default:
                    {
                        // the process index
                        index += result - WAIT_OBJECT_0;

                        // the process
                        process = (tb_process_t*)processes[index];
                        tb_assert_and_check_return_val(process, -1);

                        // save process info
                        infolist[infosize].index    = index;
                        infolist[infosize].process  = (tb_process_ref_t)process;
                        infolist[infosize].status   = tb_kernel32()->GetExitCodeProcess(process->pi.hProcess, &exitcode)? (tb_long_t)exitcode : -1;  
                        infosize++;

                        // close process handles
                        tb_process_handle_close((tb_process_ref_t)process);

                        // next index
                        index++;
                    }
                    break;
                }
            }
        }
        break;
    }

    // ok?
    return infosize;
}
