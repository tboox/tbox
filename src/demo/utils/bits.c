/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * test
 */
static tb_void_t tb_test_bits_swap_u16()
{
    __tb_volatile__ tb_uint16_t x = 0x1234;
    __tb_volatile__ tb_size_t   n = 100000001;
    __tb_volatile__ tb_hong_t   dt = tb_mclock();

    while (n--) x = tb_bits_swap_u16(x);

    dt = tb_mclock() - dt;
    tb_trace_i("[bitops]: swap_u16: %x => %x, %llu ms", 0x1234, x, dt);
}
static tb_void_t tb_test_bits_swap_u32()
{
    __tb_volatile__ tb_uint32_t x = 0x12345678;
    __tb_volatile__ tb_size_t   n = 100000001;
    __tb_volatile__ tb_hong_t   dt = tb_mclock();

    while (n--) x = tb_bits_swap_u32(x);

    dt = tb_mclock() - dt;
    tb_trace_i("[bitops]: swap_u32: %x => %x, %llu ms", 0x12345678, x, dt);
}
static tb_void_t tb_test_bits_swap_u64()
{
    __tb_volatile__ tb_hize_t   x = 0x12345678;
    __tb_volatile__ tb_size_t   n = 100000001;
    __tb_volatile__ tb_hong_t   dt = tb_mclock();

    while (n--) x = tb_bits_swap_u64(x);

    dt = tb_mclock() - dt;
    tb_trace_i("[bitops]: swap_u64: %x => %llx, %llu ms", 0x12345678, x, dt);
}
static tb_void_t tb_test_bits_ubits32(tb_uint32_t x)
{
    __tb_volatile__ tb_size_t   b = 0;
    __tb_volatile__ tb_size_t   n = 0;
    __tb_volatile__ tb_hong_t   ts = 0;
    __tb_volatile__ tb_hong_t   tg = 0;
    for (b = 0; b < 8; ++b)
    {
        tb_trace_i("[bitops]: ubits32 b: %d x: %x", b, x);
        for (n = 0; n <= 32; ++n)
        {
            tb_byte_t                   p[256] = {0};
            __tb_volatile__ tb_size_t   n1 = 100000;
            __tb_volatile__ tb_size_t   n2 = 100000;
            __tb_volatile__ tb_uint32_t y = 0;
            __tb_volatile__ tb_hong_t   t1, t2;
            __tb_volatile__ tb_uint32_t xx = n < 32? (x & ((tb_uint32_t)(1 << n) - 1)) : x;

            t1 = tb_mclock();
            while (n1--) tb_bits_set_ubits32(p, b, x, n);
            t1 = tb_mclock() - t1;
            ts += t1;

            t2 = tb_mclock();
            while (n2--) y = tb_bits_get_ubits32(p, b, n);
            t2 = tb_mclock() - t2;
            tg += t1;

            if (xx != y)
            {
                tb_trace_i("[bitops]: ubits32 set: b: %x, n: %x, x: %x, %llu ms", b, n, xx, t1);
                tb_trace_i("[bitops]: ubits32 get: b: %x, n: %x, y: %x, %llu ms", b, n, y, t2);
            }
        }
    }
    tb_trace_i("[bitops]: ubits32 set: %llu ms, get: %llu ms", ts, tg);
}
static tb_void_t tb_test_bits_sbits32(tb_sint32_t x)
{
    __tb_volatile__ tb_size_t   b = 0;
    __tb_volatile__ tb_size_t   n = 0;
    __tb_volatile__ tb_hong_t   ts = 0;
    __tb_volatile__ tb_hong_t   tg = 0;
    for (b = 0; b < 8; ++b)
    {
        tb_trace_i("[bitops]: sbits32 b: %d x: %d", b, x);
        for (n = 2; n <= 32; ++n)
        {
            tb_byte_t                   p[256] = {0};
            __tb_volatile__ tb_size_t   n1 = 100000;
            __tb_volatile__ tb_size_t   n2 = 100000;
            __tb_volatile__ tb_sint32_t y = 0;
            __tb_volatile__ tb_hong_t   t1, t2;
            __tb_volatile__ tb_sint32_t xx = ((x >> 31) << (n - 1)) | (x & ((tb_uint32_t)(1 << (n - 1)) - 1));

            t1 = tb_mclock();
            while (n1--) tb_bits_set_sbits32(p, b, x, n);
            t1 = tb_mclock() - t1;
            ts += t1;

            t2 = tb_mclock();
            while (n2--) y = tb_bits_get_sbits32(p, b, n);
            t2 = tb_mclock() - t2;
            tg += t1;

            if (xx != y)
            {
                tb_trace_i("[bitops]: sbits32 set: b: %d, n: %u, x: %d, %llu ms", b, n, xx, t1);
                tb_trace_i("[bitops]: sbits32 get: b: %u, n: %u, y: %d, %llu ms", b, n, y, t2);
            }
        }
    }
    tb_trace_i("[bitops]: sbits32 set: %llu ms, get: %llu ms", ts, tg);
}
static tb_void_t tb_test_bits_u32_be(tb_uint32_t x)
{
    tb_byte_t                   p[8] = {0};
    __tb_volatile__ tb_uint32_t y = 0;
    __tb_volatile__ tb_size_t   n1 = 100000;
    __tb_volatile__ tb_size_t   n2 = 100000;
    __tb_volatile__ tb_hong_t   ts = 0;
    __tb_volatile__ tb_hong_t   tg = 0;
    __tb_volatile__ tb_hong_t   t1, t2;

    t1 = tb_mclock();
    while (n1--) tb_bits_set_u32_be(p, x);
    t1 = tb_mclock() - t1;
    ts += t1;

    t2 = tb_mclock();
    while (n2--) y = tb_bits_get_u32_be(p);
    t2 = tb_mclock() - t2;
    tg += t1;

    tb_trace_i("[bitops]: u32_be set: x: %x, %llu ms, get: y: %x, %llu ms", x, ts, y, tg);
}
static tb_void_t tb_test_bits_u32_le(tb_uint32_t x)
{
    tb_byte_t                   p[8] = {0};
    __tb_volatile__ tb_uint32_t y = 0;
    __tb_volatile__ tb_size_t   n1 = 100000;
    __tb_volatile__ tb_size_t   n2 = 100000;
    __tb_volatile__ tb_hong_t   ts = 0;
    __tb_volatile__ tb_hong_t   tg = 0;
    __tb_volatile__ tb_hong_t   t1, t2;

    t1 = tb_mclock();
    while (n1--) tb_bits_set_u32_le(p, x);
    t1 = tb_mclock() - t1;
    ts += t1;

    t2 = tb_mclock();
    while (n2--) y = tb_bits_get_u32_le(p);
    t2 = tb_mclock() - t2;
    tg += t1;

    tb_trace_i("[bitops]: u32_le set: x: %x, %llu ms, get: y: %x, %llu ms", x, ts, y, tg);
}
static tb_void_t tb_test_bits_u24_be(tb_uint32_t x)
{
    tb_byte_t                   p[8] = {0};
    __tb_volatile__ tb_uint32_t y = 0;
    __tb_volatile__ tb_size_t   n1 = 100000;
    __tb_volatile__ tb_size_t   n2 = 100000;
    __tb_volatile__ tb_hong_t   ts = 0;
    __tb_volatile__ tb_hong_t   tg = 0;
    __tb_volatile__ tb_hong_t   t1, t2;

    t1 = tb_mclock();
    while (n1--) tb_bits_set_u24_be(p, x);
    t1 = tb_mclock() - t1;
    ts += t1;

    t2 = tb_mclock();
    while (n2--) y = tb_bits_get_u24_be(p);
    t2 = tb_mclock() - t2;
    tg += t1;

    tb_trace_i("[bitops]: u24_be set: x: %x, %llu ms, get: y: %x, %llu ms", x, ts, y, tg);
}
static tb_void_t tb_test_bits_u24_le(tb_uint32_t x)
{
    tb_byte_t                   p[8] = {0};
    __tb_volatile__ tb_uint32_t y = 0;
    __tb_volatile__ tb_size_t   n1 = 100000;
    __tb_volatile__ tb_size_t   n2 = 100000;
    __tb_volatile__ tb_hong_t   ts = 0;
    __tb_volatile__ tb_hong_t   tg = 0;
    __tb_volatile__ tb_hong_t   t1, t2;

    t1 = tb_mclock();
    while (n1--) tb_bits_set_u24_le(p, x);
    t1 = tb_mclock() - t1;
    ts += t1;

    t2 = tb_mclock();
    while (n2--) y = tb_bits_get_u24_le(p);
    t2 = tb_mclock() - t2;
    tg += t1;

    tb_trace_i("[bitops]: u24_le set: x: %x, %llu ms, get: y: %x, %llu ms", x, ts, y, tg);
}
static tb_void_t tb_test_bits_u16_be(tb_uint16_t x)
{
    tb_byte_t                   p[8] = {0};
    __tb_volatile__ tb_uint16_t y = 0;
    __tb_volatile__ tb_size_t   n1 = 100000;
    __tb_volatile__ tb_size_t   n2 = 100000;
    __tb_volatile__ tb_hong_t   ts = 0;
    __tb_volatile__ tb_hong_t   tg = 0;
    __tb_volatile__ tb_hong_t   t1, t2;

    t1 = tb_mclock();
    while (n1--) tb_bits_set_u16_be(p, x);
    t1 = tb_mclock() - t1;
    ts += t1;

    t2 = tb_mclock();
    while (n2--) y = tb_bits_get_u16_be(p);
    t2 = tb_mclock() - t2;
    tg += t1;

    tb_trace_i("[bitops]: u16_be set: x: %x, %llu ms, get: y: %x, %llu ms", x, ts, y, tg);
}
static tb_void_t tb_test_bits_u16_le(tb_uint16_t x)
{
    tb_byte_t                   p[8] = {0};
    __tb_volatile__ tb_uint16_t y = 0;
    __tb_volatile__ tb_size_t   n1 = 100000;
    __tb_volatile__ tb_size_t   n2 = 100000;
    __tb_volatile__ tb_hong_t   ts = 0;
    __tb_volatile__ tb_hong_t   tg = 0;
    __tb_volatile__ tb_hong_t   t1, t2;

    t1 = tb_mclock();
    while (n1--) tb_bits_set_u16_le(p, x);
    t1 = tb_mclock() - t1;
    ts += t1;

    t2 = tb_mclock();
    while (n2--) y = tb_bits_get_u16_le(p);
    t2 = tb_mclock() - t2;
    tg += t1;

    tb_trace_i("[bitops]: u16_le set: x: %x, %llu ms, get: y: %x, %llu ms", x, ts, y, tg);
}

