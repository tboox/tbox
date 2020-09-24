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
    tb_wprintf(L"wcs: %s (%zu chars) %x", L"中文", tb_arrayn(L"中文") - 1, L"中文"[0]);
    for (tb_size_t i = 1; i < tb_arrayn(L"中文") - 1; i++)
    {
        tb_printf(",%x", L"中文"[i]);
    }
    tb_print("\n");

    tb_printf("mbs: %s (%zu chars) %I8x", data, size, data[0]);
    for (tb_size_t i = 1; i < size; i++)
    {
        tb_printf(",%I8x", data[i]);
    }
    tb_print("\n");
    return 0;
}
