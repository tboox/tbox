/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_pipe_pair_main(tb_int_t argc, tb_char_t** argv)
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
    return 0;
}
