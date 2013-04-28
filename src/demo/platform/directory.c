/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"
#include <stdlib.h>

/* /////////////////////////////////////////////////////////////////////////////////////
 * callback
 */ 
static tb_void_t tb_directory_walk_func(tb_char_t const* path, tb_file_info_t const* info, tb_pointer_t data)
{
	tb_print("path[%c]: %s, size: %llu", info->type == TB_FILE_TYPE_DIR? 'd' : 'f', path, info->size);
}

/* ///////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// temporary
	tb_char_t temp[4096] = {0};
	if (tb_directory_temp(temp, 4096)) tb_print("temporary: %s", temp);

	// current
	tb_char_t curt[4096] = {0};
	if (tb_directory_curt(curt, 4096)) tb_print("current: %s", curt);

	// current
	tb_directory_walk(argv[1]? argv[1] : curt, tb_true, tb_directory_walk_func, tb_null);

	// exit
	tb_exit();
	return 0;
}
