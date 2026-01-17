/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_wctype_test_lower(tb_char_t const* input, tb_char_t const* expected)
{
    // convert input to wchar
    tb_size_t n = tb_strlen(input);
    tb_wchar_t* iw = (tb_wchar_t*)tb_malloc0((n + 1) * sizeof(tb_wchar_t));
    tb_mbstowcs(iw, input, n + 1);

    // convert expected to wchar
    tb_size_t m = tb_strlen(expected);
    tb_wchar_t* ew = (tb_wchar_t*)tb_malloc0((m + 1) * sizeof(tb_wchar_t));
    tb_mbstowcs(ew, expected, m + 1);

    // do conversion
    tb_wchar_t* p = iw;
    while (*p)
    {
        *p = tb_towlower(*p);
        p++;
    }

    // check
    if (!tb_wcscmp(iw, ew))
    {
        tb_trace_i("lower check passed: %s -> %s", input, expected);
    }
    else
    {
        tb_trace_i("lower check failed: %s -> %s", input, expected);
    }

    tb_free(iw);
    tb_free(ew);
}

static tb_void_t tb_demo_wctype_test_upper(tb_char_t const* input, tb_char_t const* expected)
{
    // convert input to wchar
    tb_size_t n = tb_strlen(input);
    tb_wchar_t* iw = (tb_wchar_t*)tb_malloc0((n + 1) * sizeof(tb_wchar_t));
    tb_mbstowcs(iw, input, n + 1);

    // convert expected to wchar
    tb_size_t m = tb_strlen(expected);
    tb_wchar_t* ew = (tb_wchar_t*)tb_malloc0((m + 1) * sizeof(tb_wchar_t));
    tb_mbstowcs(ew, expected, m + 1);

    // do conversion
    tb_wchar_t* p = iw;
    while (*p)
    {
        *p = tb_towupper(*p);
        p++;
    }

    // check
    if (!tb_wcscmp(iw, ew))
    {
        tb_trace_i("upper check passed: %s -> %s", input, expected);
    }
    else
    {
        tb_trace_i("upper check failed: %s -> %s", input, expected);
    }

    tb_free(iw);
    tb_free(ew);
}

tb_int_t tb_demo_libc_wctype_main(tb_int_t argc, tb_char_t** argv)
{
    // set locale
    tb_setlocale();

    // tests
    tb_demo_wctype_test_lower("Hello", "hello");
    tb_demo_wctype_test_upper("Hello", "HELLO");

    // Russian characters
    tb_demo_wctype_test_lower("\xD0\x97\xD0\xB2\xD0\xB5\xD0\xB7\xD0\xB4\xD0\xB0 \xD0\xA5\xD1\x8D\xD0\xBD\xD1\x81\xD0\xB8\xD0\xBD", "\xD0\xB7\xD0\xB2\xD0\xB5\xD0\xB7\xD0\xB4\xD0\xB0 \xD1\x85\xD1\x8D\xD0\xBD\xD1\x81\xD0\xB8\xD0\xBD");
    // Russian characters
    tb_demo_wctype_test_upper("\xD0\x97\xD0\xB2\xD0\xB5\xD0\xB7\xD0\xB4\xD0\xB0 \xD0\xA5\xD1\x8D\xD0\xBD\xD1\x81\xD0\xB8\xD0\xBD", "\xD0\x97\xD0\x92\xD0\x95\xD0\x97\xD0\x94\xD0\x90 \xD0\xA5\xD0\xAD\xD0\x9D\xD0\xA1\xD0\x98\xD0\x9D");

    // Text with Chinese and Emoji
    tb_demo_wctype_test_lower("Test \xE6\xBA\x90\xE6\x96\x87\xE4\xBB\xB6\xF0\x9F\x8E\x86 Message", "test \xE6\xBA\x90\xE6\x96\x87\xE4\xBB\xB6\xF0\x9F\x8E\x86 message");
    // Text with Chinese and Emoji
    tb_demo_wctype_test_upper("Test \xE6\xBA\x90\xE6\x96\x87\xE4\xBB\xB6\xF0\x9F\x8E\x86 Message", "TEST \xE6\xBA\x90\xE6\x96\x87\xE4\xBB\xB6\xF0\x9F\x8E\x86 MESSAGE");

    // reset locale
    tb_resetlocale();
    return 0;
}
