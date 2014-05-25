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
 * @file        scoped_string.c
 * @ingroup     string
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "scoped_string.h"
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
tb_bool_t tb_scoped_string_init(tb_scoped_string_t* string)
{
    tb_assert_and_check_return_val(string, tb_false);
    return tb_scoped_buffer_init(string);
}
tb_void_t tb_scoped_string_exit(tb_scoped_string_t* string)
{
    if (string) tb_scoped_buffer_exit(string);
}
tb_char_t const* tb_scoped_string_cstr(tb_scoped_string_t const* string)
{
    tb_assert_and_check_return_val(string, tb_null);
    return (tb_char_t const*)tb_scoped_buffer_data((tb_scoped_buffer_t*)string);
}
tb_size_t tb_scoped_string_size(tb_scoped_string_t const* string)
{
    tb_assert_and_check_return_val(string, 0);
    tb_size_t n = tb_scoped_buffer_size(string);
    return n > 0? n - 1 : 0;
}
tb_void_t tb_scoped_string_clear(tb_scoped_string_t* string)
{
    tb_assert_and_check_return(string);

    // clear buffer
    tb_scoped_buffer_clear(string);

    // clear string
    tb_char_t* p = (tb_char_t*)tb_scoped_buffer_data(string);
    if (p) p[0] = '\0';
}
tb_char_t const* tb_scoped_string_strip(tb_scoped_string_t* string, tb_size_t n)
{
    tb_assert_and_check_return_val(string, tb_null);

    // out?
    tb_check_return_val(n < tb_scoped_string_size(string), tb_scoped_string_cstr(string));

    // strip
    tb_char_t* p = (tb_char_t*)tb_scoped_buffer_resize(string, n + 1);
    if (p) p[n] = '\0';
    return p;
}
tb_long_t tb_scoped_string_strchr(tb_scoped_string_t const* string, tb_size_t p, tb_char_t c)
{
    tb_char_t const*    s = tb_scoped_string_cstr(string);
    tb_size_t           n = tb_scoped_string_size(string);
    tb_assert_and_check_return_val(s && p && p < n, -1);

    tb_char_t* q = tb_strchr(s + p, c);
    return (q? q - s : -1);
}
tb_long_t tb_scoped_string_strichr(tb_scoped_string_t const* string, tb_size_t p, tb_char_t c)
{
    tb_char_t const*    s = tb_scoped_string_cstr(string);
    tb_size_t           n = tb_scoped_string_size(string);
    tb_assert_and_check_return_val(s && p && p < n, -1);

    tb_char_t* q = tb_strichr(s + p, c);
    return (q? q - s : -1);
}
tb_long_t tb_scoped_string_strrchr(tb_scoped_string_t const* string, tb_size_t p, tb_char_t c)
{
    tb_char_t const*    s = tb_scoped_string_cstr(string);
    tb_size_t           n = tb_scoped_string_size(string);
    tb_assert_and_check_return_val(s && p && p < n, -1);

    tb_char_t* q = tb_strnrchr(s + p, n, c);
    return (q? q - s : -1);
}
tb_long_t tb_scoped_string_strirchr(tb_scoped_string_t const* string, tb_size_t p, tb_char_t c)
{
    tb_char_t const*    s = tb_scoped_string_cstr(string);
    tb_size_t           n = tb_scoped_string_size(string);
    tb_assert_and_check_return_val(s && p && p < n, -1);

    tb_char_t* q = tb_strnirchr(s + p, n, c);
    return (q? q - s : -1);
}
tb_long_t tb_scoped_string_strstr(tb_scoped_string_t const* string, tb_size_t p, tb_scoped_string_t const* s)
{
    return tb_scoped_string_cstrstr(string, p, tb_scoped_string_cstr(s));
}
tb_long_t tb_scoped_string_stristr(tb_scoped_string_t const* string, tb_size_t p, tb_scoped_string_t const* s)
{
    return tb_scoped_string_cstristr(string, p, tb_scoped_string_cstr(s));
}
tb_long_t tb_scoped_string_cstrstr(tb_scoped_string_t const* string, tb_size_t p, tb_char_t const* s2)
{
    tb_char_t const*    s = tb_scoped_string_cstr(string);
    tb_size_t           n = tb_scoped_string_size(string);
    tb_assert_and_check_return_val(s && p && p < n, -1);

    tb_char_t* q = tb_strstr(s + p, s2);
    return (q? q - s : -1);
}
tb_long_t tb_scoped_string_cstristr(tb_scoped_string_t const* string, tb_size_t p, tb_char_t const* s2)
{   
    tb_char_t const*    s = tb_scoped_string_cstr(string);
    tb_size_t           n = tb_scoped_string_size(string);
    tb_assert_and_check_return_val(s && p && p < n, -1);

    tb_char_t* q = tb_stristr(s + p, s2);
    return (q? q - s : -1);
}
tb_long_t tb_scoped_string_strrstr(tb_scoped_string_t const* string, tb_size_t p, tb_scoped_string_t const* s)
{
    return tb_scoped_string_cstrrstr(string, p, tb_scoped_string_cstr(s));
}
tb_long_t tb_scoped_string_strirstr(tb_scoped_string_t const* string, tb_size_t p, tb_scoped_string_t const* s)
{
    return tb_scoped_string_cstrirstr(string, p, tb_scoped_string_cstr(s));
}
tb_long_t tb_scoped_string_cstrrstr(tb_scoped_string_t const* string, tb_size_t p, tb_char_t const* s2)
{   
    tb_char_t const*    s = tb_scoped_string_cstr(string);
    tb_size_t           n = tb_scoped_string_size(string);
    tb_assert_and_check_return_val(s && p && p < n, -1);

    tb_char_t* q = tb_strnrstr(s + p, n, s2);
    return (q? q - s : -1);
}
tb_long_t tb_scoped_string_cstrirstr(tb_scoped_string_t const* string, tb_size_t p, tb_char_t const* s2)
{
    tb_char_t const*    s = tb_scoped_string_cstr(string);
    tb_size_t           n = tb_scoped_string_size(string);
    tb_assert_and_check_return_val(s && p && p < n, -1);

    tb_char_t* q = tb_strnirstr(s + p, n, s2);
    return (q? q - s : -1);
}
tb_char_t const* tb_scoped_string_strcpy(tb_scoped_string_t* string, tb_scoped_string_t const* s)
{
    // check
    tb_assert_and_check_return_val(s, tb_null);

    tb_size_t n = tb_scoped_string_size(s);
    if (n) return tb_scoped_string_cstrncpy(string, tb_scoped_string_cstr(s), n);
    else
    {
        tb_scoped_string_clear(string);
        return tb_null;
    }
}
tb_char_t const* tb_scoped_string_cstrcpy(tb_scoped_string_t* string, tb_char_t const* s)
{
    // check
    tb_assert_and_check_return_val(s, tb_null);
    return tb_scoped_string_cstrncpy(string, s, tb_strlen(s));
}
tb_char_t const* tb_scoped_string_cstrncpy(tb_scoped_string_t* string, tb_char_t const* s, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(string && s && n, tb_null);

    tb_char_t* p = (tb_char_t*)tb_scoped_buffer_memncpy(string, (tb_byte_t const*)s, n + 1);
    if (p) p[tb_scoped_string_size(string)] = '\0';
    return p;
}
tb_char_t const* tb_scoped_string_cstrfcpy(tb_scoped_string_t* string, tb_char_t const* fmt, ...)
{
    // check
    tb_assert_and_check_return_val(string && fmt, tb_null);

    // format data
    tb_char_t p[TB_SCOPED_STRING_FMTD_SIZE] = {0};
    tb_size_t n = 0;
    tb_vsnprintf_format(p, TB_SCOPED_STRING_FMTD_SIZE, fmt, &n);
    tb_assert_and_check_return_val(n, tb_null);
    
    return tb_scoped_string_cstrncpy(string, p, n);
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * chrcat
 */
tb_char_t const* tb_scoped_string_chrcat(tb_scoped_string_t* string, tb_char_t c)
{
    // check
    tb_assert_and_check_return_val(string, tb_null);
    
    tb_char_t* p = (tb_char_t*)tb_scoped_buffer_memnsetp(string, tb_scoped_string_size(string), c, 2);
    if (p) p[tb_scoped_string_size(string)] = '\0';
    return p;
}
tb_char_t const* tb_scoped_string_chrncat(tb_scoped_string_t* string, tb_char_t c, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(string, tb_null);

    tb_char_t* p = (tb_char_t*)tb_scoped_buffer_memnsetp(string, tb_scoped_string_size(string), c, n + 1);
    if (p) p[tb_scoped_string_size(string)] = '\0';
    return p;
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * strcat
 */
tb_char_t const* tb_scoped_string_strcat(tb_scoped_string_t* string, tb_scoped_string_t const* s)
{
    // check
    tb_assert_and_check_return_val(s, tb_null);
    return tb_scoped_string_cstrncat(string, tb_scoped_string_cstr(s), tb_scoped_string_size(s));
}
tb_char_t const* tb_scoped_string_cstrcat(tb_scoped_string_t* string, tb_char_t const* s)
{
    // check
    tb_assert_and_check_return_val(s, tb_null);
    return tb_scoped_string_cstrncat(string, s, tb_strlen(s));
}
tb_char_t const* tb_scoped_string_cstrncat(tb_scoped_string_t* string, tb_char_t const* s, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(string && s && n, tb_null);
    tb_char_t* p = (tb_char_t*)tb_scoped_buffer_memncpyp(string, tb_scoped_string_size(string), (tb_byte_t const*)s, n + 1);
    if (p) p[tb_scoped_string_size(string)] = '\0';
    return p;
}
tb_char_t const* tb_scoped_string_cstrfcat(tb_scoped_string_t* string, tb_char_t const* fmt, ...)
{
    // check
    tb_assert_and_check_return_val(string && fmt, tb_null);

    // format data
    tb_char_t p[TB_SCOPED_STRING_FMTD_SIZE] = {0};
    tb_long_t n = 0;
    tb_vsnprintf_format(p, TB_SCOPED_STRING_FMTD_SIZE, fmt, &n);
    tb_assert_and_check_return_val(n, tb_null);
    
    return tb_scoped_string_cstrncat(string, p, n);
}
tb_long_t tb_scoped_string_strcmp(tb_scoped_string_t* string, tb_scoped_string_t const* s)
{
    // check
    tb_assert_and_check_return_val(string && s, 0);
    return tb_scoped_string_cstrncmp(string, tb_scoped_string_cstr(s), tb_scoped_string_size(s) + 1);
}
tb_long_t tb_scoped_string_strimp(tb_scoped_string_t* string, tb_scoped_string_t const* s)
{
    // check
    tb_assert_and_check_return_val(string && s, 0);
    return tb_scoped_string_cstrnicmp(string, tb_scoped_string_cstr(s), tb_scoped_string_size(s) + 1);
}
tb_long_t tb_scoped_string_cstrcmp(tb_scoped_string_t* string, tb_char_t const* s)
{
    // check
    tb_assert_and_check_return_val(string && s, 0);
    return tb_scoped_string_cstrncmp(string, s, tb_strlen(s) + 1);
}
tb_long_t tb_scoped_string_cstricmp(tb_scoped_string_t* string, tb_char_t const* s)
{
    // check
    tb_assert_and_check_return_val(string && s, 0);
    return tb_scoped_string_cstrnicmp(string, s, tb_strlen(s) + 1);
}
tb_long_t tb_scoped_string_cstrncmp(tb_scoped_string_t* string, tb_char_t const* s, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(string && s, 0);
    return tb_strncmp(tb_scoped_string_cstr(string), s, n);
}
tb_long_t tb_scoped_string_cstrnicmp(tb_scoped_string_t* string, tb_char_t const* s, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(string && s, 0);
    return tb_strnicmp(tb_scoped_string_cstr(string), s, n);
}

