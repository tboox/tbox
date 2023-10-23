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
 * @file        process.h
 * @ingroup     platform
 *
 */
#ifndef TB_PLATFORM_PROCESS_H
#define TB_PLATFORM_PROCESS_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "pipe.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the process flag enum
typedef enum __tb_process_flag_e
{
    TB_PROCESS_FLAG_NONE    = 0
,   TB_PROCESS_FLAG_SUSPEND = 1     //!< suspend process
,   TB_PROCESS_FLAG_DETACH  = 2     //!< all subprocesses will be exited when the parent process is exited (ctrl+c or onexit) if this flag is not setted
,   TB_PROCESS_FLAG_NO_WINDOW = 4   //!< avoid to launch the console, currently only for windows when spawning command line in a GUI app

}tb_process_flag_e;

/// the process redirect type enum
typedef enum __tb_process_redirect_type_e
{
    TB_PROCESS_REDIRECT_TYPE_NONE       = 0
,   TB_PROCESS_REDIRECT_TYPE_FILEPATH   = 1     //!< redirect to file path
,   TB_PROCESS_REDIRECT_TYPE_FILE       = 2     //!< redirect to file
,   TB_PROCESS_REDIRECT_TYPE_PIPE       = 3     //!< redirect to pipe

}tb_process_redirect_type_e;

/// the process redirect io type
typedef union __tb_process_redirect_io_t
{
    tb_pipe_file_ref_t  pipe;
    tb_file_ref_t       file;
    tb_char_t const*    path;

}tb_process_redirect_io_t;

/// the process attribute type
typedef struct __tb_process_attr_t
{
    /// the flags
    tb_uint16_t                 flags;

    /// the stdin redirect type
    tb_uint16_t                 intype;

    /// the stdin file mode
    tb_uint16_t                 inmode;

    /// the stdout redirect type
    tb_uint16_t                 outtype;

    /*! the stdout file mode
     *
     * default: TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC
     *
     * support:
     *
     * - TB_FILE_MODE_WO
     * - TB_FILE_MODE_RW
     * - TB_FILE_MODE_CREAT
     * - TB_FILE_MODE_APPEND
     * - TB_FILE_MODE_TRUNC
     */
    tb_uint16_t                 outmode;

    /// the stderr redirect type
    tb_uint16_t                 errtype;

    /// the stderr file mode
    tb_uint16_t                 errmode;

    /// the stdin
    tb_process_redirect_io_t    in;

    /// the stdout
    tb_process_redirect_io_t    out;

    /// the stderr
    tb_process_redirect_io_t    err;

    /*! the environment
     *
     * @code

        tb_char_t const* envp[] =
        {
            "path=/usr/bin"
        ,   tb_null
        };

        attr.envp = envp;

     * @endcode
     *
     * the envp argument is an array of pointers to null-terminated strings
     * and must be terminated by a null pointer
     *
     * if the value of envp is null, then the child process inherits
     * the environment of the parent process.
     */
    tb_char_t const**           envp;

    // the given current working directory for child process
    tb_char_t const*            curdir;

    /// the user private data
    tb_cpointer_t               priv;

}tb_process_attr_t, *tb_process_attr_ref_t;

/// the process ref type
typedef __tb_typeref__(process);

/// the process wait info type
typedef struct __tb_process_waitinfo_t
{
    // the index of the processes
    tb_int_t                    index;

    // the status
    tb_int_t                    status;

    // the process
    tb_process_ref_t            process;

}tb_process_waitinfo_t, *tb_process_waitinfo_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! run a given process
 *
 * @code

    // init argv
    tb_char_t const* argv[] =
    {
        "echo"
    ,   "hello"
    ,   tb_null
    };

    // init envp
    tb_char_t const* envp[] =
    {
        "path=/usr/bin"
    ,   tb_null
    };

    // init attr
    tb_process_attr_t attr = {0};
    attr.envp = envp;

    // run bash
    if (tb_process_run("echo", argv, &attr) == 0)
    {
        // trace
        tb_trace_i("ok");
    }

    // run bash
    if (tb_process_run("/bin/echo", tb_null, tb_null) == 0)
    {
        // trace
        tb_trace_i("ok");
    }

 * @endcode
 *
 * @param pathname      the process path or name
 * @param argv          the list of arguments must be terminated by a null pointer
 *                      and must be terminated by a null pointer
 *                      and argv[0] is the self path name
 * @param attr          the process attributes
 *
 * @return              the status value, failed: -1, ok: 0, other: error code
 */
tb_long_t               tb_process_run(tb_char_t const* pathname, tb_char_t const* argv[], tb_process_attr_ref_t attr);

