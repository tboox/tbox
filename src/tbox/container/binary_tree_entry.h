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
 * @file        binary_tree_entry.h
 * @ingroup     container
 *
 */
#ifndef TB_CONTAINER_BINARY_TREE_ENTRY_H
#define TB_CONTAINER_BINARY_TREE_ENTRY_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "iterator.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/// the binary tree entry
#define tb_binary_tree_entry(head, entry)   ((((tb_byte_t*)(entry)) - (head)->eoff))

/*! init the binary tree entry 
 *
 * @code
 *
    // the xxxx entry type
    typedef struct __tb_xxxx_entry_t 
    {
        // the binary tree entry
        tb_binary_tree_entry_t      entry;

        // the data
        tb_size_t                   data;

    }tb_xxxx_entry_t;

    // the xxxx entry copy func
    static tb_void_t tb_xxxx_entry_copy(tb_pointer_t ltem, tb_pointer_t rtem)
    {
        // check
        tb_assert_return(ltem && rtem);

        // copy it
        ((tb_xxxx_entry_t*)ltem)->data = ((tb_xxxx_entry_t*)rtem)->data;
    }

    // init the binary tree
    tb_binary_tree_entry_head_t binary_tree;
    tb_binary_tree_entry_init(&binary_tree, tb_xxxx_entry_t, entry, tb_xxxx_entry_copy);

 * @endcode
 */
#define tb_binary_tree_entry_init(binary_tree, type, entry, copy)     tb_binary_tree_entry_init_(binary_tree, tb_offsetof(type, entry), sizeof(type), copy)

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/*! the binary tree entry type
 * 
 * <pre>
 *
 *                                          head
 *                               -------------------------
 *                              |                         |
 *                             left                      right
 *                        --------------             -------------
 *                       |              |           |             |
 *                      ...            ...         ...           ...
 *                   ---------       ----
 *                  |         |     |
 *                 ...       ...   last 
 * </pre>
 */
typedef struct __tb_binary_tree_entry_t 
{
    /// the left entry
    struct __tb_binary_tree_entry_t*    left;

    /// the right entry
    struct __tb_binary_tree_entry_t*    right;

}tb_binary_tree_entry_t;

/// the binary tree entry head type
typedef struct __tb_binary_tree_entry_head_t 
{
    /// the left entry
    struct __tb_binary_tree_entry_t*    left;

    /// the right entry
    struct __tb_binary_tree_entry_t*    right;

    /// the binary tree size
    tb_size_t                           size;

    /// the iterator 
    tb_iterator_t                       itor;

    /// the entry offset
    tb_size_t                           eoff;

    /// the entry copy func
    tb_entry_copy_t                     copy;

}tb_binary_tree_entry_head_t;

/// the binary tree entry ref type
typedef tb_binary_tree_entry_t*         tb_binary_tree_entry_ref_t;

/// the binary tree entry head ref type
typedef tb_binary_tree_entry_head_t*    tb_binary_tree_entry_head_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! the binary tree iterator
 *
 * @param binary_tree                       the binary tree
 *
 * @return                                  the binary tree iterator
 */
tb_iterator_ref_t                           tb_binary_tree_entry_itor(tb_binary_tree_entry_head_ref_t binary_tree);

/*! init binary tree
 *
 * @param binary_tree                       the binary tree
 * @param entry_offset                      the entry offset 
 * @param entry_size                        the entry size 
 * @param copy                              the copy func of the entry for algorithm, .e.g sort
 */
tb_void_t                                   tb_binary_tree_entry_init_(tb_binary_tree_entry_head_ref_t binary_tree, tb_size_t entry_offset, tb_size_t entry_size, tb_entry_copy_t copy);

/*! exit binary tree
 *
 * @param binary_tree                       the binary tree
 */ 
tb_void_t                                   tb_binary_tree_entry_exit(tb_binary_tree_entry_head_ref_t binary_tree);

/*! clear binary_tree
 *
 * @param binary_tree                       the binary tree
 */
static __tb_inline__ tb_void_t              tb_binary_tree_entry_clear(tb_binary_tree_entry_head_ref_t binary_tree)
{
    // check
    tb_assert(binary_tree);

}

/*! the binary tree entry count
 *
 * @param binary_tree                       the binary tree
 *
 * @return                                  the binary tree entry count
 */
static __tb_inline__ tb_size_t              tb_binary_tree_entry_size(tb_binary_tree_entry_head_ref_t binary_tree)
{ 
    // check
    tb_assert(binary_tree);

    // done
    return binary_tree->size;
}


/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif

