/* ///////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 

tb_int_t test2_main(tb_int_t argc, tb_char_t** argv);

tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// c++
	test2_main(argc, argv);

	tb_byte_t byte[1] = {0xf0};
	tb_gstream_t* gst = tb_gstream_init_from_url("sock://255.255.255.255:9090?udp=");
	tb_gstream_bopen(gst);
	tb_print("1");
//	tb_gstream_bwrit(gst, byte, 1);
	tb_print("2");
//	tb_gstream_bfwrit(gst, TB_NULL, 0);
	tb_print("3");
	tb_gstream_wait(gst, TB_AIOO_ETYPE_READ, -1);
	tb_print("4");
	tb_gstream_bread(gst, byte, 1);
	tb_print("%x", byte[0]);

	tb_exit();
	return 0;
}
