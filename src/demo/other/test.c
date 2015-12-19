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
    // init stream
    tb_stream_ref_t stream = tb_stream_init_from_url(argv[1]);
    if (stream)
    {
        // open stream
        if (tb_stream_open(stream))
        {
            // read all cstr and append '\0'
            tb_size_t    size = 0;
            tb_char_t*   cstr = (tb_char_t*)tb_stream_bread_all(stream, tb_true, &size);
            if (cstr)
            {
                // trace
                tb_trace_i("%s: %lu", cstr, size);

                // exit cstr
                tb_free(cstr);
            }
        }

        // exit stream
        tb_stream_exit(stream);
    }
#endif

    return 0;
}
