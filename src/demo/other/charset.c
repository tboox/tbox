/* ///////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_other_charset_main(tb_int_t argc, tb_char_t** argv)
{
	// check
	tb_assert_and_check_return_val(argc == 5, 0);

	// init stream
	tb_gstream_t* istream = tb_gstream_init_from_url(argv[1]);
	tb_gstream_t* ostream = tb_gstream_init_from_file(argv[2], TB_FILE_MODE_WO | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC);
	if (istream && ostream && tb_gstream_open(istream) && tb_gstream_open(ostream))
	{
		// init data & size
		tb_hong_t isize = tb_stream_size(istream);
		if (isize > 0)
		{
			tb_long_t 	osize = isize << 2;
			tb_byte_t* 	idata = tb_malloc(isize);
			tb_byte_t* 	odata = tb_malloc(osize);
			if (idata && odata && tb_gstream_bread(istream, idata, isize))
			{
				// conv
				osize = tb_charset_conv_data(tb_charset_type(argv[3]), tb_charset_type(argv[4]), idata, isize, odata, osize);
				tb_trace_i("conv: %ld bytes", osize);
				
				// save
				if (osize > 0) tb_gstream_bwrit(ostream, odata, osize);
			}

			// exit data
			if (idata) tb_free(idata);
			if (odata) tb_free(odata);
		}
	
		// exit stream
		tb_gstream_exit(istream);
		tb_gstream_exit(ostream);
	}

	return 0;
}

