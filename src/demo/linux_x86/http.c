#include "tplat/tplat.h"
#include "../../tbox.h"

int main(int argc, char** argv)
{
	tplat_size_t regular_block_n[TPLAT_POOL_REGULAR_CHUNCK_MAX_COUNT] = {10, 10, 10, 10, 10, 10, 10};
	tplat_pool_create(TB_CONFIG_MEMORY_POOL_INDEX, malloc(1024 * 1024), 1024 * 1024, regular_block_n);

	tb_http_t* http = tb_http_create();
	if (http)
	{
		tb_string_t string;
		tb_string_init(&string);
		if (TB_TRUE == tb_http_open(http, argv[1], argv[2], TB_HTTP_METHOD_GET))
		{
			TB_DBG("url: %s", tb_http_url(http, &string));
			TB_DBG("code: %d stream: %s size: %d"
				, tb_http_code(http)
				, tb_http_stream(http) == TB_TRUE? "true" : "false"
				, tb_http_size(http));

			TB_DBG("reply: %s", tb_http_recv_string(http, &string));
		}
		else
		{
			TB_DBG("cannot request: %s%s%s", argv[1], argv[2]? "?" : "", argv[2]);
		}
		tb_http_destroy(http);
		tb_string_uninit(&string);
	}

	return 0;
}

