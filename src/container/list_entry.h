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
 * @file        list_entry.h
 * @ingroup     container
 *
 */
#ifndef TB_CONTAINER_LIST_ENTRY_H
#define TB_CONTAINER_LIST_ENTRY_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "iterator.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/// the list entry
#define tb_list_entry(list, entry)                  ((((tb_byte_t*)(entry)) - (list)->eoff))

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/*! the double list entry type
 * 
 * <pre>
 * list: list => ... => last
 *        |               |
 *        <---------------
 *
 * </pre>
 */
typedef struct __tb_list_entry_t 
{
    /// the next entry
    struct __tb_list_entry_t*   next;

    /// the prev entry
    struct __tb_list_entry_t*   prev;

}tb_list_entry_t;

/// the list entry head type
typedef struct __tb_list_entry_head_t 
{
    /// the next entry
    struct __tb_list_entry_t*   next;

    /// the prev entry
    struct __tb_list_entry_t*   prev;

    /// the list size
    tb_size_t                   size;

    /// the entry offset
    tb_size_t                   eoff;

    /// the iterator 
    tb_iterator_t               itor;

}tb_list_entry_head_t;

/// the list entry ref type
typedef tb_list_entry_t*        tb_list_entry_ref_t;

/// the list entry head ref type
typedef tb_list_entry_head_t*   tb_list_entry_head_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! the list iterator
 *
 * @param list                              the list
 *
 * @return                                  the list iterator
 */
tb_iterator_ref_t                           tb_list_entry_itor(tb_list_entry_head_ref_t list);

/*! init list
 *
 * @param list                              the list
 * @param offset                            the entry offset
 */
tb_void_t                                   tb_list_entry_init(tb_list_entry_head_ref_t list, tb_size_t offset);

/*! exit list
 *
 * @param list                              the list
 */ 
tb_void_t                                   tb_list_entry_exit(tb_list_entry_head_ref_t list);

/*! clear list
 *
 * @param list                              the list
 */
static __tb_inline__ tb_void_t              tb_list_entry_clear(tb_list_entry_head_ref_t list)
{
    // check
    tb_assert(list);

    // clear it
    list->next = (tb_list_entry_ref_t)list;
    list->prev = (tb_list_entry_ref_t)list;
    list->size = 0;
}

/*! the list is null?
 *
 * @param list                              the list
 *
 * @return                                  tb_true or tb_false
 */
static __tb_inline__ tb_bool_t              tb_list_entry_null(tb_list_entry_head_ref_t list)
{ 
    // check
    tb_assert(list);

    // done
    return list->next == (tb_list_entry_ref_t)list? tb_true : tb_false;
}

/*! the list entry count
 *
 * @param list                              the list
 *
 * @return                                  the list entry count
 */
static __tb_inline__ tb_size_t              tb_list_entry_size(tb_list_entry_head_ref_t list)
{ 
    // check
    tb_assert(list);

    // done
    return list->size;
}

/*! the list next entry
 *
 * @param list                              the list
 * @param entry                             the entry
 *
 * @return                                  the next entry
 */
static __tb_inline__ tb_list_entry_ref_t    tb_list_entry_next(tb_list_entry_head_ref_t list, tb_list_entry_ref_t entry)
{
    // check
    tb_assert(entry);

    // done
    return entry->next;
}

/*! the list prev entry
 *
 * @param list                              the list
 * @param entry                             the entry
 *
 * @return                                  the prev entry
 */
static __tb_inline__ tb_list_entry_ref_t    tb_list_entry_prev(tb_list_entry_head_ref_t list, tb_list_entry_ref_t entry)
{ 
    // check
    tb_assert(entry);

    // done
    return entry->prev;
}

/*! the list head entry
 *
 * @param list                              the list
 *
 * @return                                  the head entry
 */
static __tb_inline__ tb_list_entry_ref_t    tb_list_entry_head(tb_list_entry_head_ref_t list)
{
    // check
    tb_assert(list);

    // done
    return list->next;
}

/*! the list last entry
 *
 * @param list                              the list
 *
 * @return                                  the last entry
 */
static __tb_inline__ tb_list_entry_ref_t    tb_list_entry_last(tb_list_entry_head_ref_t list)
{ 
    // check
    tb_assert(list);

    // done
    return list->prev;
}

