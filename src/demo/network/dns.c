/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_DNS_TEST_INVALID_HOST 		(0)

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// test the invalid host
#if TB_DNS_TEST_INVALID_HOST
	// add not dns host
	tb_dns_list_adds("127.0.0.1");

	// add not ipv4 host
	tb_dns_list_adds("localhost");
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
