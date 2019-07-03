/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_stdfile_main(tb_int_t argc, tb_char_t** argv)
{
    tb_stdfile_ref_t file = tb_stdfile_init(TB_STDFILE_TYPE_STDOUT);
    if (file)
    {
        tb_stdfile_writ(file, (tb_byte_t const*)"hello world!\n", sizeof("hello world!\n"));
        tb_stdfile_exit(file);
    }
    return 0;
}
