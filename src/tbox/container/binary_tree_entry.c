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
 * @file        binary_tree_entry.c
 * @ingroup     container
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "binary_tree_entry.h"
#include "../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_iterator_ref_t tb_binary_tree_entry_itor(tb_binary_tree_entry_head_ref_t binary_tree)
{
    // check
    tb_assert_and_check_return_val(binary_tree, tb_null);

    // the iterator
    return &binary_tree->itor;
}
tb_void_t tb_binary_tree_entry_init_(tb_binary_tree_entry_head_ref_t binary_tree, tb_size_t entry_offset, tb_size_t entry_size, tb_entry_copy_t copy)
{
    // check
    tb_assert_and_check_return(binary_tree && entry_size > sizeof(tb_binary_tree_entry_t));

    // init it
    binary_tree->left          = (tb_binary_tree_entry_ref_t)binary_tree;
    binary_tree->right          = (tb_binary_tree_entry_ref_t)binary_tree;
    binary_tree->size          = 0;
    binary_tree->eoff          = entry_offset;
    binary_tree->copy          = copy;

}
tb_void_t tb_binary_tree_entry_exit(tb_binary_tree_entry_head_ref_t binary_tree)
{
    // check
    tb_assert_and_check_return(binary_tree);

    // exit it
    binary_tree->left = (tb_binary_tree_entry_ref_t)binary_tree;
    binary_tree->right = (tb_binary_tree_entry_ref_t)binary_tree;
    binary_tree->size = 0;
}


