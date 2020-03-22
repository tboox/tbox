/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_int_t tb_demo_thread_func(tb_cpointer_t priv)
{
    // get arguments
    tb_char_t** argv = (tb_char_t**)priv;
    tb_assert_and_check_return_val(argv, -1);

    // trace
    tb_trace_i("run: %s ..", argv[1]);

    // run process in group
    tb_process_attr_t attr = {0};
    attr.group = tb_process_group();
    tb_long_t ok = tb_process_run(argv[1], (tb_char_t const**)(argv + 1), &attr);

    // trace
    tb_trace_i("run: %s: %ld", argv[1], ok);
    return 0;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_process_group_main(tb_int_t argc, tb_char_t** argv)
{
    // start processes 
    tb_size_t i = 0;
    tb_thread_ref_t threads[10] = {0};
    for (i = 0; i < tb_arrayn(threads); i++)
        threads[i] = tb_thread_init(tb_null, tb_demo_thread_func, argv, 0);

    // wait 
    tb_getchar();

    // kill all processes in group
    tb_process_group_kill(tb_process_group());

    // wait threads
    for (i = 0; i < tb_arrayn(threads); i++)
    {
        tb_thread_ref_t thread = threads[i];
        if (thread)
        {
            tb_thread_wait(thread, -1, tb_null);
            tb_thread_exit(thread);
        }
    }
    return 0;
}
