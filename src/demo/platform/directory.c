/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* ////////////////////////////////////////////////////////////////////////////////////////////////////
 * callback
 */ 
static tb_void_t tb_directory_walk_func(tb_char_t const* path, tb_file_info_t const* info, tb_cpointer_t priv)
{
	// check
	tb_assert_and_check_return(path && info);

	// the modify time
	tb_tm_t mtime = {0};
	tb_localtime(info->mtime, &mtime);

	// dump
	tb_trace_i( "path[%c]: %s, size: %llu, mtime: %04ld-%02ld-%02ld %02ld:%02ld:%02ld"
			, info->type == TB_FILE_TYPE_DIRECTORY? 'd' : 'f'
			, path
			, info->size
			, mtime.year
			, mtime.month
			, mtime.mday
			, mtime.hour
			, mtime.minute
			, mtime.second);
}


/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_directory_main(tb_int_t argc, tb_char_t** argv)
{
#if 1
	// temporary
	tb_char_t temp[4096] = {0};
	if (tb_directory_temp(temp, 4096)) tb_trace_i("temporary: %s", temp);

	// current
	tb_char_t curt[4096] = {0};
	if (tb_directory_curt(curt, 4096)) tb_trace_i("current: %s", curt);

	// current
	tb_directory_walk(argv[1]? argv[1] : curt, tb_true, tb_true, tb_directory_walk_func, tb_null);
#elif 0
	tb_directory_remove(argv[1]);
#else
//	tb_directory_walk(argv[1], tb_true, tb_true, tb_directory_walk_func, tb_null);
	tb_directory_copy(argv[1], argv[2]);
#endif

	return 0;
}
