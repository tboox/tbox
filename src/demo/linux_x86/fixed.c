#include "tbox.h"

static tb_fixed_t tb_fixed_test_angle[360] = 
{
	0x0
,	0x477
,	0x8ef
,	0xd67
,	0x11df
,	0x1657
,	0x1ace
,	0x1f46
,	0x23be
,	0x2836
,	0x2cae
,	0x3126
,	0x359d
,	0x3a15
,	0x3e8d
,	0x4305
,	0x477d
,	0x4bf4
,	0x506c
,	0x54e4
,	0x595c
,	0x5dd4
,	0x624c
,	0x66c3
,	0x6b3b
,	0x6fb3
,	0x742b
,	0x78a3
,	0x7d1a
,	0x8192
,	0x860a
,	0x8a82
,	0x8efa
,	0x9372
,	0x97e9
,	0x9c61
,	0xa0d9
,	0xa551
,	0xa9c9
,	0xae40
,	0xb2b8
,	0xb730
,	0xbba8
,	0xc020
,	0xc498
,	0xc90f
,	0xcd87
,	0xd1ff
,	0xd677
,	0xdaef
,	0xdf66
,	0xe3de
,	0xe856
,	0xecce
,	0xf146
,	0xf5be
,	0xfa35
,	0xfead
,	0x10325
,	0x1079d
,	0x10c15
,	0x1108c
,	0x11504
,	0x1197c
,	0x11df4
,	0x1226c
,	0x126e4
,	0x12b5b
,	0x12fd3
,	0x1344b
,	0x138c3
,	0x13d3b
,	0x141b2
,	0x1462a
,	0x14aa2
,	0x14f1a
,	0x15392
,	0x1580a
,	0x15c81
,	0x160f9
,	0x16571
,	0x169e9
,	0x16e61
,	0x172d8
,	0x17750
,	0x17bc8
,	0x18040
,	0x184b8
,	0x18930
,	0x18da7
,	0x1921f
,	0x19697
,	0x19b0f
,	0x19f87
,	0x1a3fe
,	0x1a876
,	0x1acee
,	0x1b166
,	0x1b5de
,	0x1ba56
,	0x1becd
,	0x1c345
,	0x1c7bd
,	0x1cc35
,	0x1d0ad
,	0x1d524
,	0x1d99c
,	0x1de14
,	0x1e28c
,	0x1e704
,	0x1eb7c
,	0x1eff3
,	0x1f46b
,	0x1f8e3
,	0x1fd5b
,	0x201d3
,	0x2064b
,	0x20ac2
,	0x20f3a
,	0x213b2
,	0x2182a
,	0x21ca2
,	0x22119
,	0x22591
,	0x22a09
,	0x22e81
,	0x232f9
,	0x23771
,	0x23be8
,	0x24060
,	0x244d8
,	0x24950
,	0x24dc8
,	0x2523f
,	0x256b7
,	0x25b2f
,	0x25fa7
,	0x2641f
,	0x26897
,	0x26d0e
,	0x27186
,	0x275fe
,	0x27a76
,	0x27eee
,	0x28365
,	0x287dd
,	0x28c55
,	0x290cd
,	0x29545
,	0x299bd
,	0x29e34
,	0x2a2ac
,	0x2a724
,	0x2ab9c
,	0x2b014
,	0x2b48b
,	0x2b903
,	0x2bd7b
,	0x2c1f3
,	0x2c66b
,	0x2cae3
,	0x2cf5a
,	0x2d3d2
,	0x2d84a
,	0x2dcc2
,	0x2e13a
,	0x2e5b1
,	0x2ea29
,	0x2eea1
,	0x2f319
,	0x2f791
,	0x2fc09
,	0x30080
,	0x304f8
,	0x30970
,	0x30de8
,	0x31260
,	0x316d7
,	0x31b4f
,	0x31fc7
,	0x3243f
,	0x328b7
,	0x32d2f
,	0x331a6
,	0x3361e
,	0x33a96
,	0x33f0e
,	0x34386
,	0x347fd
,	0x34c75
,	0x350ed
,	0x35565
,	0x359dd
,	0x35e55
,	0x362cc
,	0x36744
,	0x36bbc
,	0x37034
,	0x374ac
,	0x37923
,	0x37d9b
,	0x38213
,	0x3868b
,	0x38b03
,	0x38f7b
,	0x393f2
,	0x3986a
,	0x39ce2
,	0x3a15a
,	0x3a5d2
,	0x3aa49
,	0x3aec1
,	0x3b339
,	0x3b7b1
,	0x3bc29
,	0x3c0a1
,	0x3c518
,	0x3c990
,	0x3ce08
,	0x3d280
,	0x3d6f8
,	0x3db6f
,	0x3dfe7
,	0x3e45f
,	0x3e8d7
,	0x3ed4f
,	0x3f1c7
,	0x3f63e
,	0x3fab6
,	0x3ff2e
,	0x403a6
,	0x4081e
,	0x40c96
,	0x4110d
,	0x41585
,	0x419fd
,	0x41e75
,	0x422ed
,	0x42764
,	0x42bdc
,	0x43054
,	0x434cc
,	0x43944
,	0x43dbc
,	0x44233
,	0x446ab
,	0x44b23
,	0x44f9b
,	0x45413
,	0x4588a
,	0x45d02
,	0x4617a
,	0x465f2
,	0x46a6a
,	0x46ee2
,	0x47359
,	0x477d1
,	0x47c49
,	0x480c1
,	0x48539
,	0x489b0
,	0x48e28
,	0x492a0
,	0x49718
,	0x49b90
,	0x4a008
,	0x4a47f
,	0x4a8f7
,	0x4ad6f
,	0x4b1e7
,	0x4b65f
,	0x4bad6
,	0x4bf4e
,	0x4c3c6
,	0x4c83e
,	0x4ccb6
,	0x4d12e
,	0x4d5a5
,	0x4da1d
,	0x4de95
,	0x4e30d
,	0x4e785
,	0x4ebfc
,	0x4f074
,	0x4f4ec
,	0x4f964
,	0x4fddc
,	0x50254
,	0x506cb
,	0x50b43
,	0x50fbb
,	0x51433
,	0x518ab
,	0x51d22
,	0x5219a
,	0x52612
,	0x52a8a
,	0x52f02
,	0x5337a
,	0x537f1
,	0x53c69
,	0x540e1
,	0x54559
,	0x549d1
,	0x54e48
,	0x552c0
,	0x55738
,	0x55bb0
,	0x56028
,	0x564a0
,	0x56917
,	0x56d8f
,	0x57207
,	0x5767f
,	0x57af7
,	0x57f6e
,	0x583e6
,	0x5885e
,	0x58cd6
,	0x5914e
,	0x595c6
,	0x59a3d
,	0x59eb5
,	0x5a32d
,	0x5a7a5
,	0x5ac1d
,	0x5b094
,	0x5b50c
,	0x5b984
,	0x5bdfc
,	0x5c274
,	0x5c6ec
,	0x5cb63
,	0x5cfdb
,	0x5d453
,	0x5d8cb
,	0x5dd43
,	0x5e1ba
,	0x5e632
,	0x5eaaa
,	0x5ef22
,	0x5f39a
,	0x5f812
,	0x5fc89
,	0x60101
,	0x60579
,	0x609f1
,	0x60e69
,	0x612e1
,	0x61758
,	0x61bd0
,	0x62048
,	0x624c0
,	0x62938
,	0x62daf
,	0x63227
,	0x6369f
,	0x63b17
,	0x63f8f
,	0x64407
};

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
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_int_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_fixed_round(x);
	}
	t = tb_mclock() - t;
	tb_printf("[fixed]: round(%f): %d, %d ms\n", tb_fixed_to_float(x), r, (tb_int_t)t);
}
static void tb_fixed_test_floor(tb_fixed_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_int_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_fixed_floor(x);
	}
	t = tb_mclock() - t;
	tb_printf("[fixed]: floor(%f): %d, %d ms\n", tb_fixed_to_float(x), r, (tb_int_t)t);
}
static void tb_fixed_test_ceil(tb_fixed_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_int_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_fixed_ceil(x);
	}
	t = tb_mclock() - t;
	tb_printf("[fixed]: ceil(%f): %d, %d ms\n", tb_fixed_to_float(x), r, (tb_int_t)t);
}
static void tb_fixed_test_mul(tb_fixed_t a, tb_fixed_t b)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_fixed_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_fixed_mul(a, b);
	}
	t = tb_mclock() - t;
	tb_printf("[fixed]: mul(%f, %f): %f, %d ms\n", tb_fixed_to_float(a), tb_fixed_to_float(b), tb_fixed_to_float(r), (tb_int_t)t);

}
static void tb_fixed_test_div(tb_fixed_t a, tb_fixed_t b)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_fixed_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_fixed_div(a, b);
	}
	t = tb_mclock() - t;
	tb_printf("[fixed]: div(%f, %f): %f, %d ms\n", tb_fixed_to_float(a), tb_fixed_to_float(b), tb_fixed_to_float(r), (tb_int_t)t);
}
static void tb_fixed_test_invert(tb_fixed_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_fixed_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_fixed_invert(x);
	}
	t = tb_mclock() - t;
	tb_printf("[fixed]: invert(%f): %f, %d ms\n", tb_fixed_to_float(x), tb_fixed_to_float(r), (tb_int_t)t);
}
static void tb_fixed_test_sqre(tb_fixed_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_fixed_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_fixed_sqre(x);
	}
	t = tb_mclock() - t;
	tb_printf("[fixed]: sqre(%f): %f, %d ms\n", tb_fixed_to_float(x), tb_fixed_to_float(r), (tb_int_t)t);
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
static void tb_fixed_test_iclog2(tb_fixed_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_uint32_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_fixed_iclog2(x);
	}
	t = tb_mclock() - t;
	tb_printf("[fixed]: iclog2(%f) = %d, %d ms\n", tb_fixed_to_float(x), r, (tb_int_t)t);
}
static void tb_fixed_test_irlog2(tb_fixed_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_uint32_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_fixed_irlog2(x);
	}
	t = tb_mclock() - t;
	tb_printf("[fixed]: irlog2(%f) = %d, %d ms\n", tb_fixed_to_float(x), r, (tb_int_t)t);
}
static void tb_fixed_test_exp(tb_fixed_t x)
{
	__tb_volatile__ tb_int_t 	n = 10000000;
	__tb_volatile__ tb_fixed_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		r = tb_fixed_exp(x);
	}
	t = tb_mclock() - t;
	tb_printf("[fixed]: exp(%f) = %f, %d ms\n", tb_fixed_to_float(x), tb_fixed_to_float(r), (tb_int_t)t);
}
static void tb_fixed_test_sin()
{
#if 1
	__tb_volatile__ tb_int_t 	i = 0;
	__tb_volatile__ tb_int_t 	n = 10000000 / 360;
	__tb_volatile__ tb_fixed_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		for (i = 0; i < 360; i++)
			r = tb_fixed_sin(tb_fixed_test_angle[i]);
	}
	t = tb_mclock() - t;

	for (i = 0; i < 360; i++)
	{
		r = tb_fixed_sin(tb_fixed_test_angle[i]);
		tb_printf("[fixed]: sin(%f) = %f, angle: %d\n", tb_fixed_to_float(tb_fixed_test_angle[i]), tb_fixed_to_float(r), i);
	}
	tb_printf("[fixed]: sin(0 - 360), %d ms\n", (tb_int_t)t);
