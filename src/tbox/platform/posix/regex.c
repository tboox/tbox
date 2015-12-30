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
 * @file        regex.c
 * @ingroup     platform
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <regex.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the regex type
typedef struct __tb_regex_t
{
    // the code
    regex_t             code;

    // the results 
    tb_vector_ref_t     results;

    // the mode
    tb_size_t           mode;

    // the match data
    regmatch_t*         match_data;

    // the match maxn
    tb_size_t           match_maxn;

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
        tb_int_t options = REG_EXTENDED | REG_ENHANCED;
        if (mode & TB_REGEX_MODE_CASELESS) options |= REG_ICASE;
        if (mode & TB_REGEX_MODE_MULTILINE) options |= REG_NEWLINE;

        // init code
        tb_int_t error = regcomp(&regex->code, pattern, options);
        if (error)
        {
#ifdef __tb_debug__
            tb_char_t info[256] = {0};
            regerror(error, &regex->code, info, sizeof(info));

            // trace
            tb_trace_d("compile failed: %s\n", info);
#endif

            // end
            break;
        }

        // save mode
        regex->mode = mode;

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
    if (regex->match_data) tb_free(regex->match_data);
    regex->match_data = tb_null;
    regex->match_maxn = 0;

    // exit results
    if (regex->results) tb_vector_exit(regex->results);
    regex->results = tb_null;

    // exit code
    regfree(&regex->code);

    // exit it
    tb_free(regex);
}
tb_long_t tb_regex_match(tb_regex_ref_t self, tb_char_t const* cstr, tb_size_t size, tb_size_t start, tb_size_t* plength, tb_vector_ref_t* presults)
{
    // check
    tb_regex_t* regex = (tb_regex_t*)self;
    tb_assert_and_check_return_val(regex && cstr, -1);

    // done
    tb_long_t ok = -1;
    do
    {
        // clear length first
        if (plength) *plength = 0;

        // end?
        tb_check_break(start < size);

        // init match data
        if (!regex->match_data)
        {
            regex->match_maxn = 16;
            regex->match_data = (regmatch_t*)tb_malloc_bytes(sizeof(regmatch_t) * regex->match_maxn);
        }
        tb_assert_and_check_break(regex->match_data);

        // check
        tb_assert(size <= tb_strlen(cstr));

        // match it
        tb_long_t error = -1;
        while (REG_ESPACE == (error = regexec(&regex->code, cstr + start, regex->match_maxn, regex->match_data, 0)))
        {
            // grow match data
            regex->match_maxn <<= 1;
            regex->match_data = (regmatch_t*)tb_ralloc_bytes(regex->match_data, sizeof(regmatch_t) * regex->match_maxn);
            tb_assert_and_check_break(regex->match_data);
        }
        if (error)
        {
            // no match?
            tb_check_break(error != REG_NOMATCH);

#ifdef __tb_debug__
            // get error info
            tb_char_t info[256] = {0};
            regerror(error, &regex->code, info, sizeof(info));

            // trace
            tb_trace_d("match failed at offset %lu: error: %s\n", start, info);
#endif

            // end
            break;
        }

        // get the match offset and length
        regmatch_t const*   match = regex->match_data;
        tb_size_t           count = 1 + regex->code.re_nsub;
        tb_size_t           offset = start + (tb_size_t)match[0].rm_so;
        tb_size_t           length = (tb_size_t)match[0].rm_eo - match[0].rm_so;
        tb_assert_and_check_break(offset + length <= size);

        // trace
        tb_trace_d("matched count: %lu, offset: %lu, length: %lu", count, offset, length);

        // save results
        if (presults)
        {
            // init results if not exists
            tb_vector_ref_t results = *presults;
            if (!results)
            {
                // init it
                if (!regex->results) regex->results = tb_vector_init(16, tb_element_mem(sizeof(tb_regex_match_t), tb_regex_match_exit, tb_null));

                // save it
                *presults = results = regex->results;
            }
            tb_assert_and_check_break(results);

            // clear it first
            tb_vector_clear(results);

            // done
            tb_long_t           i = 0;
            tb_regex_match_t    entry;
            for (i = 0; i < count; i++)
            {
                // get substring offset and length
                tb_size_t substr_offset = start + match[i].rm_so;
                tb_size_t substr_length = match[i].rm_eo - match[i].rm_so;
                tb_assert_and_check_break(substr_offset + substr_length <= size);

                // make match entry
                entry.cstr  = tb_strndup(cstr + substr_offset, substr_length);
                entry.size  = substr_length;
                entry.start = substr_offset;
                tb_assert_and_check_break(entry.cstr);
                
                // trace
                tb_trace_d("    matched: [%lu, %lu]: %s", entry.start, entry.size, entry.cstr);

                // append it
                tb_vector_insert_tail(results, &entry);
            }
            tb_assert_and_check_break(i == count);
        }

        // save length 
        if (plength) *plength = length;

        // ok
        ok = offset;

    } while (0);

    // ok?
    return ok;
}
tb_char_t const* tb_regex_replace(tb_regex_ref_t self, tb_char_t const* cstr, tb_size_t size, tb_size_t start, tb_char_t const* replace_cstr, tb_size_t replace_size, tb_size_t* plength)
{
    tb_assert_noimpl();
    return tb_null;
}
