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
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_int_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_float_round(x);
	}
	t = tb_mclock() - t;
	tb_printf("[float]: round(%f): %d, %d ms\n", tb_float_to_float(x), r, (tb_int_t)t);
}
static void tb_float_test_floor(tb_float_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_int_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_float_floor(x);
	}
	t = tb_mclock() - t;
	tb_printf("[float]: floor(%f): %d, %d ms\n", tb_float_to_float(x), r, (tb_int_t)t);
}
static void tb_float_test_ceil(tb_float_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_int_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_float_ceil(x);
	}
	t = tb_mclock() - t;
	tb_printf("[float]: ceil(%f): %d, %d ms\n", tb_float_to_float(x), r, (tb_int_t)t);
}
static void tb_float_test_mul(tb_float_t a, tb_float_t b)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_float_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_float_mul(a, b);
	}
	t = tb_mclock() - t;
	tb_printf("[float]: mul(%f, %f): %f, %d ms\n", tb_float_to_float(a), tb_float_to_float(b), tb_float_to_float(r), (tb_int_t)t);

}
static void tb_float_test_div(tb_float_t a, tb_float_t b)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_float_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_float_div(a, b);
	}
	t = tb_mclock() - t;
	tb_printf("[float]: div(%f, %f): %f, %d ms\n", tb_float_to_float(a), tb_float_to_float(b), tb_float_to_float(r), (tb_int_t)t);
}
static void tb_float_test_invert(tb_float_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_float_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_float_invert(x);
	}
	t = tb_mclock() - t;
	tb_printf("[float]: invert(%f): %f, %d ms\n", tb_float_to_float(x), tb_float_to_float(r), (tb_int_t)t);
}
static void tb_float_test_sqre(tb_float_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_float_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_float_sqre(x);
	}
	t = tb_mclock() - t;
	tb_printf("[float]: sqre(%f): %f, %d ms\n", tb_float_to_float(x), tb_float_to_float(r), (tb_int_t)t);
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
static void tb_float_test_iclog2(tb_float_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_uint32_t	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_float_iclog2(x);
	}
	t = tb_mclock() - t;
	tb_printf("[float]: iclog2(%f) = %d, %d ms\n", tb_float_to_float(x), r, (tb_int_t)t);
}
static void tb_float_test_irlog2(tb_float_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_uint32_t	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_float_irlog2(x);
	}
	t = tb_mclock() - t;
	tb_printf("[float]: irlog2(%f) = %d, %d ms\n", tb_float_to_float(x), r, (tb_int_t)t);
}
static void tb_float_test_exp(tb_float_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_float_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_float_exp(x);
	}
	t = tb_mclock() - t;
	tb_printf("[float]: exp(%f) = %f, %d ms\n", tb_float_to_float(x), r, (tb_int_t)t);
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
	tb_float_test_div(TB_FLOAT_ONE, TB_FLOAT_PI);
	tb_float_test_invert(TB_FLOAT_PI);
	tb_float_test_sqre(TB_FLOAT_PI);
	tb_float_test_sqre(-TB_FLOAT_PI);
	tb_float_test_sqrt(TB_FLOAT_PI);
	tb_float_test_exp(TB_FLOAT_PI);
	tb_float_test_ilog2(TB_FLOAT_PI);
	tb_float_test_ilog2(1024.0);
	tb_float_test_iclog2(TB_FLOAT_PI);
	tb_float_test_irlog2(TB_FLOAT_PI);
#endif

	return 0;
}

