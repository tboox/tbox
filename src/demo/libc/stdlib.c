/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"
#include <ctype.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_TEST_ISXXX           (0)

#define TB_TEST_TOLOWER         (0)
#define TB_TEST_TOUPPER         (0)

#define TB_TEST_SBTOI32         (1)
#define TB_TEST_SBTOU32         (1)
#define TB_TEST_SBTOF           (1)

#define TB_TEST_STOI32          (1)
#define TB_TEST_STOU32          (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if TB_TEST_ISXXX
static tb_void_t tb_check_is()
{
    tb_int_t i = 0;
    for (i = 0; i < 256; i++)
    {
        if ((tb_isspace(i)? 1 : 0) != (isspace(i)? 1 : 0)) tb_printf("[e] isspace: 0x%02x\n", i);
        if ((tb_isalpha(i)? 1 : 0) != (isalpha(i)? 1 : 0)) tb_printf("[e] isalpha: 0x%02x\n", i);
        if ((tb_isdigit(i)? 1 : 0) != (isdigit(i)? 1 : 0)) tb_printf("[e] isdigit: 0x%02x\n", i);
        if ((tb_isupper(i)? 1 : 0) != (isupper(i)? 1 : 0)) tb_printf("[e] isupper: 0x%02x\n", i);
        if ((tb_islower(i)? 1 : 0) != (islower(i)? 1 : 0)) tb_printf("[e] islower: 0x%02x\n", i);
        if ((tb_isascii(i)? 1 : 0) != (isascii(i)? 1 : 0)) tb_printf("[e] isascii: 0x%02x\n", i);
    }
}
#endif
static tb_void_t tb_make_isspace_table()
{
    tb_int_t i = 0;
    for (i = 0; i < 256; i++)
    {
        if (isspace(i)) tb_printf("0x%02x\n", i);
    }
}
static tb_void_t tb_make_isalpha_table()
{
    tb_int_t i = 0;
    for (i = 0; i < 256; i++)
    {
        if (isalpha(i)) tb_printf("0x%02x\n", i);
    }
}
#if TB_TEST_TOUPPER
static tb_void_t tb_check_toupper()
{
    tb_int_t i = 0;
    for (i = 0; i < 256; i++)
    {
        if ((tb_toupper(i)? 1 : 0) != (toupper(i)? 1 : 0)) tb_printf("[e] toupper: 0x%02x = 0x%02x\n", i, toupper(i));
    }
}
#endif
#if TB_TEST_TOLOWER
static tb_void_t tb_check_tolower()
{
    tb_int_t i = 0;
    for (i = 0; i < 256; i++)
    {
        if ((tb_tolower(i)? 1 : 0) != (tolower(i)? 1 : 0)) tb_printf("[e] tolower: 0x%02x = 0x%02x\n", i, tolower(i));
    }
}
#endif
static tb_void_t tb_test_sbtou32(tb_char_t const* s, tb_int_t base, tb_uint32_t val)
{
    tb_printf("s%dtou32(%s) = %u [?= %u]\n", base, s, tb_sbtou32(s, base), val);
}
static tb_void_t tb_test_sbtoi32(tb_char_t const* s, tb_int_t base, tb_int32_t val)
{
    tb_printf("s%dtoi32(%s) = %d [?= %d]\n", base, s, tb_sbtoi32(s, base), val);
}

