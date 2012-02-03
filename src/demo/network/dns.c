/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_DNS_TEST_INVALID_HOST 		(1)
#define TB_DNS_TEST_MORE_HOST 			(1)

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

	// test the more host
#if TB_DNS_TEST_MORE_HOST
	tb_dns_list_adds("205.252.144.228");
	tb_dns_list_adds("208.151.69.65");
	tb_dns_list_adds("202.181.202.140");
	tb_dns_list_adds("202.181.224.2");
	tb_dns_list_adds("202.175.3.8");
	tb_dns_list_adds("202.175.3.3");
	tb_dns_list_adds("168.95.192.1");
	tb_dns_list_adds("168.95.1.1");
	tb_dns_list_adds("208.67.222.222");
	tb_dns_list_adds("205.171.2.65");
	tb_dns_list_adds("193.0.14.129");
	tb_dns_list_adds("202.12.27.33");
	tb_dns_list_adds("202.216.228.18");
	tb_dns_list_adds("209.166.160.132");
	tb_dns_list_adds("208.96.10.221");
	tb_dns_list_adds("61.144.56.101");
	tb_dns_list_adds("202.101.98.55");
	tb_dns_list_adds("202.96.128.166");
	tb_dns_list_adds("202.96.209.134");
	tb_dns_list_adds("221.12.65.228");
#endif

	// dump
#ifdef TB_DEBUG
	tb_dns_list_dump();
#endif

	// time
	tb_int64_t time = tb_mclock();

	// done
	tb_ipv4_t ipv4;
	tb_char_t data[16];
	if (tb_dns_look_try4(argv[1], &ipv4) || tb_dns_look_done(argv[1], &ipv4))
		tb_print("[demo]: %s => %s, %lld ms", argv[1], tb_ipv4_get(&ipv4, data, 16), tb_mclock() - time);

	// exit
	tb_exit();
	return 0;
}
