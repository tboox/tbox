/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * test
 */
static tb_void_t tb_demo_regex_test_match_simple(tb_char_t const* pattern, tb_char_t const* content)
{
    // trace
    tb_trace_i("match_simple: %s, %s", content, pattern);

    // done
    tb_vector_ref_t results = tb_regex_match_done_simple(pattern, 0, content);
    if (results)
    {
        // show results
        tb_for_all_if (tb_regex_match_ref_t, entry, results, entry)
        {
            // trace
            tb_trace_i("[%lu, %lu]: %s", entry->start, entry->size, entry->cstr);
        }

        // exit results
        tb_vector_exit(results);
    }

    // trace
    tb_trace_i("");
}
static tb_void_t tb_demo_regex_test_replace_simple(tb_char_t const* pattern, tb_char_t const* content, tb_char_t const* replacement)
{
    // trace
    tb_trace_i("replace_simple: %s, %s to %s", content, pattern, replacement);

    // done
    tb_char_t const* results = tb_regex_replace_done_simple(pattern, 0, content, replacement);
    if (results)
    {
        // trace
        tb_trace_i(": %s", results);

        // exit results
        tb_free(results);
    }
}
static tb_void_t tb_demo_regex_test_match_global(tb_char_t const* pattern, tb_char_t const* content)
{
    // trace
    tb_trace_i("match_global: %s, %s", content, pattern);

    // init regex
    tb_regex_ref_t regex = tb_regex_init(pattern, 0);
    if (regex)
    {
        // done
        tb_long_t       start = 0;
        tb_size_t       length = 0;
        tb_vector_ref_t results = tb_null;
        while ((start = tb_regex_match_cstr(regex, content, start + length, &length, &results)) >= 0 && results)
        {
            // trace
            tb_trace_i("[%lu, %lu]: ", start, length);

            // show results
            tb_for_all_if (tb_regex_match_ref_t, entry, results, entry)
            {
                // trace
                tb_trace_i("    [%lu, %lu]: %s", entry->start, entry->size, entry->cstr);
            }
        }

        // exit regex
        tb_regex_exit(regex);
    }

    // trace
    tb_trace_i("");
}
static tb_void_t tb_demo_regex_test_replace_global(tb_char_t const* pattern, tb_char_t const* content, tb_char_t const* replacement)
{
    // trace
    tb_trace_i("replace_global: %s, %s to %s", content, pattern, replacement);

    // done
    tb_char_t const* results = tb_regex_replace_done_simple(pattern, TB_REGEX_MODE_GLOBAL, content, replacement);
    if (results)
    {
        // trace
        tb_trace_i(": %s", results);

        // exit results
        tb_free(results);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_regex_main(tb_int_t argc, tb_char_t** argv)
{
    // test arguments
    if (argv[1] && argv[2]) tb_demo_regex_test_match_global(argv[1], argv[2]);

    // test match
    tb_demo_regex_test_match_simple("\\w+", "hello world");
    tb_demo_regex_test_match_global("\\w+", "hello world");

    tb_demo_regex_test_match_simple("(\\w+)\\s+?(\\w+)", "hello world");
    tb_demo_regex_test_match_global("(\\w+)\\s+?(\\w+)", "hello world");

    // test replace
    tb_demo_regex_test_replace_simple("\\w+", "hello world", "hi");
    tb_demo_regex_test_replace_global("\\w+", "hello world", "hi");

    // ok
    return 0;
}
