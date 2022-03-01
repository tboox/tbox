/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

static tb_void_t tb_demo_path_test_directory(tb_char_t const* path, tb_char_t const* excepted)
{
    tb_char_t data[TB_PATH_MAXN] = {0};
    tb_char_t const* result = tb_path_directory(path, data, sizeof(data));
    if (result && excepted && !tb_strcmp(result, excepted))
        tb_trace_i("directory(%s): %s passed!", path, result);
    else if (!result && !excepted)
        tb_trace_i("directory(%s): null passed!", path);
    else tb_trace_i("directory(%s): %s != %s", path, result, excepted);
}

static tb_void_t tb_demo_path_test_absolute(tb_char_t const* path, tb_char_t const* excepted)
{
}

static tb_void_t tb_demo_path_test_relative(tb_char_t const* path, tb_char_t const* excepted)
{
}

static tb_void_t tb_demo_path_test_translate(tb_char_t const* path, tb_char_t const* excepted)
{
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_platform_path_main(tb_int_t argc, tb_char_t** argv)
{
    tb_demo_path_test_directory("", tb_null);
    tb_demo_path_test_directory(".", tb_null);
    tb_demo_path_test_directory("/tmp", "/");
    tb_demo_path_test_directory("/tmp/", "/");
    tb_demo_path_test_directory("/tmp/xxx", "/tmp");
    tb_demo_path_test_directory("/tmp/xxx/", "/tmp");
    tb_demo_path_test_directory("/", tb_null);
#ifdef TB_CONFIG_OS_WINDOWS
    tb_demo_path_test_directory("c:", tb_null);
    tb_demo_path_test_directory("c:\\", tb_null);
    tb_demo_path_test_directory("c:\\xxx", "c:");
    tb_demo_path_test_directory("c:\\xxx\\yyy", "c:\\xxx");
#endif

    tb_demo_path_test_absolute("", "");
    tb_demo_path_test_relative("", "");
    tb_demo_path_test_translate("", "");
    return 0;
}
