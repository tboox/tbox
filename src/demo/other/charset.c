/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_other_charset_main(tb_int_t argc, tb_char_t** argv)
{
    // check
    tb_assert_and_check_return_val(argc == 5, 0);

    // init stream
    tb_basic_stream_t* istream = tb_basic_stream_init_from_url(argv[1]);
    tb_basic_stream_t* ostream = tb_basic_stream_init_from_file(argv[2], TB_FILE_MODE_WO | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC);
    if (istream && ostream && tb_basic_stream_open(istream) && tb_basic_stream_open(ostream))
    {
        // init data & size
        tb_hong_t isize = tb_stream_size(istream);
        if (isize > 0)
        {
            tb_long_t   osize = isize << 2;
            tb_byte_t*  idata = tb_malloc(isize);
            tb_byte_t*  odata = tb_malloc(osize);
            if (idata && odata && tb_basic_stream_bread(istream, idata, isize))
            {
                // conv
                osize = tb_charset_conv_data(tb_charset_type(argv[3]), tb_charset_type(argv[4]), idata, isize, odata, osize);
                tb_trace_i("conv: %ld bytes", osize);
                
                // save
                if (osize > 0) tb_basic_stream_bwrit(ostream, odata, osize);
            }

            // exit data
            if (idata) tb_free(idata);
            if (odata) tb_free(odata);
        }
    
        // exit stream
        tb_basic_stream_exit(istream);
        tb_basic_stream_exit(ostream);
    }

    return 0;
}

