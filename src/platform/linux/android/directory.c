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
 * @file        directory.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "android_directory"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "android.h"
#include "../../directory.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_size_t tb_directory_temp(tb_char_t* path, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(path && maxn > 4, 0);

    // the jni environment
    tb_size_t   size = 0;
    JNIEnv*     jenv = tb_android_jenv();
    if (jenv)
    {
        // enter
        if ((*jenv)->PushLocalFrame(jenv, 5) >= 0) 
        {
            // done
            jboolean error = tb_false;
            do
            {
                // get the environment class
                jclass environment_class = (*jenv)->FindClass(jenv, "android/os/Environment");
                tb_assert_and_check_break(!(error = (*jenv)->ExceptionCheck(jenv)) && environment_class);

            } while (0);

            // exception? clear it
            if (error) (*jenv)->ExceptionClear(jenv);

            // leave
            (*jenv)->PopLocalFrame(jenv, tb_null);
        }
    }
    else
    {
        // the temporary directory
        tb_strlcpy(path, "/tmp", maxn - 1);
        path[4] = '\0';
        size = 4;
    }

    // ok?
    return size;
}

