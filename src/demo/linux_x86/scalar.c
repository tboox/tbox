#include "tbox.h"
static void tb_scalar_test_constant()
{
	tb_printf("[scalar]: one = %f\n", tb_scalar_to_float(TB_SCALAR_ONE));
	tb_printf("[scalar]: half = %f\n", tb_scalar_to_float(TB_SCALAR_HALF));
	tb_printf("[scalar]: max = %f\n", tb_scalar_to_float(TB_SCALAR_MAX));
	tb_printf("[scalar]: min = %f\n", tb_scalar_to_float(TB_SCALAR_MIN));
	tb_printf("[scalar]: nan = %f\n", tb_scalar_to_float(TB_SCALAR_NAN));
	tb_printf("[scalar]: inf = %f\n", tb_scalar_to_float(TB_SCALAR_INF));
	tb_printf("[scalar]: pi = %f\n", tb_scalar_to_float(TB_SCALAR_PI));
	tb_printf("[scalar]: sqrt2 = %f\n", tb_scalar_to_float(TB_SCALAR_SQRT2));
}
static void tb_scalar_test_round(tb_scalar_t x)
{
	tb_printf("[scalar]: round(%f): %d\n", tb_scalar_to_float(x), tb_scalar_round(x));
}
static void tb_scalar_test_floor(tb_scalar_t x)
{
	tb_printf("[scalar]: floor(%f): %d\n", tb_scalar_to_float(x), tb_scalar_floor(x));
}
static void tb_scalar_test_ceil(tb_scalar_t x)
{
	tb_printf("[scalar]: ceil(%f): %d\n", tb_scalar_to_float(x), tb_scalar_ceil(x));
}
static void tb_scalar_test_mul(tb_scalar_t a, tb_scalar_t b)
{
	tb_printf("[scalar]: mul(%f, %f): %f\n", tb_scalar_to_float(a), tb_scalar_to_float(b), tb_scalar_to_float(tb_scalar_mul(a, b)));
}
static void tb_scalar_test_square(tb_scalar_t x)
{
	tb_printf("[scalar]: square(%f): %f\n", tb_scalar_to_float(x), tb_scalar_to_float(tb_scalar_square(x)));
}
static void tb_scalar_test_sqrt(tb_scalar_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_scalar_t r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_scalar_sqrt(x);
	}
	t = tb_mclock() - t;
	tb_printf("[scalar]: sqrt(%f) = %f, %d ms\n", tb_scalar_to_float(x), tb_scalar_to_float(r), (tb_int_t)t);
}
static void tb_scalar_test_ilog2(tb_scalar_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_uint32_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_scalar_ilog2(x);
	}
	t = tb_mclock() - t;
	tb_printf("[scalar]: ilog2(%f) = %d, %d ms\n", tb_scalar_to_float(x), r, (tb_int_t)t);
}
int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	tb_scalar_test_constant();
	tb_scalar_test_round(TB_SCALAR_PI);
	tb_scalar_test_round(-TB_SCALAR_PI);
	tb_scalar_test_floor(TB_SCALAR_PI);
	tb_scalar_test_floor(-TB_SCALAR_PI);
	tb_scalar_test_ceil(TB_SCALAR_PI);
	tb_scalar_test_ceil(-TB_SCALAR_PI);
	tb_scalar_test_mul(TB_SCALAR_PI, -TB_SCALAR_PI);
	tb_scalar_test_square(TB_SCALAR_PI);
	tb_scalar_test_square(-TB_SCALAR_PI);
	tb_scalar_test_sqrt(TB_SCALAR_PI);
	tb_scalar_test_ilog2(TB_SCALAR_PI);
	tb_scalar_test_ilog2(tb_float_to_scalar(1024.));

	return 0;
}
