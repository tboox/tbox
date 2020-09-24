/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_hash_md5_main(tb_int_t argc, tb_char_t** argv)
{
    tb_byte_t ob[16];
    tb_size_t on = tb_md5_make((tb_byte_t const*)argv[1], tb_strlen(argv[1]), ob, 16);
    if (on != 16) return 0;

    tb_size_t i = 0;
    tb_char_t md5[256] = {0};
    for (i = 0; i < 16; ++i) tb_snprintf(md5 + (i << 1), 3, "%02x", ob[i]);
    tb_printf("%s: %lu\n", md5, on);

    return 0;
}
