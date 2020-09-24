/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_string_static_string_main(tb_int_t argc, tb_char_t** argv)
{
    tb_static_string_t  s;
    tb_char_t       b[4096];
    tb_static_string_init(&s, b, 4096);

    tb_static_string_cstrcpy(&s, "hello");
    tb_static_string_chrcat(&s, ' ');
    tb_static_string_cstrfcat(&s, "%s", "world");
    tb_trace_i("%s", tb_static_string_cstr(&s));

    tb_static_string_exit(&s);

    return 0;
}
