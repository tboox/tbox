/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * test
 */
tb_void_t tb_demo_test3(tb_noarg_t);
tb_void_t tb_demo_test3()
{
    tb_backtrace_dump("\t", tb_null, 10);
}
static tb_void_t tb_demo_test2()
{
    tb_demo_test3();
}
tb_void_t tb_demo_test(tb_size_t size);
tb_void_t tb_demo_test(tb_size_t size)
{
    if (size) tb_demo_test(size - 1);
    else tb_demo_test2();
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_backtrace_main(tb_int_t argc, tb_char_t** argv)
{
    // done
    tb_demo_test(argv[1]? tb_atoi(argv[1]) : 10);
    return 0;
}
