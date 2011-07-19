#include "tbox.h"
int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	tb_char_t ob[4096];
	tb_size_t on = tb_base64_encode(argv[1], tb_cstring_size(argv[1]), ob, 4096);
	//tb_size_t on = tb_base64_decode(argv[1], tb_cstring_size(argv[1]), ob, 4096);
	tb_printf("%s\n", ob);

	tb_exit();
	return 0;
}
