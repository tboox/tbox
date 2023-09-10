/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_object_writer_main(tb_int_t argc, tb_char_t** argv)
{
    /* init a dictionary object
     *
     * {"key1": "hello", "key2" :"world", "key3": 12345, "key4": true, "key5": ["1", "2", []]}
     */
    tb_object_ref_t dict = tb_oc_dictionary_init(0, tb_false);
    if (dict)
    {
        // key1 => hello
        tb_oc_dictionary_insert(dict, "key1", tb_oc_string_init_from_cstr("hello"));

        // key2 => world
        tb_oc_dictionary_insert(dict, "key2", tb_oc_string_init_from_cstr("world"));

        // key3 => 12345
        tb_oc_dictionary_insert(dict, "key3", tb_oc_number_init_from_sint32(12345));

        // key4 => true
        tb_oc_dictionary_insert(dict, "key4", tb_oc_boolean_true());

        // key5 => ["1", "2"]
        tb_object_ref_t array = tb_oc_array_init(0, tb_false);
        if (array)
        {
            tb_oc_array_append(array, tb_oc_string_init_from_cstr("1"));
            tb_oc_array_append(array, tb_oc_string_init_from_cstr("2"));
            tb_oc_array_append(array, tb_oc_array_init(0, tb_false));
            tb_oc_dictionary_insert(dict, "key5", array);
        }

        // key6 => "hello world"\n
        tb_oc_dictionary_insert(dict, "key6", tb_oc_string_init_from_cstr("hello\nworld\tusa\"china\\india"));

        // write dictionary to json file
        tb_object_writ_to_url(dict, argv[1], TB_OBJECT_FORMAT_JSON/* | TB_OBJECT_FORMAT_DEFLATE*/);

        // exit dictionary
        tb_object_exit(dict);
    }
    return 0;
}

