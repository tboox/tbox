/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_other_test_main(tb_int_t argc, tb_char_t** argv)
{
#if 0
    tb_stream_ref_t stream = tb_stream_init_from_url(argv[1]);
    if (stream)
    {
        if (tb_stream_open(stream))
        {
            tb_long_t size = 0;
            tb_char_t line[8192];
            while ((size = tb_stream_bread_line(stream, line, sizeof(line))) >= 0)
                tb_trace_i("line: %s", line);
        }
        tb_stream_exit(stream);
    }
#endif
    return 0;
}