static tb_void_t tb_test_bits_s32_be(tb_sint32_t x)
{
    tb_byte_t                   p[8] = {0};
    __tb_volatile__ tb_sint32_t y = 0;
    __tb_volatile__ tb_size_t   n1 = 100000;
    __tb_volatile__ tb_size_t   n2 = 100000;
    __tb_volatile__ tb_hong_t   ts = 0;
    __tb_volatile__ tb_hong_t   tg = 0;
    __tb_volatile__ tb_hong_t   t1, t2;

    t1 = tb_mclock();
    while (n1--) tb_bits_set_s32_be(p, x);
    t1 = tb_mclock() - t1;
    ts += t1;

    t2 = tb_mclock();
    while (n2--) y = tb_bits_get_s32_be(p);
    t2 = tb_mclock() - t2;
    tg += t1;

    tb_trace_i("[bitops]: s32_be set: x: %d, %llu ms, get: y: %d, %llu ms", x, ts, y, tg);
}
static tb_void_t tb_test_bits_s32_le(tb_sint32_t x)
{
    tb_byte_t                   p[8] = {0};
    __tb_volatile__ tb_sint32_t y = 0;
    __tb_volatile__ tb_size_t   n1 = 100000;
    __tb_volatile__ tb_size_t   n2 = 100000;
    __tb_volatile__ tb_hong_t   ts = 0;
    __tb_volatile__ tb_hong_t   tg = 0;
    __tb_volatile__ tb_hong_t   t1, t2;

    t1 = tb_mclock();
    while (n1--) tb_bits_set_s32_le(p, x);
    t1 = tb_mclock() - t1;
    ts += t1;

    t2 = tb_mclock();
    while (n2--) y = tb_bits_get_s32_le(p);
    t2 = tb_mclock() - t2;
    tg += t1;

    tb_trace_i("[bitops]: s32_le set: x: %d, %llu ms, get: y: %d, %llu ms", x, ts, y, tg);
}
static tb_void_t tb_test_bits_s24_be(tb_sint32_t x)
{
    tb_byte_t                   p[8] = {0};
    __tb_volatile__ tb_sint32_t y = 0;
    __tb_volatile__ tb_size_t   n1 = 100000;
    __tb_volatile__ tb_size_t   n2 = 100000;
    __tb_volatile__ tb_hong_t   ts = 0;
    __tb_volatile__ tb_hong_t   tg = 0;
    __tb_volatile__ tb_hong_t   t1, t2;

    t1 = tb_mclock();
    while (n1--) tb_bits_set_s24_be(p, x);
    t1 = tb_mclock() - t1;
    ts += t1;

    t2 = tb_mclock();
    while (n2--) y = tb_bits_get_s24_be(p);
    t2 = tb_mclock() - t2;
    tg += t1;

    tb_trace_i("[bitops]: s24_be set: x: %d, %llu ms, get: y: %d, %llu ms", x, ts, y, tg);
}
static tb_void_t tb_test_bits_s24_le(tb_sint32_t x)
{
    tb_byte_t                   p[8] = {0};
    __tb_volatile__ tb_sint32_t y = 0;
    __tb_volatile__ tb_size_t   n1 = 100000;
    __tb_volatile__ tb_size_t   n2 = 100000;
    __tb_volatile__ tb_hong_t   ts = 0;
    __tb_volatile__ tb_hong_t   tg = 0;
    __tb_volatile__ tb_hong_t   t1, t2;

    t1 = tb_mclock();
    while (n1--) tb_bits_set_s24_le(p, x);
    t1 = tb_mclock() - t1;
    ts += t1;

    t2 = tb_mclock();
    while (n2--) y = tb_bits_get_s24_le(p);
    t2 = tb_mclock() - t2;
    tg += t1;

    tb_trace_i("[bitops]: s24_le set: x: %d, %llu ms, get: y: %d, %llu ms", x, ts, y, tg);
}
static tb_void_t tb_test_bits_s16_be(tb_sint16_t x)
{
    tb_byte_t                   p[8] = {0};
    __tb_volatile__ tb_sint16_t y = 0;
    __tb_volatile__ tb_size_t   n1 = 100000;
    __tb_volatile__ tb_size_t   n2 = 100000;
    __tb_volatile__ tb_hong_t   ts = 0;
    __tb_volatile__ tb_hong_t   tg = 0;
    __tb_volatile__ tb_hong_t   t1, t2;

    t1 = tb_mclock();
    while (n1--) tb_bits_set_s16_be(p, x);
    t1 = tb_mclock() - t1;
    ts += t1;

    t2 = tb_mclock();
    while (n2--) y = tb_bits_get_s16_be(p);
    t2 = tb_mclock() - t2;
    tg += t1;

    tb_trace_i("[bitops]: s16_be set: x: %d, %llu ms, get: y: %d, %llu ms", x, ts, y, tg);
}
static tb_void_t tb_test_bits_s16_le(tb_sint16_t x)
{
    tb_byte_t                   p[8] = {0};
    __tb_volatile__ tb_sint16_t y = 0;
    __tb_volatile__ tb_size_t   n1 = 100000;
    __tb_volatile__ tb_size_t   n2 = 100000;
    __tb_volatile__ tb_hong_t   ts = 0;
    __tb_volatile__ tb_hong_t   tg = 0;
    __tb_volatile__ tb_hong_t   t1, t2;

    t1 = tb_mclock();
    while (n1--) tb_bits_set_s16_le(p, x);
    t1 = tb_mclock() - t1;
    ts += t1;

    t2 = tb_mclock();
    while (n2--) y = tb_bits_get_s16_le(p);
    t2 = tb_mclock() - t2;
    tg += t1;

    tb_trace_i("[bitops]: s16_le set: x: %d, %llu ms, get: y: %d, %llu ms", x, ts, y, tg);
}
#ifdef TB_CONFIG_TYPE_HAVE_FLOAT
static tb_void_t tb_test_bits_double_bbe(tb_double_t x)
{
    tb_byte_t                   p[8] = {0};
    __tb_volatile__ tb_double_t y = 0;
    __tb_volatile__ tb_size_t   n1 = 100000;
    __tb_volatile__ tb_size_t   n2 = 100000;
    __tb_volatile__ tb_hong_t   ts = 0;
    __tb_volatile__ tb_hong_t   tg = 0;
    __tb_volatile__ tb_hong_t   t1, t2;

    t1 = tb_mclock();
    while (n1--) tb_bits_set_double_bbe(p, x);
    t1 = tb_mclock() - t1;
    ts += t1;

    t2 = tb_mclock();
    while (n2--) y = tb_bits_get_double_bbe(p);
    t2 = tb_mclock() - t2;
    tg += t1;

    tb_trace_i("[bitops]: double_bbe set: x: %lf, %llu ms, get: y: %lf, %llu ms", x, ts, y, tg);
}
static tb_void_t tb_test_bits_double_ble(tb_double_t x)
{
    tb_byte_t                   p[8] = {0};
    __tb_volatile__ tb_double_t y = 0;
    __tb_volatile__ tb_size_t   n1 = 100000;
    __tb_volatile__ tb_size_t   n2 = 100000;
    __tb_volatile__ tb_hong_t   ts = 0;
    __tb_volatile__ tb_hong_t   tg = 0;
    __tb_volatile__ tb_hong_t   t1, t2;

    t1 = tb_mclock();
    while (n1--) tb_bits_set_double_ble(p, x);
    t1 = tb_mclock() - t1;
    ts += t1;

    t2 = tb_mclock();
    while (n2--) y = tb_bits_get_double_ble(p);
    t2 = tb_mclock() - t2;
    tg += t1;

    tb_trace_i("[bitops]: double_ble set: x: %lf, %llu ms, get: y: %lf, %llu ms", x, ts, y, tg);
}
static tb_void_t tb_test_bits_double_lbe(tb_double_t x)
{
    tb_byte_t                   p[8] = {0};
    __tb_volatile__ tb_double_t y = 0;
    __tb_volatile__ tb_size_t   n1 = 100000;
    __tb_volatile__ tb_size_t   n2 = 100000;
    __tb_volatile__ tb_hong_t   ts = 0;
    __tb_volatile__ tb_hong_t   tg = 0;
    __tb_volatile__ tb_hong_t   t1, t2;

    t1 = tb_mclock();
    while (n1--) tb_bits_set_double_lbe(p, x);
    t1 = tb_mclock() - t1;
    ts += t1;

    t2 = tb_mclock();
    while (n2--) y = tb_bits_get_double_lbe(p);
    t2 = tb_mclock() - t2;
    tg += t1;

    tb_trace_i("[bitops]: double_lbe set: x: %lf, %llu ms, get: y: %lf, %llu ms", x, ts, y, tg);
}
static tb_void_t tb_test_bits_double_lle(tb_double_t x)
{
    tb_byte_t                   p[8] = {0};
    __tb_volatile__ tb_double_t y = 0;
    __tb_volatile__ tb_size_t   n1 = 100000;
    __tb_volatile__ tb_size_t   n2 = 100000;
    __tb_volatile__ tb_hong_t   ts = 0;
    __tb_volatile__ tb_hong_t   tg = 0;
    __tb_volatile__ tb_hong_t   t1, t2;

    t1 = tb_mclock();
    while (n1--) tb_bits_set_double_lle(p, x);
    t1 = tb_mclock() - t1;
    ts += t1;

    t2 = tb_mclock();
    while (n2--) y = tb_bits_get_double_lle(p);
    t2 = tb_mclock() - t2;
    tg += t1;

    tb_trace_i("[bitops]: double_lle set: x: %lf, %llu ms, get: y: %lf, %llu ms", x, ts, y, tg);
}
static tb_void_t tb_test_bits_float_be(tb_float_t x)
{
    tb_byte_t                   p[8] = {0};
    __tb_volatile__ tb_float_t  y = 0;
    __tb_volatile__ tb_size_t   n1 = 100000;
    __tb_volatile__ tb_size_t   n2 = 100000;
    __tb_volatile__ tb_hong_t   ts = 0;
    __tb_volatile__ tb_hong_t   tg = 0;
    __tb_volatile__ tb_hong_t   t1, t2;

    t1 = tb_mclock();
    while (n1--) tb_bits_set_float_be(p, x);
    t1 = tb_mclock() - t1;
    ts += t1;

    t2 = tb_mclock();
    while (n2--) y = tb_bits_get_float_be(p);
    t2 = tb_mclock() - t2;
    tg += t1;

    tb_trace_i("[bitops]: float_be set: x: %f, %llu ms, get: y: %f, %llu ms", x, ts, y, tg);
}
static tb_void_t tb_test_bits_float_le(tb_float_t x)
{
    tb_byte_t                   p[8] = {0};
    __tb_volatile__ tb_float_t  y = 0;
    __tb_volatile__ tb_size_t   n1 = 100000;
    __tb_volatile__ tb_size_t   n2 = 100000;
    __tb_volatile__ tb_hong_t   ts = 0;
    __tb_volatile__ tb_hong_t   tg = 0;
    __tb_volatile__ tb_hong_t   t1, t2;

    t1 = tb_mclock();
    while (n1--) tb_bits_set_float_le(p, x);
    t1 = tb_mclock() - t1;
    ts += t1;

    t2 = tb_mclock();
    while (n2--) y = tb_bits_get_float_le(p);
    t2 = tb_mclock() - t2;
    tg += t1;

    tb_trace_i("[bitops]: float_le set: x: %f, %llu ms, get: y: %f, %llu ms", x, ts, y, tg);
}
#endif
static tb_void_t tb_test_bits_cl0_u32_be(tb_uint32_t x)
{
    __tb_volatile__ tb_size_t   i = 0;
    __tb_volatile__ tb_size_t   n = 100000;
    __tb_volatile__ tb_hong_t t = tb_mclock();
    while (n--) i = tb_bits_cl0_u32_be(x);
    t = tb_mclock() - t;

    tb_trace_i("[bitops]: u32_be: cl0: %032lb => %lu, %lld ms", x, i, t);
}
static tb_void_t tb_test_bits_cl0_u32_le(tb_uint32_t x)
{
    __tb_volatile__ tb_size_t   i = 0;
    __tb_volatile__ tb_size_t   n = 100000;
    __tb_volatile__ tb_hong_t t = tb_mclock();
    while (n--) i = tb_bits_cl0_u32_le(x);
    t = tb_mclock() - t;

    tb_trace_i("[bitops]: u32_le: cl0: %032lb => %lu, %lld ms", x, i, t);
}
static tb_void_t tb_test_bits_cl0_u64_be(tb_uint64_t x)
{
    __tb_volatile__ tb_size_t   i = 0;
    __tb_volatile__ tb_size_t   n = 100000;
    __tb_volatile__ tb_hong_t t = tb_mclock();
    while (n--) i = tb_bits_cl0_u64_be(x);
    t = tb_mclock() - t;

    tb_trace_i("[bitops]: u64_be: cl0: %064llb => %lu, %lld ms", x, i, t);
}
static tb_void_t tb_test_bits_cl0_u64_le(tb_uint64_t x)
{
    __tb_volatile__ tb_size_t   i = 0;
    __tb_volatile__ tb_size_t   n = 100000;
    __tb_volatile__ tb_hong_t t = tb_mclock();
    while (n--) i = tb_bits_cl0_u64_le(x);
    t = tb_mclock() - t;

    tb_trace_i("[bitops]: u64_le: cl0: %064llb => %lu, %lld ms", x, i, t);
}
static tb_void_t tb_test_bits_cl1_u32_be(tb_uint32_t x)
{
    __tb_volatile__ tb_size_t   i = 0;
    __tb_volatile__ tb_size_t   n = 100000;
    __tb_volatile__ tb_hong_t t = tb_mclock();
    while (n--) i = tb_bits_cl1_u32_be(x);
    t = tb_mclock() - t;

    tb_trace_i("[bitops]: u32_be: cl1: %032lb => %lu, %lld ms", x, i, t);
}
static tb_void_t tb_test_bits_cl1_u32_le(tb_uint32_t x)
{
    __tb_volatile__ tb_size_t   i = 0;
    __tb_volatile__ tb_size_t   n = 100000;
    __tb_volatile__ tb_hong_t t = tb_mclock();
    while (n--) i = tb_bits_cl1_u32_le(x);
    t = tb_mclock() - t;

    tb_trace_i("[bitops]: u32_le: cl1: %032lb => %lu, %lld ms", x, i, t);
}
static tb_void_t tb_test_bits_cl1_u64_be(tb_uint64_t x)
{
    __tb_volatile__ tb_size_t   i = 0;
    __tb_volatile__ tb_size_t   n = 100000;
    __tb_volatile__ tb_hong_t t = tb_mclock();
    while (n--) i = tb_bits_cl1_u64_be(x);
    t = tb_mclock() - t;

    tb_trace_i("[bitops]: u64_be: cl1: %064llb => %lu, %lld ms", x, i, t);
}
static tb_void_t tb_test_bits_cl1_u64_le(tb_uint64_t x)
{
    __tb_volatile__ tb_size_t   i = 0;
    __tb_volatile__ tb_size_t   n = 100000;
    __tb_volatile__ tb_hong_t t = tb_mclock();
    while (n--) i = tb_bits_cl1_u64_le(x);
    t = tb_mclock() - t;

    tb_trace_i("[bitops]: u64_le: cl1: %064llb => %lu, %lld ms", x, i, t);
}
static tb_void_t tb_test_bits_cb0_u32(tb_uint32_t x)
{
    __tb_volatile__ tb_size_t   i = 0;
    __tb_volatile__ tb_size_t   n = 100000;
    __tb_volatile__ tb_hong_t t = tb_mclock();
    while (n--) i = tb_bits_cb0_u32(x);
    t = tb_mclock() - t;

    tb_trace_i("[bitops]: u32: cb0: %032lb => %lu, %lld ms", x, i, t);
}
static tb_void_t tb_test_bits_cb0_u64(tb_uint64_t x)
{
    __tb_volatile__ tb_size_t   i = 0;
    __tb_volatile__ tb_size_t   n = 100000;
    __tb_volatile__ tb_hong_t t = tb_mclock();
    while (n--) i = tb_bits_cb0_u64(x);
    t = tb_mclock() - t;

    tb_trace_i("[bitops]: u64: cb0: %064llb => %lu, %lld ms", x, i, t);
}
static tb_void_t tb_test_bits_cb1_u32(tb_uint32_t x)
{
    __tb_volatile__ tb_size_t   i = 0;
    __tb_volatile__ tb_size_t   n = 100000;
    __tb_volatile__ tb_hong_t t = tb_mclock();
    while (n--) i = tb_bits_cb1_u32(x);
    t = tb_mclock() - t;

    tb_trace_i("[bitops]: u32: cb1: %032lb => %lu, %lld ms", x, i, t);
}
static tb_void_t tb_test_bits_cb1_u64(tb_uint64_t x)
{
    __tb_volatile__ tb_size_t   i = 0;
    __tb_volatile__ tb_size_t   n = 100000;
    __tb_volatile__ tb_hong_t t = tb_mclock();
    while (n--) i = tb_bits_cb1_u64(x);
    t = tb_mclock() - t;

    tb_trace_i("[bitops]: u64: cb1: %064llb => %lu, %lld ms", x, i, t);
}
static tb_void_t tb_test_bits_fb0_u32_be(tb_uint32_t x)
{
    __tb_volatile__ tb_size_t   i = 0;
    __tb_volatile__ tb_size_t   n = 100000;
    __tb_volatile__ tb_hong_t t = tb_mclock();
    while (n--) i = tb_bits_fb0_u32_be(x);
    t = tb_mclock() - t;

    tb_trace_i("[bitops]: u32_be: fb0: %032lb => %lu, %lld ms", x, i, t);
}
static tb_void_t tb_test_bits_fb0_u32_le(tb_uint32_t x)
{
    __tb_volatile__ tb_size_t   i = 0;
    __tb_volatile__ tb_size_t   n = 100000;
    __tb_volatile__ tb_hong_t t = tb_mclock();
    while (n--) i = tb_bits_fb0_u32_le(x);
    t = tb_mclock() - t;

    tb_trace_i("[bitops]: u32_le: fb0: %032lb => %lu, %lld ms", x, i, t);
}
static tb_void_t tb_test_bits_fb0_u64_be(tb_uint64_t x)
{
    __tb_volatile__ tb_size_t   i = 0;
    __tb_volatile__ tb_size_t   n = 100000;
    __tb_volatile__ tb_hong_t t = tb_mclock();
    while (n--) i = tb_bits_fb0_u64_be(x);
    t = tb_mclock() - t;

    tb_trace_i("[bitops]: u64_be: fb0: %064llb => %lu, %lld ms", x, i, t);
}
static tb_void_t tb_test_bits_fb0_u64_le(tb_uint64_t x)
{
    __tb_volatile__ tb_size_t   i = 0;
    __tb_volatile__ tb_size_t   n = 100000;
    __tb_volatile__ tb_hong_t t = tb_mclock();
    while (n--) i = tb_bits_fb0_u64_le(x);
    t = tb_mclock() - t;

    tb_trace_i("[bitops]: u64_le: fb0: %064llb => %lu, %lld ms", x, i, t);
}
static tb_void_t tb_test_bits_fb1_u32_be(tb_uint32_t x)
{
    __tb_volatile__ tb_size_t   i = 0;
    __tb_volatile__ tb_size_t   n = 100000;
    __tb_volatile__ tb_hong_t t = tb_mclock();
    while (n--) i = tb_bits_fb1_u32_be(x);
    t = tb_mclock() - t;

    tb_trace_i("[bitops]: u32_be: fb1: %032lb => %lu, %lld ms", x, i, t);
}
static tb_void_t tb_test_bits_fb1_u32_le(tb_uint32_t x)
{
    __tb_volatile__ tb_size_t   i = 0;
    __tb_volatile__ tb_size_t   n = 100000;
    __tb_volatile__ tb_hong_t t = tb_mclock();
    while (n--) i = tb_bits_fb1_u32_le(x);
    t = tb_mclock() - t;

    tb_trace_i("[bitops]: u32_le: fb1: %032lb => %lu, %lld ms", x, i, t);
}
static tb_void_t tb_test_bits_fb1_u64_be(tb_uint64_t x)
{
    __tb_volatile__ tb_size_t   i = 0;
    __tb_volatile__ tb_size_t   n = 100000;
    __tb_volatile__ tb_hong_t t = tb_mclock();
    while (n--) i = tb_bits_fb1_u64_be(x);
    t = tb_mclock() - t;

    tb_trace_i("[bitops]: u64_be: fb1: %064llb => %lu, %lld ms", x, i, t);
}
static tb_void_t tb_test_bits_fb1_u64_le(tb_uint64_t x)
{
    __tb_volatile__ tb_size_t   i = 0;
    __tb_volatile__ tb_size_t   n = 100000;
    __tb_volatile__ tb_hong_t t = tb_mclock();
    while (n--) i = tb_bits_fb1_u64_le(x);
    t = tb_mclock() - t;

    tb_trace_i("[bitops]: u64_le: fb1: %064llb => %lu, %lld ms", x, i, t);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_utils_bits_main(tb_int_t argc, tb_char_t** argv)
{
#if 1
    tb_test_bits_swap_u16();
    tb_test_bits_swap_u32();
    tb_test_bits_swap_u64();
#endif

#if 1
    tb_trace_i("");
    tb_test_bits_ubits32(0x87654321);
    tb_test_bits_ubits32(0x12345678);

    tb_trace_i("");
    tb_test_bits_sbits32(0x87654321);
    tb_test_bits_sbits32(0x12345678);

    tb_trace_i("");
    tb_test_bits_sbits32(-300);
    tb_test_bits_sbits32(300);

    tb_trace_i("");
    tb_test_bits_u32_be(0x87654321);
    tb_test_bits_u32_be(0x12345678);

    tb_test_bits_u32_le(0x87654321);
    tb_test_bits_u32_le(0x12345678);

    tb_trace_i("");
    tb_test_bits_u24_be(0x654321);
    tb_test_bits_u24_be(0x345678);

    tb_test_bits_u24_le(0x654321);
    tb_test_bits_u24_le(0x345678);

    tb_trace_i("");
    tb_test_bits_u16_be(0x4321);
    tb_test_bits_u16_be(0x5678);

    tb_test_bits_u16_le(0x4321);
    tb_test_bits_u16_le(0x5678);

    tb_trace_i("");
    tb_test_bits_s32_be(0x8765F321);
    tb_test_bits_s32_be(0x1234F678);

    tb_test_bits_s32_le(0x8765F321);
    tb_test_bits_s32_le(0x1234F678);

    tb_trace_i("");
    tb_test_bits_s24_be(123456);
    tb_test_bits_s24_be(-123456);

    tb_test_bits_s24_le(123456);
    tb_test_bits_s24_le(-123456);

    tb_trace_i("");
    tb_test_bits_s16_be(4321);
    tb_test_bits_s16_be(-4321);

    tb_test_bits_s16_le(4321);
    tb_test_bits_s16_le(-4321);

#ifdef TB_CONFIG_TYPE_HAVE_FLOAT
    tb_trace_i("");
    tb_test_bits_double_bbe(3.1415926);
    tb_test_bits_double_ble(3.1415926);

    tb_test_bits_double_lbe(3.1415926);
    tb_test_bits_double_lle(3.1415926);

    tb_trace_i("");
    tb_test_bits_float_be(3.1415926f);
    tb_test_bits_float_le(3.1415926f);

    tb_test_bits_float_be(3.1415926f);
    tb_test_bits_float_le(3.1415926f);
#endif
#endif

#if 1
    tb_trace_i("");
    tb_test_bits_cl0_u32_be(0);
    tb_test_bits_cl0_u32_be(1);
    tb_test_bits_cl0_u32_be(2);
    tb_test_bits_cl0_u32_be(8);
    tb_test_bits_cl0_u32_be(123);
    tb_test_bits_cl0_u32_be(123456);
    tb_test_bits_cl0_u32_be(800000);
    tb_test_bits_cl0_u32_be(31415926);
    tb_test_bits_cl0_u32_be(0xffffffff);
    tb_test_bits_cl0_u32_be(0xfffffff0);
    tb_test_bits_cl0_u32_be(0x0fffffff);
    tb_test_bits_cl0_u32_be(0xfff00fff);
    tb_test_bits_cl0_u32_be(0x0ffffff0);
    tb_test_bits_cl0_u32_be(0x0000000f);
    tb_test_bits_cl0_u32_be(0xf0000000);

    tb_trace_i("");
    tb_test_bits_cl0_u32_le(0);
    tb_test_bits_cl0_u32_le(1);
    tb_test_bits_cl0_u32_le(2);
    tb_test_bits_cl0_u32_le(8);
    tb_test_bits_cl0_u32_le(123);
    tb_test_bits_cl0_u32_le(123456);
    tb_test_bits_cl0_u32_le(800000);
    tb_test_bits_cl0_u32_le(31415926);
    tb_test_bits_cl0_u32_le(0xffffffff);
    tb_test_bits_cl0_u32_le(0xfffffff0);
    tb_test_bits_cl0_u32_le(0x0fffffff);
    tb_test_bits_cl0_u32_le(0xfff00fff);
    tb_test_bits_cl0_u32_le(0x0ffffff0);
    tb_test_bits_cl0_u32_le(0x0000000f);
    tb_test_bits_cl0_u32_le(0xf0000000);

    tb_trace_i("");
    tb_test_bits_cl0_u64_be(0);
    tb_test_bits_cl0_u64_be(1);
    tb_test_bits_cl0_u64_be(2);
    tb_test_bits_cl0_u64_be(8);
    tb_test_bits_cl0_u64_be(123);
    tb_test_bits_cl0_u64_be(123456);
    tb_test_bits_cl0_u64_be(800000);
    tb_test_bits_cl0_u64_be(31415926);
    tb_test_bits_cl0_u64_be(0xffffffffffffffffull);
    tb_test_bits_cl0_u64_be(0xfffffffffffffff0ull);
    tb_test_bits_cl0_u64_be(0x0fffffffffffffffull);
    tb_test_bits_cl0_u64_be(0xfff00ffffff00fffull);
    tb_test_bits_cl0_u64_be(0x0ffffff00ffffff0ull);
    tb_test_bits_cl0_u64_be(0x000000000000000full);
    tb_test_bits_cl0_u64_be(0xf000000000000000ull);

    tb_trace_i("");
    tb_test_bits_cl0_u64_le(0);
    tb_test_bits_cl0_u64_le(1);
    tb_test_bits_cl0_u64_le(2);
    tb_test_bits_cl0_u64_le(8);
    tb_test_bits_cl0_u64_le(123);
    tb_test_bits_cl0_u64_le(123456);
    tb_test_bits_cl0_u64_le(800000);
    tb_test_bits_cl0_u64_le(31415926);
    tb_test_bits_cl0_u64_le(0xffffffffffffffffull);
    tb_test_bits_cl0_u64_le(0xfffffffffffffff0ull);
    tb_test_bits_cl0_u64_le(0x0fffffffffffffffull);
    tb_test_bits_cl0_u64_le(0xfff00ffffff00fffull);
    tb_test_bits_cl0_u64_le(0x0ffffff00ffffff0ull);
    tb_test_bits_cl0_u64_le(0x000000000000000full);
    tb_test_bits_cl0_u64_le(0xf000000000000000ull);
#endif

#if 1
    tb_trace_i("");
    tb_test_bits_cl1_u32_be(0);
    tb_test_bits_cl1_u32_be(1);
    tb_test_bits_cl1_u32_be(2);
    tb_test_bits_cl1_u32_be(8);
    tb_test_bits_cl1_u32_be(123);
    tb_test_bits_cl1_u32_be(123456);
    tb_test_bits_cl1_u32_be(800000);
    tb_test_bits_cl1_u32_be(31415926);
    tb_test_bits_cl1_u32_be(0xffffffff);
    tb_test_bits_cl1_u32_be(0xfffffff0);
    tb_test_bits_cl1_u32_be(0x0fffffff);
    tb_test_bits_cl1_u32_be(0xfff00fff);
    tb_test_bits_cl1_u32_be(0x0ffffff0);
    tb_test_bits_cl1_u32_be(0x0000000f);
    tb_test_bits_cl1_u32_be(0xf0000000);

    tb_trace_i("");
    tb_test_bits_cl1_u32_le(0);
    tb_test_bits_cl1_u32_le(1);
    tb_test_bits_cl1_u32_le(2);
    tb_test_bits_cl1_u32_le(8);
    tb_test_bits_cl1_u32_le(123);
    tb_test_bits_cl1_u32_le(123456);
    tb_test_bits_cl1_u32_le(800000);
    tb_test_bits_cl1_u32_le(31415926);
    tb_test_bits_cl1_u32_le(0xffffffff);
    tb_test_bits_cl1_u32_le(0xfffffff0);
    tb_test_bits_cl1_u32_le(0x0fffffff);
    tb_test_bits_cl1_u32_le(0xfff00fff);
    tb_test_bits_cl1_u32_le(0x0ffffff0);
    tb_test_bits_cl1_u32_le(0x0000000f);
    tb_test_bits_cl1_u32_le(0xf0000000);

    tb_trace_i("");
    tb_test_bits_cl1_u64_be(0);
    tb_test_bits_cl1_u64_be(1);
    tb_test_bits_cl1_u64_be(2);
    tb_test_bits_cl1_u64_be(8);
    tb_test_bits_cl1_u64_be(123);
    tb_test_bits_cl1_u64_be(123456);
    tb_test_bits_cl1_u64_be(800000);
    tb_test_bits_cl1_u64_be(31415926);
    tb_test_bits_cl1_u64_be(0xffffffffffffffffull);
    tb_test_bits_cl1_u64_be(0xfffffffffffffff0ull);
    tb_test_bits_cl1_u64_be(0x0fffffffffffffffull);
    tb_test_bits_cl1_u64_be(0xfff00ffffff00fffull);
    tb_test_bits_cl1_u64_be(0x0ffffff00ffffff0ull);
    tb_test_bits_cl1_u64_be(0x000000000000000full);
    tb_test_bits_cl1_u64_be(0xf000000000000000ull);

    tb_trace_i("");
    tb_test_bits_cl1_u64_le(0);
    tb_test_bits_cl1_u64_le(1);
    tb_test_bits_cl1_u64_le(2);
    tb_test_bits_cl1_u64_le(8);
    tb_test_bits_cl1_u64_le(123);
    tb_test_bits_cl1_u64_le(123456);
    tb_test_bits_cl1_u64_le(800000);
    tb_test_bits_cl1_u64_le(31415926);
    tb_test_bits_cl1_u64_le(0xffffffffffffffffull);
    tb_test_bits_cl1_u64_le(0xfffffffffffffff0ull);
    tb_test_bits_cl1_u64_le(0x0fffffffffffffffull);
    tb_test_bits_cl1_u64_le(0xfff00ffffff00fffull);
    tb_test_bits_cl1_u64_le(0x0ffffff00ffffff0ull);
    tb_test_bits_cl1_u64_le(0x000000000000000full);
    tb_test_bits_cl1_u64_le(0xf000000000000000ull);
#endif

#if 1
    tb_trace_i("");
    tb_test_bits_cb0_u32(0);
    tb_test_bits_cb0_u32(1);
    tb_test_bits_cb0_u32(2);
    tb_test_bits_cb0_u32(8);
    tb_test_bits_cb0_u32(123);
    tb_test_bits_cb0_u32(123456);
    tb_test_bits_cb0_u32(800000);
    tb_test_bits_cb0_u32(31415926);
    tb_test_bits_cb0_u32(0xffffffff);
    tb_test_bits_cb0_u32(0xfffffff0);
    tb_test_bits_cb0_u32(0x0fffffff);
    tb_test_bits_cb0_u32(0xfff00fff);
    tb_test_bits_cb0_u32(0x0ffffff0);
    tb_test_bits_cb0_u32(0x0000000f);
    tb_test_bits_cb0_u32(0xf0000000);

    tb_trace_i("");
    tb_test_bits_cb0_u64(0);
    tb_test_bits_cb0_u64(1);
    tb_test_bits_cb0_u64(2);
    tb_test_bits_cb0_u64(8);
    tb_test_bits_cb0_u64(123);
    tb_test_bits_cb0_u64(123456);
    tb_test_bits_cb0_u64(800000);
    tb_test_bits_cb0_u64(31415926);
    tb_test_bits_cb0_u64(0xffffffffffffffffull);
    tb_test_bits_cb0_u64(0xfffffffffffffff0ull);
    tb_test_bits_cb0_u64(0x0fffffffffffffffull);
    tb_test_bits_cb0_u64(0xfff00ffffff00fffull);
    tb_test_bits_cb0_u64(0x0ffffff00ffffff0ull);
    tb_test_bits_cb0_u64(0x000000000000000full);
    tb_test_bits_cb0_u64(0xf000000000000000ull);
#endif

#if 1
    tb_trace_i("");
    tb_test_bits_cb1_u32(0);
    tb_test_bits_cb1_u32(1);
    tb_test_bits_cb1_u32(2);
    tb_test_bits_cb1_u32(8);
    tb_test_bits_cb1_u32(123);
    tb_test_bits_cb1_u32(123456);
    tb_test_bits_cb1_u32(800000);
    tb_test_bits_cb1_u32(31415926);
    tb_test_bits_cb1_u32(0xffffffff);
    tb_test_bits_cb1_u32(0xfffffff0);
    tb_test_bits_cb1_u32(0x0fffffff);
    tb_test_bits_cb1_u32(0xfff00fff);
    tb_test_bits_cb1_u32(0x0ffffff0);
    tb_test_bits_cb1_u32(0x0000000f);
    tb_test_bits_cb1_u32(0xf0000000);

    tb_trace_i("");
    tb_test_bits_cb1_u64(0);
    tb_test_bits_cb1_u64(1);
    tb_test_bits_cb1_u64(2);
    tb_test_bits_cb1_u64(8);
    tb_test_bits_cb1_u64(123);
    tb_test_bits_cb1_u64(123456);
    tb_test_bits_cb1_u64(800000);
    tb_test_bits_cb1_u64(31415926);
    tb_test_bits_cb1_u64(0xffffffffffffffffull);
    tb_test_bits_cb1_u64(0xfffffffffffffff0ull);
    tb_test_bits_cb1_u64(0x0fffffffffffffffull);
    tb_test_bits_cb1_u64(0xfff00ffffff00fffull);
    tb_test_bits_cb1_u64(0x0ffffff00ffffff0ull);
    tb_test_bits_cb1_u64(0x000000000000000full);
    tb_test_bits_cb1_u64(0xf000000000000000ull);
#endif


#if 1
    tb_trace_i("");
    tb_test_bits_fb0_u32_be(0);
    tb_test_bits_fb0_u32_be(1);
    tb_test_bits_fb0_u32_be(2);
    tb_test_bits_fb0_u32_be(8);
    tb_test_bits_fb0_u32_be(123);
    tb_test_bits_fb0_u32_be(123456);
    tb_test_bits_fb0_u32_be(800000);
    tb_test_bits_fb0_u32_be(31415926);
    tb_test_bits_fb0_u32_be(0xffffffff);
    tb_test_bits_fb0_u32_be(0xfffffff0);
    tb_test_bits_fb0_u32_be(0x0fffffff);
    tb_test_bits_fb0_u32_be(0xfff00fff);
    tb_test_bits_fb0_u32_be(0x0ffffff0);
    tb_test_bits_fb0_u32_be(0x0000000f);
    tb_test_bits_fb0_u32_be(0xf0000000);

    tb_trace_i("");
    tb_test_bits_fb0_u32_le(0);
    tb_test_bits_fb0_u32_le(1);
    tb_test_bits_fb0_u32_le(2);
    tb_test_bits_fb0_u32_le(8);
    tb_test_bits_fb0_u32_le(123);
    tb_test_bits_fb0_u32_le(123456);
    tb_test_bits_fb0_u32_le(800000);
    tb_test_bits_fb0_u32_le(31415926);
    tb_test_bits_fb0_u32_le(0xffffffff);
    tb_test_bits_fb0_u32_le(0xfffffff0);
    tb_test_bits_fb0_u32_le(0x0fffffff);
    tb_test_bits_fb0_u32_le(0xfff00fff);
    tb_test_bits_fb0_u32_le(0x0ffffff0);
    tb_test_bits_fb0_u32_le(0x0000000f);
    tb_test_bits_fb0_u32_le(0xf0000000);

    tb_trace_i("");
    tb_test_bits_fb0_u64_be(0);
    tb_test_bits_fb0_u64_be(1);
    tb_test_bits_fb0_u64_be(2);
    tb_test_bits_fb0_u64_be(8);
    tb_test_bits_fb0_u64_be(123);
    tb_test_bits_fb0_u64_be(123456);
    tb_test_bits_fb0_u64_be(800000);
    tb_test_bits_fb0_u64_be(31415926);
    tb_test_bits_fb0_u64_be(0xffffffffffffffffull);
    tb_test_bits_fb0_u64_be(0xfffffffffffffff0ull);
    tb_test_bits_fb0_u64_be(0x0fffffffffffffffull);
    tb_test_bits_fb0_u64_be(0xfff00ffffff00fffull);
    tb_test_bits_fb0_u64_be(0x0ffffff00ffffff0ull);
    tb_test_bits_fb0_u64_be(0x000000000000000full);
    tb_test_bits_fb0_u64_be(0xf000000000000000ull);

    tb_trace_i("");
    tb_test_bits_fb0_u64_le(0);
    tb_test_bits_fb0_u64_le(1);
    tb_test_bits_fb0_u64_le(2);
    tb_test_bits_fb0_u64_le(8);
    tb_test_bits_fb0_u64_le(123);
    tb_test_bits_fb0_u64_le(123456);
    tb_test_bits_fb0_u64_le(800000);
    tb_test_bits_fb0_u64_le(31415926);
    tb_test_bits_fb0_u64_le(0xffffffffffffffffull);
    tb_test_bits_fb0_u64_le(0xfffffffffffffff0ull);
    tb_test_bits_fb0_u64_le(0x0fffffffffffffffull);
    tb_test_bits_fb0_u64_le(0xfff00ffffff00fffull);
    tb_test_bits_fb0_u64_le(0x0ffffff00ffffff0ull);
    tb_test_bits_fb0_u64_le(0x000000000000000full);
    tb_test_bits_fb0_u64_le(0xf000000000000000ull);
#endif

#if 1
    tb_trace_i("");
    tb_test_bits_fb1_u32_be(0);
    tb_test_bits_fb1_u32_be(1);
    tb_test_bits_fb1_u32_be(2);
    tb_test_bits_fb1_u32_be(8);
    tb_test_bits_fb1_u32_be(123);
    tb_test_bits_fb1_u32_be(123456);
    tb_test_bits_fb1_u32_be(800000);
    tb_test_bits_fb1_u32_be(31415926);
    tb_test_bits_fb1_u32_be(0xffffffff);
    tb_test_bits_fb1_u32_be(0xfffffff0);
    tb_test_bits_fb1_u32_be(0x0fffffff);
    tb_test_bits_fb1_u32_be(0xfff00fff);
    tb_test_bits_fb1_u32_be(0x0ffffff0);
    tb_test_bits_fb1_u32_be(0x0000000f);
    tb_test_bits_fb1_u32_be(0xf0000000);

    tb_trace_i("");
    tb_test_bits_fb1_u32_le(0);
    tb_test_bits_fb1_u32_le(1);
    tb_test_bits_fb1_u32_le(2);
    tb_test_bits_fb1_u32_le(8);
    tb_test_bits_fb1_u32_le(123);
    tb_test_bits_fb1_u32_le(123456);
    tb_test_bits_fb1_u32_le(800000);
    tb_test_bits_fb1_u32_le(31415926);
    tb_test_bits_fb1_u32_le(0xffffffff);
    tb_test_bits_fb1_u32_le(0xfffffff0);
    tb_test_bits_fb1_u32_le(0x0fffffff);
    tb_test_bits_fb1_u32_le(0xfff00fff);
    tb_test_bits_fb1_u32_le(0x0ffffff0);
    tb_test_bits_fb1_u32_le(0x0000000f);
    tb_test_bits_fb1_u32_le(0xf0000000);

    tb_trace_i("");
    tb_test_bits_fb1_u64_be(0);
    tb_test_bits_fb1_u64_be(1);
    tb_test_bits_fb1_u64_be(2);
    tb_test_bits_fb1_u64_be(8);
    tb_test_bits_fb1_u64_be(123);
    tb_test_bits_fb1_u64_be(123456);
    tb_test_bits_fb1_u64_be(800000);
    tb_test_bits_fb1_u64_be(31415926);
    tb_test_bits_fb1_u64_be(0xffffffffffffffffull);
    tb_test_bits_fb1_u64_be(0xfffffffffffffff0ull);
    tb_test_bits_fb1_u64_be(0x0fffffffffffffffull);
    tb_test_bits_fb1_u64_be(0xfff00ffffff00fffull);
    tb_test_bits_fb1_u64_be(0x0ffffff00ffffff0ull);
    tb_test_bits_fb1_u64_be(0x000000000000000full);
    tb_test_bits_fb1_u64_be(0xf000000000000000ull);

    tb_trace_i("");
    tb_test_bits_fb1_u64_le(0);
    tb_test_bits_fb1_u64_le(1);
    tb_test_bits_fb1_u64_le(2);
    tb_test_bits_fb1_u64_le(8);
    tb_test_bits_fb1_u64_le(123);
    tb_test_bits_fb1_u64_le(123456);
    tb_test_bits_fb1_u64_le(800000);
    tb_test_bits_fb1_u64_le(31415926);
    tb_test_bits_fb1_u64_le(0xffffffffffffffffull);
    tb_test_bits_fb1_u64_le(0xfffffffffffffff0ull);
    tb_test_bits_fb1_u64_le(0x0fffffffffffffffull);
    tb_test_bits_fb1_u64_le(0xfff00ffffff00fffull);
    tb_test_bits_fb1_u64_le(0x0ffffff00ffffff0ull);
    tb_test_bits_fb1_u64_le(0x000000000000000full);
    tb_test_bits_fb1_u64_le(0xf000000000000000ull);
#endif

    return 0;
}
