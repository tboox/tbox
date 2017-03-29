/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_path_main(tb_int_t argc, tb_char_t** argv)
{
    // the absolute path
    tb_char_t path[TB_PATH_MAXN] = {0};
//    tb_trace_i("%s", tb_path_directory(argv[1], path, TB_PATH_MAXN));
//    tb_trace_i("%s", tb_path_relative_to(argv[2], argv[1], path, TB_PATH_MAXN));
    tb_trace_i("%s", tb_path_absolute_to(argv[2], argv[1], path, TB_PATH_MAXN));
    return 0;
}
