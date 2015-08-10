/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_utils_adler32_main(tb_int_t argc, tb_char_t** argv)
{
    // data
    tb_byte_t*  p = (tb_byte_t*)argv[1];
    tb_size_t   n = tb_strlen((tb_char_t const*)p);

    // trace
    tb_printf("[adler32]: %x\n", tb_adler32_encode(0, p, n));
    return 0;
}