/*! run a given process from the command line
 *
 * @param cmd           the command line
 * @param attr          the process attributes
 *
 * @return              the status value, failed: -1, ok: 0, other: error code
 */
tb_long_t               tb_process_run_cmd(tb_char_t const* cmd, tb_process_attr_ref_t attr);

/*! init a given process
 *
 * @code

    // init process
    tb_process_ref_t process = tb_process_init("/bin/echo", tb_null, tb_null);
    if (process)
    {
        // wait process
        tb_long_t status = 0;
        if (tb_process_wait(process, &status, 10) > 0)
        {
            // trace
            tb_trace_i("process exited: %ld", status);
        }
        // kill process
        else
        {
            // kill it
            tb_process_kill(process);

            // wait it again
            tb_process_wait(process, &status, -1);
        }

        // exit process
        tb_process_exit(process);
    }

 * @endcode
 *
 * @param pathname      the process path or name
 *
 * @param argv          the list of arguments must be terminated by a null pointer
 *                      and must be terminated by a null pointer
 *                      and argv[0] is the self path name
 *
 * @param attr          the process attributes
 *
 * @return              the process
 */
tb_process_ref_t        tb_process_init(tb_char_t const* pathname, tb_char_t const* argv[], tb_process_attr_ref_t attr);

/*! init a given process from the command line
 *
 * @param cmd           the command line
 * @param attr          the process attributes
 *
 * @return              the process
 */
tb_process_ref_t        tb_process_init_cmd(tb_char_t const* cmd, tb_process_attr_ref_t attr);

/*! exit the process
 *
 * @param process       the process
 */
tb_void_t               tb_process_exit(tb_process_ref_t process);

/*! kill the process
 *
 * @param process       the process
 */
tb_void_t               tb_process_kill(tb_process_ref_t process);

/*! get the user private data
 *
 * @param process       the process
 *
 * @return              the user private data
 */
tb_cpointer_t           tb_process_priv(tb_process_ref_t process);

/*! set the user private data
 *
 * @param process       the process
 * @param priv          the user private data
 */
tb_void_t               tb_process_priv_set(tb_process_ref_t process, tb_cpointer_t priv);

/*! resume the process
 *
 * @param process       the process
 */
tb_void_t               tb_process_resume(tb_process_ref_t process);

/*! suspend the process
 *
 * @param process       the process
 */
tb_void_t               tb_process_suspend(tb_process_ref_t process);

/*! wait the process
 *
 * @param process       the process
 * @param pstatus       the process exited status pointer, maybe null
 * @param timeout       the timeout (ms), infinity: -1
 *
 * @return              wait failed: -1, timeout: 0, ok: 1
 */
tb_long_t               tb_process_wait(tb_process_ref_t process, tb_long_t* pstatus, tb_long_t timeout);

/*! wait the process list
 *
 * @code

    // init processes
    tb_size_t        count1 = 0;
    tb_process_ref_t processes1[5] = {0};
    tb_process_ref_t processes2[5] = {0};
    for (; count1 < 4; count1++)
    {
        processes1[count1] = tb_process_init(argv[1], (tb_char_t const**)(argv + 1), tb_null);
        tb_assert_and_check_break(processes1[count1]);
    }

    // ok?
    while (count1)
    {
        // trace
        tb_trace_i("waiting: %ld", count1);

        // wait processes
        tb_long_t               infosize = -1;
        tb_process_waitinfo_t   infolist[4];
        if ((infosize = tb_process_waitlist(processes1, infolist, tb_arrayn(infolist), -1)) > 0)
        {
            tb_size_t i = 0;
            for (i = 0; i < infosize; i++)
            {
                // trace
                tb_trace_i("process(%ld:%p) exited: %ld", infolist[i].index, infolist[i].process, infolist[i].status);

                // exit process
                if (infolist[i].process) tb_process_exit(infolist[i].process);

                // remove this process
                processes1[infolist[i].index] = tb_null;
            }

            // update processes
            tb_size_t count2 = 0;
            for (i = 0; i < count1; i++)
            {
                if (processes1[i]) processes2[count2++] = processes1[i];
            }
            tb_memcpy(processes1, processes2, count2 * sizeof(tb_process_ref_t));
            processes1[count2] = tb_null;
            count1 = count2;
        }
    }

 * @endcode
 *
 * @param processes     the null-terminated process list
 * @param infolist      the info list
 * @param infomaxn      the info maxn
 * @param timeout       the timeout (ms), infinity: -1
 *
 * @return              > 0: the info list size, 0: timeout, -1: failed
 */
tb_long_t               tb_process_waitlist(tb_process_ref_t const* processes, tb_process_waitinfo_ref_t infolist, tb_size_t infomaxn, tb_long_t timeout);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
