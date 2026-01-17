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

    tb_demo_wctype_test_lower("Звезда Хэнсин", "звезда хэнсин");
    tb_demo_wctype_test_upper("Звезда Хэнсин", "ЗВЕЗДА ХЭНСИН");

    tb_demo_wctype_test_lower("Test 源文件🎆 Message", "test 源文件🎆 message");
    tb_demo_wctype_test_upper("Test 源文件🎆 Message", "TEST 源文件🎆 MESSAGE");

    // reset locale
    tb_resetlocale();
    return 0;
}