#else

	tb_int_t i = 0;
	for (i = 0; i < 360; i++)
	{
		tb_printf(",\t0x%x\n", tb_float_to_fixed(i * TB_FLOAT_PI / 180));
	}
#endif
}
static void tb_fixed_test_cos()
{
	__tb_volatile__ tb_int_t 	i = 0;
	__tb_volatile__ tb_int_t 	n = 10000000 / 360;
	__tb_volatile__ tb_fixed_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		for (i = 0; i < 360; i++)
			r = tb_fixed_cos(tb_fixed_test_angle[i]);
	}
	t = tb_mclock() - t;

	for (i = 0; i < 360; i++)
	{
		r = tb_fixed_cos(tb_fixed_test_angle[i]);
		tb_printf("[fixed]: cos(%f) = %f, angle: %d\n", tb_fixed_to_float(tb_fixed_test_angle[i]), tb_fixed_to_float(r), i);
	}
	tb_printf("[fixed]: cos(0 - 360), %d ms\n", (tb_int_t)t);
}
static void tb_fixed_test_tan()
{
	__tb_volatile__ tb_int_t 	i = 0;
	__tb_volatile__ tb_int_t 	n = 10000000 / 360;
	__tb_volatile__ tb_fixed_t 	r = 0;
	tb_int64_t t = tb_mclock();
	while (n--)
	{
		for (i = 0; i < 360; i++)
			r = tb_fixed_tan(tb_fixed_test_angle[i]);
	}
	t = tb_mclock() - t;

	for (i = 0; i < 360; i++)
	{
		r = tb_fixed_tan(tb_fixed_test_angle[i]);
		tb_printf("[fixed]: tan(%f) = %f, angle: %d\n", tb_fixed_to_float(tb_fixed_test_angle[i]), tb_fixed_to_float(r), i);
	}
	tb_printf("[fixed]: tan(0 - 360), %d ms\n", (tb_int_t)t);
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
	tb_fixed_test_div(TB_FIXED_ONE, TB_FIXED_PI);
	tb_fixed_test_invert(TB_FIXED_PI);
	tb_fixed_test_sqre(TB_FIXED_PI);
	tb_fixed_test_sqre(-TB_FIXED_PI);
	tb_fixed_test_sqrt(TB_FIXED_PI);
	tb_fixed_test_exp(TB_FIXED_PI);
	tb_fixed_test_ilog2(TB_FIXED_PI);
	tb_fixed_test_ilog2(tb_float_to_fixed(1024.));
	tb_fixed_test_iclog2(TB_FIXED_PI);
	tb_fixed_test_irlog2(TB_FIXED_PI);
	tb_fixed_test_sin();
	tb_fixed_test_cos();
	tb_fixed_test_tan();

	return 0;
}
