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
 * @file        directory.m
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../directory.h"
#import <Foundation/Foundation.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_size_t tb_directory_temp(tb_char_t* path, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(path && maxn > 4, 0);

    // the temp
    NSString*           temp = NSTemporaryDirectory();
    tb_char_t const*    cstr = [temp UTF8String];
    tb_size_t           size = [temp length];
    if (temp)
    {
        // copy it
        if (size >= maxn) size = maxn - 1;
        tb_strlcpy(path, cstr, size);
        path[size] = '\0';
    }
    else
    {
        // copy the default temporary directory
        tb_strlcpy(path, "/tmp", maxn - 1);
        path[4] = '\0';
        size = 4;
    }

    // ok?
    return size;
}

