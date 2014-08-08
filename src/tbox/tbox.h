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
 * @file        tbox.h
 *
 */
#ifndef TB_TBOX_H
#define TB_TBOX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "algorithm/algorithm.h"
#include "container/container.h"
#include "platform/platform.h"
#include "database/database.h"
#include "network/network.h"
#include "charset/charset.h"
#include "memory/memory.h"
#include "stream/stream.h"
#include "string/string.h"
#include "object/object.h"
#include "utils/utils.h"
#include "math/math.h"
#include "libc/libc.h"
#include "libm/libm.h"
#include "asio/asio.h"
#include "xml/xml.h"
#include "zip/zip.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the compile mode
#define TB_MODE_DEBUG           (1)
#define TB_MODE_SMALL           (2)

#ifdef __tb_debug__
#   define __tb_mode_debug__    TB_MODE_DEBUG
#else
#   define __tb_mode_debug__    (0)
#endif

#ifdef __tb_small__
#   define __tb_mode_small__    TB_MODE_SMALL
#else
#   define __tb_mode_small__    (0)
#endif

/*! init tbox
 *
 * @param priv      the platform private data
 *                  pass JNIEnv* env for android
 *                  pass tb_null for other platform
 * @param data      the memory data for the memory pool, uses the native memory if be tb_null
 * @param size      the memory size for the memory pool, uses the native memory if be zero
 *
 * @return          tb_true or tb_false
 *
 * @code
    #include "tbox/tbox.h"

    int main(int argc, char** argv)
    {
        // init tbox
        if (!tb_init(tb_null, tb_null, 0)) return 0;

        // print info with tag
        tb_trace_i("hello tbox");

        // print info only for debug
        tb_trace_d("hello tbox"); 

        // print error info
        tb_trace_e("hello tbox");

        // init stream
        tb_stream_ref_t stream = tb_stream_init_from_url("http://www.xxxx.com/index.html");
        if (stream)
        {
            // save stream data to file
            tb_transfer_done_to_url(stream, "/home/file/index.html", 0, tb_null, tb_null);

            // exit stream
            tb_stream_exit(stream);
        }

        // block: save http to file
        tb_transfer_done_url("http://www.xxxx.com/index.html", "/home/file/index.html", 0, tb_null, tb_null);

        // async: save http to file
        tb_transfer_pool_done(tb_transfer_pool(), "http://www.xxxx.com/index0.html", "/home/file/index0.html", 0, 0, tb_null, tb_null);
        tb_transfer_pool_done(tb_transfer_pool(), "http://www.xxxx.com/index1.html", "/home/file/index2.html", 0, 0, tb_null, tb_null);
        tb_transfer_pool_done(tb_transfer_pool(), "http://www.xxxx.com/index2.html", "/home/file/index3.html", 0, 0, tb_null, tb_null);
        tb_transfer_pool_done(tb_transfer_pool(), "http://www.xxxx.com/index3.html", "/home/file/index4.html", 0, 0, tb_null, tb_null);

        // ...

        // exit tbox
        tb_exit();
        return 0;
    }
 * @endcode
 */
#define tb_init(priv, data, size)     tb_init_(priv, data, size, (tb_size_t)(__tb_mode_debug__ | __tb_mode_small__), TB_VERSION_BUILD)

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
__tb_extern_c_enter__

/*! init the tbox library
 *
 * @param priv      the platform private data
 *                  pass JNIEnv* env for android
 *                  pass tb_null for other platform
 * @param data      the memory data for the memory pool, uses the native memory if be tb_null
 * @param size      the memory size for the memory pool, uses the native memory if be zero
 * @param mode      the compile mode for check __tb_small__ and __tb_debug__
 * @param build     the build version
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_init_(tb_handle_t priv, tb_byte_t* data, tb_size_t size, tb_size_t mode, tb_hize_t build);

/// exit the tbox library
tb_void_t           tb_exit(tb_noarg_t);

/*! the tbox version
 *
 * @return          the tbox version
 */
tb_version_t const* tb_version(tb_noarg_t);

__tb_extern_c_leave__
#endif
