/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the hash32 entry type
typedef struct __tb_demo_hash32_entry_t
{
    // the hash name
    tb_char_t const*        name;

    // the hash function
    tb_uint32_t             (*hash)(tb_char_t const* cstr, tb_uint32_t seed);

}tb_demo_hash32_entry_t, *tb_demo_hash32_entry_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * wrapers
 */
static tb_uint32_t tb_demo_djb2_make_from_cstr(tb_char_t const* cstr, tb_uint32_t seed)
{
    return (tb_uint32_t)tb_djb2_make_from_cstr(cstr, seed);
}
static tb_uint32_t tb_demo_crc32_make_from_cstr(tb_char_t const* cstr, tb_uint32_t seed)
{
    return tb_crc_make_from_cstr(TB_CRC_MODE_32_IEEE_LE, cstr, seed);
}
static tb_uint32_t tb_demo_bkdr_make_from_cstr(tb_char_t const* cstr, tb_uint32_t seed)
{
    return (tb_uint32_t)tb_bkdr_make_from_cstr(cstr, seed);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */
static tb_demo_hash32_entry_t g_hash32_entries[] =
{
    { "fnv32",      tb_fnv32_make_from_cstr         }
,   { "djb2",       tb_demo_djb2_make_from_cstr     }
,   { "adler32",    tb_adler32_make_from_cstr       }
,   { "crc32",      tb_demo_crc32_make_from_cstr    }
,   { "bkdr",       tb_demo_bkdr_make_from_cstr     }
,   { tb_null,      tb_null}
};

/* //////////////////////////////////////////////////////////////////////////////////////
 * test
 */
static tb_void_t tb_demo_hash32_test(tb_char_t const* cstr)
{
    // done
    tb_demo_hash32_entry_ref_t entry = g_hash32_entries;
    for (; entry && entry->name; entry++)
    {
        __tb_volatile__ tb_uint32_t v = 0;
        __tb_volatile__ tb_uint32_t n = 10000000;
        __tb_volatile__ tb_hong_t   t = tb_mclock();
        while (n--)
        {
            v = entry->hash(cstr, n);
        }
        t = tb_mclock() - t;

        // trace
        tb_trace_i("[hash]: %s: %#08x %ld ms", entry->name, v, t);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_hash_benchmark_main(tb_int_t argc, tb_char_t** argv)
{
    tb_demo_hash32_test(argv[1]);
    return 0;
}
