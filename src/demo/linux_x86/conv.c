#include "tplat/tplat.h"
#include "../../tbox.h"

#define CONV_TEST_IS 			(0)

#define CONV_TEST_FTOI 			(0)
#define CONV_TEST_FTOU 			(0)

#define CONV_TEST_TOLOWER 		(0)
#define CONV_TEST_TOUPPER 		(0)

#define TB_TEST_CONV_SBTOI32 	(1)
#define TB_TEST_CONV_SBTOU32 	(1)
#define TB_TEST_CONV_SBTOF 		(1)

#define TB_TEST_CONV_STOI32 	(1)
#define TB_TEST_CONV_STOU32 	(1)


static void tb_conv_check_is()
{
	tb_int_t i = 0;
	for (i = 0; i < 256; i++)
	{
		if ((TB_CONV_ISSPACE(i)? 1 : 0) != (isspace(i)? 1 : 0)) tplat_printf("[e] isspace: 0x%02x\n", i);
		if ((TB_CONV_ISALPHA(i)? 1 : 0) != (isalpha(i)? 1 : 0)) tplat_printf("[e] isalpha: 0x%02x\n", i);
		if ((TB_CONV_ISDIGIT(i)? 1 : 0) != (isdigit(i)? 1 : 0)) tplat_printf("[e] isdigit: 0x%02x\n", i);
		if ((TB_CONV_ISUPPER(i)? 1 : 0) != (isupper(i)? 1 : 0)) tplat_printf("[e] isupper: 0x%02x\n", i);
		if ((TB_CONV_ISLOWER(i)? 1 : 0) != (islower(i)? 1 : 0)) tplat_printf("[e] islower: 0x%02x\n", i);
		if ((TB_CONV_ISASCII(i)? 1 : 0) != (isascii(i)? 1 : 0)) tplat_printf("[e] isascii: 0x%02x\n", i);
	}
}
static void tb_conv_make_space_table()
{
	tb_int_t i = 0;
	for (i = 0; i < 256; i++)
	{
		if (isspace(i)) tplat_printf("0x%02x\n", i);
	}
}
static void tb_conv_make_isalpha_table()
{
	tb_int_t i = 0;
	for (i = 0; i < 256; i++)
	{
		if (isalpha(i)) tplat_printf("0x%02x\n", i);
	}
}
static void tb_conv_test_ftoi32(tb_float_t f)
{
	tplat_printf("ftoi32(%lf) = %d, int(%lf) = %d\n", f, TB_CONV_FTOI32(f), f, (tb_int32_t)f);
}
static void tb_conv_test_ftou32(tb_float_t f)
{
	tplat_printf("ftou32(%lf) = %u, uint(%lf) = %u\n", f, TB_CONV_FTOU32(f), f, (tb_uint32_t)f);
}
static void tb_conv_check_toupper()
{
	tb_int_t i = 0;
	for (i = 0; i < 256; i++)
	{
		if ((TB_CONV_TOUPPER(i)? 1 : 0) != (toupper(i)? 1 : 0)) tplat_printf("[e] toupper: 0x%02x = 0x%02x\n", i, toupper(i));
	}
}
static void tb_conv_check_tolower()
{
	tb_int_t i = 0;
	for (i = 0; i < 256; i++)
	{
		if ((TB_CONV_TOLOWER(i)? 1 : 0) != (tolower(i)? 1 : 0)) tplat_printf("[e] tolower: 0x%02x = 0x%02x\n", i, tolower(i));
	}
}
static void tb_conv_test_sbtou32(tb_char_t const* s, tb_int_t base, tb_uint32_t val)
{
	tplat_printf("s%dtou32(%s) = %u [?= %u]\n", base, s, TB_CONV_SBTOU32(s, base), val);
}
static void tb_conv_test_sbtoi32(tb_char_t const* s, tb_int_t base, tb_int32_t val)
{
	tplat_printf("s%dtoi32(%s) = %d [?= %d]\n", base, s, TB_CONV_SBTOI32(s, base), val);
}
static void tb_conv_test_sbtof(tb_char_t const* s, tb_int_t base, tb_float_t val)
{
	tplat_printf("s%dtof(%s) = %lf [?= %lf]\n", base, s, TB_CONV_SBTOF(s, base), val);
}
static void tb_conv_test_stou32(tb_char_t const* s, tb_uint32_t val)
{
	tplat_printf("stou32(%s) = %u [?= %u]\n", s, TB_CONV_STOU32(s), val);
}
static void tb_conv_test_stoi32(tb_char_t const* s, tb_int32_t val)
{
	tplat_printf("stoi32(%s) = %d [?= %d]\n", s, TB_CONV_STOI32(s), val);
}
static void tb_conv_test_stof(tb_char_t const* s, tb_float_t val)
{
	tplat_printf("stof(%s) = %lf [?= %lf]\n", s, TB_CONV_STOF(s), val);
}
int main(int argc, char** argv)
{
	tplat_size_t regular_block_n[TPLAT_POOL_REGULAR_CHUNCK_MAX_COUNT] = {10, 10, 10, 10, 10, 10, 10};
	tplat_pool_create(TB_CONFIG_MEMORY_POOL_INDEX, malloc(1024 * 1024), 1024 * 1024, regular_block_n);

	//tb_conv_make_isspace_table();
	//tb_conv_make_isalpha_table();

#if CONV_TEST_IS
	tplat_printf("===============================\n");
	tb_conv_check_is();
#endif

#if CONV_TEST_FTOI
	tplat_printf("===============================\n");
	tb_conv_test_ftoi32(0.1415926);
	tb_conv_test_ftoi32(-0.1415926);

	tb_conv_test_ftoi32(3.1415926);
	tb_conv_test_ftoi32(-3.1415926);

	tb_conv_test_ftoi32(-1);
	tb_conv_test_ftoi32((tb_uint32_t)-1);
	tb_conv_test_ftoi32(0xffffffff);
#endif

#if CONV_TEST_FTOU
	tplat_printf("===============================\n");
	tb_conv_test_ftou32(0.1415926);
	tb_conv_test_ftou32(-0.1415926);

	tb_conv_test_ftou32(3.1415926);
	tb_conv_test_ftou32(-3.1415926);

	tb_conv_test_ftou32(-1);
	tb_conv_test_ftou32((tb_uint32_t)-1);
	tb_conv_test_ftou32(0xffffffff);
#endif

#if CONV_TEST_TOLOWER
	tplat_printf("===============================\n");
	tb_conv_check_tolower();
#endif

#if CONV_TEST_TOUPPER
	tplat_printf("===============================\n");
	tb_conv_check_toupper();
#endif

#if TB_TEST_CONV_SBTOU32
	tplat_printf("===============================\n");
	tb_conv_test_sbtou32("1110111001100011001100110", 2, 31415926);
	tb_conv_test_sbtou32("0b1110111001100011001100110", 2, 31415926);
	tb_conv_test_sbtou32("167143146", 8, 31415926);
	tb_conv_test_sbtou32("0167143146", 8, 31415926);
	tb_conv_test_sbtou32("31415926", 10, 31415926);
	tb_conv_test_sbtou32("1dcc666", 16, 31415926);
	tb_conv_test_sbtou32("0x1dcc666", 16, 31415926);
#endif


#if TB_TEST_CONV_SBTOI32
	tplat_printf("===============================\n");
	tb_conv_test_sbtoi32("1110111001100011001100110", 2, 31415926);
	tb_conv_test_sbtoi32("0b1110111001100011001100110", 2, 31415926);
	tb_conv_test_sbtoi32("167143146", 8, 31415926);
	tb_conv_test_sbtoi32("0167143146", 8, 31415926);
	tb_conv_test_sbtoi32("31415926", 10, 31415926);
	tb_conv_test_sbtoi32("1dcc666", 16, 31415926);
	tb_conv_test_sbtoi32("0x1dcc666", 16, 31415926);

	tplat_printf("\n");
	tb_conv_test_sbtoi32("-1110111001100011001100110", 2, -31415926);
	tb_conv_test_sbtoi32("-0b1110111001100011001100110", 2, -31415926);
	tb_conv_test_sbtoi32("-167143146", 8, -31415926);
	tb_conv_test_sbtoi32("-0167143146", 8, -31415926);
	tb_conv_test_sbtoi32("-31415926", 10, -31415926);
	tb_conv_test_sbtoi32("-1dcc666", 16, -31415926);
	tb_conv_test_sbtoi32("-0x1dcc666", 16, -31415926);
#endif

#if TB_TEST_CONV_SBTOF
	tplat_printf("===============================\n");
	tb_conv_test_sbtof("0", 10, 0);
	tb_conv_test_sbtof("0.", 10, 0.);
	tb_conv_test_sbtof("0.0", 10, 0.0);
	tb_conv_test_sbtof("00.00", 10, 00.00);
	tb_conv_test_sbtof("3", 10, 3);
	tb_conv_test_sbtof("3.", 10, 3.);
	tb_conv_test_sbtof("3.1415926", 10, 3.1415926);
	tb_conv_test_sbtof("0.1415926", 10, 0.1415926);
	tb_conv_test_sbtof("3.123456789123456789", 10, 3.123456789123456789);
	tb_conv_test_sbtof("00003.0001415926000", 10, 00003.0001415926000);
	tb_conv_test_sbtof("4294967295", 10, 4294967295);
	tb_conv_test_sbtof("4294967295.", 10, 4294967295.);

	tplat_printf("\n");
	tb_conv_test_sbtof("-0", 10, -0);
	tb_conv_test_sbtof("-0.", 10, -0.);
	tb_conv_test_sbtof("-0.0", 10, -0.0);
	tb_conv_test_sbtof("-00.00", 10, -00.00);
	tb_conv_test_sbtof("-3", 10, -3);
	tb_conv_test_sbtof("-3.", 10, -3.);
	tb_conv_test_sbtof("-3.1415926", 10, -3.1415926);
	tb_conv_test_sbtof("-0.1415926", 10, -0.1415926);
	tb_conv_test_sbtof("-3.123456789123456789", 10, -3.123456789123456789);
	tb_conv_test_sbtof("-00003.0001415926000", 10, -00003.0001415926000);
	tb_conv_test_sbtof("-4294967295", 10, -4294967295);
	tb_conv_test_sbtof("-4294967295.", 10, -4294967295.f);

	tplat_printf("===============================\n");
	tb_conv_test_sbtof("0", 2, 0);
	tb_conv_test_sbtof("0.", 2, 0.);
	tb_conv_test_sbtof("0.0", 2, 0.0);
	tb_conv_test_sbtof("00.00", 2, 00.00);
	tb_conv_test_sbtof("11", 2, 3);
	tb_conv_test_sbtof("11.", 2, 3.);
	tb_conv_test_sbtof("11.001001", 2, 3.140625);
	tb_conv_test_sbtof("0.001001", 2, 0.140625);
	tb_conv_test_sbtof("11111111111111111111111111111111", 2, 0b11111111111111111111111111111111);
	tb_conv_test_sbtof("11111111111111111111111111111111.", 2, 4294967295.);
#endif

#if TB_TEST_CONV_STOU32
	tplat_printf("===============================\n");
	tb_conv_test_stou32("0b1110111001100011001100110", 31415926);
	tb_conv_test_stou32("0167143146", 31415926);
	tb_conv_test_stou32("31415926", 31415926);
	tb_conv_test_stou32("0x1dcc666", 31415926);
#endif


#if TB_TEST_CONV_STOI32
	tplat_printf("===============================\n");
	tb_conv_test_stoi32("0b1110111001100011001100110", 31415926);
	tb_conv_test_stoi32("0167143146", 31415926);
	tb_conv_test_stoi32("31415926", 31415926);
	tb_conv_test_stoi32("0x1dcc666", 31415926);

	tplat_printf("\n");
	tb_conv_test_stoi32("-0b1110111001100011001100110", -31415926);
	tb_conv_test_stoi32("-0167143146", -31415926);
	tb_conv_test_stoi32("-31415926", -31415926);
	tb_conv_test_stoi32("-0x1dcc666", -31415926);
#endif

	tb_float_t f = 4294967294.1234567;
	tb_float_t d = (f - (tb_uint32_t)f);
	TB_DBG("%lf", d);


	return 0;
}
