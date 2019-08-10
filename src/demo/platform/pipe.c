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
        tb_pipe_file_writ(pipe, (tb_byte_t const*)"hello world!", sizeof("hello world!"));
        tb_pipe_file_exit(pipe);
    }
    return 0;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_pipe_main(tb_int_t argc, tb_char_t** argv)
{
    // test the anonymous pipe
    tb_pipe_file_ref_t file[2] = {};
    if (tb_pipe_file_init_pair(file, 0))
    {
        // write data to pipe
        tb_pipe_file_writ(file[1], (tb_byte_t const*)"hello world!", sizeof("hello world!"));

        // read data from pipe
        tb_byte_t data[2096] = {0};
        tb_long_t read = tb_pipe_file_read(file[0], data, sizeof(data));
        tb_trace_i("%s, read: %ld", data, read);

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
            tb_byte_t data[2096] = {0};
            tb_long_t read = tb_pipe_file_read(pipe, data, sizeof(data));
            tb_trace_i("%s, read: %ld", data, read);
            tb_pipe_file_exit(pipe);
        }

        tb_thread_wait(thread, -1, tb_null);
        tb_thread_exit(thread);
    }
    return 0;
}
