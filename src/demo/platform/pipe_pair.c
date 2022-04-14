/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define BUFSIZE     (8192 << 1)
#define COUNT       (10000)

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_int_t tb_demo_pipe_pair_read(tb_cpointer_t priv)
{
    tb_pipe_file_ref_t file = (tb_pipe_file_ref_t)priv;
    tb_byte_t data[BUFSIZE];
    tb_size_t count = 0;
    while (1)
    {
        count++;
        if (!tb_pipe_file_bread(file, data, sizeof(data))) break;
        tb_trace_i("read %lu", count);
    }
    tb_pipe_file_exit(file);
    return 0;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_pipe_pair_main(tb_int_t argc, tb_char_t** argv)
{
    // test the anonymous pipe
    tb_pipe_file_ref_t file[2] = {};
    if (tb_pipe_file_init_pair(file, tb_null, 8192))
    {
        // start the read thread
        tb_thread_ref_t thread = tb_thread_init(tb_null, tb_demo_pipe_pair_read, file[0], 0);

        // write data to pipe
        tb_byte_t data[BUFSIZE];
        tb_size_t count = COUNT;
        while (count--)
        {
            tb_trace_i("write ..");
            if (!tb_pipe_file_bwrit(file[1], data, sizeof(data))) break;
        }

        // exit pipe files
        tb_pipe_file_exit(file[1]);

        // exit thread
        tb_thread_wait(thread, -1, tb_null);
        tb_thread_exit(thread);
    }
    return 0;
}
