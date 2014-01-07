/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* ///////////////////////////////////////////////////////////////////////
 * test
 */
static tb_void_t tb_integer_test_isqrti(tb_size_t x)
{
	__tb_volatile__ tb_long_t 	n = 10000000;
	__tb_volatile__ tb_long_t 	r = 0;
	tb_hong_t t = tb_mclock();
	while (n--)
	{
		r = tb_isqrti(x);
	}
	t = tb_mclock() - t;
	tb_printf("[integer]: isqrti(%lu): %lu, %lld ms\n", x, r, t);
}
static tb_void_t tb_integer_test_ilog2i(tb_size_t x)
{
	__tb_volatile__ tb_long_t 	n = 10000000;
	__tb_volatile__ tb_long_t 	r = 0;
	tb_hong_t t = tb_mclock();
	while (n--)
	{
		r = tb_ilog2i(x);
	}
	t = tb_mclock() - t;
	tb_printf("[integer]: ilog2i(%lu): %lu, %lld ms\n", x, r, t);
}

/* ///////////////////////////////////////////////////////////////////////
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

	return 0;
}
