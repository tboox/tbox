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
    tb_uint32_t             (*hash)(tb_byte_t const* data, tb_size_t size, tb_uint32_t seed);

}tb_demo_hash32_entry_t, *tb_demo_hash32_entry_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * wrapers
 */
static tb_uint32_t tb_demo_rs_make(tb_byte_t const* data, tb_size_t size, tb_uint32_t seed)
{
    return (tb_uint32_t)tb_rs_make(data, size, seed);
}
static tb_uint32_t tb_demo_ap_make(tb_byte_t const* data, tb_size_t size, tb_uint32_t seed)
{
    return (tb_uint32_t)tb_ap_make(data, size, seed);
}
static tb_uint32_t tb_demo_djb2_make(tb_byte_t const* data, tb_size_t size, tb_uint32_t seed)
{
    return (tb_uint32_t)tb_djb2_make(data, size, seed);
}
static tb_uint32_t tb_demo_sdbm_make(tb_byte_t const* data, tb_size_t size, tb_uint32_t seed)
{
    return (tb_uint32_t)tb_sdbm_make(data, size, seed);
}
static tb_uint32_t tb_demo_bkdr_make(tb_byte_t const* data, tb_size_t size, tb_uint32_t seed)
{
    return (tb_uint32_t)tb_bkdr_make(data, size, seed);
}
static tb_uint32_t tb_demo_murmur_make(tb_byte_t const* data, tb_size_t size, tb_uint32_t seed)
{
    return (tb_uint32_t)tb_murmur_make(data, size, seed);
}
static tb_uint32_t tb_demo_blizzard_make(tb_byte_t const* data, tb_size_t size, tb_uint32_t seed)
{
    return (tb_uint32_t)tb_blizzard_make(data, size, seed);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */
static tb_demo_hash32_entry_t g_hash32_entries[] =
{
    { "fnv32   ",   tb_fnv32_make           }
,   { "fnv32-1a",   tb_fnv32_1a_make        }
,   { "rs      ",   tb_demo_rs_make         }
,   { "ap      ",   tb_demo_ap_make         }
,   { "djb2    ",   tb_demo_djb2_make       }
,   { "sdbm    ",   tb_demo_sdbm_make       }
,   { "adler32 ",   tb_adler32_make         }
,   { "crc32   ",   tb_crc32_make           }
,   { "crc32-le",   tb_crc32_le_make        }
,   { "bkdr    ",   tb_demo_bkdr_make       }
,   { "murmur  ",   tb_demo_murmur_make     }
,   { "blizzard",   tb_demo_blizzard_make   }
,   { tb_null,      tb_null                 }
};

/* //////////////////////////////////////////////////////////////////////////////////////
 * test
 */
static tb_void_t tb_demo_hash32_test()
{
    // init data
    tb_size_t   size = 1024 * 1024;
    tb_byte_t*  data = tb_malloc_bytes(size);
    tb_assert_and_check_return(data);

    // make data
    tb_size_t i = 0;
    for (i = 0; i < size; i++) data[i] = (tb_byte_t)tb_random_range(0, 0xff);

    // done (1M)
    tb_demo_hash32_entry_ref_t entry = g_hash32_entries;
    for (; entry && entry->name; entry++)
    {
        __tb_volatile__ tb_uint32_t v = 0;
        __tb_volatile__ tb_uint32_t n = 1000000;
        __tb_volatile__ tb_hong_t   t = tb_mclock();
        while (n--)
        {
            v = entry->hash(data, 1024, n);
        }
        t = tb_mclock() - t;

        // trace
        tb_trace_i("[hash(1K)]: %s: %08x %ld ms", entry->name, v, t);
    }

    // trace
    tb_trace_i("");

    // done (1M)
    entry = g_hash32_entries;
    for (; entry && entry->name; entry++)
    {
        __tb_volatile__ tb_uint32_t v = 0;
        __tb_volatile__ tb_uint32_t n = 1000;
        __tb_volatile__ tb_hong_t   t = tb_mclock();
        while (n--)
        {
            v = entry->hash(data, size, n);
        }
        t = tb_mclock() - t;

        // trace
        tb_trace_i("[hash(1M)]: %s: %08x %ld ms", entry->name, v, t);
    }

    // exit data
    tb_free(data);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_hash_benchmark_main(tb_int_t argc, tb_char_t** argv)
{
    tb_demo_hash32_test();
    return 0;
}
