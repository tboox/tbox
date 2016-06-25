/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_libc_mbstowcs_main(tb_int_t argc, tb_char_t** argv)
{
    // convert it
    tb_wchar_t  data[256];
    tb_size_t   size = tb_mbstowcs(data, "中文", tb_arrayn(data));
    tb_assert_and_check_return_val(size != -1, 0);

    // trace
    tb_wprintf(L"%s\n", data);
    return 0;
}
