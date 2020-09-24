/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */
static tb_option_item_t g_options[] =
{
    {   '-'
    ,   "demo"
    ,   TB_OPTION_MODE_VAL
    ,   TB_OPTION_TYPE_CSTR
    ,   "the demo:\n"
        "    demo0: the demo test0\n"
        "    demo1: the demo test1\n"
        "    demo2: the demo test2\n"
        "    demo3: the demo test3\n"
    }
,   {'-',   "lower",        TB_OPTION_MODE_KEY,         TB_OPTION_TYPE_BOOL,        "display lower string"}
,   {'-',   "upper",        TB_OPTION_MODE_KEY,         TB_OPTION_TYPE_BOOL,        "display upper string"}
,   {'i',   "integer",      TB_OPTION_MODE_KEY_VAL,     TB_OPTION_TYPE_INTEGER,     "display integer value"}
,   {'f',   "float",        TB_OPTION_MODE_KEY_VAL,     TB_OPTION_TYPE_FLOAT,       "display float value"}
,   {'b',   "boolean",      TB_OPTION_MODE_KEY_VAL,     TB_OPTION_TYPE_BOOL,        "display boolean value"}
,   {'s',   "string",       TB_OPTION_MODE_KEY_VAL,     TB_OPTION_TYPE_CSTR,        "display string value"}
,   {'h',   "help",         TB_OPTION_MODE_KEY,         TB_OPTION_TYPE_BOOL,        "display this help and exit"}
,   {'v',   "version",      TB_OPTION_MODE_KEY,         TB_OPTION_TYPE_BOOL,        "output version information and exit"}
,   {'-',   "file0",        TB_OPTION_MODE_VAL,         TB_OPTION_TYPE_CSTR,        "the file0 path"}
,   {'-',   "file1",        TB_OPTION_MODE_VAL,         TB_OPTION_TYPE_CSTR,        "the file1 path"}
,   {'-',   tb_null,        TB_OPTION_MODE_MORE,        TB_OPTION_TYPE_NONE,        tb_null}

};

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_utils_option_main(tb_int_t argc, tb_char_t** argv)
{
    // init option
    tb_option_ref_t option = tb_option_init("option", "the option command test demo", g_options);
    if (option)
    {
        // done option
        if (tb_option_done(option, argc - 1, &argv[1]))
        {
            // done dump
            tb_option_dump(option);

            // done help
            if (tb_option_find(option, "help"))
                tb_option_help(option);
#ifdef TB_CONFIG_INFO_HAVE_VERSION
            // done version
            else if (tb_option_find(option, "version"))
            {
                tb_version_t const* version = tb_version();
                if (version) tb_trace_i("version: tbox-v%u.%u.%u.%llu", version->major, version->minor, version->alter, version->build);
            }
#endif
            else
            {
                // done integer
                if (tb_option_find(option, "i"))
                    tb_trace_i("integer: %lld", tb_option_item_sint64(option, "i"));
                // done string
                if (tb_option_find(option, "s"))
                    tb_trace_i("string: %s", tb_option_item_cstr(option, "s"));
#ifdef TB_CONFIG_TYPE_HAVE_FLOAT
                // done float
                if (tb_option_find(option, "f"))
                    tb_trace_i("float: %f", tb_option_item_float(option, "f"));
#endif
                // done boolean
                if (tb_option_find(option, "b"))
                    tb_trace_i("boolean: %s", tb_option_item_bool(option, "b")? "y" : "n");
                // done demo
                if (tb_option_find(option, "demo"))
                    tb_trace_i("demo: %s", tb_option_item_cstr(option, "demo"));
                // done file0
                if (tb_option_find(option, "file0"))
                    tb_trace_i("file0: %s", tb_option_item_cstr(option, "file0"));
                // done file1
                if (tb_option_find(option, "file1"))
                    tb_trace_i("file1: %s", tb_option_item_cstr(option, "file1"));

                // done more
                tb_size_t more = 0;
                while (1)
                {
                    tb_char_t name[64] = {0};
                    tb_snprintf(name, 63, "more%lu", more++);
                    if (tb_option_find(option, name))
                        tb_trace_i("%s: %s", name, tb_option_item_cstr(option, name));
                    else break;
                }
            }
        }
        else tb_option_help(option);

        // exit option
        tb_option_exit(option);
    }

    return 0;
}
