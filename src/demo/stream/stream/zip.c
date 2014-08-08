/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
#ifdef TB_CONFIG_MODULE_HAVE_ZIP
tb_int_t tb_demo_stream_zip_main(tb_int_t argc, tb_char_t** argv)
{
    // init istream
    tb_stream_ref_t istream = tb_stream_init_from_url(argv[1]);

    // init ostream
    tb_stream_ref_t ostream = tb_stream_init_from_file(argv[2], TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC);

    // filter istream or ostream?
    tb_stream_ref_t iostream = istream;
//  tb_stream_ref_t iostream = ostream;

    // init fstream
//  tb_stream_ref_t fstream = tb_stream_init_filter_from_zip(iostream, TB_ZIP_ALGO_RLC, TB_ZIP_ACTION_INFLATE);
//  tb_stream_ref_t fstream = tb_stream_init_filter_from_zip(iostream, TB_ZIP_ALGO_RLC, TB_ZIP_ACTION_DEFLATE);
//  tb_stream_ref_t fstream = tb_stream_init_filter_from_zip(iostream, TB_ZIP_ALGO_ZLIB, TB_ZIP_ACTION_INFLATE);
//  tb_stream_ref_t fstream = tb_stream_init_filter_from_zip(iostream, TB_ZIP_ALGO_ZLIB, TB_ZIP_ACTION_DEFLATE);
//  tb_stream_ref_t fstream = tb_stream_init_filter_from_zip(iostream, TB_ZIP_ALGO_GZIP, TB_ZIP_ACTION_INFLATE);
    tb_stream_ref_t fstream = tb_stream_init_filter_from_zip(iostream, TB_ZIP_ALGO_GZIP, TB_ZIP_ACTION_DEFLATE);   
//  tb_stream_ref_t fstream = tb_stream_init_filter_from_zip(iostream, TB_ZIP_ALGO_ZLIBRAW, TB_ZIP_ACTION_INFLATE);
//  tb_stream_ref_t fstream = tb_stream_init_filter_from_zip(iostream, TB_ZIP_ALGO_ZLIBRAW, TB_ZIP_ACTION_DEFLATE);

    // done
    if (istream && ostream && fstream) 
    {
        // save it
        tb_hong_t save = 0;
        if (iostream == istream) save = tb_transfer_done(fstream, ostream, 0, tb_null, tb_null);
        else save = tb_transfer_done(istream, fstream, 0, tb_null, tb_null);

        // trace
        tb_trace_i("save: %lld bytes, size: %lld bytes", save, tb_stream_size(istream));
    }

    // exit fstream
    tb_stream_exit(fstream);

    // exit istream
    tb_stream_exit(istream);

    // exit ostream
    tb_stream_exit(ostream);
    return 0;
}
#else
tb_int_t tb_demo_stream_zip_main(tb_int_t argc, tb_char_t** argv)
{
    return 0;
}
#endif
