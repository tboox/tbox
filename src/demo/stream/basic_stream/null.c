/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_stream_basic_stream_null_main(tb_int_t argc, tb_char_t** argv)
{
    // init istream
    tb_basic_stream_t* istream = tb_basic_stream_init_from_url(argv[1]);

    // init ostream
    tb_basic_stream_t* ostream = tb_basic_stream_init_from_file(argv[2], TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC);

    // filter istream or ostream?
    tb_basic_stream_t* iostream = istream;
//  tb_basic_stream_t* iostream = ostream;

    // init fstream
    tb_basic_stream_t* fstream = tb_basic_stream_init_filter_from_null(iostream);

    // done
    if (istream && ostream && fstream) 
    {
        // save it
        tb_hong_t save = 0;
        if (iostream == istream) save = tb_transfer_done(fstream, ostream, 0, tb_null, tb_null);
        else save = tb_transfer_done(istream, fstream, 0, tb_null, tb_null);

        // trace
        tb_trace_i("save: %lld bytes, size: %lld bytes", save, tb_basic_stream_size(istream));
    }

    // exit fstream
    tb_basic_stream_exit(fstream);

    // exit istream
    tb_basic_stream_exit(istream);

    // exit ostream
    tb_basic_stream_exit(ostream);
    return 0;
}
