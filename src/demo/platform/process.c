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
        tb_process_waitinfo_t   infolist[4] = {{0}};
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
#endif

    // ok
    return 0;
}
