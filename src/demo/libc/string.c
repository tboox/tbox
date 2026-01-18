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
#define TB_TEST_CASE        (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * case
 */
static tb_void_t tb_test_strupr(tb_char_t const* s)
{
    tb_char_t b[4096];
    tb_strcpy(b, s);
    tb_trace_i("strupr(%s) = %s", s, tb_strupr(b));
}
static tb_void_t tb_test_strlwr(tb_char_t const* s)
{
    tb_char_t b[4096];
    tb_strcpy(b, s);
    tb_trace_i("strlwr(%s) = %s", s, tb_strlwr(b));
}
static tb_void_t tb_test_wcsupr(tb_char_t const* s)
{
    // convert to wchar_t
    tb_wchar_t w[4096];
    tb_mbstowcs(w, s, 4096);

    // upper
    tb_wcsupr(w);

    // convert to char
    tb_char_t b[4096];
    tb_wcstombs(b, w, 4096);

    // trace
    tb_trace_i("wcsupr(%s) = %s", s, b);
}
static tb_void_t tb_test_wcslwr(tb_char_t const* s)
{
    // convert to wchar_t
    tb_wchar_t w[4096];
    tb_mbstowcs(w, s, 4096);

    // lower
    tb_wcslwr(w);

    // convert to char
    tb_char_t b[4096];
    tb_wcstombs(b, w, 4096);

    // trace
    tb_trace_i("wcslwr(%s) = %s", s, b);
}
#ifdef TB_CONFIG_MODULE_HAVE_CHARSET
static tb_void_t tb_test_strlwr_utf8(tb_char_t const* s)
{
    tb_char_t b[4096];
    tb_strcpy(b, s);
    tb_long_t n = tb_charset_utf8_tolower(b, tb_strlen(b));
    tb_trace_i("strlwr_utf8(%s) = %s, size: %ld", s, b, n);
}
static tb_void_t tb_test_strupr_utf8(tb_char_t const* s)
{
    tb_char_t b[4096];
    tb_strcpy(b, s);
    tb_long_t n = tb_charset_utf8_toupper(b, tb_strlen(b));
    tb_trace_i("strupr_utf8(%s) = %s, size: %ld", s, b, n);
}
#endif

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
    tb_trace_i("%lld ms, tb_test_strcmp(%s, %s) = %ld", t, s1, s2, r);
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
    tb_trace_i("%lld ms, tb_test_strncmp(%s, %s, %u) = %ld", t, s1, s2, size, r);
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
    tb_trace_i("%lld ms, tb_test_stricmp(%s, %s) = %ld", t, s1, s2, r);
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
    tb_trace_i("%lld ms, tb_test_strnicmp(%s, %s, %u) = %ld", t, s1, s2, size, r);
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
    tb_trace_i("%lld ms, tb_test_strlen(%s) = %ld", t, s, r);
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
    tb_trace_i("%lld ms, tb_test_strnlen(%s, %u) = %ld", t, s, size, r);
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
    tb_trace_i("%lld ms, tb_test_strcpy(%s) = %s", t, s2, s1);
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
    tb_trace_i("%lld ms, tb_test_strncpy(%s, %d) = %s", t, s2, size, s1);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_libc_string_main(tb_int_t argc, tb_char_t** argv)
{
#if TB_TEST_CMP
    tb_trace_i("=================================================================");
    tb_test_strcmp("", "");
    tb_test_strcmp("1", "1");
    tb_test_strcmp("1234567890", "1234567890");
    tb_test_strcmp("1234567890abcbefg", "1234567890ABCBEFG");
    tb_test_strcmp("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz");
    tb_test_strcmp("1234", "1234567890");
    tb_test_strcmp("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz1234567890");

    tb_trace_i("");
    tb_test_stricmp("", "");
    tb_test_stricmp("1", "1");
    tb_test_stricmp("1234567890", "1234567890");
    tb_test_stricmp("1234567890abcbefg", "1234567890ABCBEFG");
    tb_test_stricmp("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz");
    tb_test_stricmp("1234", "1234567890");
    tb_test_stricmp("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz1234567890");

    tb_trace_i("");
    tb_test_strncmp("", "", 10);
    tb_test_strncmp("1", "1", 10);
    tb_test_strncmp("1234567890", "1234567890", 10);
    tb_test_strncmp("1234567890abcbefg", "1234567890ABCBEFG", 10);
    tb_test_strncmp("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz", 10);
    tb_test_strncmp("1234", "1234567890", 10);
    tb_test_strncmp("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz1234567890", 20);

    tb_trace_i("");
    tb_test_strnicmp("", "", 10);
    tb_test_strnicmp("1", "1", 10);
    tb_test_strnicmp("1234567890", "1234567890", 10);
    tb_test_strnicmp("1234567890abcbefg", "1234567890ABCBEFG", 10);
    tb_test_strnicmp("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz", 10);
    tb_test_strnicmp("1234", "1234567890", 10);
    tb_test_strnicmp("abcdefghijklmnopqrstuvwxyz1234567890", "abcdefghijklmnopqrstuvwxyz1234567890", 20);

#endif

#if TB_TEST_LEN
    tb_trace_i("=================================================================");
    tb_test_strlen("");
    tb_test_strlen("1");
    tb_test_strlen("1234567890");
    tb_test_strlen("1234567890abcbefg");
    tb_test_strlen("abcdefghijklmnopqrstuvwxyz1234567890");
    tb_test_strlen("abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890");

    tb_trace_i("");
    tb_test_strnlen("", 10);
    tb_test_strnlen("1", 10);
    tb_test_strnlen("1234567890", 10);
    tb_test_strnlen("1234567890abcbefg", 20);
    tb_test_strnlen("abcdefghijklmnopqrstuvwxyz1234567890", 20);
    tb_test_strnlen("abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890", 60);

#endif

#if TB_TEST_CPY
    tb_trace_i("=================================================================");
    tb_test_strcpy("");
    tb_test_strcpy("1");
    tb_test_strcpy("1234567890");
    tb_test_strcpy("1234567890abcbefg");
    tb_test_strcpy("abcdefghijklmnopqrstuvwxyz1234567890");
    tb_test_strcpy("abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890");

    tb_trace_i("");
    tb_test_strncpy("", 5);
    tb_test_strncpy("1", 5);
    tb_test_strncpy("1234567890", 5);
    tb_test_strncpy("1234567890abcbefg", 5);
    tb_test_strncpy("abcdefghijklmnopqrstuvwxyz1234567890", 5);
    tb_test_strncpy("abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklmnopqrstuvwxyz1234567890", 50);

#endif

#if TB_TEST_CASE
    tb_trace_i("=================================================================");
    tb_test_strupr("hello");
    tb_test_strupr("Hello");
    tb_test_strlwr("HELLO");
    tb_test_strlwr("Hello");

    tb_trace_i("");
    tb_test_wcsupr("hello");
    tb_test_wcsupr("Hello");
    // Russian characters
    tb_test_wcsupr("\xD0\x97\xD0\xB2\xD0\xB5\xD0\xB7\xD0\xB4\xD0\xB0 \xD0\xA5\xD1\x8D\xD0\xBD\xD1\x81\xD0\xB8\xD0\xBD");
    // Text with Chinese and Emoji
    tb_test_wcsupr("Test \xE6\xBA\x90\xE6\x96\x87\xE4\xBB\xB6\xF0\x9F\x8E\x86 Message");

    tb_trace_i("");
    tb_test_wcslwr("HELLO");
    tb_test_wcslwr("Hello");
    // Russian characters
    tb_test_wcslwr("\xD0\x97\xD0\xB2\xD0\xB5\xD0\xB7\xD0\xB4\xD0\xB0 \xD0\xA5\xD1\x8D\xD0\xBD\xD1\x81\xD0\xB8\xD0\xBD");
    // Text with Chinese and Emoji
    tb_test_wcslwr("Test \xE6\xBA\x90\xE6\x96\x87\xE4\xBB\xB6\xF0\x9F\x8E\x86 Message");

    tb_trace_i("");
#ifdef TB_CONFIG_MODULE_HAVE_CHARSET
    tb_test_strlwr_utf8("HELLO");
    tb_test_strlwr_utf8("Hello");
    // Russian characters
    tb_test_strlwr_utf8("\xD0\x97\xD0\xB2\xD0\xB5\xD0\xB7\xD0\xB4\xD0\xB0 \xD0\xA5\xD1\x8D\xD0\xBD\xD1\x81\xD0\xB8\xD0\xBD");
    // Text with Chinese and Emoji
    tb_test_strlwr_utf8("Test \xE6\xBA\x90\xE6\x96\x87\xE4\xBB\xB6\xF0\x9F\x8E\x86 Message");

    tb_trace_i("");
    tb_test_strupr_utf8("HELLO");
    tb_test_strupr_utf8("Hello");
    // Russian characters
    tb_test_strupr_utf8("\xD0\x97\xD0\xB2\xD0\xB5\xD0\xB7\xD0\xB4\xD0\xB0 \xD0\xA5\xD1\x8D\xD0\xBD\xD1\x81\xD0\xB8\xD0\xBD");
    // Text with Chinese and Emoji
    tb_test_strupr_utf8("Test \xE6\xBA\x90\xE6\x96\x87\xE4\xBB\xB6\xF0\x9F\x8E\x86 Message");
#endif
#endif

    return 0;
}
