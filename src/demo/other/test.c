#include "tbox.h"

int test2_main(int argc, char** argv);

int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// c++
	test2_main(argc, argv);

#if 0
	tb_bool_t ret = tb_file_exists(argv[1]);
	tb_printf("[%s]: %s\n", ret? "true" : "false", argv[1]);
#endif

#if 0
	tb_bool_t ret = tb_file_delete(argv[1], TB_FILE_TYPE_FILE);
	tb_printf("[%s]: %s\n", ret? "true" : "false", argv[1]);
#endif

	tb_hong_t t = tb_uclock();
	tb_msleep(1);
	t = tb_uclock() - t;
	tb_print("%lld", t);
	return 0;
}
