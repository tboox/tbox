/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_container_hash_set_main(tb_int_t argc, tb_char_t** argv)
{
    // init hash
    tb_hash_set_ref_t hash = tb_hash_set_init(8, tb_element_str(tb_true));
    if (hash)
    {
        tb_hash_set_insert(hash, "1");
        tb_hash_set_insert(hash, "9");
        tb_hash_set_insert(hash, "4");
        tb_hash_set_insert(hash, "6");
        tb_hash_set_insert(hash, "8");
        tb_hash_set_insert(hash, "2");
        tb_hash_set_insert(hash, "3");
        tb_hash_set_insert(hash, "5");
        tb_hash_set_insert(hash, "7");
        tb_for_all_if (tb_char_t const*, s, hash, s)
        {
            tb_trace_i("%s", s);
        }
        tb_hash_set_exit(hash);
    }
    return 0;
}
