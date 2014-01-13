/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_stream_gstream_charset_main(tb_int_t argc, tb_char_t** argv)
{
	// init stream
	tb_gstream_t* ist = tb_gstream_init_from_url(argv[1]);
	tb_gstream_t* ost = tb_gstream_init_from_url(argv[2]);
	tb_gstream_t* cst = tb_gstream_init_filter_from_charset(ist, tb_charset_type(argv[3]), tb_charset_type(argv[4]));
	if (ist && ost && cst) 
	{
		// save it
		tb_hong_t save = tb_tstream_save_gg(cst, ost, 0, tb_null, tb_null);

		// trace
		tb_print("save: %lld bytes, size: %llu bytes", save, tb_gstream_size(ist));
	}

	// exit stream
	tb_gstream_exit(cst);
	tb_gstream_exit(ist);
	tb_gstream_exit(ost);

	return 0;
}
