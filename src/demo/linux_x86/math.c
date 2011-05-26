#include "tbox.h"
#include <math.h>
#include <stdlib.h>

#define MATH_TEST_FEXPF1 		(0)
#define MATH_TEST_FEXPI 		(0)
#define MATH_TEST_FEXPF 		(0)

#define MATH_TEST_FPOW2F1 		(0)
#define MATH_TEST_FPOW2I 		(0)
#define MATH_TEST_FPOW2F 		(0)

#define MATH_TEST_ILOG2I 		(0)
#define MATH_TEST_ILOG2F 		(0)
#define MATH_TEST_IRLOG2I 		(0)
#define MATH_TEST_IRLOG2F 		(0)
#define MATH_TEST_ICLOG2I 		(0)
#define MATH_TEST_ICLOG2F 		(0)

#define MATH_TEST_ISQRTI 		(1)
#define MATH_TEST_ISQRTF 		(1)

#define MATH_TEST_IROUND 		(0)
#define MATH_TEST_IFLOOR 		(0)
#define MATH_TEST_ICEIL 		(0)

#define MATH_TEST_ABSI 			(0)
#define MATH_TEST_ABSF 			(0)

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
 * log
 */
static void tb_math_test_ilog2i(tb_uint32_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = TB_MATH_ILOG2I(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_ilog2i(%u) = %d\n", (tb_int_t)t, x, r);
}
static void tb_math_test_ilog2f(tb_float_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = TB_MATH_ILOG2F(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_ilog2f(%lf) = %d\n", (tb_int_t)t, x, r);
}
static void tb_math_test_ilog2i_libc(tb_uint32_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tb_float_t log2 = log(2);
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = (log((tb_float_t)x + .1) / log2);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_ilog2i_libc(%u) = %d\n", (tb_int_t)t, x, r);
}
static void tb_math_test_ilog2f_libc(tb_float_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tb_float_t log2 = log(2);
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = (log(x) / log2);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_ilog2f_libc(%lf) = %d\n", (tb_int_t)t, x, r);
}
static void tb_math_test_irlog2i(tb_uint32_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = TB_MATH_IRLOG2I(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_irlog2i(%u) = %d\n", (tb_int_t)t, x, r);
}
static void tb_math_test_irlog2f(tb_float_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = TB_MATH_IRLOG2F(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_irlog2f(%lf) = %d\n", (tb_int_t)t, x, r);
}
static void tb_math_test_irlog2i_libc(tb_uint32_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tb_float_t log2 = log(2);
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = TB_MATH_IROUND(log(x) / log2);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_irlog2i_libc(%u) = %d\n", (tb_int_t)t, x, r);
}
static void tb_math_test_irlog2f_libc(tb_float_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tb_float_t log2 = log(2);
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = TB_MATH_IROUND(log(x) / log2);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_iclog2f_libc(%lf) = %d\n", (tb_int_t)t, x, r);
}
static void tb_math_test_iclog2i(tb_uint32_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = TB_MATH_ICLOG2I(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_iclog2i(%u) = %d\n", (tb_int_t)t, x, r);
}
static void tb_math_test_iclog2f(tb_float_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = TB_MATH_ICLOG2F(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_iclog2f(%lf) = %d\n", (tb_int_t)t, x, r);
}
static void tb_math_test_iclog2i_libc(tb_uint32_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tb_float_t log2 = log(2);
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = TB_MATH_ICEIL(log(x) / log2);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_iclog2i_libc(%u) = %d\n", (tb_int_t)t, x, r);
}
static void tb_math_test_iclog2f_libc(tb_float_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tb_float_t log2 = log(2);
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = TB_MATH_ICEIL(log(x) / log2);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_iclog2f_libc(%lf) = %d\n", (tb_int_t)t, x, r);
}
/* ////////////////////////////////////////////////////////////////////////
 * sqrt
 */
static void tb_math_test_isqrti(tb_uint32_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = TB_MATH_ISQRTI(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_isqrti(%u) = %d\n", (tb_int_t)t, x, r);
}
static void tb_math_test_isqrtf(tb_float_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = TB_MATH_ISQRTF(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_isqrtf(%lf) = %d\n", (tb_int_t)t, x, r);
}
static void tb_math_test_isqrti_libc(tb_uint32_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = sqrt(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_isqrti_libc(%u) = %d\n", (tb_int_t)t, x, r);
}
static void tb_math_test_isqrtf_libc(tb_float_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = sqrt(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_isqrtf_libc(%lf) = %d\n", (tb_int_t)t, x, r);
}

/* ////////////////////////////////////////////////////////////////////////
 * round
 */
static void tb_math_test_iround(tb_float_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = TB_MATH_IROUND(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_iround(%lf) = %d\n", (tb_int_t)t, x, r);
}
static void tb_math_test_ifloor(tb_float_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = TB_MATH_IFLOOR(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_ifloor(%lf) = %d\n", (tb_int_t)t, x, r);
}
static void tb_math_test_iceil(tb_float_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = TB_MATH_ICEIL(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_iceil(%lf) = %d\n", (tb_int_t)t, x, r);
}
static void tb_math_test_iround_libc(tb_float_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = round(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_iround_libc(%lf) = %d\n", (tb_int_t)t, x, r);
}
static void tb_math_test_ifloor_libc(tb_float_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = floor(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_ifloor_libc(%lf) = %d\n", (tb_int_t)t, x, r);
}
static void tb_math_test_iceil_libc(tb_float_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = ceil(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_iceil_libc(%lf) = %d\n", (tb_int_t)t, x, r);
}

/* ////////////////////////////////////////////////////////////////////////
 * abs
 */
static void tb_math_test_absi(tb_int_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = TB_MATH_ABSI(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_absi(%d) = %d\n", (tb_int_t)t, x, r);
}
static void tb_math_test_absi_libc(tb_int_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = abs(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_absi_libc(%d) = %d\n", (tb_int_t)t, x, r);
}
static void tb_math_test_absf(tb_float_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_float_t 	r = 0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = TB_MATH_ABSF(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_absf(%lf) = %lf\n", (tb_int_t)t, x, r);
}
static void tb_math_test_absf_libc(tb_float_t x)
{
	__tplat_volatile__ tb_int_t 	n = 10000000;
	__tplat_volatile__ tb_float_t 	r = 0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = fabs(x);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_math_absf_libc(%lf) = %lf\n", (tb_int_t)t, x, r);
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
static void tb_math_make_irlog2i_table()
{
	tb_uint32_t i = 1;
	tb_uint32_t a = 0;
	tb_uint32_t n = 1 << 31;
	for (i = 0; i < n; i++)
	{
		tb_uint32_t x = TB_MATH_IROUND(log((tb_float_t)i) / log(2.));
		if (x == a)
		{
			tplat_printf(",\t%u \t// %d\n", i, x);
			a++;
		}
	}
}
static void tb_math_make_iclog2i_table()
{
	tb_uint32_t i = 1;
	tb_uint32_t a = 0;
	tb_uint32_t n = 1 << 31;
	for (i = 0; i < n; i++)
	{
		tb_uint32_t x = TB_MATH_ICEIL(log((tb_float_t)i) / log(2.));
		if (x == a)
		{
			tplat_printf(",\t%u \t// %d\n", i, x);
			a++;
		}
	}
}
static void tb_math_make_ilog2i_table()
{
	tb_uint32_t i = 1;
	tb_uint32_t a = 0;
	tb_uint32_t n = 1 << 31;
	for (i = 0; i < n; i++)
	{
		tb_uint32_t x = (tb_uint32_t)(log((tb_float_t)i + .1) / log(2.));
		if (x == a)
		{
			tplat_printf(",\t%u \t// %d\n", i, x);
			a++;
		}
	}
}
int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	//tb_math_make_fexpi_table();
	//tb_math_make_fpow2i_table();
	//tb_math_make_ilog2i_table();
	//tb_math_make_irlog2i_table();
	//tb_math_make_iclog2i_table();

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

#if MATH_TEST_ILOG2I
	tplat_printf("===================================:\n");
	tb_math_test_ilog2i(1);
	tb_math_test_ilog2i(10);
	tb_math_test_ilog2i(100);
	tb_math_test_ilog2i(1000);
	tb_math_test_ilog2i(1024);
	tb_math_test_ilog2i(2048);
	tb_math_test_ilog2i(4096);
	tb_math_test_ilog2i((1 << 31) + 100);

	tplat_printf("\n");
	tb_math_test_ilog2i_libc(1);
	tb_math_test_ilog2i_libc(10);
	tb_math_test_ilog2i_libc(100);
	tb_math_test_ilog2i_libc(1000);
	tb_math_test_ilog2i_libc(1024);
	tb_math_test_ilog2i_libc(2048);
	tb_math_test_ilog2i_libc(4096);
	tb_math_test_ilog2i_libc((1 << 31) + 100);

#endif

#if MATH_TEST_ILOG2F
	tplat_printf("===================================:\n");
	tb_math_test_ilog2f(1.1415926f);
	tb_math_test_ilog2f(10.1415926f);
	tb_math_test_ilog2f(100.1415926f);
	tb_math_test_ilog2f(1000.1415926f);
	tb_math_test_ilog2f(10000.1415926f);
	tb_math_test_ilog2f(100000.1415926f);
	tb_math_test_ilog2f(1024);
	tb_math_test_ilog2f((1 << 31) + 100);

	tplat_printf("\n");
	tb_math_test_ilog2f_libc(1.1415926f);
	tb_math_test_ilog2f_libc(10.1415926f);
	tb_math_test_ilog2f_libc(100.1415926f);
	tb_math_test_ilog2f_libc(1000.1415926f);
	tb_math_test_ilog2f_libc(10000.1415926f);
	tb_math_test_ilog2f_libc(100000.1415926f);
	tb_math_test_ilog2f_libc(1024);
	tb_math_test_ilog2f_libc((1 << 31) + 100);
#endif

#if MATH_TEST_IRLOG2I
	tplat_printf("===================================:\n");
	tb_math_test_irlog2i(1);
	tb_math_test_irlog2i(10);
	tb_math_test_irlog2i(100);
	tb_math_test_irlog2i(1000);
	tb_math_test_irlog2i(1024);
	tb_math_test_irlog2i(2048);
	tb_math_test_irlog2i(4096);
	tb_math_test_irlog2i((1 << 31) + 100);

	tplat_printf("\n");
	tb_math_test_irlog2i_libc(1);
	tb_math_test_irlog2i_libc(10);
	tb_math_test_irlog2i_libc(100);
	tb_math_test_irlog2i_libc(1000);
	tb_math_test_irlog2i_libc(1024);
	tb_math_test_irlog2i_libc(2048);
	tb_math_test_irlog2i_libc(4096);
	tb_math_test_irlog2i_libc((1 << 31) + 100);

#endif

#if MATH_TEST_IRLOG2F
	tplat_printf("===================================:\n");
	tb_math_test_irlog2f(1.1415926f);
	tb_math_test_irlog2f(10.1415926f);
	tb_math_test_irlog2f(100.1415926f);
	tb_math_test_irlog2f(1000.1415926f);
	tb_math_test_irlog2f(10000.1415926f);
	tb_math_test_irlog2f(100000.1415926f);
	tb_math_test_irlog2f(1024);
	tb_math_test_irlog2f((1 << 31) + 100);

	tplat_printf("\n");
	tb_math_test_irlog2f_libc(1.1415926f);
	tb_math_test_irlog2f_libc(10.1415926f);
	tb_math_test_irlog2f_libc(100.1415926f);
	tb_math_test_irlog2f_libc(1000.1415926f);
	tb_math_test_irlog2f_libc(10000.1415926f);
	tb_math_test_irlog2f_libc(100000.1415926f);
	tb_math_test_irlog2f_libc(1024);
	tb_math_test_irlog2f_libc((1 << 31) + 100);
#endif

#if MATH_TEST_ICLOG2I
	tplat_printf("===================================:\n");
	tb_math_test_iclog2i(1);
	tb_math_test_iclog2i(10);
	tb_math_test_iclog2i(100);
	tb_math_test_iclog2i(1000);
	tb_math_test_iclog2i(1024);
	tb_math_test_iclog2i(2048);
	tb_math_test_iclog2i(4096);
	tb_math_test_iclog2i((1 << 31) + 100);

	tplat_printf("\n");
	tb_math_test_iclog2i_libc(1);
	tb_math_test_iclog2i_libc(10);
	tb_math_test_iclog2i_libc(100);
	tb_math_test_iclog2i_libc(1000);
	tb_math_test_iclog2i_libc(1024);
	tb_math_test_iclog2i_libc(2048);
	tb_math_test_iclog2i_libc(4096);
	tb_math_test_iclog2i_libc((1 << 31) + 100);

#endif

#if MATH_TEST_ICLOG2F
	tplat_printf("===================================:\n");
	tb_math_test_iclog2f(1.1415926f);
	tb_math_test_iclog2f(10.1415926f);
	tb_math_test_iclog2f(100.1415926f);
	tb_math_test_iclog2f(1000.1415926f);
	tb_math_test_iclog2f(10000.1415926f);
	tb_math_test_iclog2f(100000.1415926f);
	tb_math_test_iclog2f(1024);
	tb_math_test_iclog2f((1 << 31) + 100);

	tplat_printf("\n");
	tb_math_test_iclog2f_libc(1.1415926f);
	tb_math_test_iclog2f_libc(10.1415926f);
	tb_math_test_iclog2f_libc(100.1415926f);
	tb_math_test_iclog2f_libc(1000.1415926f);
	tb_math_test_iclog2f_libc(10000.1415926f);
	tb_math_test_iclog2f_libc(100000.1415926f);
	tb_math_test_iclog2f_libc(1024);
	tb_math_test_iclog2f_libc((1 << 31) + 100);
#endif


#if MATH_TEST_ISQRTI
	tplat_printf("===================================:\n");
	tb_math_test_isqrti(1);
	tb_math_test_isqrti(10);
	tb_math_test_isqrti(100);
	tb_math_test_isqrti(1000);
	tb_math_test_isqrti(1024);
	tb_math_test_isqrti(2048);
	tb_math_test_isqrti(4096);
	tb_math_test_isqrti((1 << 31) + 100);

	tplat_printf("\n");
	tb_math_test_isqrti_libc(1);
	tb_math_test_isqrti_libc(10);
	tb_math_test_isqrti_libc(100);
	tb_math_test_isqrti_libc(1000);
	tb_math_test_isqrti_libc(1024);
	tb_math_test_isqrti_libc(2048);
	tb_math_test_isqrti_libc(4096);
	tb_math_test_isqrti_libc((1 << 31) + 100);

#endif

#if MATH_TEST_ISQRTF
	tplat_printf("===================================:\n");
	tb_math_test_isqrtf(1.1415926f);
	tb_math_test_isqrtf(10.1415926f);
	tb_math_test_isqrtf(100.1415926f);
	tb_math_test_isqrtf(1000.1415926f);
	tb_math_test_isqrtf(10000.1415926f);
	tb_math_test_isqrtf(100000.1415926f);
	tb_math_test_isqrtf(1024);
	tb_math_test_isqrtf((1 << 31) + 100);

	tplat_printf("\n");
	tb_math_test_isqrtf_libc(1.1415926f);
	tb_math_test_isqrtf_libc(10.1415926f);
	tb_math_test_isqrtf_libc(100.1415926f);
	tb_math_test_isqrtf_libc(1000.1415926f);
	tb_math_test_isqrtf_libc(10000.1415926f);
	tb_math_test_isqrtf_libc(100000.1415926f);
	tb_math_test_isqrtf_libc(1024);
	tb_math_test_isqrtf_libc((1 << 31) + 100);
#endif

#if MATH_TEST_IROUND
	tplat_printf("===================================:\n");
	tb_math_test_iround(0.);
	tb_math_test_iround(3.0000000f);
	tb_math_test_iround(3.0415926f);
	tb_math_test_iround(3.1415926f);
	tb_math_test_iround(3.2415926f);
	tb_math_test_iround(3.3415926f);
	tb_math_test_iround(3.4415926f);
	tb_math_test_iround(3.5415926f);
	tb_math_test_iround(3.6415926f);
	tb_math_test_iround(3.7415926f);
	tb_math_test_iround(3.8415926f);
	tb_math_test_iround(3.9415926f);

	tb_math_test_iround(-0.);
	tb_math_test_iround(-3.0000000f);
	tb_math_test_iround(-3.0415926f);
	tb_math_test_iround(-3.1415926f);
	tb_math_test_iround(-3.2415926f);
	tb_math_test_iround(-3.3415926f);
	tb_math_test_iround(-3.4415926f);
	tb_math_test_iround(-3.5415926f);
	tb_math_test_iround(-3.6415926f);
	tb_math_test_iround(-3.7415926f);
	tb_math_test_iround(-3.8415926f);
	tb_math_test_iround(-3.9415926f);

	tplat_printf("\n");
	tb_math_test_iround_libc(0.);
	tb_math_test_iround_libc(3.0000000f);
	tb_math_test_iround_libc(3.0415926f);
	tb_math_test_iround_libc(3.1415926f);
	tb_math_test_iround_libc(3.2415926f);
	tb_math_test_iround_libc(3.3415926f);
	tb_math_test_iround_libc(3.4415926f);
	tb_math_test_iround_libc(3.5415926f);
	tb_math_test_iround_libc(3.6415926f);
	tb_math_test_iround_libc(3.7415926f);
	tb_math_test_iround_libc(3.8415926f);
	tb_math_test_iround_libc(3.9415926f);

	tb_math_test_iround_libc(-0.);
	tb_math_test_iround_libc(-3.0000000f);
	tb_math_test_iround_libc(-3.0415926f);
	tb_math_test_iround_libc(-3.1415926f);
	tb_math_test_iround_libc(-3.2415926f);
	tb_math_test_iround_libc(-3.3415926f);
	tb_math_test_iround_libc(-3.4415926f);
	tb_math_test_iround_libc(-3.5415926f);
	tb_math_test_iround_libc(-3.6415926f);
	tb_math_test_iround_libc(-3.7415926f);
	tb_math_test_iround_libc(-3.8415926f);
	tb_math_test_iround_libc(-3.9415926f);
#endif

#if MATH_TEST_IFLOOR
	tplat_printf("===================================:\n");
	tb_math_test_ifloor(0.);
	tb_math_test_ifloor(3.0000000f);
	tb_math_test_ifloor(3.0415926f);
	tb_math_test_ifloor(3.1415926f);
	tb_math_test_ifloor(3.2415926f);
	tb_math_test_ifloor(3.3415926f);
	tb_math_test_ifloor(3.4415926f);
	tb_math_test_ifloor(3.5415926f);
	tb_math_test_ifloor(3.6415926f);
	tb_math_test_ifloor(3.7415926f);
	tb_math_test_ifloor(3.8415926f);
	tb_math_test_ifloor(3.9415926f);

	tb_math_test_ifloor(-0.);
	tb_math_test_ifloor(-3.0000000f);
	tb_math_test_ifloor(-3.0415926f);
	tb_math_test_ifloor(-3.1415926f);
	tb_math_test_ifloor(-3.2415926f);
	tb_math_test_ifloor(-3.3415926f);
	tb_math_test_ifloor(-3.4415926f);
	tb_math_test_ifloor(-3.5415926f);
	tb_math_test_ifloor(-3.6415926f);
	tb_math_test_ifloor(-3.7415926f);
	tb_math_test_ifloor(-3.8415926f);
	tb_math_test_ifloor(-3.9415926f);
	
	tplat_printf("\n");
	tb_math_test_ifloor_libc(0.);
	tb_math_test_ifloor_libc(3.0000000f);
	tb_math_test_ifloor_libc(3.0415926f);
	tb_math_test_ifloor_libc(3.1415926f);
	tb_math_test_ifloor_libc(3.2415926f);
	tb_math_test_ifloor_libc(3.3415926f);
	tb_math_test_ifloor_libc(3.4415926f);
	tb_math_test_ifloor_libc(3.5415926f);
	tb_math_test_ifloor_libc(3.6415926f);
	tb_math_test_ifloor_libc(3.7415926f);
	tb_math_test_ifloor_libc(3.8415926f);
	tb_math_test_ifloor_libc(3.9415926f);

	tb_math_test_ifloor_libc(-0.);
	tb_math_test_ifloor_libc(-3.0000000f);
	tb_math_test_ifloor_libc(-3.0415926f);
	tb_math_test_ifloor_libc(-3.1415926f);
	tb_math_test_ifloor_libc(-3.2415926f);
	tb_math_test_ifloor_libc(-3.3415926f);
	tb_math_test_ifloor_libc(-3.4415926f);
	tb_math_test_ifloor_libc(-3.5415926f);
	tb_math_test_ifloor_libc(-3.6415926f);
	tb_math_test_ifloor_libc(-3.7415926f);
	tb_math_test_ifloor_libc(-3.8415926f);
	tb_math_test_ifloor_libc(-3.9415926f);
#endif

#if MATH_TEST_ICEIL
	tplat_printf("===================================:\n");
	tb_math_test_iceil(0.);
	tb_math_test_iceil(3.0000000f);
	tb_math_test_iceil(3.0415926f);
	tb_math_test_iceil(3.1415926f);
	tb_math_test_iceil(3.2415926f);
	tb_math_test_iceil(3.3415926f);
	tb_math_test_iceil(3.4415926f);
	tb_math_test_iceil(3.5415926f);
	tb_math_test_iceil(3.6415926f);
	tb_math_test_iceil(3.7415926f);
	tb_math_test_iceil(3.8415926f);
	tb_math_test_iceil(3.9415926f);

	tb_math_test_iceil(-0.);
	tb_math_test_iceil(-3.0000000f);
	tb_math_test_iceil(-3.0415926f);
	tb_math_test_iceil(-3.1415926f);
	tb_math_test_iceil(-3.2415926f);
	tb_math_test_iceil(-3.3415926f);
	tb_math_test_iceil(-3.4415926f);
	tb_math_test_iceil(-3.5415926f);
	tb_math_test_iceil(-3.6415926f);
	tb_math_test_iceil(-3.7415926f);
	tb_math_test_iceil(-3.8415926f);
	tb_math_test_iceil(-3.9415926f);

	tplat_printf("\n");
	tb_math_test_iceil_libc(0.);
	tb_math_test_iceil_libc(3.0000000f);
	tb_math_test_iceil_libc(3.0415926f);
	tb_math_test_iceil_libc(3.1415926f);
	tb_math_test_iceil_libc(3.2415926f);
	tb_math_test_iceil_libc(3.3415926f);
	tb_math_test_iceil_libc(3.4415926f);
	tb_math_test_iceil_libc(3.5415926f);
	tb_math_test_iceil_libc(3.6415926f);
	tb_math_test_iceil_libc(3.7415926f);
	tb_math_test_iceil_libc(3.8415926f);
	tb_math_test_iceil_libc(3.9415926f);

	tb_math_test_iceil_libc(-0.);
	tb_math_test_iceil_libc(-3.0000000f);
	tb_math_test_iceil_libc(-3.0415926f);
	tb_math_test_iceil_libc(-3.1415926f);
	tb_math_test_iceil_libc(-3.2415926f);
	tb_math_test_iceil_libc(-3.3415926f);
	tb_math_test_iceil_libc(-3.4415926f);
	tb_math_test_iceil_libc(-3.5415926f);
	tb_math_test_iceil_libc(-3.6415926f);
	tb_math_test_iceil_libc(-3.7415926f);
	tb_math_test_iceil_libc(-3.8415926f);
	tb_math_test_iceil_libc(-3.9415926f);
#endif

#if MATH_TEST_ABSI
	tplat_printf("===================================:\n");
	tb_math_test_absi(0);
	tb_math_test_absi(-0);
	tb_math_test_absi(3);
	tb_math_test_absi(-3);

	tplat_printf("\n");
	tb_math_test_absi_libc(0);
	tb_math_test_absi_libc(-0);
	tb_math_test_absi_libc(3);
	tb_math_test_absi_libc(-3);
#endif

#if MATH_TEST_ABSF
	tplat_printf("===================================:\n");
	tb_math_test_absf(0.);
	tb_math_test_absf(-0.);
	tb_math_test_absf(3.1415926);
	tb_math_test_absf(-3.1415926);

	tplat_printf("\n");
	tb_math_test_absf_libc(0.);
	tb_math_test_absf_libc(-0.);
	tb_math_test_absf_libc(3.1415926);
	tb_math_test_absf_libc(-3.1415926);
#endif

	return 0;
}
