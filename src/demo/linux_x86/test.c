#include "tbox.h"

int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

#if 0
	tb_bool_t ret = tb_file_exists(argv[1]);
	tb_printf("[%s]: %s\n", ret? "true" : "false", argv[1]);
#endif

#if 1
	tb_bool_t ret = tb_file_create(argv[1], TB_FILE_TYPE_DIR);
	tb_printf("[%s]: %s\n", ret? "true" : "false", argv[1]);
#endif

	return 0;
}
