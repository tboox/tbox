/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// list
#if 0
	tb_dns_list_adds("211.95.1.123");
	tb_dns_list_adds("180.168.255.18");
	tb_dns_list_adds("180.168.255.118");
#endif

	// dump
#ifdef TB_DEBUG
	tb_dns_list_dump();
#endif

	// done
	tb_char_t 			data[16];
	tb_char_t const* 	addr = tb_dns_look_done(argv[1], data, 16);
	tb_print("[demo]: %s => %s", argv[1], addr);

	// exit
	tb_exit();
	return 0;
}
