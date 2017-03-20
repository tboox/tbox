/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */ 
static tb_bool_t tb_demo_transfer_done_func(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_cpointer_t priv)
{
    // percent
    tb_size_t percent = 0;
    if (size > 0) percent = (tb_size_t)((offset * 100) / size);
    else if (state == TB_STATE_OK) percent = 100;

    // trace
    tb_trace_i("done: %llu bytes, rate: %lu bytes/s, percent: %lu%%, state: %s", save, rate, percent, tb_state_cstr(state));

    // ok
    return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_stream_async_transfer_main(tb_int_t argc, tb_char_t** argv)
{
    // done
    tb_async_transfer_ref_t transfer = tb_null;
    do
    {
        // init transfer
        transfer = tb_async_transfer_init(tb_null, tb_true);
        tb_assert_and_check_break(transfer);

        // init transfer stream
        if (!tb_async_transfer_init_istream_from_url(transfer, argv[1])) break;
        if (!tb_async_transfer_init_ostream_from_url(transfer, argv[2])) break;

        // limit rate
        tb_async_transfer_limitrate(transfer, argv[3]? tb_atoi(argv[3]) : 0);

        // trace
        tb_trace_i("done: ..");

        // open and done transfer
        if (!tb_async_transfer_open_done(transfer, 0, tb_demo_transfer_done_func, tb_null)) break;

        // wait
        getchar();

        // trace
        tb_trace_i("pause: ..");

        // pause transfer
        tb_async_transfer_pause(transfer);

        // wait 
        getchar();

        // trace
        tb_trace_i("resume: ..");

        // resume transfer
        if (!tb_async_transfer_resume(transfer)) break;

        // wait
        getchar();

        // trace
        tb_trace_i("done: ..");

        // open and done transfer
        if (!tb_async_transfer_open_done(transfer, 0, tb_demo_transfer_done_func, tb_null)) break;

        // wait
        getchar();

    } while (0);

    // trace
    tb_trace_i("exit: ..");

    // exit transfer
    if (transfer) tb_async_transfer_exit(transfer);
    transfer = tb_null;

    return 0;
}
