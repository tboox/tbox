/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"
#include <string.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * check
 */
static tb_bool_t check_memset_u8(tb_byte_t* dst, tb_uint8_t src, tb_size_t size)
{
    tb_uint8_t* p = (tb_uint8_t*)dst;
    tb_uint8_t* e = p + size;
    while (p < e)
    {
        if (*p != src) return tb_false;
        p++;
    }

    return tb_true;
}
static tb_bool_t check_memset_u16(tb_byte_t* dst, tb_uint16_t src, tb_size_t size)
{
    tb_uint16_t* p = (tb_uint16_t*)dst;
    tb_uint16_t* e = p + size;
    while (p < e)
    {
        if (*p != src) return tb_false;
        p++;
    }

    return tb_true;
}
static tb_bool_t check_memset_u24(tb_byte_t* dst, tb_uint32_t src, tb_size_t size)
{
    tb_byte_t* p = (tb_byte_t*)dst;
    tb_byte_t* e = p + (size * 3);
    while (p < e)
    {
        if ((*((tb_uint32_t*)p) & 0xffffff) != (src & 0xffffff)) return tb_false;
        p += 3;
    }

    return tb_true;
}
static tb_bool_t check_memset_u32(tb_byte_t* dst, tb_uint32_t src, tb_size_t size)
{
    tb_uint32_t* p = (tb_uint32_t*)dst;
    tb_uint32_t* e = p + size;
    while (p < e)
    {
        if (*p != src) return tb_false;
        p++;
    }

    return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_memory_memops_main(tb_int_t argc, tb_char_t** argv)
{
    // init
    __tb_volatile__ tb_size_t i = 0;
    __tb_volatile__ tb_hong_t dt = 0;

    tb_size_t   size = 15 * 1024 * 1024;
    tb_byte_t*  data = tb_malloc_bytes(size);

    tb_size_t   size2 = 15 * 1024 * 1024;
    tb_byte_t*  data2 = tb_malloc_bytes(size2);

    // test: u8 x 1024
    memset(data, 0, size);

    dt = tb_mclock();
    for (i = 0; i < 1000000; i++) tb_memset(data, 0xbe, 1024 + 3);
    dt = tb_mclock() - dt;
    tb_printf("memset[1k]: %lld ms\n", dt);
    if (!check_memset_u8(data, 0xbe, 1024 + 3)) tb_printf("check failed\n");

    // test: u8 x 1024 * 1024
    memset(data, 0, size);

    dt = tb_mclock();
    for (i = 0; i < 1000; i++) tb_memset(data, 0xbe, 1024 * 1024 + 3);
    dt = tb_mclock() - dt;
    tb_printf("memset[1m]: %lld ms\n", dt);
    if (!check_memset_u8(data, 0xbe, 1024 * 1024 + 3)) tb_printf("check failed\n");

    // test: u8 x 1024
    memset(data2, 0, size2);

    dt = tb_mclock();
    for (i = 0; i < 1000000; i++) tb_memcpy(data2, data, 1024 + 3);
    dt = tb_mclock() - dt;
    tb_printf("memcpy[1k]: %lld ms\n", dt);
    if (!check_memset_u8(data2, 0xbe, 1024 + 3)) tb_printf("check failed\n");

    // test: u8 x 1024 * 1024
    memset(data2, 0, size2);

    dt = tb_mclock();
    for (i = 0; i < 1000; i++) tb_memcpy(data2, data, 1024 * 1024 + 3);
    dt = tb_mclock() - dt;
    tb_printf("memcpy[1m]: %lld ms\n", dt);
    if (!check_memset_u8(data2, 0xbe, 1024 * 1024 + 3)) tb_printf("check failed\n");

    // test: u8 x 1024
    dt = tb_mclock();
    for (i = 0; i < 1000000; i++) tb_memmov(data + 1024 + 3, data, 1024 + 3);
    dt = tb_mclock() - dt;
    tb_printf("memmov[1k]: %lld ms\n", dt);
    if (!check_memset_u8(data + 1024 + 3, 0xbe, 1024 + 3)) tb_printf("check failed\n");

    // test: u8 x 1024 * 1024
    dt = tb_mclock();
    for (i = 0; i < 1000; i++) tb_memmov(data + 1024 * 1024 + 3, data, 1024 * 1024 + 3);
    dt = tb_mclock() - dt;
    tb_printf("memmov[1m]: %lld ms\n", dt);
    if (!check_memset_u8(data + 1024 * 1024 + 3, 0xbe, 1024 * 1024 + 3)) tb_printf("check failed\n");

    // test: u16 x 1024
    memset(data, 0, size);

    dt = tb_mclock();
    for (i = 0; i < 1000000; i++) tb_memset_u16(data, 0xbeef, 1024 + 3);
    dt = tb_mclock() - dt;
    tb_printf("memset_u16[1k]: %lld ms\n", dt);
    if (!check_memset_u16(data, 0xbeef, 1024 + 3)) tb_printf("check failed\n");

    // test: u16 x 1024 * 1024
    memset(data, 0, size);

    dt = tb_mclock();
    for (i = 0; i < 1000; i++) tb_memset_u16(data, 0xbeef, 1024 * 1024 + 3);
    dt = tb_mclock() - dt;
    tb_printf("memset_u16[1m]: %lld ms\n", dt);
    if (!check_memset_u16(data, 0xbeef, 1024 * 1024 + 3)) tb_printf("check failed\n");

    // test: u24 x 1024
    memset(data, 0, size);

    dt = tb_mclock();
    for (i = 0; i < 1000000; i++) tb_memset_u24(data, 0xbeefaa, 1024 + 3);
    dt = tb_mclock() - dt;
    tb_printf("memset_u24[1k]: %lld ms\n", dt);
    if (!check_memset_u24(data, 0xbeefaa, 1024 + 3)) tb_printf("check failed\n");

    // test: u24 x 1024 * 1024
    memset(data, 0, size);

    dt = tb_mclock();
    for (i = 0; i < 1000; i++) tb_memset_u24(data, 0xbeefaa, 1024 * 1024 + 3);
    dt = tb_mclock() - dt;
    tb_printf("memset_u24[1m]: %lld ms\n", dt);
    if (!check_memset_u24(data, 0xbeefaa, 1024 * 1024 + 3)) tb_printf("check failed\n");

    // test: u32 x 1024
    memset(data, 0, size);

    dt = tb_mclock();
    for (i = 0; i < 1000000; i++) tb_memset_u32(data, 0xbeefbeaf, 1024 + 3);
    dt = tb_mclock() - dt;
    tb_printf("memset_u32[1k]: %lld ms\n", dt);
    if (!check_memset_u32(data, 0xbeefbeaf, 1024 + 3)) tb_printf("check failed\n");

    // test: u32 x 1024 * 1024
    memset(data, 0, size);

    dt = tb_mclock();
    for (i = 0; i < 1000; i++) tb_memset_u32(data, 0xbeefbeaf, 1024 * 1024 + 3);
    dt = tb_mclock() - dt;
    tb_printf("memset_u32[1m]: %lld ms\n", dt);
    if (!check_memset_u32(data, 0xbeefbeaf, 1024 * 1024 + 3)) tb_printf("check failed\n");


    return 0;
}
