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
 * Copyright (C) 2009-present, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        process.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#if defined(__MINGW32__) || defined(__MINGW64__)
// enable UpdateProcThreadAttribute
#   undef _WIN32_WINNT
#   define _WIN32_WINNT 0x0600
#endif
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
    STARTUPINFOEX           si;
    STARTUPINFO*            psi;

    // the process info
    PROCESS_INFORMATION     pi;

    // the file handles
    HANDLE                  file_handles[3];
    DWORD                   file_handles_count;

    // the stdin redirect type
    tb_uint16_t             intype;

    // the stdout redirect type
    tb_uint16_t             outtype;

    // the stderr redirect type
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

// the global process group
static HANDLE g_process_group = tb_null;

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
    if (process->pi.hThread && process->pi.hThread != INVALID_HANDLE_VALUE)
        CloseHandle(process->pi.hThread);
    process->pi.hThread = INVALID_HANDLE_VALUE;

    // close process handle
    if (process->pi.hProcess && process->pi.hProcess != INVALID_HANDLE_VALUE)
        CloseHandle(process->pi.hProcess);
    process->pi.hProcess = INVALID_HANDLE_VALUE;

    // exit file handles
    for (tb_size_t i = 0; i < process->file_handles_count; i++)
    {
        HANDLE handle = process->file_handles[i];
        if (handle && handle != INVALID_HANDLE_VALUE)
        {
            tb_file_exit((tb_file_ref_t)handle);
            process->file_handles[i] = INVALID_HANDLE_VALUE;
        }
    }
    process->file_handles_count = 0;

    // reset std handles
    process->psi->hStdInput = INVALID_HANDLE_VALUE;
    process->psi->hStdOutput = INVALID_HANDLE_VALUE;
    process->psi->hStdError = INVALID_HANDLE_VALUE;
}
tb_bool_t tb_process_group_init()
{
    if (!g_process_group)
    {
        // create process job
        g_process_group = tb_kernel32()->CreateJobObjectW(tb_null, tb_null);
        tb_assert_and_check_return_val(g_process_group && g_process_group != INVALID_HANDLE_VALUE, tb_false);

        // set job limits, kill all processes on job when the job is destroyed.
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION job_limits;
        memset(&job_limits, 0, sizeof(job_limits));
        job_limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE | JOB_OBJECT_LIMIT_BREAKAWAY_OK;
        tb_kernel32()->SetInformationJobObject(g_process_group, JobObjectExtendedLimitInformation, &job_limits, sizeof(job_limits));

    }
    return g_process_group != tb_null;
}
tb_void_t tb_process_group_exit()
{
    // kill all processes on job
    if (g_process_group)
        tb_kernel32()->TerminateJobObject(g_process_group, 0);
}
static tb_void_t tb_process_args_append(tb_string_ref_t result, tb_char_t const* cstr)
{
    // need wrap quote?
    tb_char_t ch;
    tb_char_t const* p = cstr;
    tb_bool_t wrap_quote = tb_false;
    while ((ch = *p))
    {
        if (ch == ' ' || ch == '(' || ch == ')') wrap_quote = tb_true;
        p++;
    }

    // wrap begin quote
    if (wrap_quote) tb_string_chrcat(result, '\"');

    // escape characters
    p = cstr;
    while ((ch = *p))
    {
        // escape '"' or '\\'
        if (ch == '\"' || (wrap_quote && ch == '\\'))
            tb_string_chrcat(result, '\\');
        tb_string_chrcat(result, ch);
        p++;
    }

    // wrap end quote
    if (wrap_quote) tb_string_chrcat(result, '\"');
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
            tb_bool_t first = tb_true;
            tb_char_t const* p = tb_null;
            while ((p = *argv++))
            {
                if (first) first = tb_false;
                else tb_string_chrcat(&args, ' ');
                tb_process_args_append(&args, p);
            }
        }
        // only path name?
        else tb_process_args_append(&args, pathname);

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
    tb_wchar_t*     environment = tb_null;
    tb_bool_t       userenv     = tb_false;
    tb_wchar_t*     command     = tb_null;
    LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList = tb_null;
    tb_bool_t                    lpAttributeListInited = tb_false;
    do
    {
        // make process
        process = tb_malloc0_type(tb_process_t);
        tb_assert_and_check_break(process);

        // init startup info
        process->psi = &process->si.StartupInfo;
        process->psi->cb = sizeof(process->si);

        // save the user private data
        if (attr) process->priv = attr->priv;

        // detach process?
        tb_bool_t detach = attr && (attr->flags & TB_PROCESS_FLAG_DETACH);

        // init flags
        // see: https://learn.microsoft.com/en-us/windows/win32/procthread/process-creation-flags
        DWORD flags = CREATE_UNICODE_ENVIRONMENT;
        if (attr && attr->flags & TB_PROCESS_FLAG_NO_WINDOW) flags |= CREATE_NO_WINDOW;
        if (attr && attr->flags & TB_PROCESS_FLAG_SUSPEND) flags |= CREATE_SUSPENDED;
        if (!detach) flags |= CREATE_BREAKAWAY_FROM_JOB; // create process with parent process group by default, need set JOB_OBJECT_LIMIT_BREAKAWAY_OK limit for job

        // get the cmd size
        tb_size_t cmdn = tb_strlen(cmd);
        tb_assert_and_check_break(cmdn);

        // init unicode command
        command = tb_nalloc_type(cmdn + 1, tb_wchar_t);
        tb_assert_and_check_break(command);

        // make command
        tb_size_t size = tb_atow(command, cmd, cmdn + 1);
        tb_assert_and_check_break(size != -1);

        // init the current directory
        tb_wchar_t curdir[TB_PATH_MAXN];
        if (attr && attr->curdir)
        {
            tb_size_t size = tb_atow(curdir, attr->curdir, tb_arrayn(curdir));
            tb_assert_and_check_break(size != -1);
        }

        // reset size
        size = 0;

        // make environment
        tb_char_t const*    p = tb_null;
        tb_size_t           maxn = 0;
        tb_char_t const**   envp = attr? attr->envp : tb_null;
#ifdef TB_COMPILER_LIKE_UNIX
        /* we use unix environments on msys/cygwin,
         * because GetEnvironmentStringsW cannot get all envars
         *
         * TODO we need fix cmd path, stdout/stderr file handle and path to windows-style path for msys,cygwin/gcc
         */
        if (!envp) envp = (tb_char_t const**)environ;
#endif
        while (envp && (p = *envp++))
        {
            // get size
            tb_size_t n = tb_strlen(p);

            // ensure data space
            tb_size_t space = n * 3 / 2;
            if (!environment)
            {
                maxn = space + TB_PATH_MAXN;
                environment = tb_nalloc_type(maxn + 1, tb_wchar_t);
            }
            else if (size + space > maxn)
            {
                maxn = size + space + TB_PATH_MAXN;
                environment = tb_ralloc_type(environment, maxn + 1, tb_wchar_t);
            }
            tb_assert_and_check_break(environment);

            // append it
            tb_size_t real = tb_atow(environment + size, p, maxn - size);
            tb_assert_and_check_break(real != -1);

            // update size and fill '\0'
            size += real;
            environment[size] = L'\0';
            size++;
        }

        // end
        if (environment) environment[size++] = L'\0';
        // uses the current user environment if be null
        else
        {
            // get user environment
            environment = (tb_wchar_t*)tb_kernel32()->GetEnvironmentStringsW();

            // mark as the user environment
            userenv = tb_true;
        }

        // redirect
        HANDLE handlesToInherit[3];
        DWORD  handlesToInheritCount = 0;
        if (attr)
        {
            // redirect from stdin
            process->intype = attr->intype;
            if (attr->intype == TB_PROCESS_REDIRECT_TYPE_FILEPATH && attr->in.path)
            {
                // the inmode
                tb_size_t inmode = attr->inmode;

                // no mode? uses the default mode
                if (!inmode) inmode = TB_FILE_MODE_RO;

                // open file
                HANDLE hStdInput = (HANDLE)tb_file_init(attr->in.path, inmode);
                tb_assertf_pass_and_check_break(hStdInput, "cannot redirect stdin to file: %s", attr->in.path);

                // enable inherit
                tb_kernel32()->SetHandleInformation(hStdInput, HANDLE_FLAG_INHERIT, TRUE);
                handlesToInherit[handlesToInheritCount++] = hStdInput;
                process->file_handles[process->file_handles_count++] = hStdInput;
                process->psi->hStdInput = hStdInput;
            }
            else if ((attr->intype == TB_PROCESS_REDIRECT_TYPE_PIPE && attr->in.pipe) ||
                     (attr->intype == TB_PROCESS_REDIRECT_TYPE_FILE && attr->in.file))
            {
                // enable handles
                HANDLE hStdInput = attr->intype == TB_PROCESS_REDIRECT_TYPE_PIPE? tb_pipe_file_handle(attr->in.pipe) : (HANDLE)attr->in.file;

                // enable inherit
                tb_kernel32()->SetHandleInformation(hStdInput, HANDLE_FLAG_INHERIT, TRUE);
                handlesToInherit[handlesToInheritCount++] = hStdInput;
                process->psi->hStdInput = hStdInput;
            }

            // redirect to stdout
            process->outtype = attr->outtype;
            if (attr->outtype == TB_PROCESS_REDIRECT_TYPE_FILEPATH && attr->out.path)
            {
                // the outmode
                tb_size_t outmode = attr->outmode;

                // no mode? uses the default mode
                if (!outmode) outmode = TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC;

                // open file
                HANDLE hStdOutput = (HANDLE)tb_file_init(attr->out.path, outmode);
                tb_assertf_pass_and_check_break(hStdOutput, "cannot redirect stdout to file: %s", attr->out.path);

                // enable inherit
                tb_kernel32()->SetHandleInformation(hStdOutput, HANDLE_FLAG_INHERIT, TRUE);
                handlesToInherit[handlesToInheritCount++] = hStdOutput;
                process->file_handles[process->file_handles_count++] = hStdOutput;
                process->psi->hStdOutput = hStdOutput;
            }
            else if ((attr->outtype == TB_PROCESS_REDIRECT_TYPE_PIPE && attr->out.pipe) ||
                     (attr->outtype == TB_PROCESS_REDIRECT_TYPE_FILE && attr->out.file))
            {
                // enable handles
                HANDLE hStdOutput = attr->outtype == TB_PROCESS_REDIRECT_TYPE_PIPE? tb_pipe_file_handle(attr->out.pipe) : (HANDLE)attr->out.file;

                // enable inherit
                tb_kernel32()->SetHandleInformation(hStdOutput, HANDLE_FLAG_INHERIT, TRUE);
                handlesToInherit[handlesToInheritCount++] = hStdOutput;
                process->psi->hStdOutput = hStdOutput;
            }

            // redirect to stderr
            process->errtype = attr->errtype;
            if (attr->errtype == TB_PROCESS_REDIRECT_TYPE_FILEPATH && attr->err.path)
            {
                // the errmode
                tb_size_t errmode = attr->errmode;

                // no mode? uses the default mode
                if (!errmode) errmode = TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC;

                // open file
                HANDLE hStdError = (HANDLE)tb_file_init(attr->err.path, errmode);
                tb_assertf_pass_and_check_break(hStdError, "cannot redirect stderr to file: %s", attr->err.path);

                // enable inherit
                tb_kernel32()->SetHandleInformation(hStdError, HANDLE_FLAG_INHERIT, TRUE);
                handlesToInherit[handlesToInheritCount++] = hStdError;
                process->file_handles[process->file_handles_count++] = hStdError;
                process->psi->hStdError = hStdError;
            }
            else if ((attr->errtype == TB_PROCESS_REDIRECT_TYPE_PIPE && attr->err.pipe) ||
                     (attr->errtype == TB_PROCESS_REDIRECT_TYPE_FILE && attr->err.file))
            {
                // enable handles
                HANDLE hStdError = attr->errtype == TB_PROCESS_REDIRECT_TYPE_PIPE? tb_pipe_file_handle(attr->err.pipe) : (HANDLE)attr->err.file;

                // we need duplicate it if output to stdout/stderr pipes in same time
                if (attr->outtype == TB_PROCESS_REDIRECT_TYPE_PIPE && attr->out.pipe &&
                    attr->errtype == TB_PROCESS_REDIRECT_TYPE_PIPE && attr->err.pipe &&
                    attr->out.pipe == attr->err.pipe)
                {
                    HANDLE hStdOutput = hStdError;
                    if (!DuplicateHandle(GetCurrentProcess(), hStdOutput, GetCurrentProcess(), &hStdError, 0, TRUE, DUPLICATE_SAME_ACCESS))
                        break;
                    process->file_handles[process->file_handles_count++] = hStdError;
                }

                // enable inherit
                tb_kernel32()->SetHandleInformation(hStdError, HANDLE_FLAG_INHERIT, TRUE);
                handlesToInherit[handlesToInheritCount++] = hStdError;
                process->psi->hStdError = hStdError;
            }
        }

        /* we just inherit the given handles
         *
         * @see https://github.com/xmake-io/xmake/issues/2902#issuecomment-1326934902
         */
        BOOL bInheritHandle = handlesToInheritCount > 0;
        if (bInheritHandle && tb_kernel32()->InitializeProcThreadAttributeList)
        {
            SIZE_T attributeListSize = 0;
            if (tb_kernel32()->InitializeProcThreadAttributeList(tb_null, 1, 0, &attributeListSize) ||
                GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                lpAttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)tb_malloc(attributeListSize);
                if (lpAttributeList && tb_kernel32()->InitializeProcThreadAttributeList(lpAttributeList, 1, 0, &attributeListSize))
                {
                    lpAttributeListInited = tb_true;
                    if (tb_kernel32()->UpdateProcThreadAttribute(lpAttributeList, 0,
                            PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
                            handlesToInherit,
                            handlesToInheritCount * sizeof(HANDLE), tb_null, tb_null))
                    {
                        process->si.lpAttributeList = lpAttributeList;
                        flags |= EXTENDED_STARTUPINFO_PRESENT;
                    }
                }
            }
        }

        /* we just use the default std handles if lpAttributeList is not supported
         *
         * @see https://github.com/xmake-io/xmake/issues/3138#issuecomment-1338970250
         */
        if (bInheritHandle)
            process->psi->dwFlags |= STARTF_USESTDHANDLES;

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
        if (!tb_kernel32()->CreateProcessW(tb_null,
                command,
                &sap,
                &sat,
                bInheritHandle,
                flags,
                (LPVOID)environment,
                attr && attr->curdir? curdir : tb_null,
                process->psi,
                &process->pi))
        {
            /* It maybe fails because inside some sessions all user processes belong to a system-created job object named like
             * "\Sessions\x\BaseNamedObjects\Winlogon Job x-xxxxxxxx" (including rdpinit.exe and rdpshell.exe processes),
             * and this job doesn't allow their procceses to escape via CREATE_BREAKAWAY_FROM_JOB flag (it doesn't have the JOB_OBJECT_LIMIT_BREAKAWAY_OK limit/right set).
             *
             * we attempt to remove CREATE_BREAKAWAY_FROM_JOB flag and try to run it again
             */
            if ((flags & CREATE_BREAKAWAY_FROM_JOB) && GetLastError() == ERROR_ACCESS_DENIED)
            {
                flags &= ~CREATE_BREAKAWAY_FROM_JOB;
                if (!tb_kernel32()->CreateProcessW(tb_null,
                        command,
                        &sap,
                        &sat,
                        bInheritHandle,
                        flags,
                        (LPVOID)environment,
                        attr && attr->curdir? curdir : tb_null,
                        process->psi,
                        &process->pi))
                    break;
            }
            else break;
        }

        // attach this process to the parent process group by default
        if (g_process_group && !detach)
            tb_kernel32()->AssignProcessToJobObject(g_process_group, process->pi.hProcess);

        // check it
        tb_assert_and_check_break(process->pi.hThread != INVALID_HANDLE_VALUE);
        tb_assert_and_check_break(process->pi.hProcess != INVALID_HANDLE_VALUE);

        // ok
        ok = tb_true;

    } while (0);

    // exit attributes list
    if (lpAttributeList)
    {
        if (lpAttributeListInited)
            tb_kernel32()->DeleteProcThreadAttributeList(lpAttributeList);
        tb_free(lpAttributeList);
    }
    lpAttributeList = tb_null;

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

