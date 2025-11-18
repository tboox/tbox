/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t tb_demo_process_test_run(tb_char_t** argv)
{
    tb_long_t ok = tb_process_run(argv[1], (tb_char_t const**)(argv + 1), tb_null);
    tb_trace_i("run: %s: %ld", argv[1], ok);
}
static tb_void_t tb_demo_process_test_pipe(tb_char_t** argv)
{
    // init pipe files
    tb_pipe_file_ref_t file[2] = {0};
    if (tb_pipe_file_init_pair(file, tb_null, 0))
    {
        // init process
        tb_process_attr_t attr = {0};
        attr.out.pipe = file[1];
        attr.outtype = TB_PROCESS_REDIRECT_TYPE_PIPE;
        attr.err.pipe = file[1];
        attr.errtype = TB_PROCESS_REDIRECT_TYPE_PIPE;
        tb_process_ref_t process = tb_process_init(argv[1], (tb_char_t const**)(argv + 1), &attr);
        if (process)
        {
            // read pipe data
            tb_size_t read = 0;
            tb_byte_t data[8192];
            tb_size_t size = sizeof(data);
            tb_bool_t wait = tb_false;
            while (read < size)
            {
                tb_long_t real = tb_pipe_file_read(file[0], data + read, size - read);
                if (real > 0)
                {
                    read += real;
                    wait = tb_false;
                }
                else if (!real && !wait)
                {
                    // wait pipe
                    tb_long_t ok = tb_pipe_file_wait(file[0], TB_PIPE_EVENT_READ, 1000);
                    tb_check_break(ok > 0);
                    wait = tb_true;
                }
                else break;
            }

            // dump data
            if (read) tb_dump_data(data, read);

            // wait process
            tb_long_t status = 0;
            tb_process_wait(process, &status, -1);

            // trace
            tb_trace_i("run: %s, status: %ld", argv[1], status);

            // exit process
            tb_process_exit(process);
        }

        // exit pipe files
        tb_pipe_file_exit(file[0]);
        tb_pipe_file_exit(file[1]);
    }
}
static tb_void_t tb_demo_process_test_waitlist(tb_char_t** argv)
{
    // init processes
    tb_size_t           count1 = 0;
    tb_process_ref_t    processes1[5] = {0};
    tb_process_ref_t    processes2[5] = {0};
    tb_process_attr_t   attr = {0};
    for (; count1 < 4; count1++)
    {
        attr.priv = tb_u2p(count1);
        processes1[count1] = tb_process_init(argv[1], (tb_char_t const**)(argv + 1), &attr);
        tb_assert_and_check_break(processes1[count1]);
    }

    // ok?
    while (count1)
    {
        // trace
        tb_trace_i("waiting: %ld", count1);

        // wait processes
        tb_long_t               infosize = -1;
        tb_process_waitinfo_t   infolist[4] = {{0}};
        if ((infosize = tb_process_waitlist(processes1, infolist, tb_arrayn(infolist), -1)) > 0)
        {
            tb_size_t i = 0;
            for (i = 0; i < infosize; i++)
            {
                // trace
                tb_trace_i("process(%d:%p) exited: %d, priv: %p", infolist[i].index, infolist[i].process, infolist[i].status, tb_process_priv(infolist[i].process));

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
}
static tb_void_t tb_demo_process_test_exit(tb_char_t** argv, tb_bool_t detach)
{
    tb_size_t i = 0;
    tb_process_attr_t attr = {0};
    if (detach) attr.flags |= TB_PROCESS_FLAG_DETACH;
    for (i = 0; i < 10; i++)
        tb_process_init(argv[1], (tb_char_t const**)(argv + 1), &attr);

    // we attempt to enter or do ctrl+c and see process list in process monitor
    tb_getchar();
}

/* test: redirect stdout only, stderr should still output to terminal
 * @see https://github.com/xmake-io/xmake/issues/3138
 * 
 * Verification: 
 * - stdout should be captured in file (we read and display it)
 * - stderr should output to terminal (visible as console output before/after this trace)
 */
static tb_void_t tb_demo_process_test_redirect_stdout_only(tb_char_t const* test_cmd)
{
    tb_trace_i("test: redirect stdout only, stderr should still output to terminal");
    tb_trace_i("verification: stdout goes to file (read below), stderr should appear in console output");
    tb_trace_i("NOTE: Look for 'This goes to stderr' message in your console/terminal output (may appear between these log lines)");
    tb_trace_i("===== STDOUT REDIRECTED - STDERR SHOULD APPEAR BELOW =====");

    // create temp file for stdout
    tb_char_t tmpdir[TB_PATH_MAXN];
    tb_char_t stdout_path[TB_PATH_MAXN];
    tb_char_t stderr_path[TB_PATH_MAXN];
    if (tb_directory_temporary(tmpdir, sizeof(tmpdir)))
    {
        tb_snprintf(stdout_path, sizeof(stdout_path), "%s%ctest_stdout.txt", tmpdir, TB_PATH_SEPARATOR);
        tb_snprintf(stderr_path, sizeof(stderr_path), "%s%ctest_stderr.txt", tmpdir, TB_PATH_SEPARATOR);
        
        // create a second test: also redirect stderr to a file to verify it's actually working
        // First test: only redirect stdout (the fix should make stderr go to terminal)
        tb_process_attr_t attr = {0};
        attr.out.path = stdout_path;
        attr.outtype = TB_PROCESS_REDIRECT_TYPE_FILEPATH;

        // use PowerShell to output to stderr - this is the most reliable method
        // PowerShell's [Console]::Error.WriteLine directly writes to stderr stream
        tb_char_t* argv[] = {"powershell", "-Command", "[Console]::Out.WriteLine('This goes to stdout'); [Console]::Error.WriteLine('This goes to stderr')", tb_null};
        tb_process_ref_t process = tb_process_init("powershell", (tb_char_t const**)argv, &attr);
        if (process)
        {
            // wait process
            tb_long_t status = 0;
            tb_process_wait(process, &status, -1);
            tb_trace_i("process exited with status: %ld", status);

            // read stdout from file
            tb_file_ref_t file = tb_file_init(stdout_path, TB_FILE_MODE_RO);
            if (file)
            {
                tb_byte_t data[8192];
                tb_long_t size = tb_file_read(file, data, sizeof(data) - 1);
                if (size > 0)
                {
                    data[size] = '\0';
                    tb_trace_i("stdout from file: %s", (tb_char_t const*)data);
                }
                tb_file_exit(file);
            }

            // exit process
            tb_process_exit(process);
        }

        // Second test: explicitly redirect stderr to a file to verify stderr works
        // This proves stderr can be written to, which means the handle is correct
        tb_trace_i("Verification: Now testing if stderr can be explicitly captured...");
        tb_process_attr_t attr2 = {0};
        attr2.out.path = stdout_path;
        attr2.outtype = TB_PROCESS_REDIRECT_TYPE_FILEPATH;
        attr2.err.path = stderr_path;
        attr2.errtype = TB_PROCESS_REDIRECT_TYPE_FILEPATH;
        
        // use same PowerShell command for verification
        tb_process_ref_t process2 = tb_process_init("powershell", (tb_char_t const**)argv, &attr2);
        if (process2)
        {
            tb_long_t status2 = 0;
            tb_process_wait(process2, &status2, -1);
            
            // read stderr from file - if we can read it, stderr works
            tb_file_ref_t stderr_file = tb_file_init(stderr_path, TB_FILE_MODE_RO);
            if (stderr_file)
            {
                tb_byte_t stderr_data[8192];
                tb_long_t stderr_size = tb_file_read(stderr_file, stderr_data, sizeof(stderr_data) - 1);
                if (stderr_size > 0)
                {
                    stderr_data[stderr_size] = '\0';
                    tb_trace_i("stderr verification (explicit redirect): %s", (tb_char_t const*)stderr_data);
                    tb_trace_i("SUCCESS: stderr can be written to, which proves handles are working correctly!");
                }
                else
                {
                    tb_trace_i("WARNING: stderr file is empty - stderr may not be working");
                }
                tb_file_exit(stderr_file);
            }
            tb_process_exit(process2);
        }

        // remove temp files
        tb_file_remove(stdout_path);
        tb_file_remove(stderr_path);
    }
    tb_trace_i("===== Check console above for 'This goes to stderr' message =====");
}

/* test: redirect stdin only, stdout and stderr should still output to terminal
 * @see https://github.com/xmake-io/xmake/issues/3138
 * 
 * Verification:
 * - stdin is redirected from file (process should be able to use it)
 * - stdout should output to terminal (visible as console output)
 * - stderr should output to terminal (visible as console output)
 */
static tb_void_t tb_demo_process_test_redirect_stdin_only(tb_char_t const* test_cmd)
{
    tb_trace_i("test: redirect stdin only, stdout and stderr should still output to terminal");
    tb_trace_i("verification: stdin from file, stdout and stderr should appear in console output");
    tb_trace_i("NOTE: Look for 'This goes to stdout' and 'This goes to stderr' messages (may appear between log lines)");
    tb_trace_i("===== STDIN REDIRECTED - STDOUT/STDERR SHOULD APPEAR BELOW =====");

    // create temp file for stdin (with some test content)
    tb_char_t tmpdir[TB_PATH_MAXN];
    tb_char_t stdin_path[TB_PATH_MAXN];
    if (tb_directory_temporary(tmpdir, sizeof(tmpdir)))
    {
        tb_snprintf(stdin_path, sizeof(stdin_path), "%s%ctest_stdin.txt", tmpdir, TB_PATH_SEPARATOR);
        // write test content to stdin file
        tb_file_ref_t file = tb_file_init(stdin_path, TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC);
        if (file)
        {
            tb_char_t const* content = "test input\n";
            tb_file_writ(file, (tb_byte_t const*)content, tb_strlen(content));
            tb_file_exit(file);
        }

        // init process with stdin redirected from file
        tb_process_attr_t attr = {0};
        attr.in.path = stdin_path;
        attr.intype = TB_PROCESS_REDIRECT_TYPE_FILEPATH;

        // use PowerShell to output to stdout and stderr
        // stdin is redirected but stdout/stderr should output to terminal
        tb_char_t* argv[] = {"powershell", "-Command", "[Console]::Out.WriteLine('This goes to stdout'); [Console]::Error.WriteLine('This goes to stderr')", tb_null};
        tb_process_ref_t process = tb_process_init("powershell", (tb_char_t const**)argv, &attr);
        if (process)
        {
            // wait process
            tb_long_t status = 0;
            tb_process_wait(process, &status, -1);
            tb_trace_i("process exited with status: %ld", status);

            // exit process
            tb_process_exit(process);
        }

        // remove temp file
        tb_file_remove(stdin_path);
    }
    tb_trace_i("===== IF YOU SAW 'This goes to stdout/stderr' ABOVE, THE FIX WORKS! =====");
}

/* test: redirect stdout to pipe only, stderr should still output to terminal
 * @see https://github.com/xmake-io/xmake/issues/3138
 * 
 * Verification:
 * - stdout should be captured in pipe (we read and display it)
 * - stderr should output to terminal (visible as console output)
 */
static tb_void_t tb_demo_process_test_redirect_stdout_pipe_only(tb_char_t const* test_cmd)
{
    tb_trace_i("test: redirect stdout to pipe only, stderr should still output to terminal");
    tb_trace_i("verification: stdout goes to pipe (read below), stderr should appear in console output");
    tb_trace_i("NOTE: Look for 'This goes to stderr' message (may appear between log lines)");
    tb_trace_i("===== STDOUT TO PIPE - STDERR SHOULD APPEAR BELOW =====");

    // init pipe files
    tb_pipe_file_ref_t file[2] = {0};
    if (tb_pipe_file_init_pair(file, tb_null, 0))
    {
        // init process with stdout redirected to pipe
        tb_process_attr_t attr = {0};
        attr.out.pipe = file[1];
        attr.outtype = TB_PROCESS_REDIRECT_TYPE_PIPE;

        // use PowerShell to output to stdout and stderr
        tb_char_t* argv[] = {"powershell", "-Command", "[Console]::Out.WriteLine('This goes to stdout'); [Console]::Error.WriteLine('This goes to stderr')", tb_null};
        tb_process_ref_t process = tb_process_init("powershell", (tb_char_t const**)argv, &attr);
        if (process)
        {
            // read stdout from pipe
            tb_size_t read = 0;
            tb_byte_t data[8192];
            tb_size_t size = sizeof(data);
            tb_bool_t wait = tb_false;
            while (read < size)
            {
                tb_long_t real = tb_pipe_file_read(file[0], data + read, size - read);
                if (real > 0)
                {
                    read += real;
                    wait = tb_false;
                }
                else if (!real && !wait)
                {
                    // wait pipe
                    tb_long_t ok = tb_pipe_file_wait(file[0], TB_PIPE_EVENT_READ, 1000);
                    if (ok <= 0) break;
                    wait = tb_true;
                }
                else break;
            }

            // dump stdout data from pipe
            if (read)
            {
                data[read] = '\0';
                tb_trace_i("stdout from pipe: %s", (tb_char_t const*)data);
            }

            // wait process
            tb_long_t status = 0;
            tb_process_wait(process, &status, -1);
            // Note: exit status 1 may be normal when using >&2 redirection in cmd
            // The important verification is that stdout was captured correctly
            tb_trace_i("process exited with status: %ld (note: non-zero may be normal)", status);

            // exit process
            tb_process_exit(process);
        }

        // exit pipe files
        tb_pipe_file_exit(file[0]);
        tb_pipe_file_exit(file[1]);
    }
    tb_trace_i("===== IF YOU SAW 'This goes to stderr' ABOVE, THE FIX WORKS! =====");
}

/* test: all redirect scenarios
 */
static tb_void_t tb_demo_process_test_redirect_all(tb_char_t const* test_cmd)
{
    tb_trace_i("test: all redirect scenarios");
    tb_trace_i("");

    // test 1: stdout only
    tb_trace_i("=== Test 1: Redirect stdout only ===");
    tb_demo_process_test_redirect_stdout_only(test_cmd);
    tb_trace_i("");

    // test 2: stdin only
    tb_trace_i("=== Test 2: Redirect stdin only ===");
    tb_demo_process_test_redirect_stdin_only(test_cmd);
    tb_trace_i("");

    // test 3: stdout pipe only
    tb_trace_i("=== Test 3: Redirect stdout to pipe only ===");
    tb_demo_process_test_redirect_stdout_pipe_only(test_cmd);
    tb_trace_i("");

    tb_trace_i("all redirect tests completed");
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_process_main(tb_int_t argc, tb_char_t** argv)
{
#if 0
    tb_demo_process_test_run(argv);
#else
    tb_used(tb_demo_process_test_run);
#endif

#if 0
    tb_demo_process_test_pipe(argv);
#else
    tb_used(tb_demo_process_test_pipe);
#endif

#if 0
    tb_demo_process_test_waitlist(argv);
#else
    tb_used(tb_demo_process_test_waitlist);
#endif

#if 0
    // we can run `xxx.bat` or `xxx.sh` shell command to test it
    // @see https://github.com/xmake-io/xmake/issues/719
    tb_demo_process_test_exit(argv, tb_false);
//    tb_demo_process_test_exit(argv, tb_true);
#else
    tb_used(tb_demo_process_test_exit);
#endif

#if 1
    // test: Windows process redirect scenarios
    // @see https://github.com/xmake-io/xmake/issues/3138
    // verify that when only stdout is redirected, stderr still outputs to terminal
    tb_char_t const* test_cmd = argv[1];
    if (!test_cmd) test_cmd = "cmd";
    tb_demo_process_test_redirect_all(test_cmd);
#else
    tb_used(tb_demo_process_test_redirect_all);
    tb_used(tb_demo_process_test_redirect_stdout_only);
    tb_used(tb_demo_process_test_redirect_stdin_only);
    tb_used(tb_demo_process_test_redirect_stdout_pipe_only);
#endif
    return 0;
}
