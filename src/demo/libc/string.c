/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_TEST_CMP         (1)
#define TB_TEST_LEN         (1)
#define TB_TEST_CPY         (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * compare
 */
static tb_void_t tb_test_strcmp(tb_char_t const* s1, tb_char_t const* s2)
{
    __tb_volatile__ tb_long_t   n = 1000000;
    __tb_volatile__ tb_long_t   r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = tb_strcmp(s1, s2);
    }
    t = tb_mclock() - t;
    tb_printf("%lld ms, tb_test_strcmp(%s, %s) = %ld\n", t, s1, s2, r);
}
static tb_void_t tb_test_strncmp(tb_char_t const* s1, tb_char_t const* s2, tb_size_t size)
{
    __tb_volatile__ tb_long_t   n = 1000000;
    __tb_volatile__ tb_long_t   r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = tb_strncmp(s1, s2, size);
    }
    t = tb_mclock() - t;
    tb_printf("%lld ms, tb_test_strncmp(%s, %s, %u) = %ld\n", t, s1, s2, size, r);
}
static tb_void_t tb_test_stricmp(tb_char_t const* s1, tb_char_t const* s2)
{
    __tb_volatile__ tb_long_t   n = 1000000;
    __tb_volatile__ tb_long_t   r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = tb_stricmp(s1, s2);
    }
    t = tb_mclock() - t;
    tb_printf("%lld ms, tb_test_stricmp(%s, %s) = %ld\n", t, s1, s2, r);
}
static tb_void_t tb_test_strnicmp(tb_char_t const* s1, tb_char_t const* s2, tb_size_t size)
{
    __tb_volatile__ tb_long_t   n = 1000000;
    __tb_volatile__ tb_long_t   r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = tb_strnicmp(s1, s2, size);
    }
    t = tb_mclock() - t;
    tb_printf("%lld ms, tb_test_strnicmp(%s, %s, %u) = %ld\n", t, s1, s2, size, r);
}
static tb_void_t tb_test_strlen(tb_char_t const* s)
{
    __tb_volatile__ tb_long_t   n = 1000000;
    __tb_volatile__ tb_long_t   r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = tb_strlen(s);
    }
    t = tb_mclock() - t;
    tb_printf("%lld ms, tb_test_strlen(%s) = %ld\n", t, s, r);
}
static tb_void_t tb_test_strnlen(tb_char_t const* s, tb_size_t size)
{
    __tb_volatile__ tb_long_t   n = 1000000;
    __tb_volatile__ tb_long_t   r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = tb_strnlen(s, size);
    }
    t = tb_mclock() - t;
    tb_printf("%lld ms, tb_test_strnlen(%s, %u) = %ld\n", t, s, size, r);
}
static tb_void_t tb_test_strcpy(tb_char_t const* s2)
{
    __tb_volatile__ tb_int_t    n = 1000000;
    tb_char_t s1[4096];
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        tb_strcpy(s1, s2);
    }
    t = tb_mclock() - t;
    tb_printf("%lld ms, tb_test_strcpy(%s) = %s\n", t, s2, s1);
}
static tb_void_t tb_test_strncpy(tb_char_t const* s2, tb_size_t size)
{
    __tb_volatile__ tb_int_t    n = 1000000;
    tb_char_t s1[4096];
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        tb_strlcpy(s1, s2, size);
    }
    t = tb_mclock() - t;
    tb_printf("%lld ms, tb_test_strncpy(%s, %d) = %s\n", t, s2, size, s1);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_libc_string_main(tb_int_t argc, tb_char_t** argv)
{
#if TB_TEST_CMP
    tb_printf("=================================================================\n");
    tb_test_strcmp("", "");
    tb_test_strcmp("1", "1");
    tb_test_strcmp("1234567890", "1234567890");
    tb_test_strcmp("1234567890abcbefg", "1234567890ABCBEFG");
    tb_test_strcmp("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz");
    tb_test_strcmp("1234", "1234567890");
    tb_test_strcmp("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz1234567890");

    tb_printf("\n");
    tb_test_stricmp("", "");
    tb_test_stricmp("1", "1");
    tb_test_stricmp("1234567890", "1234567890");
    tb_test_stricmp("1234567890abcbefg", "1234567890ABCBEFG");
    tb_test_stricmp("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz");
    tb_test_stricmp("1234", "1234567890");
    tb_test_stricmp("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz1234567890");

    tb_printf("\n");
    tb_test_strncmp("", "", 10);
    tb_test_strncmp("1", "1", 10);
    tb_test_strncmp("1234567890", "1234567890", 10);
    tb_test_strncmp("1234567890abcbefg", "1234567890ABCBEFG", 10);
    tb_test_strncmp("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz", 10);
    tb_test_strncmp("1234", "1234567890", 10);
    tb_test_strncmp("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz1234567890", 20);

    tb_printf("\n");
    tb_test_strnicmp("", "", 10);
    tb_test_strnicmp("1", "1", 10);
    tb_test_strnicmp("1234567890", "1234567890", 10);
    tb_test_strnicmp("1234567890abcbefg", "1234567890ABCBEFG", 10);
    tb_test_strnicmp("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz", 10);
    tb_test_strnicmp("1234", "1234567890", 10);
    tb_test_strnicmp("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz1234567890", 20);

#endif

#if TB_TEST_LEN
    tb_printf("=================================================================\n");
    tb_test_strlen("");
    tb_test_strlen("1");
    tb_test_strlen("1234567890");
    tb_test_strlen("1234567890abcbefg");
    tb_test_strlen("abcdefghijklmnopqrstuvwxyz1234567890");
    tb_test_strlen("abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890");

    tb_printf("\n");
    tb_test_strnlen("", 10);
    tb_test_strnlen("1", 10);
    tb_test_strnlen("1234567890", 10);
    tb_test_strnlen("1234567890abcbefg", 20);
    tb_test_strnlen("abcdefghijklmnopqrstuvwxyz1234567890", 20);
    tb_test_strnlen("abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890", 60);

#endif

#if TB_TEST_CPY
    tb_printf("=================================================================\n");
    tb_test_strcpy("");
    tb_test_strcpy("1");
    tb_test_strcpy("1234567890");
    tb_test_strcpy("1234567890abcbefg");
    tb_test_strcpy("abcdefghijklmnopqrstuvwxyz1234567890");
    tb_test_strcpy("abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890");

    tb_printf("\n");
    tb_test_strncpy("", 5);
    tb_test_strncpy("1", 5);
    tb_test_strncpy("1234567890", 5);
    tb_test_strncpy("1234567890abcbefg", 5);
    tb_test_strncpy("abcdefghijklmnopqrstuvwxyz1234567890", 5);
    tb_test_strncpy("abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890", 50);

#endif

    return 0;
}
