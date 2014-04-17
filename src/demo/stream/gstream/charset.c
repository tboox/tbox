/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_stream_gstream_charset_main(tb_int_t argc, tb_char_t** argv)
{
	// init istream
	tb_gstream_t* istream = tb_gstream_init_from_url(argv[1]);

	// init ostream
	tb_gstream_t* ostream = tb_gstream_init_from_file(argv[2], TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC);

	// filter istream or ostream?
	tb_gstream_t* iostream = istream;
//	tb_gstream_t* iostream = ostream;

	// init fstream
	tb_gstream_t* fstream = tb_gstream_init_filter_from_charset(iostream, tb_charset_type(argv[3]), tb_charset_type(argv[4]));

	// done
	if (istream && ostream && fstream) 
	{
		// save it
		tb_hong_t save = 0;
		if (iostream == istream) save = tb_tstream_save_gg(fstream, ostream, 0, tb_null, tb_null);
		else save = tb_tstream_save_gg(istream, fstream, 0, tb_null, tb_null);

		// trace
		tb_trace_i("save: %lld bytes, size: %lld bytes", save, tb_stream_size(istream));
	}

	// exit fstream
	tb_gstream_exit(fstream);

	// exit istream
	tb_gstream_exit(istream);

	// exit ostream
	tb_gstream_exit(ostream);
	return 0;
}
