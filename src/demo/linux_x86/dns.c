#include "tplat/tplat.h"
#include "../../tbox.h"

int main(int argc, char** argv)
{
	tplat_size_t regular_block_n[TPLAT_POOL_REGULAR_CHUNCK_MAX_COUNT] = {10, 10, 10, 10, 10, 10, 10};
	tplat_pool_create(TB_CONFIG_MEMORY_POOL_INDEX, malloc(1024 * 1024), 1024 * 1024, regular_block_n);

	tb_dns_server_add("211.95.1.123");
	tb_dns_server_add("180.168.255.18");
	tb_dns_server_add("180.168.255.118");
	tb_dns_server_dump();

	if (!argv[1]) return 0;

	tb_char_t ip[16];
	tb_char_t const* s = tb_dns_lookup(argv[1], ip);
	tplat_printf("host: %s, ip: %s\n", argv[1], s? s : "");
	return 0;
}
