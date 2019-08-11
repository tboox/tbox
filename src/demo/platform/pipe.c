/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_DEMO_PIPE_NAME   "hello"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_int_t tb_demo_thread_writ(tb_cpointer_t priv)
{
    // write data to pipe
    tb_pipe_file_ref_t pipe = tb_pipe_file_init(TB_DEMO_PIPE_NAME, TB_FILE_MODE_WO, 0);
    if (pipe)
    {
        tb_byte_t const* data = (tb_byte_t const*)"hello world!";
        tb_size_t        size = sizeof("hello world!");
        tb_bool_t ok = tb_pipe_file_bwrit(pipe, data, size);
        tb_trace_i("write: %s, ok: %d", data, ok);
        tb_pipe_file_exit(pipe);
    }
    return 0;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_pipe_main(tb_int_t argc, tb_char_t** argv)
{
    // data
    tb_byte_t const* data = (tb_byte_t const*)"hello world!";
    tb_size_t        size = sizeof("hello world!");

    // test the anonymous pipe
    tb_pipe_file_ref_t file[2] = {};
    if (tb_pipe_file_init_pair(file, 0))
    {
        // write data to pipe
        tb_pipe_file_bwrit(file[1], data, size);

        // read data from pipe
        tb_byte_t buffer[2096] = {0};
        tb_bool_t ok = tb_pipe_file_bread(file[0], buffer, size);
        tb_trace_i("read: %s, ok: %d", buffer, ok);

        // exit pipe files
        tb_pipe_file_exit(file[0]);
        tb_pipe_file_exit(file[1]);
    }

    // test the named pipe
    tb_thread_ref_t thread = tb_thread_init(tb_null, tb_demo_thread_writ, tb_null, 0);
    if (thread)
    {
        tb_sleep(1);
        tb_pipe_file_ref_t pipe = tb_pipe_file_init(TB_DEMO_PIPE_NAME, TB_FILE_MODE_RO, 0);
        if (pipe)
        {
            // wait read events
            tb_pipe_file_wait(pipe, TB_PIPE_EVENT_READ, -1);

            // read data from pipe
            tb_byte_t buffer[2096] = {0};
            tb_bool_t ok = tb_pipe_file_bread(pipe, buffer, size);
            tb_trace_i("read: %s, ok: %d", buffer, ok);
            tb_pipe_file_exit(pipe);
        }

        tb_thread_wait(thread, -1, tb_null);
        tb_thread_exit(thread);
    }
    return 0;
}
