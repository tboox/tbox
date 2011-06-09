#include "tbox.h"

#ifdef TB_CONFIG_TYPE_FLOAT
static void tb_float_test_constant()
{
	tb_printf("[float]: one = %f\n", tb_float_to_float(TB_FLOAT_ONE));
	tb_printf("[float]: half = %f\n", tb_float_to_float(TB_FLOAT_HALF));
	tb_printf("[float]: max = %f\n", tb_float_to_float(TB_FLOAT_MAX));
	tb_printf("[float]: min = %f\n", tb_float_to_float(TB_FLOAT_MIN));
	tb_printf("[float]: nan = %f\n", tb_float_to_float(TB_FLOAT_NAN));
	tb_printf("[float]: inf = %f\n", tb_float_to_float(TB_FLOAT_INF));
	tb_printf("[float]: pi = %f\n", tb_float_to_float(TB_FLOAT_PI));
	tb_printf("[float]: sqrt2 = %f\n", tb_float_to_float(TB_FLOAT_SQRT2));
}
static void tb_float_test_round(tb_float_t x)
{
	tb_printf("[float]: round(%f): %d\n", tb_float_to_float(x), tb_float_round(x));
}
static void tb_float_test_floor(tb_float_t x)
{
	tb_printf("[float]: floor(%f): %d\n", tb_float_to_float(x), tb_float_floor(x));
}
static void tb_float_test_ceil(tb_float_t x)
{
	tb_printf("[float]: ceil(%f): %d\n", tb_float_to_float(x), tb_float_ceil(x));
}
static void tb_float_test_mul(tb_float_t a, tb_float_t b)
{
	tb_printf("[float]: mul(%f, %f): %f\n", tb_float_to_float(a), tb_float_to_float(b), tb_float_to_float(tb_float_mul(a, b)));
}
static void tb_float_test_square(tb_float_t x)
{
	tb_printf("[float]: square(%f): %f\n", tb_float_to_float(x), tb_float_to_float(tb_float_square(x)));
}
static void tb_float_test_sqrt(tb_float_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_float_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_float_sqrt(x);
	}
	t = tb_mclock() - t;
	tb_printf("[float]: sqrt(%f) = %f, %d ms\n", tb_float_to_float(x), tb_float_to_float(r), (tb_int_t)t);
}
static void tb_float_test_ilog2(tb_float_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_uint32_t	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_float_ilog2(x);
	}
	t = tb_mclock() - t;
	tb_printf("[float]: ilog2(%f) = %d, %d ms\n", tb_float_to_float(x), r, (tb_int_t)t);
}
#endif

int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

#ifdef TB_CONFIG_TYPE_FLOAT
	tb_float_test_constant();
	tb_float_test_round(TB_FLOAT_PI);
	tb_float_test_round(-TB_FLOAT_PI);
	tb_float_test_floor(TB_FLOAT_PI);
	tb_float_test_floor(-TB_FLOAT_PI);
	tb_float_test_ceil(TB_FLOAT_PI);
	tb_float_test_ceil(-TB_FLOAT_PI);
	tb_float_test_mul(TB_FLOAT_PI, -TB_FLOAT_PI);
	tb_float_test_square(TB_FLOAT_PI);
	tb_float_test_square(-TB_FLOAT_PI);
	tb_float_test_sqrt(TB_FLOAT_PI);
	tb_float_test_ilog2(TB_FLOAT_PI);
	tb_float_test_ilog2(1024.0);
#endif

	return 0;
}

