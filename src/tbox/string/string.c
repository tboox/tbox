/*!The Treasure Box Library
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * Copyright (C) 2009 - 2018, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        string.c
 * @ingroup     string
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"
#include "../libc/libc.h"
#include "../utils/utils.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
// the maximum grow size of value string 
#ifdef __tb_small__
#   define TB_SCOPED_STRING_FMTD_SIZE       (4096)
#else
#   define TB_SCOPED_STRING_FMTD_SIZE       (8192)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_string_init(tb_string_ref_t string)
{
    // check
    tb_assert_and_check_return_val(string, tb_false);

    // init
    tb_bool_t ok = tb_buffer_init(string);

    // clear it
    tb_string_clear(string);

    // ok?
    return ok;
}
tb_void_t tb_string_exit(tb_string_ref_t string)
{
    if (string) tb_buffer_exit(string);
}
tb_char_t const* tb_string_cstr(tb_string_ref_t string)
{
    // check
    tb_assert_and_check_return_val(string, tb_null);

    // the cstr
    return tb_string_size(string)? (tb_char_t const*)tb_buffer_data((tb_buffer_ref_t)string) : tb_null;
}
tb_size_t tb_string_size(tb_string_ref_t string)
{
    // check
    tb_assert_and_check_return_val(string, 0);

    // the size
    tb_size_t n = tb_buffer_size(string);
    return n > 0? n - 1 : 0;
}
tb_void_t tb_string_clear(tb_string_ref_t string)
{
    // check
    tb_assert_and_check_return(string);

    // clear buffer
    tb_buffer_clear(string);

    // clear string
    tb_char_t* p = (tb_char_t*)tb_buffer_data(string);
    if (p) p[0] = '\0';
}
tb_char_t const* tb_string_strip(tb_string_ref_t string, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(string, tb_null);

    // out?
    tb_check_return_val(n < tb_string_size(string), tb_string_cstr(string));

    // strip
    tb_char_t* p = (tb_char_t*)tb_buffer_resize(string, n + 1);
    if (p) p[n] = '\0';

    // ok?
    return p;
}
tb_char_t const* tb_string_ltrim(tb_string_ref_t string)
{
    // check
    tb_assert_and_check_return_val(string, tb_null);

    // init
    tb_char_t*  s = (tb_char_t*)tb_string_cstr(string);
    tb_size_t   n = tb_string_size(string);
    tb_check_return_val(s && n, tb_null);

    // done
    tb_char_t*  p = s;
    tb_char_t*  e = s + n;
    while (p < e && tb_isspace(*p)) p++;

    // strip it
    if (p < e) 
    {
        // move it if exists spaces
        if (p > s) tb_buffer_memmov(string, p - s);
    }
    // clear it 
    else tb_string_clear(string);

    // ok?
    return tb_string_cstr(string);
}
tb_char_t const* tb_string_rtrim(tb_string_ref_t string)
{
    // check
    tb_assert_and_check_return_val(string, tb_null);

    // init
    tb_char_t*  s = (tb_char_t*)tb_string_cstr(string);
    tb_size_t   n = tb_string_size(string);
    tb_check_return_val(s && n, tb_null);

    // done
    tb_char_t*  e = s + n - 1;
    while (e >= s && tb_isspace(*e)) e--;

    // strip it
    if (e >= s) tb_string_strip(string, e - s + 1);
    // clear it 
    else tb_string_clear(string);

    // ok?
    return tb_string_cstr(string);
}
tb_char_t tb_string_charat(tb_string_ref_t string, tb_size_t p)
{
    // check
    tb_char_t const*    s = tb_string_cstr(string);
    tb_size_t           n = tb_string_size(string);
    tb_assert_and_check_return_val(s && p < n, '\0');

    // get it
    return s[p];
}
tb_long_t tb_string_strchr(tb_string_ref_t string, tb_size_t p, tb_char_t c)
{
    // check
    tb_char_t const*    s = tb_string_cstr(string);
    tb_size_t           n = tb_string_size(string);
    tb_assert_and_check_return_val(s && p < n, -1);

    // done
    tb_char_t* q = tb_strchr(s + p, c);
    return (q? q - s : -1);
}
tb_long_t tb_string_strichr(tb_string_ref_t string, tb_size_t p, tb_char_t c)
{
    // check
    tb_char_t const*    s = tb_string_cstr(string);
    tb_size_t           n = tb_string_size(string);
    tb_assert_and_check_return_val(s && p < n, -1);

    // done
    tb_char_t* q = tb_strichr(s + p, c);
    return (q? q - s : -1);
}
tb_long_t tb_string_strrchr(tb_string_ref_t string, tb_size_t p, tb_char_t c)
{
    // check
    tb_char_t const*    s = tb_string_cstr(string);
    tb_size_t           n = tb_string_size(string);
    tb_assert_and_check_return_val(s && p < n, -1);

    // done
    tb_char_t* q = tb_strnrchr(s + p, n, c);
    return (q? q - s : -1);
}
tb_long_t tb_string_strirchr(tb_string_ref_t string, tb_size_t p, tb_char_t c)
{
    // check
    tb_char_t const*    s = tb_string_cstr(string);
    tb_size_t           n = tb_string_size(string);
    tb_assert_and_check_return_val(s && p < n, -1);

    // done
    tb_char_t* q = tb_strnirchr(s + p, n, c);
    return (q? q - s : -1);
}
tb_long_t tb_string_strstr(tb_string_ref_t string, tb_size_t p, tb_string_ref_t s)
{
    return tb_string_cstrstr(string, p, tb_string_cstr(s));
}
tb_long_t tb_string_stristr(tb_string_ref_t string, tb_size_t p, tb_string_ref_t s)
{
    return tb_string_cstristr(string, p, tb_string_cstr(s));
}
tb_long_t tb_string_cstrstr(tb_string_ref_t string, tb_size_t p, tb_char_t const* s2)
{
    // check
    tb_char_t const*    s = tb_string_cstr(string);
    tb_size_t           n = tb_string_size(string);
    tb_assert_and_check_return_val(s && p < n, -1);

    // done
    tb_char_t* q = tb_strstr(s + p, s2);
    return (q? q - s : -1);
}
tb_long_t tb_string_cstristr(tb_string_ref_t string, tb_size_t p, tb_char_t const* s2)
{   
    // check
    tb_char_t const*    s = tb_string_cstr(string);
    tb_size_t           n = tb_string_size(string);
    tb_assert_and_check_return_val(s && p < n, -1);

    // done
    tb_char_t* q = tb_stristr(s + p, s2);
    return (q? q - s : -1);
}
tb_long_t tb_string_strrstr(tb_string_ref_t string, tb_size_t p, tb_string_ref_t s)
{
    return tb_string_cstrrstr(string, p, tb_string_cstr(s));
}
tb_long_t tb_string_strirstr(tb_string_ref_t string, tb_size_t p, tb_string_ref_t s)
{
    return tb_string_cstrirstr(string, p, tb_string_cstr(s));
}
tb_long_t tb_string_cstrrstr(tb_string_ref_t string, tb_size_t p, tb_char_t const* s2)
{   
    // check
    tb_char_t const*    s = tb_string_cstr(string);
    tb_size_t           n = tb_string_size(string);
    tb_assert_and_check_return_val(s && p < n, -1);

    // done
    tb_char_t* q = tb_strnrstr(s + p, n, s2);
    return (q? q - s : -1);
}
tb_long_t tb_string_cstrirstr(tb_string_ref_t string, tb_size_t p, tb_char_t const* s2)
{
    // check
    tb_char_t const*    s = tb_string_cstr(string);
    tb_size_t           n = tb_string_size(string);
    tb_assert_and_check_return_val(s && p < n, -1);

    // done
    tb_char_t* q = tb_strnirstr(s + p, n, s2);
    return (q? q - s : -1);
}
tb_char_t const* tb_string_strcpy(tb_string_ref_t string, tb_string_ref_t s)
{
    // check
    tb_assert_and_check_return_val(s, tb_null);

    // done
    tb_size_t n = tb_string_size(s);
    if (n) return tb_string_cstrncpy(string, tb_string_cstr(s), n);
    else
    {
        tb_string_clear(string);
        return tb_null;
    }
}
tb_char_t const* tb_string_cstrcpy(tb_string_ref_t string, tb_char_t const* s)
{
    // check
    tb_assert_and_check_return_val(s, tb_null);
    return tb_string_cstrncpy(string, s, tb_strlen(s));
}
tb_char_t const* tb_string_cstrncpy(tb_string_ref_t string, tb_char_t const* s, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(string && s && n, tb_null);

    tb_char_t* p = (tb_char_t*)tb_buffer_memncpy(string, (tb_byte_t const*)s, n + 1);
    if (p) p[tb_string_size(string)] = '\0';
    return p;
}
tb_char_t const* tb_string_cstrfcpy(tb_string_ref_t string, tb_char_t const* fmt, ...)
{
    // check
    tb_assert_and_check_return_val(string && fmt, tb_null);

    // format data
    tb_char_t p[TB_SCOPED_STRING_FMTD_SIZE] = {0};
    tb_size_t n = 0;
    tb_vsnprintf_format(p, TB_SCOPED_STRING_FMTD_SIZE, fmt, &n);
    tb_assert_and_check_return_val(n, tb_null);
    
    // done
    return tb_string_cstrncpy(string, p, n);
}
tb_char_t const* tb_string_chrcat(tb_string_ref_t string, tb_char_t c)
{
    // check
    tb_assert_and_check_return_val(string, tb_null);
    
    // done
    tb_char_t* p = (tb_char_t*)tb_buffer_memnsetp(string, tb_string_size(string), c, 2);
    if (p) p[tb_string_size(string)] = '\0';
    return p;
}
tb_char_t const* tb_string_chrncat(tb_string_ref_t string, tb_char_t c, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(string, tb_null);

    // done
    tb_char_t* p = (tb_char_t*)tb_buffer_memnsetp(string, tb_string_size(string), c, n + 1);
    if (p) p[tb_string_size(string)] = '\0';
    return p;
}
tb_char_t const* tb_string_strcat(tb_string_ref_t string, tb_string_ref_t s)
{
    // check
    tb_assert_and_check_return_val(s, tb_null);

    // done
    return tb_string_cstrncat(string, tb_string_cstr(s), tb_string_size(s));
}
tb_char_t const* tb_string_cstrcat(tb_string_ref_t string, tb_char_t const* s)
{
    // check
    tb_assert_and_check_return_val(s, tb_null);
    return tb_string_cstrncat(string, s, tb_strlen(s));
}
tb_char_t const* tb_string_cstrncat(tb_string_ref_t string, tb_char_t const* s, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(string && s && n, tb_null);

    // done
    tb_char_t* p = (tb_char_t*)tb_buffer_memncpyp(string, tb_string_size(string), (tb_byte_t const*)s, n + 1);
    if (p) p[tb_string_size(string)] = '\0';
    return p;
}
tb_char_t const* tb_string_cstrfcat(tb_string_ref_t string, tb_char_t const* fmt, ...)
{
    // check
    tb_assert_and_check_return_val(string && fmt, tb_null);

    // format data
    tb_char_t p[TB_SCOPED_STRING_FMTD_SIZE] = {0};
    tb_long_t n = 0;
    tb_vsnprintf_format(p, TB_SCOPED_STRING_FMTD_SIZE, fmt, &n);
    tb_assert_and_check_return_val(n, tb_null);
    
    // done
    return tb_string_cstrncat(string, p, n);
}
tb_long_t tb_string_strcmp(tb_string_ref_t string, tb_string_ref_t s)
{
    // check
    tb_assert_and_check_return_val(string && s, 0);
    return tb_string_cstrncmp(string, tb_string_cstr(s), tb_string_size(s) + 1);
}
tb_long_t tb_string_strimp(tb_string_ref_t string, tb_string_ref_t s)
{
    // check
    tb_assert_and_check_return_val(string && s, 0);
    return tb_string_cstrnicmp(string, tb_string_cstr(s), tb_string_size(s) + 1);
}
tb_long_t tb_string_cstrcmp(tb_string_ref_t string, tb_char_t const* s)
{
    // check
    tb_assert_and_check_return_val(string && s, 0);
    return tb_string_cstrncmp(string, s, tb_strlen(s) + 1);
}
tb_long_t tb_string_cstricmp(tb_string_ref_t string, tb_char_t const* s)
{
    // check
    tb_assert_and_check_return_val(string && s, 0);
    return tb_string_cstrnicmp(string, s, tb_strlen(s) + 1);
}
tb_long_t tb_string_cstrncmp(tb_string_ref_t string, tb_char_t const* s, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(string && s, 0);
    return tb_strncmp(tb_string_cstr(string), s, n);
}
tb_long_t tb_string_cstrnicmp(tb_string_ref_t string, tb_char_t const* s, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(string && s, 0);
    return tb_strnicmp(tb_string_cstr(string), s, n);
}

