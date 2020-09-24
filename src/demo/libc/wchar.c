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
static tb_void_t tb_test_wcscmp(tb_wchar_t const* s1, tb_wchar_t const* s2)
{
    __tb_volatile__ tb_long_t   n = 1000000;
    __tb_volatile__ tb_long_t   r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = tb_wcscmp(s1, s2);
    }
    t = tb_mclock() - t;
    tb_wprintf(L"%lld ms, tb_test_wcscmp(%s, %s) = %ld\n", t, s1, s2, r);
}
static tb_void_t tb_test_wcsncmp(tb_wchar_t const* s1, tb_wchar_t const* s2, tb_size_t size)
{
    __tb_volatile__ tb_long_t   n = 1000000;
    __tb_volatile__ tb_long_t   r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = tb_wcsncmp(s1, s2, size);
    }
    t = tb_mclock() - t;
    tb_wprintf(L"%lld ms, tb_test_wcsncmp(%s, %s, %lu) = %ld\n", t, s1, s2, size, r);
}
static tb_void_t tb_test_wcsicmp(tb_wchar_t const* s1, tb_wchar_t const* s2)
{
    __tb_volatile__ tb_long_t   n = 1000000;
    __tb_volatile__ tb_long_t   r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = tb_wcsicmp(s1, s2);
    }
    t = tb_mclock() - t;
    tb_wprintf(L"%lld ms, tb_test_wcsicmp(%s, %s) = %ld\n", t, s1, s2, r);
}
static tb_void_t tb_test_wcsnicmp(tb_wchar_t const* s1, tb_wchar_t const* s2, tb_size_t size)
{
    __tb_volatile__ tb_long_t   n = 1000000;
    __tb_volatile__ tb_long_t   r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = tb_wcsnicmp(s1, s2, size);
    }
    t = tb_mclock() - t;
    tb_wprintf(L"%lld ms, tb_test_wcsnicmp(%s, %s, %u) = %ld\n", t, s1, s2, size, r);
}
static tb_void_t tb_test_wcslen(tb_wchar_t const* s)
{
    __tb_volatile__ tb_long_t   n = 1000000;
    __tb_volatile__ tb_long_t   r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = tb_wcslen(s);
    }
    t = tb_mclock() - t;
    tb_wprintf(L"%lld ms, tb_test_wcslen(%s) = %ld\n", t, s, r);
}
static tb_void_t tb_test_wcsnlen(tb_wchar_t const* s, tb_size_t size)
{
    __tb_volatile__ tb_long_t   n = 1000000;
    __tb_volatile__ tb_long_t   r = 0;
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        r = tb_wcsnlen(s, size);
    }
    t = tb_mclock() - t;
    tb_wprintf(L"%lld ms, tb_test_wcsnlen(%s, %ld) = %d\n", t, s, size, r);
}
static tb_void_t tb_test_wcscpy(tb_wchar_t const* s2)
{
    __tb_volatile__ tb_int_t    n = 1000000;
    tb_wchar_t s1[4096];
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        tb_wcscpy(s1, s2);
    }
    t = tb_mclock() - t;
    tb_wprintf(L"%lld ms, tb_test_wcscpy(%s) = %s\n", t, s2, s1);
}
static tb_void_t tb_test_wcsncpy(tb_wchar_t const* s2, tb_size_t size)
{
    __tb_volatile__ tb_int_t    n = 1000000;
    tb_wchar_t s1[4096];
    tb_hong_t t = tb_mclock();
    while (n--)
    {
        tb_wcslcpy(s1, s2, size);
    }
    t = tb_mclock() - t;
    tb_wprintf(L"%lld ms, tb_test_wcsncpy(%s, %lu) = %s\n", t, s2, size, s1);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_libc_wchar_main(tb_int_t argc, tb_char_t** argv)
{
#if TB_TEST_CMP
    tb_wprintf(L"=================================================================\n");
    tb_test_wcscmp(L"", L"");
    tb_test_wcscmp(L"1", L"1");
    tb_test_wcscmp(L"1234567890", L"1234567890");
    tb_test_wcscmp(L"1234567890abcbefg", L"1234567890ABCBEFG");
    tb_test_wcscmp(L"abcdefghijklmnopqrstuvwxyz1234567890", L"abcdefghijklmnopqrstuvwxyz");
    tb_test_wcscmp(L"1234", L"1234567890");
    tb_test_wcscmp(L"abcdefghijklmnopqrstuvwxyz1234567890", L"abcdefghijklmnopqrstuvwxyz1234567890");

    tb_wprintf(L"\n");
    tb_test_wcsicmp(L"", L"");
    tb_test_wcsicmp(L"1", L"1");
    tb_test_wcsicmp(L"1234567890", L"1234567890");
    tb_test_wcsicmp(L"1234567890abcbefg", L"1234567890ABCBEFG");
    tb_test_wcsicmp(L"abcdefghijklmnopqrstuvwxyz1234567890", L"abcdefghijklmnopqrstuvwxyz");
    tb_test_wcsicmp(L"1234", L"1234567890");
    tb_test_wcsicmp(L"abcdefghijklmnopqrstuvwxyz1234567890", L"abcdefghijklmnopqrstuvwxyz1234567890");

    tb_wprintf(L"\n");
    tb_test_wcsncmp(L"", L"", 10);
    tb_test_wcsncmp(L"1", L"1", 10);
    tb_test_wcsncmp(L"1234567890", L"1234567890", 10);
    tb_test_wcsncmp(L"1234567890abcbefg", L"1234567890ABCBEFG", 10);
    tb_test_wcsncmp(L"abcdefghijklmnopqrstuvwxyz1234567890", L"abcdefghijklmnopqrstuvwxyz", 10);
    tb_test_wcsncmp(L"1234", L"1234567890", 10);
    tb_test_wcsncmp(L"abcdefghijklmnopqrstuvwxyz1234567890", L"abcdefghijklmnopqrstuvwxyz1234567890", 20);

    tb_wprintf(L"\n");
    tb_test_wcsnicmp(L"", L"", 10);
    tb_test_wcsnicmp(L"1", L"1", 10);
    tb_test_wcsnicmp(L"1234567890", L"1234567890", 10);
    tb_test_wcsnicmp(L"1234567890abcbefg", L"1234567890ABCBEFG", 10);
    tb_test_wcsnicmp(L"abcdefghijklmnopqrstuvwxyz1234567890", L"abcdefghijklmnopqrstuvwxyz", 10);
    tb_test_wcsnicmp(L"1234", L"1234567890", 10);
    tb_test_wcsnicmp(L"abcdefghijklmnopqrstuvwxyz1234567890", L"abcdefghijklmnopqrstuvwxyz1234567890", 20);

#endif

#if TB_TEST_LEN
    tb_wprintf(L"=================================================================\n");
    tb_test_wcslen(L"");
    tb_test_wcslen(L"1");
    tb_test_wcslen(L"1234567890");
    tb_test_wcslen(L"1234567890abcbefg");
    tb_test_wcslen(L"abcdefghijklmnopqrstuvwxyz1234567890");
    tb_test_wcslen(L"abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890");

    tb_wprintf(L"\n");
    tb_test_wcsnlen(L"", 10);
    tb_test_wcsnlen(L"1", 10);
    tb_test_wcsnlen(L"1234567890", 10);
    tb_test_wcsnlen(L"1234567890abcbefg", 20);
    tb_test_wcsnlen(L"abcdefghijklmnopqrstuvwxyz1234567890", 20);
    tb_test_wcsnlen(L"abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890", 60);

#endif

#if TB_TEST_CPY
    tb_wprintf(L"=================================================================\n");
    tb_test_wcscpy(L"");
    tb_test_wcscpy(L"1");
    tb_test_wcscpy(L"1234567890");
    tb_test_wcscpy(L"1234567890abcbefg");
    tb_test_wcscpy(L"abcdefghijklmnopqrstuvwxyz1234567890");
    tb_test_wcscpy(L"abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890");

    tb_wprintf(L"\n");
    tb_test_wcsncpy(L"", 5);
    tb_test_wcsncpy(L"1", 5);
    tb_test_wcsncpy(L"1234567890", 5);
    tb_test_wcsncpy(L"1234567890abcbefg", 5);
    tb_test_wcsncpy(L"abcdefghijklmnopqrstuvwxyz1234567890", 5);
    tb_test_wcsncpy(L"abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890", 50);

#endif

    return 0;
}