/*! is the list head entry?
 *
 * @param list                              the list
 * @param entry                             the entry
 *
 * @return                                  tb_true or tb_false
 */
static __tb_inline__ tb_bool_t              tb_list_entry_is_head(tb_list_entry_head_ref_t list, tb_list_entry_ref_t entry)
{
    // check
    tb_assert(list);

    // done
    return list->next == entry? tb_true : tb_false;
}

/*! is the list last entry?
 *
 * @param list                              the list
 * @param entry                             the entry
 *
 * @return                                  tb_true or tb_false
 */
static __tb_inline__ tb_bool_t              tb_list_entry_is_last(tb_list_entry_head_ref_t list, tb_list_entry_ref_t entry)
{
    // check
    tb_assert(list);

    // done
    return list->prev == entry? tb_true : tb_false;
}

/*! insert entry to the next
 *
 * @param list                              the list
 * @param node                              the list node
 * @param entry                             the inserted list entry
 */
static __tb_inline__ tb_void_t              tb_list_entry_insert_next(tb_list_entry_head_ref_t list, tb_list_entry_ref_t node, tb_list_entry_ref_t entry)
{
    // check
    tb_assert(list && node && node->next && entry);
    tb_assert(node != entry);

    // insert entry
    node->next->prev    = entry;
    entry->next         = node->next;
    entry->prev         = node;
    node->next          = entry;

    // size++
    list->size++;
}

/*! insert entry to the prev
 *
 * @param list                              the list
 * @param node                              the list node
 * @param entry                             the inserted list entry
 */
static __tb_inline__ tb_void_t              tb_list_entry_insert_prev(tb_list_entry_head_ref_t list, tb_list_entry_ref_t node, tb_list_entry_ref_t entry)
{
    // check
    tb_assert(list && node);

    // insert it
    tb_list_entry_insert_next(list, node->prev, entry);
}

/*! insert entry to the list 
 *
 * @param list                              the list
 * @param entry                             the inserted list entry
 */
static __tb_inline__ tb_void_t              tb_list_entry_insert_head(tb_list_entry_head_ref_t list, tb_list_entry_ref_t entry)
{
    tb_list_entry_insert_next(list, (tb_list_entry_ref_t)list, entry);
}

/*! insert entry to the tail 
 *
 * @param list                              the list
 * @param entry                             the inserted list entry
 */
static __tb_inline__ tb_void_t              tb_list_entry_insert_tail(tb_list_entry_head_ref_t list, tb_list_entry_ref_t entry)
{
    // check
    tb_assert(list && entry);

    // insert it
    tb_list_entry_insert_next(list, list->prev, entry);
}

/*! replace the entry
 *
 * @param list                              the list
 * @param node                              the replaced list node
 * @param entry                             the new list entry
 */
static __tb_inline__ tb_void_t              tb_list_entry_replace(tb_list_entry_head_ref_t list, tb_list_entry_ref_t node, tb_list_entry_ref_t entry)
{
    // check
    tb_assert(node && node->next && node->prev && entry);
    tb_assert(node != entry);

    // replace it
    entry->next         = node->next;
    entry->next->prev   = entry;
    entry->prev         = node->prev;
    entry->prev->next   = entry;
}

/*! replace the next entry
 *
 * @param list                              the list
 * @param node                              the list node
 * @param entry                             the new list entry
 */
static __tb_inline__ tb_void_t              tb_list_entry_replace_next(tb_list_entry_head_ref_t list, tb_list_entry_ref_t node, tb_list_entry_ref_t entry)
{
    // check
    tb_assert(node);

    // replace it
    tb_list_entry_replace(list, node->next, entry);
}

/*! replace the prev entry
 *
 * @param list                              the list
 * @param node                              the list node
 * @param entry                             the new list entry
 */
static __tb_inline__ tb_void_t              tb_list_entry_replace_prev(tb_list_entry_head_ref_t list, tb_list_entry_ref_t node, tb_list_entry_ref_t entry)
{
    // check
    tb_assert(node);

    // replace it
    tb_list_entry_replace(list, node->prev, entry);
}

