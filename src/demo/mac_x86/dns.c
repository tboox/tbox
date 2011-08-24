#include "tbox.h"

int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	tb_dns_server_add("211.95.1.123");
	tb_dns_server_add("180.168.255.18");
	tb_dns_server_add("180.168.255.118");
	tb_dns_server_dump();

	if (!argv[1]) return 0;

	tb_char_t ip[16];
	tb_char_t const* s = tb_dns_lookup(argv[1], ip);
	tb_printf("host: %s, ip: %s\n", argv[1], s? s : "");
	return 0;
}
