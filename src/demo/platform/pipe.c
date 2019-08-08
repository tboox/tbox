/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_pipe_main(tb_int_t argc, tb_char_t** argv)
{
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
    return 0;
}
