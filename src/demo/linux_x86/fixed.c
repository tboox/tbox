#include "tbox.h"
static void tb_fixed_test_constant()
{
	tb_printf("[fixed]: one = %f\n", tb_fixed_to_float(TB_FIXED_ONE));
	tb_printf("[fixed]: half = %f\n", tb_fixed_to_float(TB_FIXED_HALF));
	tb_printf("[fixed]: max = %f\n", tb_fixed_to_float(TB_FIXED_MAX));
	tb_printf("[fixed]: min = %f\n", tb_fixed_to_float(TB_FIXED_MIN));
	tb_printf("[fixed]: nan = %f\n", tb_fixed_to_float(TB_FIXED_NAN));
	tb_printf("[fixed]: inf = %f\n", tb_fixed_to_float(TB_FIXED_INF));
	tb_printf("[fixed]: pi = %f\n", tb_fixed_to_float(TB_FIXED_PI));
	tb_printf("[fixed]: sqrt2 = %f\n", tb_fixed_to_float(TB_FIXED_SQRT2));
}
static void tb_fixed_test_round(tb_fixed_t x)
{
	tb_printf("[fixed]: round(%f): %d\n", tb_fixed_to_float(x), tb_fixed_round(x));
}
static void tb_fixed_test_floor(tb_fixed_t x)
{
	tb_printf("[fixed]: floor(%f): %d\n", tb_fixed_to_float(x), tb_fixed_floor(x));
}
static void tb_fixed_test_ceil(tb_fixed_t x)
{
	tb_printf("[fixed]: ceil(%f): %d\n", tb_fixed_to_float(x), tb_fixed_ceil(x));
}
static void tb_fixed_test_mul(tb_fixed_t a, tb_fixed_t b)
{
	tb_printf("[fixed]: mul(%f, %f): %f\n", tb_fixed_to_float(a), tb_fixed_to_float(b), tb_fixed_to_float(tb_fixed_mul(a, b)));
}
static void tb_fixed_test_square(tb_fixed_t x)
{
	tb_printf("[fixed]: square(%f): %f\n", tb_fixed_to_float(x), tb_fixed_to_float(tb_fixed_square(x)));
}
static void tb_fixed_test_sqrt(tb_fixed_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_fixed_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_fixed_sqrt(x);
	}
	t = tb_mclock() - t;
	tb_printf("[fixed]: sqrt(%f) = %f, %d ms\n", tb_fixed_to_float(x), tb_fixed_to_float(r), (tb_int_t)t);
}
static void tb_fixed_test_ilog2(tb_fixed_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_uint32_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_fixed_ilog2(x);
	}
	t = tb_mclock() - t;
	tb_printf("[fixed]: ilog2(%f) = %d, %d ms\n", tb_fixed_to_float(x), r, (tb_int_t)t);
}
int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	tb_fixed_test_constant();
	tb_fixed_test_round(TB_FIXED_PI);
	tb_fixed_test_round(-TB_FIXED_PI);
	tb_fixed_test_floor(TB_FIXED_PI);
	tb_fixed_test_floor(-TB_FIXED_PI);
	tb_fixed_test_ceil(TB_FIXED_PI);
	tb_fixed_test_ceil(-TB_FIXED_PI);
	tb_fixed_test_mul(TB_FIXED_PI, -TB_FIXED_PI);
	tb_fixed_test_square(TB_FIXED_PI);
	tb_fixed_test_square(-TB_FIXED_PI);
	tb_fixed_test_sqrt(TB_FIXED_PI);
	tb_fixed_test_ilog2(TB_FIXED_PI);
	tb_fixed_test_ilog2(tb_float_to_fixed(1024.));

	return 0;
}
