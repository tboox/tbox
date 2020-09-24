/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

static tb_void_t tb_test_sha(tb_size_t mode, tb_char_t const* data)
{
    tb_byte_t ob[32];
    tb_size_t on = tb_sha_make(mode, (tb_byte_t const*)data, tb_strlen(data), ob, 32);
    tb_assert_and_check_return((on << 3) == mode);

    tb_size_t i = 0;
    tb_char_t sha[256] = {0};
    for (i = 0; i < on; ++i) tb_snprintf(sha + (i << 1), 3, "%02X", ob[i]);
    tb_printf("[sha]: %d = %s\n", mode, sha);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_hash_sha_main(tb_int_t argc, tb_char_t** argv)
{
    tb_test_sha(TB_SHA_MODE_SHA1_160, argv[1]);
    tb_test_sha(TB_SHA_MODE_SHA2_224, argv[1]);
    tb_test_sha(TB_SHA_MODE_SHA2_256, argv[1]);

    return 0;
}
