#include "tplat/tplat.h"
#include "../../tbox.h"

#define MATH_TEST_FEXPF1 		(1)
#define MATH_TEST_FEXPI 		(1)
#define MATH_TEST_FEXPF 		(1)

#define MATH_TEST_FPOW2F1 		(1)
#define MATH_TEST_FPOW2I 		(1)
#define MATH_TEST_FPOW2F 		(1)


/* ////////////////////////////////////////////////////////////////////////
 * exp
 */
static void tb_math_test_fexpf1(tb_float_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_float_t 	r = 0.0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = TB_MATH_FEXPF1(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_fexpf1(%lf) = %lf\n", (tb_int_t)t, x, r);
}
static void tb_math_test_fexpf1_libc(tb_float_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_float_t 	r = 0.0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = exp(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_fexpf1_libc(%lf) = %lf\n", (tb_int_t)t, x, r);
}
static void tb_math_test_fexpi(tb_int_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_float_t 	r = 0.0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = TB_MATH_FEXPI(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_fexpi(%d) = %lf\n", (tb_int_t)t, x, r);
}
static void tb_math_test_fexpi_libc(tb_int_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_float_t 	r = 0.0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = exp((tb_float_t)x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_fexpi_libc(%d) = %lf\n", (tb_int_t)t, x, r);
}
static void tb_math_test_fexpf(tb_float_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_float_t 	r = 0.0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = TB_MATH_FEXPF(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_fexpf(%lf) = %lf\n", (tb_int_t)t, x, r);
}
static void tb_math_test_fexpf_libc(tb_float_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_float_t 	r = 0.0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = exp(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_fexpf_libc(%lf) = %lf\n", (tb_int_t)t, x, r);
}
/* ////////////////////////////////////////////////////////////////////////
 * pow
 */
static void tb_math_test_fpow2f1(tb_float_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_float_t 	r = 0.0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = TB_MATH_FPOW2F1(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_fpow2f1(%lf) = %lf\n", (tb_int_t)t, x, r);
}
static void tb_math_test_fpow2f1_libc(tb_float_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_float_t 	r = 0.0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = pow(2, x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_fpow2f1_libc(%lf) = %lf\n", (tb_int_t)t, x, r);
}
static void tb_math_test_fpow2i(tb_int_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_float_t 	r = 0.0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = TB_MATH_FPOW2I(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_fpow2i(%d) = %lf\n", (tb_int_t)t, x, r);
}
static void tb_math_test_fpow2i_libc(tb_int_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_float_t 	r = 0.0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = pow(2, x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_fpow2i_libc(%d) = %lf\n", (tb_int_t)t, x, r);
}
static void tb_math_test_fpow2f(tb_float_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_float_t 	r = 0.0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = TB_MATH_FPOW2F(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_fpow2f(%lf) = %lf\n", (tb_int_t)t, x, r);
}
static void tb_math_test_fpow2f_libc(tb_float_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_float_t 	r = 0.0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = pow(2, x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_fpow2f_libc(%lf) = %lf\n", (tb_int_t)t, x, r);
}

/* ////////////////////////////////////////////////////////////////////////
 * table
 */
static void tb_math_make_fexpi_table()
{
	tplat_printf(",\t%lf\n", exp(-31.));
	tplat_printf(",\t%lf\n", exp(-30.));
	tplat_printf(",\t%lf\n", exp(-29.));
	tplat_printf(",\t%lf\n", exp(-28.));
	tplat_printf(",\t%lf\n", exp(-27.));
	tplat_printf(",\t%lf\n", exp(-26.));
	tplat_printf(",\t%lf\n", exp(-25.));
	tplat_printf(",\t%lf\n", exp(-24.));
	tplat_printf(",\t%lf\n", exp(-23.));
	tplat_printf(",\t%lf\n", exp(-22.));
	tplat_printf(",\t%lf\n", exp(-21.));
	tplat_printf(",\t%lf\n", exp(-20.));
	tplat_printf(",\t%lf\n", exp(-19.));
	tplat_printf(",\t%lf\n", exp(-18.));
	tplat_printf(",\t%lf\n", exp(-17.));
	tplat_printf(",\t%lf\n", exp(-16.));
	tplat_printf(",\t%lf\n", exp(-15.));
	tplat_printf(",\t%lf\n", exp(-14.));
	tplat_printf(",\t%lf\n", exp(-13.));
	tplat_printf(",\t%lf\n", exp(-12.));
	tplat_printf(",\t%lf\n", exp(-11.));
	tplat_printf(",\t%lf\n", exp(-10.));
	tplat_printf(",\t%lf\n", exp(-9.));
	tplat_printf(",\t%lf\n", exp(-8.));
	tplat_printf(",\t%lf\n", exp(-7.));
	tplat_printf(",\t%lf\n", exp(-6.));
	tplat_printf(",\t%lf\n", exp(-5.));
	tplat_printf(",\t%lf\n", exp(-4.));
	tplat_printf(",\t%lf\n", exp(-3.));
	tplat_printf(",\t%lf\n", exp(-2.));
	tplat_printf(",\t%lf\n", exp(-1.));
	tplat_printf(",\t%lf\n", exp(0.));
	tplat_printf(",\t%lf\n", exp(1.));
	tplat_printf(",\t%lf\n", exp(2.));
	tplat_printf(",\t%lf\n", exp(3.));
	tplat_printf(",\t%lf\n", exp(4.));
	tplat_printf(",\t%lf\n", exp(5.));
	tplat_printf(",\t%lf\n", exp(6.));
	tplat_printf(",\t%lf\n", exp(7.));
	tplat_printf(",\t%lf\n", exp(8.));
	tplat_printf(",\t%lf\n", exp(9.));
	tplat_printf(",\t%lf\n", exp(10.));
	tplat_printf(",\t%lf\n", exp(11.));
	tplat_printf(",\t%lf\n", exp(12.));
	tplat_printf(",\t%lf\n", exp(13.));
	tplat_printf(",\t%lf\n", exp(14.));
	tplat_printf(",\t%lf\n", exp(15.));
	tplat_printf(",\t%lf\n", exp(16.));
	tplat_printf(",\t%lf\n", exp(17.));
	tplat_printf(",\t%lf\n", exp(18.));
	tplat_printf(",\t%lf\n", exp(19.));
	tplat_printf(",\t%lf\n", exp(20.));
	tplat_printf(",\t%lf\n", exp(21.));
	tplat_printf(",\t%lf\n", exp(22.));
	tplat_printf(",\t%lf\n", exp(23.));
	tplat_printf(",\t%lf\n", exp(24.));
	tplat_printf(",\t%lf\n", exp(25.));
	tplat_printf(",\t%lf\n", exp(26.));
	tplat_printf(",\t%lf\n", exp(27.));
	tplat_printf(",\t%lf\n", exp(28.));
	tplat_printf(",\t%lf\n", exp(29.));
	tplat_printf(",\t%lf\n", exp(30.));
	tplat_printf(",\t%lf\n", exp(31.));

}

static void tb_math_make_fpow2i_table()
{
	tplat_printf(",\t%lf\n", pow(2, -31.));
	tplat_printf(",\t%lf\n", pow(2, -30.));
	tplat_printf(",\t%lf\n", pow(2, -29.));
	tplat_printf(",\t%lf\n", pow(2, -28.));
	tplat_printf(",\t%lf\n", pow(2, -27.));
	tplat_printf(",\t%lf\n", pow(2, -26.));
	tplat_printf(",\t%lf\n", pow(2, -25.));
	tplat_printf(",\t%lf\n", pow(2, -24.));
	tplat_printf(",\t%lf\n", pow(2, -23.));
	tplat_printf(",\t%lf\n", pow(2, -22.));
	tplat_printf(",\t%lf\n", pow(2, -21.));
	tplat_printf(",\t%lf\n", pow(2, -20.));
	tplat_printf(",\t%lf\n", pow(2, -19.));
	tplat_printf(",\t%lf\n", pow(2, -18.));
	tplat_printf(",\t%lf\n", pow(2, -17.));
	tplat_printf(",\t%lf\n", pow(2, -16.));
	tplat_printf(",\t%lf\n", pow(2, -15.));
	tplat_printf(",\t%lf\n", pow(2, -14.));
	tplat_printf(",\t%lf\n", pow(2, -13.));
	tplat_printf(",\t%lf\n", pow(2, -12.));
	tplat_printf(",\t%lf\n", pow(2, -11.));
	tplat_printf(",\t%lf\n", pow(2, -10.));
	tplat_printf(",\t%lf\n", pow(2, -9.));
	tplat_printf(",\t%lf\n", pow(2, -8.));
	tplat_printf(",\t%lf\n", pow(2, -7.));
	tplat_printf(",\t%lf\n", pow(2, -6.));
	tplat_printf(",\t%lf\n", pow(2, -5.));
	tplat_printf(",\t%lf\n", pow(2, -4.));
	tplat_printf(",\t%lf\n", pow(2, -3.));
	tplat_printf(",\t%lf\n", pow(2, -2.));
	tplat_printf(",\t%lf\n", pow(2, -1.));
	tplat_printf(",\t%lf\n", pow(2, 0.));
	tplat_printf(",\t%lf\n", pow(2, 1.));
	tplat_printf(",\t%lf\n", pow(2, 2.));
	tplat_printf(",\t%lf\n", pow(2, 3.));
	tplat_printf(",\t%lf\n", pow(2, 4.));
	tplat_printf(",\t%lf\n", pow(2, 5.));
	tplat_printf(",\t%lf\n", pow(2, 6.));
	tplat_printf(",\t%lf\n", pow(2, 7.));
	tplat_printf(",\t%lf\n", pow(2, 8.));
	tplat_printf(",\t%lf\n", pow(2, 9.));
	tplat_printf(",\t%lf\n", pow(2, 10.));
	tplat_printf(",\t%lf\n", pow(2, 11.));
	tplat_printf(",\t%lf\n", pow(2, 12.));
	tplat_printf(",\t%lf\n", pow(2, 13.));
	tplat_printf(",\t%lf\n", pow(2, 14.));
	tplat_printf(",\t%lf\n", pow(2, 15.));
	tplat_printf(",\t%lf\n", pow(2, 16.));
	tplat_printf(",\t%lf\n", pow(2, 17.));
	tplat_printf(",\t%lf\n", pow(2, 18.));
	tplat_printf(",\t%lf\n", pow(2, 19.));
	tplat_printf(",\t%lf\n", pow(2, 20.));
	tplat_printf(",\t%lf\n", pow(2, 21.));
	tplat_printf(",\t%lf\n", pow(2, 22.));
	tplat_printf(",\t%lf\n", pow(2, 23.));
	tplat_printf(",\t%lf\n", pow(2, 24.));
	tplat_printf(",\t%lf\n", pow(2, 25.));
	tplat_printf(",\t%lf\n", pow(2, 26.));
	tplat_printf(",\t%lf\n", pow(2, 27.));
	tplat_printf(",\t%lf\n", pow(2, 28.));
	tplat_printf(",\t%lf\n", pow(2, 29.));
	tplat_printf(",\t%lf\n", pow(2, 30.));
	tplat_printf(",\t%lf\n", pow(2, 31.));

}

int main(int argc, char** argv)
{
	tplat_size_t regular_block_n[TPLAT_POOL_REGULAR_CHUNCK_MAX_COUNT] = {10, 10, 10, 10, 10, 10, 10};
	tplat_pool_create(TB_CONFIG_MEMORY_POOL_INDEX, malloc(1024 * 1024), 1024 * 1024, regular_block_n);

	//tb_math_make_fexpi_table();
	//tb_math_make_fpow2i_table();

#if MATH_TEST_FEXPI
	tplat_printf("===================================:\n");
	tb_math_test_fexpi(3);
	tb_math_test_fexpi(10);
	tb_math_test_fexpi(-3);
	tb_math_test_fexpi(-10);

	tplat_printf("\n");
	tb_math_test_fexpi_libc(3);
	tb_math_test_fexpi_libc(10);
	tb_math_test_fexpi_libc(-3);
	tb_math_test_fexpi_libc(-10);

#endif

#if MATH_TEST_FEXPF1
	tplat_printf("===================================:\n");
	tb_math_test_fexpf1(0.1415926f);
	tb_math_test_fexpf1(-0.1415926f);

	tplat_printf("\n");
	tb_math_test_fexpf1_libc(0.1415926f);
	tb_math_test_fexpf1_libc(-0.1415926f);
#endif

#if MATH_TEST_FEXPF
	tplat_printf("===================================:\n");
	tb_math_test_fexpf(0.1415926f);
	tb_math_test_fexpf(3.1415926f);
	tb_math_test_fexpf(10.1415926f);

	tb_math_test_fexpf(-0.1415926f);
	tb_math_test_fexpf(-3.1415926f);
	tb_math_test_fexpf(-10.1415926f);

	tplat_printf("\n");
	tb_math_test_fexpf_libc(0.1415926f);
	tb_math_test_fexpf_libc(3.1415926f);
	tb_math_test_fexpf_libc(10.1415926f);

	tb_math_test_fexpf_libc(-0.1415926f);
	tb_math_test_fexpf_libc(-3.1415926f);
	tb_math_test_fexpf_libc(-10.1415926f);

#endif

#if MATH_TEST_FPOW2F1
	tplat_printf("===================================:\n");
	tb_math_test_fpow2f1(0.1415926f);
	tb_math_test_fpow2f1(-0.1415926f);

	tplat_printf("\n");
	tb_math_test_fpow2f1_libc(0.1415926f);
	tb_math_test_fpow2f1_libc(-0.1415926f);
#endif

#if MATH_TEST_FPOW2I
	tplat_printf("===================================:\n");
	tb_math_test_fpow2i(3);
	tb_math_test_fpow2i(10);
	tb_math_test_fpow2i(-3);
	tb_math_test_fpow2i(-10);

	tplat_printf("\n");
	tb_math_test_fpow2i_libc(3);
	tb_math_test_fpow2i_libc(10);
	tb_math_test_fpow2i_libc(-3);
	tb_math_test_fpow2i_libc(-10);

#endif

#if MATH_TEST_FPOW2F
	tplat_printf("===================================:\n");
	tb_math_test_fpow2f(0.1415926f);
	tb_math_test_fpow2f(3.1415926f);
	tb_math_test_fpow2f(10.1415926f);

	tb_math_test_fpow2f(-0.1415926f);
	tb_math_test_fpow2f(-3.1415926f);
	tb_math_test_fpow2f(-10.1415926f);

	tplat_printf("\n");
	tb_math_test_fpow2f_libc(0.1415926f);
	tb_math_test_fpow2f_libc(3.1415926f);
	tb_math_test_fpow2f_libc(10.1415926f);

	tb_math_test_fpow2f_libc(-0.1415926f);
	tb_math_test_fpow2f_libc(-3.1415926f);
	tb_math_test_fpow2f_libc(-10.1415926f);
#endif



	return 0;
}
