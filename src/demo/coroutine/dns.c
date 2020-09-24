/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_coroutine_dns(tb_cpointer_t priv)
{
    tb_ipaddr_t      addr;
    tb_hong_t        time = tb_mclock();
    tb_char_t const* name = (tb_char_t const*)priv;
    if (tb_dns_looker_done(name, &addr))
    {
        time = tb_mclock() - time;
        tb_trace_i("lookup: %s => %{ipaddr}, %lld ms", name, &addr, time);
    }
    else tb_trace_i("lookup: %s failed", name);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_coroutine_dns_main(tb_int_t argc, tb_char_t** argv)
{
    // check
    tb_assert_and_check_return_val(argc >= 2 && argv[1], -1);

    // init scheduler
    tb_co_scheduler_ref_t scheduler = tb_co_scheduler_init();
    if (scheduler)
    {
        // start dns
        tb_size_t n = argv[2]? (tb_size_t)tb_atoi(argv[2]) : 10;
        while (n--)
        {
            // start it
            tb_coroutine_start(scheduler, tb_demo_coroutine_dns, argv[1], 0);
        }

        // run scheduler
        tb_co_scheduler_loop(scheduler, tb_true);

        // exit scheduler
        tb_co_scheduler_exit(scheduler);
    }

    // end
    return 0;
}
