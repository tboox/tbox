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
 * @file        pcre2.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "pcre2/pcre2.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the regex type
typedef struct __tb_regex_t
{
    // the code
    pcre2_code*         code;

    // the match data
    pcre2_match_data*   match_data;

}tb_regex_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_regex_ref_t tb_regex_init(tb_char_t const* pattern, tb_size_t mode)
{
    // check
    tb_assert_and_check_return_val(pattern, tb_null);

    // done
    tb_bool_t   ok = tb_false;
    tb_regex_t* regex = tb_null;
    do
    {
        // make regex
        regex = (tb_regex_t*)tb_malloc0_type(tb_regex_t);
        tb_assert_and_check_break(regex);

        // init options
        tb_uint32_t options = PCRE2_UTF;
        if (mode & TB_REGEX_MODE_CASELESS) options |= PCRE2_CASELESS;
        if (mode & TB_REGEX_MODE_MULTILINE) options |= PCRE2_MULTILINE;
#ifndef __tb_debug__
        options |= PCRE2_NO_UTF_CHECK;
#endif

        // init code
        tb_int_t    errornumber;
        PCRE2_SIZE  erroroffset;
        regex->code = pcre2_compile((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED, options, &errornumber, &erroroffset, tb_null);
        if (!regex->code)
        {
#ifdef __tb_debug__
            // get error info
            PCRE2_UCHAR info[256];
            pcre2_get_error_message(errornumber, info, sizeof(info));

            // trace
            tb_trace_d("compile failed at offset %ld: %s\n", (tb_long_t)erroroffset, info);
#endif

            // end
            break;
        }

        // init match data
        regex->match_data = pcre2_match_data_create_from_pattern(regex->code, tb_null);
        tb_assert_and_check_break(regex->match_data);

        // ok 
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (regex) tb_regex_exit((tb_regex_ref_t)regex);
        regex = tb_null;
    }

    // ok?
    return (tb_regex_ref_t)regex;
}
tb_void_t tb_regex_exit(tb_regex_ref_t self)
{
    // check
    tb_regex_t* regex = (tb_regex_t*)self;
    tb_assert_and_check_return(regex);

    // exit match data
    if (regex->match_data) pcre2_match_data_free(regex->match_data);
    regex->match_data = tb_null;

    // exit code
    if (regex->code) pcre2_code_free(regex->code);
    regex->code = tb_null;

    // exit it
    tb_free(regex);
}
tb_long_t tb_regex_match(tb_regex_ref_t self, tb_size_t mode, tb_char_t const* cstr, tb_size_t size, tb_size_t start, tb_size_t* plength, tb_vector_ref_t* presults)
{
    // check
    tb_regex_t* regex = (tb_regex_t*)self;
    tb_assert_and_check_return_val(regex && regex->code && cstr, -1);

    // done
    tb_long_t ok = -1;
    do
    {
        // init options
        tb_uint32_t options = PCRE2_UTF;
#ifndef __tb_debug__
        options |= PCRE2_NO_UTF_CHECK;
#endif

        // match it
        tb_long_t count = pcre2_match(regex->code, (PCRE2_SPTR)cstr, (PCRE2_SIZE)size, (PCRE2_SIZE)start, options, regex->match_data, tb_null);
        if (count < 0)
        {

            // end
            break;
        }

    } while (0);

    // ok?
    return ok;
}
tb_char_t const* tb_regex_replace(tb_regex_ref_t self, tb_size_t mode, tb_char_t const* cstr, tb_size_t size, tb_size_t start, tb_char_t const* replace_cstr, tb_size_t replace_size, tb_size_t* plength)
{
    tb_assert_noimpl();
    return tb_null;
}
