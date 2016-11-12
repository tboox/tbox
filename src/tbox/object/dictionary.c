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
 * Copyright (C) 2009 - 2017, ruki All rights reserved.
 *
 * @author      ruki
 * @file        dictionary.c
 * @ingroup     object
 *
 */
 
/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME        "object_dictionary"
#define TB_TRACE_MODULE_DEBUG       (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "object.h"
#include "../string/string.h"
#include "../algorithm/algorithm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef __tb_small__
#   define TB_OBJECT_DICTIONARY_SIZE_DEFAULT           TB_OBJECT_DICTIONARY_SIZE_MICRO
#else
#   define TB_OBJECT_DICTIONARY_SIZE_DEFAULT           TB_OBJECT_DICTIONARY_SIZE_SMALL
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the dictionary type
typedef struct __tb_oc_dictionary_t
{
    // the object base
    tb_oc_object_t         base;

    // the capacity size
    tb_size_t           size;

    // the object hash
    tb_hash_map_ref_t   hash;

    // increase refn?
    tb_bool_t           incr;

}tb_oc_dictionary_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_oc_dictionary_t* tb_oc_dictionary_cast(tb_oc_object_ref_t object)
{
    // check
    tb_assert_and_check_return_val(object && object->type == TB_OBJECT_TYPE_DICTIONARY, tb_null);

    // cast
    return (tb_oc_dictionary_t*)object;
}
static tb_oc_object_ref_t tb_oc_dictionary_copy(tb_oc_object_ref_t object)
{
    // check
    tb_oc_dictionary_t* dictionary = tb_oc_dictionary_cast(object);
    tb_assert_and_check_return_val(dictionary, tb_null);

    // init copy
    tb_oc_dictionary_t* copy = (tb_oc_dictionary_t*)tb_oc_dictionary_init(dictionary->size, dictionary->incr);
    tb_assert_and_check_return_val(copy, tb_null);

    // walk copy
    tb_for_all (tb_oc_dictionary_item_t*, item, tb_oc_dictionary_itor((tb_oc_object_ref_t)dictionary))
    {
        if (item && item->key) 
        {
            // refn++
            if (item->val) tb_oc_object_retain(item->val);

            // copy
            tb_oc_dictionary_insert((tb_oc_object_ref_t)copy, item->key, item->val);
        }
    }

    // ok
    return (tb_oc_object_ref_t)copy;
}
static tb_void_t tb_oc_dictionary_exit(tb_oc_object_ref_t object)
{
    // check
    tb_oc_dictionary_t* dictionary = tb_oc_dictionary_cast(object);
    tb_assert_and_check_return(dictionary);

    // exit hash
    if (dictionary->hash) tb_hash_map_exit(dictionary->hash);
    dictionary->hash = tb_null;

    // exit it
    tb_free(dictionary);
}
static tb_void_t tb_oc_dictionary_clear(tb_oc_object_ref_t object)
{
    tb_oc_dictionary_t* dictionary = tb_oc_dictionary_cast(object);
    tb_assert_and_check_return(dictionary);

    // clear
    if (dictionary->hash) tb_hash_map_clear(dictionary->hash);
}
static tb_oc_dictionary_t* tb_oc_dictionary_init_base()
{
    // done
    tb_bool_t                   ok = tb_false;
    tb_oc_dictionary_t*     dictionary = tb_null;
    do
    {
        // make dictionary
        dictionary = tb_malloc0_type(tb_oc_dictionary_t);
        tb_assert_and_check_break(dictionary);

        // init dictionary
        if (!tb_oc_object_init((tb_oc_object_ref_t)dictionary, TB_OBJECT_FLAG_NONE, TB_OBJECT_TYPE_DICTIONARY)) break;

        // init base
        dictionary->base.copy   = tb_oc_dictionary_copy;
        dictionary->base.exit   = tb_oc_dictionary_exit;
        dictionary->base.clear  = tb_oc_dictionary_clear;
        
        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (dictionary) tb_oc_object_exit((tb_oc_object_ref_t)dictionary);
        dictionary = tb_null;
    }

    // ok?
    return dictionary;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_oc_object_ref_t tb_oc_dictionary_init(tb_size_t size, tb_bool_t incr)
{
    // done
    tb_bool_t               ok = tb_false;
    tb_oc_dictionary_t* dictionary = tb_null;
    do
    {
        // make dictionary
        dictionary = tb_oc_dictionary_init_base();
        tb_assert_and_check_break(dictionary);

        // using the default size
        if (!size) size = TB_OBJECT_DICTIONARY_SIZE_DEFAULT;

        // init
        dictionary->size = size;
        dictionary->incr = incr;

        // init hash
        dictionary->hash = tb_hash_map_init(size, tb_element_str(tb_true), tb_element_obj());
        tb_assert_and_check_break(dictionary->hash);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (dictionary) tb_oc_dictionary_exit((tb_oc_object_ref_t)dictionary);
        dictionary = tb_null;
    }

    // ok?
    return (tb_oc_object_ref_t)dictionary;
}
tb_size_t tb_oc_dictionary_size(tb_oc_object_ref_t object)
{
    // check
    tb_oc_dictionary_t* dictionary = tb_oc_dictionary_cast(object);
    tb_assert_and_check_return_val(dictionary && dictionary->hash, 0);

    // size
    return tb_hash_map_size(dictionary->hash);
}
tb_iterator_ref_t tb_oc_dictionary_itor(tb_oc_object_ref_t object)
{
    tb_oc_dictionary_t* dictionary = tb_oc_dictionary_cast(object);
    tb_assert_and_check_return_val(dictionary, tb_null);

    // iterator
    return (tb_iterator_ref_t)dictionary->hash;
}
tb_oc_object_ref_t tb_oc_dictionary_value(tb_oc_object_ref_t object, tb_char_t const* key)
{
    // check
    tb_oc_dictionary_t* dictionary = tb_oc_dictionary_cast(object);
    tb_assert_and_check_return_val(dictionary && dictionary->hash && key, tb_null);

    // value
    return (tb_oc_object_ref_t)tb_hash_map_get(dictionary->hash, key);
}
tb_void_t tb_oc_dictionary_remove(tb_oc_object_ref_t object, tb_char_t const* key)
{
    // check
    tb_oc_dictionary_t* dictionary = tb_oc_dictionary_cast(object);
    tb_assert_and_check_return(dictionary && dictionary->hash && key);

    // del
    return tb_hash_map_remove(dictionary->hash, key);
}
tb_void_t tb_oc_dictionary_insert(tb_oc_object_ref_t object, tb_char_t const* key, tb_oc_object_ref_t val)
{
    // check
    tb_oc_dictionary_t* dictionary = tb_oc_dictionary_cast(object);
    tb_assert_and_check_return(dictionary && dictionary->hash && key && val);

    // add
    tb_hash_map_insert(dictionary->hash, key, val);

    // refn--
    if (!dictionary->incr) tb_oc_object_exit(val);
}
tb_void_t tb_oc_dictionary_incr(tb_oc_object_ref_t object, tb_bool_t incr)
{
    // check
    tb_oc_dictionary_t* dictionary = tb_oc_dictionary_cast(object);
    tb_assert_and_check_return(dictionary);

    dictionary->incr = incr;
}