/*! replace the head entry
 *
 * @param list                              the list
 * @param entry                             the new list entry
 */
static __tb_inline__ tb_void_t              tb_list_entry_replace_head(tb_list_entry_head_ref_t list, tb_list_entry_ref_t entry)
{
    // check
    tb_assert(list);

    // replace it
    tb_list_entry_replace(list, list->next, entry);
}

/*! replace the last entry
 *
 * @param list                              the list
 * @param entry                             the new list entry
 */
static __tb_inline__ tb_void_t              tb_list_entry_replace_last(tb_list_entry_head_ref_t list, tb_list_entry_ref_t entry)
{
    // check
    tb_assert(list);

    // replace it
    tb_list_entry_replace(list, list->prev, entry);
}

/*! remove the entry
 *
 * @param list                              the list
 * @param entry                             the removed list entry
 */
static __tb_inline__ tb_void_t              tb_list_entry_remove(tb_list_entry_head_ref_t list, tb_list_entry_ref_t entry)
{
    // check
    tb_assert(list && entry && entry->next && entry->prev);

    // remove entry
    entry->next->prev = entry->prev;
    entry->prev->next = entry->next;

    // size--
    list->size--;
}

/*! remove the next entry
 *
 * @param list                              the list
 * @param entry                             the prev entry
 */
static __tb_inline__ tb_void_t              tb_list_entry_remove_next(tb_list_entry_head_ref_t list, tb_list_entry_ref_t entry)
{
    // check
    tb_assert(list && entry);

    // remove it
    tb_list_entry_remove(list, entry->next);
}

/*! remove the prev entry
 *
 * @param list                              the list
 * @param entry                             the next entry
 */
static __tb_inline__ tb_void_t              tb_list_entry_remove_prev(tb_list_entry_head_ref_t list, tb_list_entry_ref_t entry)
{
    // check
    tb_assert(list && entry);

    // remove it
    tb_list_entry_remove(list, entry->prev);
}

/*! remove the list entry
 *
 * @param list                              the list
 */
static __tb_inline__ tb_void_t              tb_list_entry_remove_head(tb_list_entry_head_ref_t list)
{
    // check
    tb_assert(list);

    // remove it
    tb_list_entry_remove(list, list->next);
}

/*! remove the last entry
 *
 * @param list                              the list
 */
static __tb_inline__ tb_void_t              tb_list_entry_remove_last(tb_list_entry_head_ref_t list)
{
    // check
    tb_assert(list);

    // remove it
    tb_list_entry_remove(list, list->prev);
}

/*! moveto the next entry
 *
 * @param list                              the list
 * @param node                              the list node
 * @param entry                             the moved list entry
 */
static __tb_inline__ tb_void_t              tb_list_entry_moveto_next(tb_list_entry_head_ref_t list, tb_list_entry_ref_t node, tb_list_entry_ref_t entry)
{
    // check
    tb_check_return(node != entry);

    // move it
    tb_list_entry_remove(list, entry);
    tb_list_entry_insert_next(list, node, entry);
}

/*! moveto the prev entry
 *
 * @param list                              the list
 * @param node                              the list node
 * @param entry                             the moved list entry
 */
static __tb_inline__ tb_void_t              tb_list_entry_moveto_prev(tb_list_entry_head_ref_t list, tb_list_entry_ref_t node, tb_list_entry_ref_t entry)
{
    // check
    tb_assert(node);

    // move it
    tb_list_entry_moveto_next(list, node->prev, entry);
}

/*! moveto the head entry
 *
 * @param list                              the list
 * @param entry                             the moved list entry
 */
static __tb_inline__ tb_void_t              tb_list_entry_moveto_head(tb_list_entry_head_ref_t list, tb_list_entry_ref_t entry)
{
    // move it
    tb_list_entry_moveto_next(list, (tb_list_entry_ref_t)list, entry);
}

/*! moveto the tail entry
 *
 * @param list                              the list
 * @param entry                             the moved list entry
 */
static __tb_inline__ tb_void_t              tb_list_entry_moveto_tail(tb_list_entry_head_ref_t list, tb_list_entry_ref_t entry)
{
    // check
    tb_assert(list);

    // move it
    tb_list_entry_moveto_next(list, list->prev, entry);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif

