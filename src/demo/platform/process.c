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

#if 1
    // we can run `xxx.bat` or `xxx.sh` shell command to test it
    // @see https://github.com/xmake-io/xmake/issues/719
    tb_demo_process_test_exit(argv, tb_false);
//    tb_demo_process_test_exit(argv, tb_true);
#else
    tb_used(tb_demo_process_test_exit);
#endif
    return 0;
}
