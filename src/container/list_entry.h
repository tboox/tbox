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

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/// the list entry
#define tb_list_entry(type, member, entry)                  tb_container_of(type, member, entry)

/// for the list entries
#define tb_list_entry_for(type, member, entry, head, tail) \
    type* entry = tb_container_of(type, member, head); \
        for (; &entry->member != (tail); entry = tb_container_of(type, member, entry->member.next))

/// for all the list entries
#define tb_list_entry_for_all(type, member, entry, list)    tb_list_entry_for(type, member, entry, (list)->next, list)

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

/// the list entry ref type
typedef tb_list_entry_t*        tb_list_entry_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init list
 *
 * @param list                              the list
 */
static __tb_inline__ tb_void_t              tb_list_entry_init(tb_list_entry_ref_t list)
{
    list->next = list;
    list->prev = list;
}

/*! the list is null?
 *
 * @param list                              the list
 *
 * @return                                  tb_true or tb_false
 */
static __tb_inline__ tb_bool_t              tb_list_entry_null(tb_list_entry_ref_t list)
{
    return list->next == list? tb_true : tb_false;
}

/*! the list next entry
 *
 * @param list                              the list
 * @param entry                             the entry
 *
 * @return                                  the next entry
 */
static __tb_inline__ tb_list_entry_ref_t    tb_list_entry_next(tb_list_entry_ref_t list, tb_list_entry_ref_t entry)
{
    return entry->next;
}

/*! the list prev entry
 *
 * @param list                              the list
 * @param entry                             the entry
 *
 * @return                                  the prev entry
 */
static __tb_inline__ tb_list_entry_ref_t    tb_list_entry_prev(tb_list_entry_ref_t list, tb_list_entry_ref_t entry)
{
    return entry->prev;
}

/*! the list head entry
 *
 * @param list                              the list
 *
 * @return                                  the head entry
 */
static __tb_inline__ tb_list_entry_ref_t    tb_list_entry_head(tb_list_entry_ref_t list)
{
    return list->next;
}

/*! the list last entry
 *
 * @param list                              the list
 *
 * @return                                  the last entry
 */
static __tb_inline__ tb_list_entry_ref_t    tb_list_entry_last(tb_list_entry_ref_t list)
{
    return list->prev;
}

/*! is the list head entry?
 *
 * @param list                              the list
 * @param entry                             the entry
 *
 * @return                                  tb_true or tb_false
 */
static __tb_inline__ tb_bool_t              tb_list_entry_is_head(tb_list_entry_ref_t list, tb_list_entry_ref_t entry)
{
    return list->next == entry? tb_true : tb_false;
}

/*! is the list last entry?
 *
 * @param list                              the list
 * @param entry                             the entry
 *
 * @return                                  tb_true or tb_false
 */
static __tb_inline__ tb_bool_t              tb_list_entry_is_last(tb_list_entry_ref_t list, tb_list_entry_ref_t entry)
{
    return list->prev == entry? tb_true : tb_false;
}

/*! insert entry to the next
 *
 * @param list                              the list
 * @param node                              the list node
 * @param entry                             the inserted list entry
 */
static __tb_inline__ tb_void_t              tb_list_entry_insert_next(tb_list_entry_ref_t list, tb_list_entry_ref_t node, tb_list_entry_ref_t entry)
{
    // tne next node
    tb_list_entry_ref_t next = node->next;

    // insert entry
    next->prev      = entry;
    entry->next     = next;
    entry->prev     = node;
    node->next      = entry;
}

/*! insert entry to the prev
 *
 * @param list                              the list
 * @param node                              the list node
 * @param entry                             the inserted list entry
 */
static __tb_inline__ tb_void_t              tb_list_entry_insert_prev(tb_list_entry_ref_t list, tb_list_entry_ref_t node, tb_list_entry_ref_t entry)
{
    tb_list_entry_insert_next(list, node->prev, entry);
}

/*! insert entry to the list 
 *
 * @param list                              the list
 * @param entry                             the inserted list entry
 */
static __tb_inline__ tb_void_t              tb_list_entry_insert_head(tb_list_entry_ref_t list, tb_list_entry_ref_t entry)
{
    tb_list_entry_insert_next(list, list, entry);
}

/*! insert entry to the tail 
 *
 * @param list                              the list
 * @param entry                             the inserted list entry
 */
static __tb_inline__ tb_void_t              tb_list_entry_insert_tail(tb_list_entry_ref_t list, tb_list_entry_ref_t entry)
{
    tb_list_entry_insert_next(list, list->prev, entry);
}

/*! remove the entry
 *
 * @param list                              the list
 * @param entry                             the removed list entry
 */
static __tb_inline__ tb_void_t              tb_list_entry_remove(tb_list_entry_ref_t list, tb_list_entry_ref_t entry)
{
    // tne prev node
    tb_list_entry_ref_t prev = entry->prev;

    // tne next node
    tb_list_entry_ref_t next = entry->next;

    // remove entry
    next->prev = prev;
    prev->next = next;
}

/*! remove the next entry
 *
 * @param list                              the list
 * @param entry                             the removed list entry
 */
static __tb_inline__ tb_void_t              tb_list_entry_remove_next(tb_list_entry_ref_t list, tb_list_entry_ref_t entry)
{
    tb_list_entry_remove(list, entry->next);
}

/*! remove the prev entry
 *
 * @param list                              the list
 * @param entry                             the removed list entry
 */
static __tb_inline__ tb_void_t              tb_list_entry_remove_prev(tb_list_entry_ref_t list, tb_list_entry_ref_t entry)
{
    tb_list_entry_remove(list, entry->prev);
}

/*! remove the list entry
 *
 * @param list                              the list
 */
static __tb_inline__ tb_void_t              tb_list_entry_remove_head(tb_list_entry_ref_t list)
{
    tb_list_entry_remove(list, list->next);
}

/*! remove the last entry
 *
 * @param list                              the list
 */
static __tb_inline__ tb_void_t              tb_list_entry_remove_last(tb_list_entry_ref_t list)
{
    tb_list_entry_remove(list, list->prev);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif

