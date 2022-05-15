/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define BUFSIZE     (32767)
#define COUNT       (100000)

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t tb_demo_coroutine_writ(tb_cpointer_t priv)
{
    tb_byte_t data[BUFSIZE];
    tb_pipe_file_ref_t pipe = (tb_pipe_file_ref_t) priv;
    for (tb_size_t i = 0; i < COUNT; i++)
    {
        if (!tb_pipe_file_bwrit(pipe, data, sizeof(data))) break;
        tb_trace_i("[-->] %.4llu", i);
    }
    tb_pipe_file_exit(pipe);
}

static tb_void_t tb_demo_coroutine_read(tb_cpointer_t priv)
{
    tb_byte_t data[BUFSIZE];
    tb_pipe_file_ref_t pipe = (tb_pipe_file_ref_t) priv;
    for (tb_size_t count = 0;; count++)
    {
        if (!tb_pipe_file_bread(pipe, data, sizeof(data))) break;
        tb_trace_i("[<--] %.4llu", count);
    }
    tb_pipe_file_exit(pipe);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_coroutine_pipe_main(tb_int_t argc, tb_char_t** argv)
{
    tb_co_scheduler_ref_t scheduler = tb_co_scheduler_init();
    if (scheduler)
    {
        tb_pipe_file_ref_t pair[2];
        if (tb_pipe_file_init_pair(pair, tb_null, 4096))
        {
            // start coroutines
            tb_coroutine_start(scheduler, tb_demo_coroutine_writ, pair[1], 0);
            tb_coroutine_start(scheduler, tb_demo_coroutine_read, pair[0], 0);

            // do loop
            tb_co_scheduler_loop(scheduler, tb_true);
        }
        tb_co_scheduler_exit(scheduler);
    }

    return 0;
}
