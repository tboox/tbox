/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define COUNT   (50)

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t tb_demo_coroutine_proc(tb_cpointer_t priv)
{
    // get arguments
    tb_char_t** argv = (tb_char_t**)priv;
    tb_assert_and_check_return(argv);

    // init process
    tb_process_ref_t process = tb_process_init(argv[1], (tb_char_t const**)(argv + 1), tb_null);
    if (process)
    {
        // wait process
        tb_long_t status = 0;
        tb_process_wait(process, &status, -1);

        // trace
        tb_trace_i("run: %s, status: %ld", argv[1], status);

        // exit process
        tb_process_exit(process);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_coroutine_process_main(tb_int_t argc, tb_char_t** argv)
{
    tb_co_scheduler_ref_t scheduler = tb_co_scheduler_init();
    if (scheduler)
    {
        // start coroutines
        tb_size_t count = COUNT;
        while (count--)
            tb_coroutine_start(scheduler, tb_demo_coroutine_proc, argv, 0);

        // do loop
        tb_co_scheduler_loop(scheduler, tb_true);
        tb_co_scheduler_exit(scheduler);
    }

    return 0;
}
