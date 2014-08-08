/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author      ruki
 * @file        tbox.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the initial count
static tb_atomic_t  g_init = 0;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static __tb_inline__ tb_bool_t tb_check_order_word()
{
    tb_uint16_t x = 0x1234;
    tb_byte_t const* p = (tb_byte_t const*)&x;

#ifdef TB_WORDS_BIGENDIAN
    // is big endian?
    return (p[0] == 0x12 && p[1] == 0x34)? tb_true : tb_false;
#else
    // is little endian?
    return (p[0] == 0x34 && p[1] == 0x12)? tb_true : tb_false;
#endif
}
static __tb_inline__ tb_bool_t tb_check_order_double()
{
#ifdef TB_CONFIG_TYPE_FLOAT
    union 
    {
        tb_uint32_t i[2];
        double      f;

    } conv;
    conv.f = 1.0f;

#   ifdef TB_FLOAT_BIGENDIAN
    // is big endian?
    return (!conv.i[1] && conv.i[0])? tb_true : tb_false;
#   else
    // is little endian?
    return (!conv.i[0] && conv.i[1])? tb_true : tb_false;
#   endif
#else
    return tb_true;
#endif
}
static __tb_inline__ tb_bool_t tb_check_mode(tb_size_t mode)
{
#ifdef __tb_debug__
    if (!(mode & TB_MODE_DEBUG))
    {
        tb_trace_e("libtbox.a has __tb_debug__ but tbox/tbox.h not");
        return tb_false;
    }
#else
    if (mode & TB_MODE_DEBUG)
    {
        tb_trace_e("tbox/tbox.h has __tb_debug__ but libtbox.a not");
        return tb_false;
    }
#endif

#ifdef __tb_small__
    if (!(mode & TB_MODE_SMALL))
    {
        tb_trace_e("libtbox.a has __tb_small__ but tbox/tbox.h not");
        return tb_false;
    }
#else
    if (mode & TB_MODE_SMALL)
    {
        tb_trace_e("tbox/tbox.h has __tb_small__ but libtbox.a not");
        return tb_false;
    }
#endif

    // ok
    return tb_true;
}
static __tb_inline__ tb_bool_t tb_version_check(tb_hize_t build)
{
    // the version oly for link the static vtag string
    tb_version_t const* version = tb_version(); tb_used(version);

    // ok
    if (build == TB_VERSION_BUILD)
    {
        tb_trace_d("version: %s", TB_VERSION_STRING);
        return tb_true;
    }
    else
    {
        tb_trace_w("version: %s != %s", TB_VERSION_STRING, TB_VERSION_BUILD_STRING);
    }

    // no
    return tb_false;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_init_(tb_handle_t priv, tb_byte_t* data, tb_size_t size, tb_size_t mode, tb_hize_t build)
{
    // is inited?
    if (tb_atomic_fetch_and_inc(&g_init)) return tb_true;

    // init trace
    if (!tb_trace_init()) return tb_false;

    // trace
    tb_trace_d("init: %p %lu", data, size);

    // check mode
    if (!tb_check_mode(mode)) return tb_false;

    // check types
    tb_assert_static(sizeof(tb_byte_t) == 1);
    tb_assert_static(sizeof(tb_uint_t) == 4);
    tb_assert_static(sizeof(tb_uint8_t) == 1);
    tb_assert_static(sizeof(tb_uint16_t) == 2);
    tb_assert_static(sizeof(tb_uint32_t) == 4);
    tb_assert_static(sizeof(tb_hize_t) == 8);
    tb_assert_static(sizeof(tb_wchar_t) == sizeof(L'w'));
    tb_assert_static(TB_CPU_BITSIZE == (sizeof(tb_size_t) << 3));
    tb_assert_static(TB_CPU_BITSIZE == (sizeof(tb_long_t) << 3));
    tb_assert_static(TB_CPU_BITSIZE == (sizeof(tb_pointer_t) << 3));
    tb_assert_static(TB_CPU_BITSIZE == (sizeof(tb_handle_t) << 3));

    // check byteorder
    tb_assert_and_check_return_val(tb_check_order_word(), tb_false);
    tb_assert_and_check_return_val(tb_check_order_double(), tb_false);

    // init singleton
    if (!tb_singleton_init()) return tb_false;

    // init memory
    if (!tb_memory_init(data, size)) return tb_false;

    // init platform
    if (!tb_platform_init(priv)) return tb_false;

    // init network 
    if (!tb_network_init()) return tb_false;

    // init object
#ifdef TB_CONFIG_MODULE_HAVE_OBJECT
    if (!tb_object_context_init()) return tb_false;
#endif

    // check version
    tb_version_check(build);

    // trace
    tb_trace_d("init: ok");

    // ok
    return tb_true;
}
tb_void_t tb_exit()
{
    // need exit?
    tb_long_t init = 0;
    if ((init = tb_atomic_dec_and_fetch(&g_init)) > 0) return ;

    // check
    tb_assert_and_check_return(!init);
    
    // kill singleton
    tb_singleton_kill();

    // exit object
#ifdef TB_CONFIG_MODULE_HAVE_OBJECT
    tb_object_context_exit();
#endif
    
    // exit network
    tb_network_exit();
    
    // exit platform
    tb_platform_exit();
    
    // exit singleton
    tb_singleton_exit();

    // exit memory
    tb_memory_exit();

    // trace
    tb_trace_d("exit: ok");

    // exit trace
    tb_trace_exit();
}
tb_version_t const* tb_version()
{
    // init version tag for binary search
    static __tb_volatile__ tb_char_t const* s_vtag = "[tbox]: [vtag]: " TB_VERSION_STRING; tb_used(s_vtag);

    // init version
    static tb_version_t s_version = {0};
    if (!s_version.major)
    {
        s_version.major = TB_VERSION_MAJOR;
        s_version.minor = TB_VERSION_MINOR;
        s_version.alter = TB_VERSION_ALTER;
        s_version.build = (tb_hize_t)tb_atoll(TB_VERSION_BUILD_STRING);
    }

    return &s_version;
}
