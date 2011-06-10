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
static void tb_scalar_test_div(tb_scalar_t a, tb_scalar_t b)
{
	tb_printf("[scalar]: div(%f, %f): %f\n", tb_scalar_to_float(a), tb_scalar_to_float(b), tb_scalar_to_float(tb_scalar_div(a, b)));
}
static void tb_scalar_test_sqre(tb_scalar_t x)
{
	tb_printf("[scalar]: sqre(%f): %f\n", tb_scalar_to_float(x), tb_scalar_to_float(tb_scalar_sqre(x)));
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
	__tb_volatile__ tb_uint32_t r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_scalar_ilog2(x);
	}
	t = tb_mclock() - t;
	tb_printf("[scalar]: ilog2(%f) = %d, %d ms\n", tb_scalar_to_float(x), r, (tb_int_t)t);
}
static void tb_scalar_test_iclog2(tb_scalar_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_uint32_t r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_scalar_iclog2(x);
	}
	t = tb_mclock() - t;
	tb_printf("[scalar]: iclog2(%f) = %d, %d ms\n", tb_scalar_to_float(x), r, (tb_int_t)t);
}
static void tb_scalar_test_irlog2(tb_scalar_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_uint32_t r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_scalar_irlog2(x);
	}
	t = tb_mclock() - t;
	tb_printf("[scalar]: irlog2(%f) = %d, %d ms\n", tb_scalar_to_float(x), r, (tb_int_t)t);
}
static void tb_scalar_test_exp(tb_scalar_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_scalar_t r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_scalar_exp(x);
	}
	t = tb_mclock() - t;
	tb_printf("[scalar]: exp(%f) = %f, %d ms\n", tb_scalar_to_float(x), tb_scalar_to_float(r), (tb_int_t)t);
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
	tb_scalar_test_div(TB_SCALAR_PI, tb_int_to_scalar(10));
	tb_scalar_test_sqre(TB_SCALAR_PI);
	tb_scalar_test_sqre(-TB_SCALAR_PI);
	tb_scalar_test_sqrt(TB_SCALAR_PI);
	tb_scalar_test_exp(TB_SCALAR_PI);
	tb_scalar_test_ilog2(TB_SCALAR_PI);
	tb_scalar_test_ilog2(tb_float_to_scalar(1024.));
	tb_scalar_test_iclog2(TB_SCALAR_PI);
	tb_scalar_test_irlog2(TB_SCALAR_PI);

	return 0;
}
