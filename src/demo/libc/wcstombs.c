/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_libc_wcstombs_main(tb_int_t argc, tb_char_t** argv)
{
    // convert it
    tb_char_t   data[256];
    tb_size_t   size = tb_wcstombs(data, L"中文", tb_arrayn(data));
    tb_assert_and_check_return_val(size != -1, 0);

    // trace
    tb_printf("中文: %s\n", data);
    return 0;
}