#ifdef TB_CONFIG_TYPE_HAVE_FLOAT
static tb_void_t tb_test_sbtof(tb_char_t const* s, tb_int_t base, tb_double_t val)
{
    tb_printf("s%dtof(%s) = %lf [?= %lf]\n", base, s, tb_sbtof(s, base), val);
}
#endif
static tb_void_t tb_test_stou32(tb_char_t const* s, tb_uint32_t val)
{
    tb_printf("stou32(%s) = %u [?= %u]\n", s, tb_stou32(s), val);
}
static tb_void_t tb_test_stoi32(tb_char_t const* s, tb_int32_t val)
{
    tb_printf("stoi32(%s) = %d [?= %d]\n", s, tb_stoi32(s), val);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_libc_stdlib_main(tb_int_t argc, tb_char_t** argv)
{
    tb_make_isspace_table();
    tb_make_isalpha_table();

#if TB_TEST_ISXXX
    tb_printf("===============================\n");
    tb_check_is();
#endif

#if TB_TEST_TOLOWER
    tb_printf("===============================\n");
    tb_check_tolower();
#endif

#if TB_TEST_TOUPPER
    tb_printf("===============================\n");
    tb_check_toupper();
#endif

#if TB_TEST_SBTOU32
    tb_printf("===============================\n");
    tb_test_sbtou32("1110111001100011001100110", 2, 31415926);
    tb_test_sbtou32("0b1110111001100011001100110", 2, 31415926);
    tb_test_sbtou32("167143146", 8, 31415926);
    tb_test_sbtou32("0167143146", 8, 31415926);
    tb_test_sbtou32("31415926", 10, 31415926);
    tb_test_sbtou32("1dcc666", 16, 31415926);
    tb_test_sbtou32("0x1dcc666", 16, 31415926);
#endif


#if TB_TEST_SBTOI32
    tb_printf("===============================\n");
    tb_test_sbtoi32("1110111001100011001100110", 2, 31415926);
    tb_test_sbtoi32("0b1110111001100011001100110", 2, 31415926);
    tb_test_sbtoi32("167143146", 8, 31415926);
    tb_test_sbtoi32("0167143146", 8, 31415926);
    tb_test_sbtoi32("31415926", 10, 31415926);
    tb_test_sbtoi32("1dcc666", 16, 31415926);
    tb_test_sbtoi32("0x1dcc666", 16, 31415926);

    tb_printf("\n");
    tb_test_sbtoi32("-1110111001100011001100110", 2, -31415926);
    tb_test_sbtoi32("-0b1110111001100011001100110", 2, -31415926);
    tb_test_sbtoi32("-167143146", 8, -31415926);
    tb_test_sbtoi32("-0167143146", 8, -31415926);
    tb_test_sbtoi32("-31415926", 10, -31415926);
    tb_test_sbtoi32("-1dcc666", 16, -31415926);
    tb_test_sbtoi32("-0x1dcc666", 16, -31415926);
#endif

#if TB_TEST_SBTOF && defined(TB_CONFIG_TYPE_HAVE_FLOAT)
    tb_printf("===============================\n");
    tb_test_sbtof("0", 10, 0);
    tb_test_sbtof("0.", 10, 0.);
    tb_test_sbtof("0.0", 10, 0.0);
    tb_test_sbtof("00.00", 10, 00.00);
    tb_test_sbtof("3", 10, 3);
    tb_test_sbtof("3.", 10, 3.);
    tb_test_sbtof("3.1415926", 10, 3.1415926);
    tb_test_sbtof("0.1415926", 10, 0.1415926);
    tb_test_sbtof("3.123456789123456789", 10, 3.123456789123456789);
    tb_test_sbtof("00003.0001415926000", 10, 00003.0001415926000);
    tb_test_sbtof("4294967295", 10, 4294967295ul);
    tb_test_sbtof("4294967295.", 10, 4294967295.);
    tb_test_sbtof("01234567", 8, 01234567);
    tb_test_sbtof("0xb14ac01c", 16, 0xb14ac01c);


    tb_printf("\n");
    tb_test_sbtof("-0", 10, -0);
    tb_test_sbtof("-0.", 10, -0.);
    tb_test_sbtof("-0.0", 10, -0.0);
    tb_test_sbtof("-00.00", 10, -00.00);
    tb_test_sbtof("-3", 10, -3);
    tb_test_sbtof("-3.", 10, -3.);
    tb_test_sbtof("-3.1415926", 10, -3.1415926);
    tb_test_sbtof("-0.1415926", 10, -0.1415926);
    tb_test_sbtof("-3.123456789123456789", 10, -3.123456789123456789);
    tb_test_sbtof("-00003.0001415926000", 10, -00003.0001415926000);
    tb_test_sbtof("-4294967295.", 10, -4294967295.f);

    tb_printf("===============================\n");
    tb_test_sbtof("0", 2, 0);
    tb_test_sbtof("0.", 2, 0.);
    tb_test_sbtof("0.0", 2, 0.0);
    tb_test_sbtof("00.00", 2, 00.00);
    tb_test_sbtof("11", 2, 3);
    tb_test_sbtof("11.", 2, 3.);
    tb_test_sbtof("11.001001", 2, 3.140625);
    tb_test_sbtof("0.001001", 2, 0.140625);
    tb_test_sbtof("11111111111111111111111111111111.", 2, 4294967295.);
#endif

#if TB_TEST_STOU32
    tb_printf("===============================\n");
    tb_test_stou32("0b1110111001100011001100110", 31415926);
    tb_test_stou32("0167143146", 31415926);
    tb_test_stou32("31415926", 31415926);
    tb_test_stou32("0x1dcc666", 31415926);
#endif


#if TB_TEST_STOI32
    tb_printf("===============================\n");
    tb_test_stoi32("0b1110111001100011001100110", 31415926);
    tb_test_stoi32("0167143146", 31415926);
    tb_test_stoi32("31415926", 31415926);
    tb_test_stoi32("0x1dcc666", 31415926);

    tb_printf("\n");
    tb_test_stoi32("-0b1110111001100011001100110", -31415926);
    tb_test_stoi32("-0167143146", -31415926);
    tb_test_stoi32("-31415926", -31415926);
    tb_test_stoi32("-0x1dcc666", -31415926);
#endif
    return 0;
}
