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
    tb_printf("mbs: %s (%zu chars) %I8x", "中文", tb_arrayn("中文") - 1, "中文"[0]);
    for (tb_size_t i = 1; i < tb_arrayn("中文") - 1; i++)
    {
        tb_printf(",%I8x", "中文"[i]);
    }
    tb_print("\n");
    tb_wprintf(L"wcs: %s (%zu chars) %x", data, size, data[0]);
    for (tb_size_t i = 1; i < size; i++)
    {
        tb_printf(",%x", data[i]);
    }
    tb_print("\n");

    return 0;
    return 0;
}
