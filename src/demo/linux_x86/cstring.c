#include "tplat/tplat.h"
#include "../../tbox.h"

#define TB_CSTRING_TEST_COMPARE 	(0)
#define TB_CSTRING_TEST_SIZE 		(0)
#define TB_CSTRING_TEST_COPY 		(1)

/* ////////////////////////////////////////////////////////////////////////
 * compare
 */
static void tb_cstring_test_compare(tb_char_t const* s1, tb_char_t const* s2)
{
	__tplat_volatile__ tb_int_t 	n = 100000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = tb_cstring_compare(s1, s2);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_cstring_test_compare(%s, %s) = %d\n", (tb_int_t)t, s1, s2, r);
}
static void tb_cstring_test_compare_libc(tb_char_t const* s1, tb_char_t const* s2)
{
	__tplat_volatile__ tb_int_t 	n = 100000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = strcmp(s1, s2);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_cstring_test_compare_libc(%s, %s) = %d\n", (tb_int_t)t, s1, s2, r);
}
static void tb_cstring_test_compare_nocase(tb_char_t const* s1, tb_char_t const* s2)
{
	__tplat_volatile__ tb_int_t 	n = 100000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = tb_cstring_compare_nocase(s1, s2);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_cstring_test_compare_nocase(%s, %s) = %d\n", (tb_int_t)t, s1, s2, r);
}
static void tb_cstring_test_compare_nocase_libc(tb_char_t const* s1, tb_char_t const* s2)
{
	__tplat_volatile__ tb_int_t 	n = 100000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = strcasecmp(s1, s2);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_cstring_test_compare_nocase_libc(%s, %s) = %d\n", (tb_int_t)t, s1, s2, r);
}
static void tb_cstring_test_size(tb_char_t const* s)
{
	__tplat_volatile__ tb_int_t 	n = 100000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = tb_cstring_size(s);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_cstring_test_size(%s) = %d\n", (tb_int_t)t, s, r);
}
static void tb_cstring_test_copy(tb_char_t const* s2)
{
	__tplat_volatile__ tb_int_t 	n = 100000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tb_char_t s1[4096];
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = tb_cstring_copy(s1, s2);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_cstring_test_copy(%s) = %s\n", (tb_int_t)t, s2, s1);
}
static void tb_cstring_test_copy_libc(tb_char_t const* s2)
{
	__tplat_volatile__ tb_int_t 	n = 100000000;
	__tplat_volatile__ tb_int_t 	r = 0;
	tb_char_t s1[4096];
	tplat_int64_t t = tplat_clock();
	while (n--)
	{
		r = strcpy(s1, s2);
	}
	t = tplat_clock() - t;
	tplat_printf("%d ms, tb_cstring_test_copy_libc(%s) = %s\n", (tb_int_t)t, s2, s1);
}
int main(int argc, char** argv)
{
	tplat_size_t regular_block_n[TPLAT_POOL_REGULAR_CHUNCK_MAX_COUNT] = {0, 0, 0, 0, 0, 0, 0};
	tplat_pool_create(TB_CONFIG_MEMORY_POOL_INDEX, malloc(1024 * 1024), 1024 * 1024, regular_block_n);

#if TB_CSTRING_TEST_COMPARE
	tplat_printf("=================================================================\n");
	tb_cstring_test_compare("", "");
	tb_cstring_test_compare("1", "1");
	tb_cstring_test_compare("1234567890", "1234567890");
	tb_cstring_test_compare("1234567890abcbefg", "1234567890ABCBEFG");
	tb_cstring_test_compare("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz");
	tb_cstring_test_compare("1234", "1234567890");
	tb_cstring_test_compare("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz1234567890");
	
	tplat_printf("\n");
	tb_cstring_test_compare_nocase("", "");
	tb_cstring_test_compare_nocase("1", "1");
	tb_cstring_test_compare_nocase("1234567890", "1234567890");
	tb_cstring_test_compare_nocase("1234567890abcbefg", "1234567890ABCBEFG");
	tb_cstring_test_compare_nocase("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz");
	tb_cstring_test_compare_nocase("1234", "1234567890");
	tb_cstring_test_compare_nocase("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz1234567890");

	tplat_printf("\n");
	tb_cstring_test_compare_libc("", "");
	tb_cstring_test_compare_libc("1", "1");
	tb_cstring_test_compare_libc("1234567890", "1234567890");
	tb_cstring_test_compare_libc("1234567890abcbefg", "1234567890ABCBEFG");
	tb_cstring_test_compare_libc("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz");
	tb_cstring_test_compare_libc("1234", "1234567890");
	tb_cstring_test_compare_libc("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz1234567890");

	tplat_printf("\n");
	tb_cstring_test_compare_nocase_libc("", "");
	tb_cstring_test_compare_nocase_libc("1", "1");
	tb_cstring_test_compare_nocase_libc("1234567890", "1234567890");
	tb_cstring_test_compare_nocase_libc("1234567890abcbefg", "1234567890ABCBEFG");
	tb_cstring_test_compare_nocase_libc("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz");
	tb_cstring_test_compare_nocase_libc("1234", "1234567890");
	tb_cstring_test_compare_nocase_libc("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz1234567890");
#endif

#if TB_CSTRING_TEST_SIZE
	tplat_printf("=================================================================\n");
	tb_cstring_test_size("");
	tb_cstring_test_size("1");
	tb_cstring_test_size("1234567890");
	tb_cstring_test_size("1234567890abcbefg");
	tb_cstring_test_size("abcdefghijklmnopqrstuvwxyz1234567890");
	
	tplat_printf("\n");
	tb_cstring_test_size_libc("");
	tb_cstring_test_size_libc("1");
	tb_cstring_test_size_libc("1234567890");
	tb_cstring_test_size_libc("1234567890abcbefg");
	tb_cstring_test_size_libc("abcdefghijklmnopqrstuvwxyz1234567890");

#endif

#if TB_CSTRING_TEST_COPY
	tplat_printf("=================================================================\n");
	tb_cstring_test_copy("");
	tb_cstring_test_copy("1");
	tb_cstring_test_copy("1234567890");
	tb_cstring_test_copy("1234567890abcbefg");
	tb_cstring_test_copy("abcdefghijklmnopqrstuvwxyz1234567890");
	
	tplat_printf("\n");
	tb_cstring_test_copy_libc("");
	tb_cstring_test_copy_libc("1");
	tb_cstring_test_copy_libc("1234567890");
	tb_cstring_test_copy_libc("1234567890abcbefg");
	tb_cstring_test_copy_libc("abcdefghijklmnopqrstuvwxyz1234567890");

#endif
	return 0;
}
