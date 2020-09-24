/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */
static tb_option_item_t g_options[] =
{
    {   'f'
    ,   "filter"
    ,   TB_OPTION_MODE_KEY_VAL
    ,   TB_OPTION_TYPE_CSTR
    ,   "the json filter\n"
        ".e.g\n"
        "\n"
        "file:\n"
        "{\n"
        "    \"string\":       \"hello world!\"\n"
        ",   \"com.xxx.xxx\":  \"hello world\"\n"
        ",   \"integer\":      31415926\n"
        ",   \"array\":\n"
        "    [\n"
        "        \"hello world!\"\n"
        "    ,   31415926\n"
        "    ,   3.1415926\n"
        "    ,   false\n"
        "    ,   true\n"
        "    ,   { \"string\": \"hello world!\" }\n"
        "    ]\n"
        ",   \"macro\":        \"$.array[2]\"\n"
        ",   \"macro2\":       \"$.com\\\\.xxx\\\\.xxx\"\n"
        ",   \"macro3\":       \"$.macro\"\n"
        ",   \"macro4\":       \"$.array\"\n"
        "}\n"
        "\n"
        "filter:\n"
        "    1. \".string\"               : hello world!\n"
        "    2. \".array[1]\"             : 31415926\n"
        "    3. \".array[5].string\"      : hello world!\n"
        "    4. \".com\\.xxx\\.xxx\"        : hello world\n"
        "    5. \".macro\"                : 3.1415926\n"
        "    6. \".macro2\"               : hello world\n"
        "    7. \".macro3\"               : 3.1415926\n"
        "    8. \".macro4[0]\"            : \"hello world!\"\n"
    }
,   {'h',   "help",         TB_OPTION_MODE_KEY,         TB_OPTION_TYPE_BOOL,        "display this help and exit"}
,   {'-',   "file",         TB_OPTION_MODE_VAL,         TB_OPTION_TYPE_CSTR,        "the json file"             }

};

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_object_jcat_main(tb_int_t argc, tb_char_t** argv)
{
    // init option
    tb_option_ref_t option = tb_option_init("jcat", "cat the json file", g_options);
    if (option)
    {
        // done option
        if (tb_option_done(option, argc - 1, &argv[1]))
        {
            // done file
            if (tb_option_find(option, "file"))
            {
                // load object
                tb_object_ref_t root = tb_object_read_from_url(tb_option_item_cstr(option, "file"));
                if (root)
                {
                    // done filter
                    tb_object_ref_t object = root;
                    if (tb_option_find(option, "filter"))
                        object = tb_object_seek(root, tb_option_item_cstr(option, "filter"), tb_true);

                    // dump object
                    if (object)
                    {
                        // done
                        tb_char_t info[8192] = {0};
                        tb_long_t size = tb_object_writ_to_data(object, (tb_byte_t*)info, sizeof(info), TB_OBJECT_FORMAT_JSON | TB_OBJECT_FORMAT_DEFLATE);
                        if (size > 0)
                        {
                            // strip string: ""
                            tb_char_t* show = info;
                            if (info[0] == '\"' && info[size - 1] == '\"')
                            {
                                show++;
                                info[size - 1] = '\0';
                            }

                            // trace
                            tb_printf("%s\n", show);
                        }
                    }

                    // exit object
                    tb_object_exit(root);
                }
            }
            else tb_option_help(option);
        }
        else tb_option_help(option);

        // exit option
        tb_option_exit(option);
    }

    // ok
    return 0;
}

