/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_DEMO_PIPE_NAME   "hello"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_named_pipe_main(tb_int_t argc, tb_char_t** argv)
{
    // init buffer
    tb_size_t  length = 100 * 1024 * 1024;
    tb_byte_t* buffer = (tb_byte_t*)tb_malloc(length);
    tb_assert_and_check_return_val(buffer, -1);

    // is server?
    tb_bool_t is_server = argc > 1 && !tb_strcmp(argv[1], "server");
    if (is_server)
    {
        tb_pipe_file_ref_t pipe = tb_pipe_file_init(TB_DEMO_PIPE_NAME, TB_PIPE_MODE_WO, 0);
        if (pipe)
        {
            // trace
            tb_trace_i("connect pipe ..");

            // connect pipe first
            tb_long_t connected = -1;
            while (!(connected = tb_pipe_file_connect(pipe)))
            {
                tb_long_t wait = tb_pipe_file_wait(pipe, TB_PIPE_EVENT_CONN, -1);
                tb_assert_and_check_break(wait > 0);
            }
            if (connected > 0)
            {
                // trace
                tb_trace_i("write data to pipe ..");

                // write data to pipe
                tb_bool_t ok = tb_pipe_file_bwrit(pipe, buffer, length);
                tb_trace_i("write ok: %d", ok);
            }
            tb_pipe_file_exit(pipe);
        }
    }
    else
    {
        tb_pipe_file_ref_t pipe = tb_pipe_file_init(TB_DEMO_PIPE_NAME, TB_PIPE_MODE_RO, 0);
        if (pipe)
        {
            // trace
            tb_trace_i("read data to pipe ..");

            // read data from pipe
            tb_bool_t ok = tb_pipe_file_bread(pipe, buffer, length);
            tb_trace_i("read ok: %d", ok);
            tb_pipe_file_exit(pipe);
        }
    }
    // exit buffer
    tb_free(buffer);
    return 0;
}
