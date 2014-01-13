/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_stream_gstream_null_main(tb_int_t argc, tb_char_t** argv)
{
	// init stream
	tb_gstream_t* ist = tb_gstream_init_from_url(argv[1]);
	tb_gstream_t* ost = tb_gstream_init_from_url(argv[2]);
	tb_gstream_t* nst = tb_gstream_init_filter_from_null(ist);
	if (ist && ost && nst) 
	{
		// save it
		tb_hong_t save = tb_tstream_save_gg(nst, ost, 0, tb_null, tb_null);

		// trace
		tb_print("save: %lld bytes, size: %llu bytes", save, tb_gstream_size(ist));
	}

	// exit stream
	tb_gstream_exit(nst);
	tb_gstream_exit(ist);
	tb_gstream_exit(ost);
	return 0;
}
