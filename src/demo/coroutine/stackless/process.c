/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define COUNT   (50)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the process type
typedef struct __tb_demo_lo_process_t
{
    // the process
    tb_process_ref_t    proc;

    // the arguments
    tb_char_t**         argv;

}tb_demo_lo_process_t, *tb_demo_lo_process_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_lo_coroutine_func(tb_lo_coroutine_ref_t coroutine, tb_cpointer_t priv)
{
    // get arguments
    tb_demo_lo_process_ref_t process = (tb_demo_lo_process_ref_t)priv;
    tb_assert_and_check_return(process && process->argv);

    // enter coroutine
    tb_lo_coroutine_enter(coroutine)
    {
        // init process
        process->proc = tb_process_init(process->argv[1], (tb_char_t const**)(process->argv + 1), tb_null);
        if (process->proc)
        {
            // wait process
            tb_lo_coroutine_wait_proc(process->proc, -1);
            if (tb_lo_coroutine_wait_result() > 0)
            {
                // trace
                tb_trace_i("run: %s, status: %ld", process->argv[1], tb_lo_coroutine_proc_status());
            }

            // exit process
            tb_process_exit(process->proc);
        }
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_lo_coroutine_process_main(tb_int_t argc, tb_char_t** argv)
{
    // init scheduler
    tb_lo_scheduler_ref_t scheduler = tb_lo_scheduler_init();
    if (scheduler)
    {
        // start coroutines
        tb_size_t            count = COUNT;
        tb_demo_lo_process_t processes[COUNT];
        while (count--)
        {
            processes[count].argv = argv;
            tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_func, &processes[count], 0);
        }

        // run scheduler
        tb_lo_scheduler_loop(scheduler, tb_true);

        // exit scheduler
        tb_lo_scheduler_exit(scheduler);
    }
    return 0;
}
