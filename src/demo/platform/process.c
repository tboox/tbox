/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_process_main(tb_int_t argc, tb_char_t** argv)
{
#if 0
    // run 
    tb_long_t ok = tb_process_run(argv[1], (tb_char_t const**)(argv + 1), tb_null);

    // trace
    tb_trace_i("run: %s: %ld", argv[1], ok);
#else
 
    // init processes
    tb_size_t        count = 0;
    tb_process_ref_t processes[5] = {0};
    for (; count < 4; count++)
    {
        processes[count] = tb_process_init(argv[1], (tb_char_t const**)(argv + 1), tb_null);
        tb_assert_and_check_break(processes[count]);
    }

    // ok?
    while (count)
    {
        // trace
        tb_trace_i("waiting: %ld", count);

        // wait processes
        tb_long_t               infosize = -1;
        tb_process_waitinfo_t   infolist[4] = {{0}};
        if ((infosize = tb_process_waitlist(processes, infolist, tb_arrayn(infolist), -1)) > 0)
        {
            tb_size_t i = 0;
            for (i = 0; i < infosize; i++)
            {
                // trace
                tb_trace_i("process(%ld:%p) exited: %ld", infolist[i].index, infolist[i].process, infolist[i].status);

                // exit process
                if (infolist[i].process) tb_process_exit(infolist[i].process);

                // remove this process
                tb_size_t index = infolist[i].index;
                if (count > index + 1) tb_memmov(processes + index, processes + index + 1, (count - index - 1) * sizeof(tb_process_ref_t));
                count--;

                // fill null-end
                processes[count] = tb_null;
            }
        }
    }
#endif

    // ok
    return 0;
}
