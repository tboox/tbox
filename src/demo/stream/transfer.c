/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */ 
//static tb_atomic_t g_clos = 0;
//static tb_atomic_t g_exit = 0;
static tb_bool_t tb_demo_transfer_save_func(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_cpointer_t priv)
{
    // check
    tb_value_t* tuple = (tb_value_t*)priv;
    tb_assert_and_check_return_val(tuple, tb_false);

    // percent
    tb_size_t percent = 0;
    if (size > 0) percent = (offset * 100) / size;
    else if (state == TB_STATE_OK) percent = 100;

    // trace
    tb_trace_i("save: %llu, rate: %lu bytes/s, percent: %lu%%, state: %s", save, rate, percent, tb_state_cstr(state));

#if 0
    // clos it
    if (state != TB_STATE_OK) 
    {
        if (tb_atomic_get(&g_exit) && !tb_atomic_fetch_and_set(&g_clos, 1))
        {
            tb_trace_i("exit: clos");
            // exit transfer
            if (tuple[0].ptr) tb_transfer_exit(tuple[0].ptr);
            tuple[0].ptr = tb_null;

            // exit istream
            if (tuple[1].ptr) tb_async_stream_exit(tuple[1].ptr);
            tuple[1].ptr = tb_null;

            // exit ostream
            if (tuple[2].ptr) tb_basic_stream_exit(tuple[2].ptr);
            tuple[2].ptr = tb_null;
        }
    }
#endif

    // ok
    return tb_true;
}
static tb_pointer_t tb_demo_transfer_loop(tb_cpointer_t priv)
{
    // the aicp
    tb_aicp_t* aicp = (tb_aicp_t*)priv;

    // done loop
    if (aicp) tb_aicp_loop(aicp);

    // exit it
    tb_thread_return(tb_null);
    return tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_stream_transfer_main(tb_int_t argc, tb_char_t** argv)
{
    // done
    tb_value_t          tuple[3];
    tb_aicp_t*          aicp = tb_null;
    tb_handle_t         transfer = tb_null;
    tb_async_stream_t*  istream = tb_null;
    tb_basic_stream_t*  ostream = tb_null;
    do
    {
        // init aicp
        aicp = tb_aicp_init(2);
        tb_assert_and_check_break(aicp);

        // init transfer
#if 1
        transfer = tb_transfer_init_uu(aicp, argv[1], argv[2], 0);
#else
        istream = tb_async_stream_init_from_url(aicp, argv[1]);
        ostream = tb_basic_stream_init_from_file(argv[2], TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC);
        transfer = tb_transfer_init_ag(istream, ostream, -1);
#endif
        tb_assert_and_check_break(transfer);

        // init loop
        if (!tb_thread_init(tb_null, tb_demo_transfer_loop, aicp, 0)) break;

        // limit rate
        tb_transfer_limitrate(transfer, argv[3]? tb_atoi(argv[3]) : 0);

        // trace
        tb_trace_i("save: ..");

        // init tuple
        tuple[0].ptr = transfer;
        tuple[1].ptr = istream;
        tuple[2].ptr = ostream;

        // open and save transfer
        if (!tb_transfer_open_save(transfer, tb_demo_transfer_save_func, tuple)) break;

        // wait
//        getchar();

        // trace
        tb_trace_i("pause: ..");

        // pause transfer
        tb_transfer_pause(transfer);

        // wait
//        getchar();

        // trace
        tb_trace_i("resume: ..");

        // start transfer
        if (!tb_transfer_resume(transfer)) break;

        // wait
//        getchar();

        // trace
        tb_trace_i("save: ..");

        // open and save transfer
        if (!tb_transfer_open_save(transfer, tb_demo_transfer_save_func, tuple)) break;

        // wait
//        getchar();

    } while (0);

    // trace
    tb_trace_i("exit: ..");

//    tb_atomic_set(&g_exit, 1);

//    tb_transfer_exit(transfer, tb_transfer_exit_func_t);

#if 0
// exit it if closed 
    if (tb_transfer_closed(transfer) && !tb_atomic_fetch_and_set(&g_clos, 1))
    {
        tb_trace_i("exit: end");
        // exit transfer
        if (tuple[0].ptr) tb_transfer_exit(tuple[0].ptr);
        tuple[0].ptr = tb_null;

        // exit istream
        if (tuple[1].ptr) tb_async_stream_exit(tuple[1].ptr);
        tuple[1].ptr = tb_null;

        // exit ostream
        if (tuple[2].ptr) tb_basic_stream_exit(tuple[2].ptr);
        tuple[2].ptr = tb_null;
    }

    while (!tb_atomic_get(&g_clos)) tb_msleep(100);
#endif

    // exit aicp
    if (aicp) tb_aicp_exit(aicp);
    aicp = tb_null;
    return 0;
}
