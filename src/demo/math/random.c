/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * test
 */
static tb_void_t tb_random_test(tb_long_t b, tb_long_t e)
{
    // init
    __tb_volatile__ tb_size_t   i = 0;
    __tb_volatile__ tb_sint32_t n = 1000000;
    __tb_volatile__ tb_long_t   rand = 0;

    // done
    tb_hong_t t = tb_mclock();
    for (i = 0; i < n; i++) rand += tb_random_range(b, e);
    t = tb_mclock() - t;

    // trace
    tb_trace_i("time: %lld, average: %d, range: %ld - %ld", t, (rand + (n >> 1)) / n, b, e);
}
#ifdef TB_CONFIG_TYPE_HAVE_FLOAT
static tb_void_t tb_random_test_float(tb_float_t b, tb_float_t e)
{
    // init
    __tb_volatile__ tb_size_t   i = 0;
    __tb_volatile__ tb_size_t   n = 1000000;
    __tb_volatile__ tb_float_t  rand = 0;

    // done
    tb_hong_t t = tb_mclock();
    for (i = 0; i < n; i++) rand += tb_random_rangef(b, e);
    t = tb_mclock() - t;

    // trace
    tb_trace_i("time: %lld, average: %f, range: %f - %f", t, rand / n, b, e);
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_math_random_main(tb_int_t argc, tb_char_t** argv)
{
    tb_random_test(600, 1000);
    tb_random_test(100, 200);
    tb_random_test(-600, 1000);
    tb_random_test(-600, 200);
    tb_random_test(-600, -200);

#ifdef TB_CONFIG_TYPE_HAVE_FLOAT
    tb_random_test_float(0., 1.);
    tb_random_test_float(0., 200.);
    tb_random_test_float(-200., 0.);
    tb_random_test_float(-200., 200.);
#endif

    return 0;
}
