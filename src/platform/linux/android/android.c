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
 * @file        android.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "android.h"
#include "../../atomic.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */
static tb_atomic_t g_jenv = 0;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_android_init(JNIEnv* jenv)
{
    // check
    if (!jenv)
    {
        // warning
        tb_trace_w("the jni environment be not inited, please pass it to the tb_init function!");
    }

    // init it
    tb_atomic_set(&g_jenv, (tb_size_t)jenv);

    // ok
    return tb_true;
}
tb_void_t tb_android_exit()
{
    // clear it
    tb_atomic_set(&g_jenv, 0);
}
JNIEnv* tb_android_jenv()
{
    // get it
    return (JNIEnv*)tb_atomic_get(&g_jenv);
}

