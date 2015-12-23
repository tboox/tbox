/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * test
 */
static tb_void_t tb_integer_test_isqrti(tb_uint32_t x)
{
    __tb_volatile__ tb_long_t   n = 10000000;
    __tb_volatile__ tb_uint32_t r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = tb_isqrti(x);
    }
    t = tb_mclock() - t;
    tb_printf("[integer]: isqrti(%u): %u, %lld ms\n", x, r, t);
}
static tb_void_t tb_integer_test_isqrti64(tb_uint64_t x)
{
    __tb_volatile__ tb_long_t   n = 10000000;
    __tb_volatile__ tb_uint32_t r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = tb_isqrti64(x);
    }
    t = tb_mclock() - t;
    tb_printf("[integer]: isqrti64(%llu): %u, %lld ms\n", x, r, t);
}
static tb_void_t tb_integer_test_ilog2i(tb_uint32_t x)
{
    __tb_volatile__ tb_long_t   n = 10000000;
    __tb_volatile__ tb_long_t   r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = tb_ilog2i(x);
    }
    t = tb_mclock() - t;
    tb_printf("[integer]: ilog2i(%u): %lu, %lld ms\n", x, r, t);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_libm_integer_main(tb_int_t argc, tb_char_t** argv)
{
    // ilog2i
    tb_integer_test_ilog2i(0);
    tb_integer_test_ilog2i(256);
    tb_integer_test_ilog2i(1096);
    tb_integer_test_ilog2i(65537);
    tb_integer_test_ilog2i(1 << 30);

    // isqrti
    tb_integer_test_isqrti(0);
    tb_integer_test_isqrti(256);
    tb_integer_test_isqrti(1096);
    tb_integer_test_isqrti(65537);
    tb_integer_test_isqrti(1 << 30);

    // isqrti64
    tb_integer_test_isqrti64(0);
    tb_integer_test_isqrti64(256);
    tb_integer_test_isqrti64(1096);
    tb_integer_test_isqrti64(65537);
    tb_integer_test_isqrti64(1 << 30);
    tb_integer_test_isqrti64(1ULL << 35);
    tb_integer_test_isqrti64(1ULL << 45);
    tb_integer_test_isqrti64(1ULL << 60);
    tb_integer_test_isqrti64(1ULL << 62);
    tb_integer_test_isqrti64((1ULL << 56) + 123456789);

    return 0;
}
