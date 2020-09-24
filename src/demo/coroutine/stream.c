/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "stream"
#define TB_TRACE_MODULE_DEBUG           (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// port
#define TB_DEMO_PORT        (9090)

// timeout
#define TB_DEMO_TIMEOUT     (-1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_coroutine_pull(tb_cpointer_t priv)
{
    // get arguments
    tb_value_ref_t args = (tb_value_ref_t)priv;

    // trace
    tb_trace_i("pull[%lu]: %s ..", args[1].ul, args[0].cstr);

    // done
    tb_hize_t       read = 0;
    tb_stream_ref_t stream = tb_null;
    do
    {
        // init stream
        stream = tb_stream_init_from_url(args[0].cstr);
        tb_assert_and_check_break(stream);

        // open stream
        if (!tb_stream_open(stream)) break;

        // read data
        tb_byte_t data[TB_STREAM_BLOCK_MAXN];
        tb_hize_t left = tb_stream_left(stream);
        do
        {
            // read data
            tb_long_t real = tb_stream_read(stream, data, TB_STREAM_BLOCK_MAXN);
            if (real > 0) read += real;
            else if (!real)
            {
                // wait
                tb_long_t wait = tb_stream_wait(stream, TB_STREAM_WAIT_READ, tb_stream_timeout(stream));
                tb_assert_and_check_break(wait >= 0);

                // timeout?
                tb_check_break(wait);

                // has read?
                tb_assert_and_check_break(wait & TB_STREAM_WAIT_READ);
            }
            else break;

            // is end?
            if (read >= left) break;

        } while(1);

    } while (0);

    // exit stream
    if (stream) tb_stream_exit(stream);
    stream = tb_null;

    // trace
    tb_trace_i("pull[%lu]: %s %ld bytes", args[1].ul, args[0].cstr, read);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_coroutine_stream_main(tb_int_t argc, tb_char_t** argv)
{
    // check
    tb_assert_and_check_return_val(argc >= 2 && argv[1], -1);

    // get url
    tb_char_t const* url = argv[1];

    // the coroutines count
    tb_size_t count = argc > 2? tb_atoi(argv[2]) : 10;
    tb_assert_and_check_return_val(count <= 256, -1);

    // init scheduler
    tb_value_t args[2 * 256];
    tb_co_scheduler_ref_t scheduler = tb_co_scheduler_init();
    if (scheduler)
    {
        // start file
        tb_size_t i = 0;
        for (i = 0; i < count; i++)
        {
            tb_size_t idx = i << 1;
            args[idx + 0].cstr = url;
            args[idx + 1].ul = i;
            tb_coroutine_start(scheduler, tb_demo_coroutine_pull, args + idx, 0);
        }

        // run scheduler
        tb_co_scheduler_loop(scheduler, tb_true);

        // exit scheduler
        tb_co_scheduler_exit(scheduler);
    }

    // end
    return 0;
}
