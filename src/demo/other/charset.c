/* ///////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// check
	tb_assert_and_check_return_val(argc == 5, 0);

	// init tbox
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// init stream
	tb_gstream_t* ist = tb_gstream_init_from_url(argv[1]);
	tb_gstream_t* ost = tb_gstream_init_from_url(argv[2]);
	if (ost) tb_gstream_ctrl(ost, TB_GSTREAM_CTRL_FILE_SET_MODE, TB_FILE_MODE_WO | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC);
	if (ist && ost && tb_gstream_bopen(ist) && tb_gstream_bopen(ost))
	{
		// init data & size
		tb_size_t 	isize = (tb_size_t)tb_gstream_size(ist);
		tb_long_t 	osize = isize << 2;
		tb_byte_t* 	idata = tb_malloc(isize);
		tb_byte_t* 	odata = tb_malloc(osize);
		if (idata && odata && tb_gstream_bread(ist, idata, isize))
		{
			// conv
			osize = tb_charset_conv_data(tb_charset_type(argv[3]), tb_charset_type(argv[4]), idata, isize, odata, osize);
			tb_print("conv: %ld bytes", osize);
			
			// save
			if (osize > 0) tb_gstream_bwrit(ost, odata, osize);
		}

		// exit data
		if (idata) tb_free(idata);
		if (odata) tb_free(odata);
	
		// exit stream
		tb_gstream_exit(ist);
		tb_gstream_exit(ost);
	}
	

	// exit tbox
	tb_exit();
}

