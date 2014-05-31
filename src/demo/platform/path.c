/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_path_main(tb_int_t argc, tb_char_t** argv)
{
    // full
    tb_char_t full[TB_PATH_MAXN] = {0};
    tb_trace_i("%s", tb_path_full(argv[1], full, TB_PATH_MAXN));
    return 0;
}
